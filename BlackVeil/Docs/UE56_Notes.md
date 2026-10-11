# Unreal Engine 5.6 Notes

**Target engine:** UE 5.6.1
**Status of this document:** an *audit* of the codebase for 5.3 -> 5.6 changes, not
a record of a successful build. The project has still never been compiled.

---

## 1. Changes already applied for 5.6

### 1.1 `BlackVeil.uproject`

| Change | Why |
|--------|-----|
| `EngineAssociation` `5.3` -> `5.6` | So the launcher opens it with your install instead of asking which engine to use |
| Removed `AnimationWarping` plugin | Experimental, and nothing in the code uses it. A missing plugin entry produces a startup warning for no benefit |
| Removed `MotionWarping` plugin | Listed but unreferenced |
| Removed `GameplayAbilities` plugin | Listed but unreferenced - no GAS code exists in the project |

### 1.2 `Source/BlackVeil/BlackVeil.Build.cs`

Removed `MotionWarping`, `AnimGraphRuntime` and `GameplayAbilities` from the
dependency list. All three were unreferenced, and each was a way for the first
build to fail on a module that was never needed.

### 1.3 `Source/*.Target.cs` - these were missing entirely

**This was a hard blocker.** A UE C++ project cannot be built by UnrealBuildTool
without a `Target.cs`. The project had a module (`BlackVeil.Build.cs`) but no
target, so the first build would have failed before compiling a single file.

Added:

- `Source/BlackVeil.Target.cs` - `TargetType.Game`, for a packaged build
- `Source/BlackVeilEditor.Target.cs` - `TargetType.Editor`, for opening the editor

Both use `BuildSettingsVersion.V5` and `EngineIncludeOrderVersion.Latest`.
`Latest` is deliberate: it avoids hard-coding an include-order enum value that
may not exist in a future engine version.

### 1.4 `Source/BlackVeil/Player/BVPlayerCharacter.cpp`

Removed `Move->NavAgentProps.bCanProne = true;`. `FNavAgentProperties` has no
`bCanProne` member, so this would not compile. Prone does not need it: prone uses
the crouch movement mode with a lowered capsule.

### 1.5 `Config/DefaultEngine.ini`

| Change | Why |
|--------|-----|
| Removed `r.Lumen.TraceMeshSDFs=1` | Retired after 5.4; produces a console warning on 5.6 |
| Moved nav agent geometry from `NavigationSystemV1` to `RecastNavMesh` | `AgentRadius` / `AgentHeight` / `AgentMaxStepHeight` are read from `RecastNavMesh` in UE5. The old placement was silently ignored, which would have produced a NavMesh built with default agent size |

---

## 2. Remaining risks for the first build

I cannot verify these without the engine. They are ranked by how likely they are
to bite, with the fix for each.

### 2.1 Deprecated direct access to light properties (likely warning, possibly error)

`BVStealthComponent.cpp` reads `LightComp->Intensity` and `Spot->OuterConeAngle`
as public members. UE5 has been migrating these behind getters. If 5.6 errors,
change:

```cpp
// BVStealthComponent.cpp, inside SampleLighting()
LightComp->Intensity            ->  LightComp->GetIntensity()
Spot->OuterConeAngle            ->  Spot->GetOuterConeAngle()
```

### 2.2 `TMap` of `TWeakObjectPtr` declared as `UPROPERTY`

`BVSquadManager.h` declares:

```cpp
UPROPERTY() TMap<FName, TArray<TWeakObjectPtr<ABVAIController>>> Squads;
```

If UHT rejects this, delete the `UPROPERTY()` line. The map is only used from
C++ and does not need reflection or garbage-collection tracing through the
property system.

### 2.3 `UAISenseConfig_Sight::SetMaxAge`

Used in `BVAIController::ConfigurePerception()`. Should be present in 5.6. If it
has moved, set the `MaxAge` member directly instead.

### 2.4 Python deprecations

The `Tools/` scripts prefer `unreal.EditorActorSubsystem` and
`unreal.LevelEditorSubsystem` and fall back to `unreal.EditorLevelLibrary`.
On 5.6 the fallbacks are deprecated and will log warnings if reached - but they
should not be reached.

**`NavMeshBoundsVolume` sizing** is the one thing to check by eye. Spawning a
brush volume from Python and scaling it does not reliably resize the brush across
engine versions. After `BuildAllMaps.build_one(N)`, open the map, press `P`, and
confirm the green navmesh covers the floor. If it does not, select
`BV_NavMeshBounds` and set the size in the Details panel.

### 2.5 Default build settings

If UBT rejects `BuildSettingsVersion.V5`, change both `Target.cs` files to
`BuildSettingsVersion.Latest`.

---

## 3. Expected first-build sequence

```bash
# 1. Generate project files
"<UE>/Engine/Build/BatchFiles/GenerateProjectFiles.bat" -project="<path>/BlackVeil.uproject" -game

# 2. Build the editor target
"<UE>/Engine/Build/BatchFiles/Build.bat" BlackVeilEditor Win64 Development \
    -project="<path>/BlackVeil.uproject" -waitmutex

# 3. Open
"<UE>/Engine/Binaries/Win64/UnrealEditor.exe" "<path>/BlackVeil.uproject"
```

Expect a batch of warnings on the first build. Warnings are fine; errors are not.
Send me the first 30 lines of the error output and section 2 above covers most of
what is likely to appear.

---

## 4. What to do once it compiles

1. Follow `README.md` section 3.4 to create the Blueprint and data assets.
2. Run `BuildAllMaps.build_all()` from the editor's Python console.
3. Open `M01`, press `P`, confirm the navmesh.
4. Press Play. You should be standing in the gatehouse of the Kostov steelworks.

That is the point at which the first mission is actually playable, and the point
at which every claim in `Docs/TestReport.md` can finally start moving from
"authored" to "verified".
