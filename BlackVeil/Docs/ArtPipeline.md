# Art and Geometry Pipeline

This document answers one question: **where does the geometry come from?**

It has two answers, because there are two different things called "geometry" here:

| | Source | Status |
|---|--------|--------|
| **Blockout geometry** | `/Engine/BasicShapes/Cube` - Unreal's built-in unit cube, scaled per wall/floor/prop by `Tools/BlockoutBuilder.py` | Implemented. Generates real static meshes with real collision |
| **Art geometry** | Environment meshes, character meshes, props, textures, materials, animations | **Does not exist. None of it.** |

The blockout is a playable level. The art pass is a separate, larger piece of work
that has not been started, and this document is the plan for it.

---

## 1. How the blockout is made

```python
BlockoutBuilder.build(1)     # spawns scaled engine cubes per the layout JSON
BlockoutFinish.finish(1)     # materials + merge into real StaticMesh assets
```

`BlockoutBuilder` reads `Content/Data/Layouts/M01_Layout.json` and spawns one
`StaticMeshActor` per wall segment, floor slab, ceiling, cover prop and stair step.
A wall with a doorway becomes three boxes (left, right, lintel). Mission 1 is
roughly 86 actors; the campaign is roughly 1,000.

`BlockoutFinish` then:

1. **Creates real Material assets** at `/Game/Materials/Blockout/` - one per role
   (Floor, Ceiling, Wall, Cover, Glass). Mid-grey and fully matte on purpose: a
   stealth blockout has to read light and shadow honestly, so nothing is glossy
   and nothing is bright enough to fake illumination.
2. **Assigns them by role**, inferred from the actor label (`Floor_*`, `Wall_*`...).
3. **Merges each room's cubes into one StaticMesh asset** at
   `/Game/Blockout/Merged/M_<RoomName>`, and merges cover props by kind into
   `Props_<Type>`. This is the step that converts ~1,000 loose cubes into a few
dozen real mesh assets an artist can open, re-UV and replace.

---

## 2. Why scaled cubes are legitimate, and where they stop being enough

Scaling a unit cube gives correct dimensions, correct collision and correct
sightlines. For blocking out a stealth level that is genuinely all you need, which
is why the genre is built this way in production.

What scaled cubes are **not** good for:

- **UVs.** A cube scaled to 3200 x 20 x 900 has badly stretched texels. Fine for
  grey, useless for brick.
- **Detail.** No trim, no door frames, no skirting, no panel lines.
- **Draw calls.** 1,000 actors is 1,000 draw calls until merged. `BlockoutFinish`
  fixes this.
- **Lightmap/nanite behaviour.** Not a real concern with Lumen, but worth knowing.

So the blockout is the right starting point and the wrong finishing point.

---

## 3. The art asset manifest

Everything the game currently expects but does not have. Paths are what the C++
and Blueprints reference, so they are not free choices.

### 3.1 Characters and animation

| Asset | Path | Notes |
|-------|------|-------|
| Player skeletal mesh | `/Game/Characters/VEIL/SK_VEIL` | Third-person operative, needs a skeleton |
| Player anim blueprint | `/Game/Characters/VEIL/ABP_VEIL` | Must read the state the C++ broadcasts (see 3.4) |
| Player anim set | `/Game/Characters/VEIL/AS_VEIL` | Idle, walk, jog, sprint, crouch-walk, prone-crawl, vault, mantle, ledge-hang, lean-L/R, takedown x5, carry-body, interact |
| Guard skeletal mesh | `/Game/Characters/Guard/SK_Guard` | 6 archetypes can share one mesh with material variants |
| Guard anim blueprint | `/Game/Characters/Guard/ABP_Guard` | Must read `bIncapacitated`, `bDead` |
| Guard anim set | `/Game/Characters/Guard/AS_Guard` | Patrol, idle-scan, walk, run, investigate, aim, fire, take-cover, incapacitated, dead |

**Free/cheap sources:** Epic's own **Lyra** sample and the **Game Animation Sample
Project** (free, includes a full locomotion set with crouch and sprint). Mixamo for
basic locomotion clips, though retargeting to a custom skeleton is required.

### 3.2 Environment

| Asset group | Path | Notes |
|-------------|------|-------|
| Modular walls/floors/ceilings | `/Game/Environment/Modular/` | The art replacement for the blockout rooms |
| Door frames + doors | `/Game/Environment/Doors/` | Must fit the 280 cm opening width the layouts use |
| Props | `/Game/Environment/Props/` | Containers, crates, barriers, machines, shelving, desks, vehicles |
| Light fixtures | `/Game/Environment/Lights/` | The blockout uses raw light actors; final needs fixtures |
| Cameras | `/Game/Environment/Security/Camera` | Security camera body |
| Terminals | `/Game/Environment/Security/Terminal` | Console mesh with a screen |
| Alarm panels | `/Game/Environment/Security/AlarmPanel` | Wall box with a status lamp |
| Hiding spots | `/Game/Environment/Hiding/` | Lockers, containers, desks |

**Sources:** **Fab** (Epic's marketplace) has modular industrial and modern-interior
kits that fit these missions directly. **Quixel Megascans** (free with Unreal) for
surfaces and set dressing. **KitBash3D** for large architecture. Blender for
anything mission-specific.

### 3.3 Gadgets, weapons, effects

| Asset | Path |
|-------|------|
| Pistol / rifle / non-lethal projector | `/Game/Weapons/` |
| Noise maker, flashbang, smoke, EMP, sticky camera | `/Game/Gadgets/` |
| Lockpick, hack device, detector, fiber-optic | `/Game/Gadgets/` |
| Smoke volume | Niagara system, `/Game/VFX/NS_Smoke` |
| Flashbang burst | Niagara system, `/Game/VFX/NS_Flashbang` |
| EMP pulse | Niagara system, `/Game/VFX/NS_EMP` |

### 3.4 Audio

The `UBVAudioManager` looks cues up by ID in `MusicCues`, `FootstepCues` and
`GadgetCues` maps. It needs, at minimum:

- Music: `BGM_Calm`, `BGM_Tension`, `BGM_Combat`
- Footsteps: one cue per `EBVSurfaceType` (Concrete, Metal, Wood, Carpet, Grass,
  Water, Gravel, Glass, Mud)
- Weapons: one per weapon ID, plus a `_Suppressed` variant for each
- Radio dialogue: one cue per `DialogueID` in `Docs/Dialogue/`

---

## 4. What the animation blueprints must read

The C++ already broadcasts everything the anim graph needs. `ABVPlayerCharacter`
exposes:

```
EBVMovementSpeed CurrentSpeed        // Prone..Sprint, drives the locomotion blend
EBVPosture      CurrentPosture      // Standing/Crouching/Prone/Hanging/InCover
bool  bInCover, bWallHugging, bLeaning, bLedgeHanging
float LeanAmount                    // -1..1, lateral lean blend
float GetCurrentPlanarSpeed()       // cm/s, for blendspace input
EBVSurfaceType GetCurrentSurface()  // for footstep notifies
```

plus delegates `OnSpeedChanged`, `OnPostureChanged`, `OnCoverChanged` so the graph
can react without polling.

`ABVGuardCharacter` exposes `bIncapacitated`, `bDead`, `bBeingCarried`,
`bConcealed` and `GuardType`, which is what the guard graph needs to pick a
locomotion set, an alert pose and a death pose.

**This is the contract.** An animator can build the whole graph against it without
reading gameplay code.

---

## 5. How to replace the blockout with art

Every generated actor carries tags specifically so the art pass can find it:

| Tag | Meaning |
|-----|---------|
| `BV_Blockout` | Any generated geometry |
| `BV_Room_<Name>` | Which room a piece belongs to |
| `BV_Cover` + `Cover_<Type>` | Cover props, by kind |
| `BV_Light` + `LightGroup_<Group>` | Light actors, by security group |
| `BV_NavMesh` | The navmesh bounds volume |

Typical replacement pass, room by room:

1. Select a merged room mesh (e.g. `/Game/Blockout/Merged/M_RollingMill`).
2. Open it in the static mesh editor to see the exact dimensions.
3. Build the room from modular kit pieces to match, deleting the merged mesh as
   you go.
4. Keep the light actors - they are gameplay, and their positions are tuned.
5. Keep the cover prop *positions*; swap the cubes for real meshes. The
   `Cover_<Type>` tag tells you what each one is meant to be.

Because the layout JSON is the source of truth, you can always regenerate a clean
blockout with `BlockoutBuilder.build(N, rebuild=True)` if a replacement pass goes
wrong.

---

## 6. Honest status

| Item | Status |
|------|--------|
| Blockout geometry generation | Implemented |
| Blockout materials | Implemented (`BlockoutFinish.create_materials`) |
| Blockout merge to real meshes | Implemented (`BlockoutFinish.merge_rooms`) |
| Blockout actually built in the editor | **No - 0 of 12** |
| Character meshes / animations | **None exist** |
| Environment art | **None exists** |
| Textures / final materials | **None exist** |
| Audio assets | **None exist** |
| Niagara effects | **None exist** |

The code is written against asset *paths*; the assets themselves are an art
production task that has not been started. What exists is the pipeline that
specifies what is needed, the tags that let art replace blockout piece by piece,
and the animation contract that lets an animator work without reading gameplay code.
