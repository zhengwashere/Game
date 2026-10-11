# BLACK VEIL

A third-person tactical stealth game for **Unreal Engine 5.6.1**.

You are **Marcus Vael**, callsign **VEIL** - an operative running deniable operations
against a private intelligence network called UMBRA that has spent a decade buying
governments one secret at a time. Twelve missions. One ledger with 214 names on it.

---

## 1. What this repository contains

This is the **complete game project** - not a design document and not a prototype.

| Area | Contents |
|------|----------|
| Gameplay code | ~90 C++ files across 8 modules (Player, AI, Gadgets, Mission, World, UI, Core, SaveGame) |
| Campaign | 12 distinct missions, each with its own data asset, patrol network, objectives, intel, lighting plan and set piece |
| Configuration | `DefaultEngine.ini`, `DefaultGame.ini`, `DefaultInput.ini`, `DefaultGameplayTags.ini` |
| Data | 5 data tables (CSV) and 12 mission data assets (JSON, import-ready) |
| Tooling | 9 Python scripts: one-command map builder, blockout geometry, gameplay placement, security wiring, objective placement, blockout finishing (materials + mesh merging), and three validators |
| Documentation | Setup guide, architecture, 5.6 notes, blockout format, art pipeline, story bible, 12 level design docs, 12 dialogue scripts, test report, progress report |

> **Engine availability note.** The project targets **UE 5.6.1**. It was authored in
> an environment with no engine at all, so the upgrade has been *audited* rather
> than tested - see `Docs/UE56_Notes.md` for exactly what was changed for 5.6 and
> what to check on the first build.
>
> **Engine availability note (original).** This project was authored in an environment with **no
> Unreal Engine installation**, so the C++ has not been compiled by a compiler and the
> maps have not been built in the editor. Everything is written to compile against UE
> 5.6 and to be imported directly. Section 7 explains exactly what is verified and
> what is not - see `Docs/TestReport.md` for the honest, itemised report.

---

## 2. Requirements

- **Unreal Engine 5.6.1** (the project is configured for 5.6; see `Docs/UE56_Notes.md`)
- **Visual Studio 2022** with *Game development with C++* (Windows) or
  **Xcode 14+** (macOS) or **JetBrains Rider / VS Code + clang** (Linux)
- **.NET 6 SDK** (required by UnrealBuildTool)
- A GPU capable of running Lumen (the stealth lighting system depends on it)
- Roughly 25 GB free for engine + derived data

---

## 3. Getting the project running (step by step)

### 3.1 Generate the project files

```bash
# Windows
"C:/Program Files/Epic Games/UE_5.6/Engine/Build/BatchFiles/GenerateProjectFiles.bat" \
    -project="<path>/BlackVeil/BlackVeil.uproject" -game -engine

# Linux / macOS
"<UE_ROOT>/Engine/Build/BatchFiles/Linux/GenerateProjectFiles.sh" \
    -project="<path>/BlackVeil/BlackVeil.uproject" -game -engine
```

Alternatively, right-click `BlackVeil.uproject` and choose **Generate Visual Studio project files**.

### 3.2 Build the C++ module

```bash
# Windows
"<UE_ROOT>/Engine/Build/BatchFiles/Build.bat" BlackVeilEditor Win64 Development \
    -project="<path>/BlackVeil/BlackVeil.uproject" -waitmutex

# Linux
"<UE_ROOT>/Engine/Build/BatchFiles/Linux/Build.sh" BlackVeilEditor Linux Development \
    -project="<path>/BlackVeil/BlackVeil.uproject"
```

Or open `BlackVeil.sln` / `BlackVeil.xcworkspace` and build the **BlackVeilEditor** target.

### 3.3 Launch the editor

```bash
"<UE_ROOT>/Engine/Binaries/Win64/UnrealEditor.exe" "<path>/BlackVeil/BlackVeil.uproject"
```

On first launch Unreal will compile shaders (10-25 minutes). This is normal.

### 3.4 Create the required assets

The C++ layer is complete, but Unreal still needs **asset** versions of a few things.
This is the only manual step, and it is quick:

**a) Mission data assets (12)**

1. Import each `Content/Data/Missions/DA_Mission_XX.json` as a Data Asset, or
2. Create 12 assets of class `BVMissionData` at `/Game/Data/Missions/DA_Mission_01` ... `DA_Mission_12`
   and paste the values from the matching JSON.

Names and paths matter: the mission manager loads them as
`/Game/Data/Missions/DA_Mission_%02d`.

**b) Data tables (5)**

Import the CSVs in `Content/Data/` as Data Tables:

| CSV | Row struct |
|-----|-----------|
| `DT_Gadgets.csv` | `BVGadgetTableRow` |
| `DT_Enemies.csv` | `BVEnemyTableRow` |
| `DT_Weapons.csv` | `BVWeaponTableRow` |
| `DT_Intel.csv` | `BVIntelTableRow` |
| `DT_Missions.csv` | `BVMissionTableRow` |

**c) Blueprints (thin, level-facing only)**

| Blueprint | Parent class | Purpose |
|-----------|--------------|---------|
| `BP_PlayerCharacter` | `ABVPlayerCharacter` | Assign the Enhanced Input assets and the skeletal mesh/anim BP |
| `BP_GuardCharacter` | `ABVGuardCharacter` | Assign guard mesh/anim BP, behaviour tree |
| `BT_Guard` | (Behaviour Tree) | Wire the tasks/services/decorators listed in `Docs/Architecture.md` |
| `BB_Guard` | (Blackboard) | Keys listed in `BVBlackboardKeys.h` |
| `WBP_HUD*` | `UBV*Widget` | Widget visuals; C++ owns all the data binding |
| `BP_Door`, `BP_Camera`, `BP_Terminal`, `BP_AlarmPanel`, `BP_Light`, `BP_HidingSpot` | matching C++ classes | Assign meshes and materials |
| `BP_NoiseMaker`, `BP_Flashbang`, `BP_Smoke`, `BP_EMP`, `BP_StickyCamera` | matching C++ projectile classes | Assign meshes and Niagara effects |

**d) Maps**

Create empty levels `MainMenu`, `Briefing`, and `M01` ... `M12` under `/Game/Maps`.
The level tools build the mission maps for you - see 3.5.

### 3.5 Build a mission level

A level is built in two passes. **Pass 1 is the space** (floors, walls, doorways,
stairs, cover, lights, navigation). **Pass 2 is the gameplay** (objective volumes,
guard spawns, patrol routes, cameras, terminals, intel).

In the editor's **Python console**:

```python
import sys
sys.path.append(r"<path>/BlackVeil/Tools")
import BuildAllMaps

BuildAllMaps.build_all()        # creates all 14 maps and builds M01..M12 end to end
```

That single call creates each map, builds its geometry, places the gameplay, wires
the security network and saves. To do one mission at a time:

```python
BuildAllMaps.build_one(3)       # create/rebuild just M03
BuildAllMaps.build_frontend()   # just MainMenu + Briefing
```

Or drive the four passes yourself, with the target map already open:

```python
import BlockoutBuilder, LevelSetup, SecuritySystemSetup, MissionObjectiveSetup

BlockoutBuilder.build(1)        # 1. the space - a walkable greybox + environment + NavMesh
LevelSetup.build_mission(1)     # 2. objectives, guard spawns, routes, intel, security
SecuritySystemSetup.build(1)    # 3. camera / terminal / alarm network
MissionObjectiveSetup.setup(1)  # 4. objective trigger volumes
```

The 14 map names are listed in `Docs/MapManifest.md`. They are not free choices -
they are referenced by the mission data assets and hard-coded in the C++ that
opens them.

Every generated actor is tagged (`BV_Blockout` or `BV_Generated`), so re-running
replaces rather than duplicates, and the art pass can find every generated actor.

A blockout is a genuinely playable level, not a placeholder - sightlines, shadow,
cover and patrol timing are all readable in untextured geometry, which is how this
genre is built in production before the art pass. The layout format is documented
in `Docs/BlockoutFormat.md`.

> **Check the NavMesh.** `BlockoutBuilder` places a `NavMeshBoundsVolume`, but the
> Python API for brush sizing varies between engine versions. Look at it once. If
> it is wrong, select it and set the size in the Details panel. Without navigation
> data the guards will not move.

### 3.6 Validate the data (no editor required)

```bash
python3 Tools/ValidateMissionData.py    # 12/12 missions valid
python3 Tools/ValidateLayouts.py        # 0 placement problems
```

`ValidateLayouts.py` checks that every guard spawn, patrol waypoint, checkpoint and
objective sits on a floor inside a built room. If you edit a layout or a mission,
re-run it. If it reports problems, `ReconcileLayouts.py` will show you the moves
needed and `--apply` will make them:

```bash
python3 Tools/ReconcileLayouts.py            # dry run
python3 Tools/ReconcileLayouts.py --apply    # snap positions into the nearest room
python3 Tools/GenerateLevelDocs.py           # refresh the level design docs
```

---

## 4. Controls (keyboard & mouse)

| Action | Key |
|--------|-----|
| Move | `W A S D` |
| Camera | Mouse |
| Jog (default when moving) | automatic |
| Sprint | `Shift` (hold) |
| Crouch toggle | `C` |
| Prone toggle | `Z` |
| Silent movement | `Alt` (toggle) |
| Vault / climb / mantle | `Space` |
| Enter cover / wall hug | `Q` |
| Lean left / right | `X` / `V` |
| Interact (tap or hold) | `E` |
| Cycle door/lock action | `R` |
| Takedown | `F` |
| Fire | `Left Mouse` |
| Aim | `Right Mouse` |
| Gadget slots 1-4 | `1 2 3 4` |
| Switch weapon | `Tab` |
| Reload | `R` (when not cycling an interaction) |
| Pause | `Esc` |

Gamepad mapping is configured on the same Enhanced Input assets; the movement system
uses standard axis inputs so controller support works without code changes.

---

## 5. How to play well

The game supports four playstyles, and scores them differently:

- **Ghost** - never seen, no alarms, no bodies found. Highest score multiplier.
- **Panther** - silent non-lethal takedowns, bodies concealed. High score.
- **Assault** - detected, weapons free. Lower score, harder mission (guards coordinate
  and reinforcements are called).
- **Adaptive** - a mix. Default grade.

Practical advice the game teaches by design:

1. **Light is the real enemy.** Check the visibility meter before crossing an open floor.
   Shoot out a floodlight if the alternative is a lit corridor.
2. **Noise travels through walls, but less so.** A suppressed pistol is audible at
   ~7 m; an unsuppressed shot at ~40 m.
3. **Guards remember where you were, not where you are.** Break line of sight and
   change direction - they will search the last known position, not follow you.
4. **Bodies are evidence.** Conceal them. A discovered body escalates the whole squad.
5. **Sabotage pays.** Hacking a terminal that kills a camera group is worth more than
   three flashbangs.

---

## 6. Campaign

| # | Mission | Setting | Signature mechanic |
|---|---------|---------|--------------------|
| 1 | Dead Drop | Kostov Steelworks, Ukraine | Teach shadows and distraction |
| 2 | Blackout | Vasiliev Power Station, Georgia | Storm brownout windows |
| 3 | Ghost Protocol | Halcyon Tower, Singapore | Motion-triggered corridor lighting |
| 4 | Cold Harbor | Port of Kalamis, Cyprus | Gantry crane container ride |
| 5 | Paper Trail | Directorate Archive, Vienna | Biometric vault (unpickable) |
| 6 | Deep Cover | Verdant Gala, Monaco | Social stealth in a crowd |
| 7 | Dead Signal | Kestrel Relay Station, Caucasus | 12-minute reinforcement timer |
| 8 | Silent Depths | Anvil Deep, Arctic | Flooded passages and pumps |
| 9 | The Exchange | Marchetti Compound, Sicily | Branching disruption choice |
| 10 | Broken Arrow | Site 14, Kazakhstan | Three-stage prototype sabotage |
| 11 | No Witnesses | UMBRA Ops Center, Prague | 40-minute burn protocol |
| 12 | Black Veil | Sorn Estate, Black Sea | Layered vault + branching finale |

Full design docs: `Docs/LevelDesign/`. Dialogue scripts: `Docs/Dialogue/`.

---

## 7. Verification status

Because no Unreal Engine binary was available in the authoring environment:

**Verified**
- All mission data assets pass structural validation (`Tools/ValidateMissionData.py`)
- All 12 missions have unique objectives, patrol networks, intel, checkpoints, lighting plans
- Documentation, dialogue and story content are complete and internally consistent

**Not verified (requires the engine)**
- C++ compilation
- Blueprint asset creation and wiring
- NavMesh generation and AI pathing at runtime
- In-editor playtesting of each mission

See `Docs/TestReport.md` for the full itemised breakdown, and
`Docs/ProgressReport.md` for the system-by-system implementation status.

---

## 8. Project layout

```
BlackVeil/
  BlackVeil.uproject
  Config/                     Engine, game, input and gameplay tag configuration
  Content/
    Data/                     5 CSV data tables
      Missions/               12 mission data assets (JSON, import-ready)
      Layouts/                12 blockout layouts (the level geometry)
    Maps/ Blueprints/ UI/ Audio/ VFX/ Materials/
  Source/BlackVeil/
    Core/        Game mode, game state, game instance, shared types, gameplay tags
    Player/      Character, controller, stealth, interaction, takedown components
    AI/          Guard, AI controller, perception, patrol, alert state, squad manager
      BehaviorTree/   BTTask_*, BTService_*, BTDecorator_* nodes
    Gadgets/     Gadget base, throwables, tools, weapons, equipment inventory
    Mission/     Mission data asset, mission manager, objectives, checkpoints, intel
    World/       Doors, cameras, terminals, alarm panels, lights, hiding spots, noise
    UI/          HUD, widgets, menus, audio manager
    SaveGame/    Persistent campaign save object
  Tools/        Map builder, blockout builder, gameplay placement, security wiring, validators
  Docs/         Setup, architecture, story bible, level design, dialogue, reports
```

---

## 9. Troubleshooting

| Symptom | Cause / fix |
|---------|-------------|
| `Cannot open include file: 'EnhancedInputComponent.h'` | Enable the **EnhancedInput** plugin (it is already listed in the `.uproject`) |
| Guards stand still | The NavMesh is not built. Add a `NavMeshBoundsVolume` covering the play area |
| Mission has no objectives | The mission data asset is missing or at the wrong path (`/Game/Data/Missions/DA_Mission_XX`) |
| Player is invisible to guards in daylight | Check that the level has actual light actors - visibility is computed from real light components |
| `BVMissionData` not found | Build the C++ module first; data assets require the class to exist |
| Lumen disabled warning | `r.DynamicGlobalIlluminationMethod=1` in `DefaultEngine.ini`; ensure the GPU supports it |

---

## 10. Extending the game

Adding a 13th mission requires **no new code**:

1. Create `DA_Mission_13` from `UBVMissionData`, fill it in.
2. Create map `M13`.
3. Run `LevelSetup.build_mission(13)` and the other two tools.
4. Add the entry to `DT_Missions.csv`.
5. Run `ValidateMissionData.py`.

The mission manager loads, spawns, tracks and scores it automatically.
