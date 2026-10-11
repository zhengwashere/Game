# Black Veil - Test Report

**Project:** Black Veil (UE 5.3+)
**Report scope:** everything implemented in this repository, with an explicit
statement of what has and has not been executed.

---

## 1. Environment limitation (read this first)

This project was authored in an environment containing **no Unreal Engine
installation and no C++ toolchain for Unreal**. The following were therefore
**impossible to execute** here:

- Compiling the C++ module
- Opening the project in the Unreal Editor
- Generating Blueprint assets
- Building NavMesh and running AI pathing
- Playtesting any mission

Everything below is reported honestly on that basis. Nothing in this report
claims a runtime result that was not actually produced.

---

## 2. What was actually executed

| Check | Method | Result |
|-------|--------|--------|
| Mission data structural validation | `python3 Tools/ValidateMissionData.py` | **12/12 missions valid** |
| Objective ID uniqueness and prerequisite resolution | Validator | Pass |
| Patrol route references from every enemy spawn | Validator | Pass |
| Gadget / weapon IDs referenced by missions exist in the code registry | Validator | Pass |
| Every mission has >=1 mandatory objective, >=1 optional objective, >=1 checkpoint, >=2 intel items | Validator | Pass |
| Header hygiene: all UCLASS/USTRUCT/UENUM headers include their `.generated.h` | Scripted scan | Pass (60/60) |
| Header hygiene: no `#include` appears after `.generated.h` | Scripted scan | Pass (0 violations) |
| No `TODO` / `FIXME` markers in the source tree | Scripted scan | Pass (0 found) |
| Include-path consistency (no references to non-existent headers) | Scripted scan | Pass after fixes |

### 2.1 Validator output

```
OK    Dead Drop
OK    Blackout
OK    Ghost Protocol
OK    Cold Harbor
OK    Paper Trail
OK    Deep Cover
OK    Dead Signal
OK    Silent Depths
OK    The Exchange
OK    Broken Arrow
OK    No Witnesses
OK    Black Veil

12/12 missions valid
```

---

## 3. What was NOT executed (and what to run once the engine is available)

| Item | Status | How to verify |
|------|--------|---------------|
| C++ compilation | **Not run** | `Build.bat BlackVeilEditor Win64 Development -project=BlackVeil.uproject` |
| Editor launch | **Not run** | Open `BlackVeil.uproject` |
| Blueprint creation | **Not run** | Follow README section 3.4 |
| Level construction | **Not run** | `LevelSetup.build_mission(N)` per map |
| NavMesh generation | **Not run** | Add `NavMeshBoundsVolume`, press `P` |
| Automation tests | **Not run** | Session Frontend > Automation > filter `BlackVeil` |
| Mission 1-12 playthrough | **Not run** | Requires a built project |

---

## 4. Automated tests included (not yet executed)

`Source/BlackVeil/Tests/BVStealthTests.cpp` contains four automation tests:

| Test | Asserts |
|------|---------|
| `BlackVeil.Stealth.NoiseRadiusTiers` | Silent = 0 radius; tiers strictly increasing |
| `BlackVeil.Stealth.SurfaceAffectsNoise` | Carpet quieter than gravel at the same speed; prone on concrete is silent |
| `BlackVeil.Mission.GradeCalculation` | Flawless = S; loud/lethal = D; one alarm cannot be S |
| `BlackVeil.Campaign.ProgressionUnlocks` | Starts at mission 1; lockpick unlocked; EMP locked until awarded; completing mission 1 unlocks mission 2 |

These are pure-logic tests requiring no map, so they will run as soon as the
module compiles.

---

## 5. Manual test plan for each mission (to run after building)

For every mission, verify in this order. This is the checklist used to gate a
mission as "complete" rather than "authored".

### 5.1 Mission load and flow

- [ ] Level opens with no Blueprint errors in the Output Log
- [ ] `UBVMissionManager::LoadMission` logs the mission name (not a missing-asset warning)
- [ ] Objectives appear in the HUD tracker at mission start
- [ ] Objectives with prerequisites are inactive until their prerequisites complete
- [ ] Completing all mandatory objectives triggers the results screen
- [ ] The results screen shows the correct stats and a computed grade
- [ ] The next mission unlocks in the campaign save

### 5.2 Stealth and visibility

- [ ] Visibility meter reads low in a dark corner and high under a floodlight
- [ ] Prone in shadow is materially harder for a guard to spot than standing in light
- [ ] Shooting out a light drops the visibility meter in that area
- [ ] An EMP-disabled light stops contributing to visibility
- [ ] The detection vignette flashes when a guard first acquires the player

### 5.3 Noise

- [ ] Sprinting is audible further than walking
- [ ] A suppressed pistol shot is audible at roughly 7 m and not at 20 m
- [ ] An unsuppressed shot is audible across the level
- [ ] A thrown noise maker pulls a guard to its location and holds them there
- [ ] A gunshot through a wall is heard, but at a reduced radius

### 5.4 AI behaviour

- [ ] A guard on patrol follows its route, waits, and looks around
- [ ] A guard who hears a noise moves to investigate rather than to the player
- [ ] A guard who loses sight searches the last known position, not the player's actual position
- [ ] A guard cannot detect the player through a wall
- [ ] A squad splits into sectors rather than stacking on one point
- [ ] A discovered body escalates the squad to Searching
- [ ] Reinforcements spawn when an alarm panel is triggered
- [ ] A guard returns to patrol after the search timer expires with no contact
- [ ] A flashbanged guard's sight range collapses, then recovers

### 5.5 Interaction

- [ ] The prompt selects the nearest object the player is aiming at, not the nearest object overall
- [ ] A locked mechanical door refuses normal opening and accepts lockpicking
- [ ] A barred door can only be forced (loudly)
- [ ] An electronic door refuses to open when its power is cut
- [ ] Hacking a terminal disables the correct camera group
- [ ] Sabotaging an alarm panel prevents its group from being raised
- [ ] Bodies can be carried and concealed, and concealed bodies stop being discoverable
- [ ] The player can hide in a locker and guards searching nearby do not find them

### 5.6 Checkpoints and failure

- [ ] Walking through a checkpoint logs it and records the player's transform
- [ ] Dying restores the player to the last checkpoint, not the level start
- [ ] Restarting from the pause menu restores the checkpoint
- [ ] Objective progress survives a checkpoint restore

### 5.7 Per-mission unique mechanics

| Mission | Verify |
|---------|--------|
| 1 | Perimeter floodlights can be shot out to open a dark corridor |
| 2 | Brownout windows switch the surveillance grid off for ~90 s |
| 3 | Motion-triggered corridor lights activate ahead of the player |
| 4 | A moving gantry container can be ridden to bypass the yard |
| 5 | The vault door cannot be picked or hacked; biometric is required |
| 6 | Guards do not attack a guest behaving normally; suspicion rises on running or crouching in public |
| 7 | The 12-minute reinforcement countdown starts on alarm and is extended by destroying the radio mast |
| 8 | Flooded passages slow movement and generate noise; pumps open a shortcut |
| 9 | The disruption choice changes the debriefing text |
| 10 | The prototype sabotage requires three stages in order |
| 11 | The 40-minute burn protocol is disabled by hacking the operations floor terminal |
| 12 | The vault requires keycard and biometric; the finale branches three ways |

---

## 6. Known risks and unverified assumptions

These are the areas most likely to need adjustment on first compile. They are
listed deliberately rather than hidden:

1. **AI Perception sight vs. lighting.** The engine's sight sense has no concept of
   darkness. This is handled by `ABVAIController::ValidateSight()`, which rejects
   detections beyond the light-adjusted range. If perception fires and is then
   rejected, the guard may briefly appear to react. Tuning `DarkSightPenalty`
   (default 0.35) will control how sharply this reads.
2. **Dynamic component creation.** `UBVEquipmentInventory` creates gadget and
   weapon components with `NewObject` + `RegisterComponent` at runtime. This is
   standard but worth verifying against your engine version's component
   registration rules for actor-owned components.
3. **Smoke blocking.** `ABVProjectile_Smoke` spawns a tagged sight blocker actor;
   the AI sight check reads the tag. This is a lightweight approach that works but
   is less precise than a volumetric occlusion test. Upgrading it is a contained
   change in `ABVAIController::ValidateSight`.
4. **Prone.** Prone reuses the crouch movement mode with a lowered capsule rather
   than a custom movement mode, so the visual pose depends on the animation
   blueprint. The capsule change is the part that matters for gameplay.
5. **Patrol route authoring.** Guards receive routes from `ABVPatrolRoute` actors
   matched by `RouteID`. If a mission's route actor is missing, the guard falls
   back to standing at their spawn point (the patrol task fails cleanly rather
   than crashing).
6. **Data asset paths.** The mission manager loads
   `/Game/Data/Missions/DA_Mission_%02d`. If assets are created at a different
   path, missions load with an empty objective set and log a warning rather than
   failing hard.

---

## 7. Summary

| Category | Authored | Executed here |
|----------|----------|---------------|
| C++ gameplay systems | Yes - 115 files | No (no toolchain) |
| Mission data (12) | Yes | Yes - validated |
| Configuration | Yes | n/a |
| Data tables | Yes - 5 | Partially (validator reads missions) |
| Level design docs | Yes - 12 | n/a |
| Dialogue scripts | Yes - 12 | n/a |
| Automation tests | Yes - 4 | No (no toolchain) |
| Playtested missions | - | **0** |

The honest headline: **the game is fully authored, structurally validated and
ready to build, but it has not been compiled or played.** Section 5 is the
checklist that converts "authored" into "verified".
