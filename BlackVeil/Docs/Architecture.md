# Black Veil - Technical Architecture

## 1. Principles

1. **C++ owns systems, Blueprints own content.** Every mechanic lives in a C++
   class. Blueprints only assign assets (meshes, animations, input assets, widget
   visuals) and hold level-specific one-off logic.
2. **Data-driven missions.** A mission is a `UBVMissionData` asset plus a level.
   Adding one requires no code change.
3. **No omniscient AI.** Nothing in the AI layer may query the player's transform
   directly. Guards act only on what perception, sound or communication delivered.
4. **One source of truth per concept.** Noise goes through `UBVNoiseSubsystem`.
   Alert state lives in `UBVAlertStateComponent`. Visibility lives in
   `UBVStealthComponent`. Nothing duplicates these.

## 2. Module map

```
Source/BlackVeil/
  Core/
    BlackVeil.h/.cpp              Module entry point, log category
    BVTypes.h                     All shared enums and structs
    BVGameplayTags.h/.cpp         Native gameplay tag registry
    BlackVeilGameInstance.h/.cpp  Campaign state, save/load, scoring, unlocks
    BVGameMode.h/.cpp             Mission lifecycle, alert-level aggregation
  Player/
    BVPlayerCharacter.h/.cpp      Movement suite, input, posture, cover, traversal
    BVPlayerController.h/.cpp     HUD ownership, pause, input modes
    BVStealthComponent.h/.cpp     Visibility and noise computation
    BVInteractionComponent.h/.cpp Focus selection and prompt generation
    BVTakedownComponent.h/.cpp    Context takedowns and body handling
  AI/
    BVGuardCharacter.h/.cpp       Guard pawn, health, status effects, flashlight
    BVAIController.h/.cpp         Perception, blackboard, state arbitration
    BVAlertStateComponent.h/.cpp  Suspicion meter and alert ladder
    BVPatrolComponent.h/.cpp      Waypoint routes
    BVPatrolRoute.h/.cpp          Named route actor for level authoring
    BVSquadManager.h/.cpp         Squad comms, coordinated search, alarms
    BVBlackboardKeys.h            Shared blackboard key names
    BehaviorTree/                 BT tasks, services, decorators
  Gadgets/
    BVGadgetBase.h/.cpp           Abstract gadget component
    BVGadgetThrowables.h/.cpp     Throwable gadget components (5)
    BVGadgetTools.h/.cpp          Held tools (lockpick, hack, detector, fiber-optic)
    BVThrowableProjectile.h/.cpp  Base thrown actor
    BVThrowableEffects.h/.cpp     Noise maker, flashbang, smoke, EMP, sticky camera
    BVWeapons.h/.cpp              Weapon base + 3 weapons
    BVEquipmentInventory.h/.cpp   Slot management, loadout setup
  Mission/
    BVMissionData.h               Mission data asset + row structs
    BVMissionManager.h/.cpp       Objectives, checkpoints, scoring, spawning
    BVObjectiveVolume.h/.cpp      Objective trigger actor
    BVCheckpoint.h/.cpp           Respawn point
    BVIntelActor.h/.cpp           Collectible intelligence
  World/
    BVNoiseSubsystem.h/.cpp       Acoustic event hub with occlusion
    BVInteractableInterface.h     Interaction contract
    BVElectronicDeviceInterface.h EMP/power contract
    BVDoor.h/.cpp                 Multi-verb doors with lock types
    BVSecurityCamera.h/.cpp       Sweeping camera with light-aware detection
    BVSecurityTerminal.h/.cpp     Hack target that manipulates other systems
    BVAlarmPanel.h/.cpp           Alarm group control and sabotage
    BVDestructibleLight.h/.cpp    Switchable/shootable light fixtures
    BVHidingSpot.h/.cpp           Body and player concealment
  UI/
    BVHUDBase.h/.cpp              Widget orchestration
    BVStealthHUDWidget, BVObjectiveTrackerWidget, BVAlertIndicatorWidget,
    BVEquipmentHUDWidget, BVSubtitleWidget, BVMissionCompleteWidget,
    BVPauseMenuWidget, BVMainMenuWidget, BVMissionSelectWidget,
    BVBriefingWidget, BVEquipmentSelectWidget, BVHackMinigameWidget
    BVAudioManager.h/.cpp         Music states, surface footsteps, dialogue
  SaveGame/
    BlackVeilSaveGame.h/.cpp      Persisted campaign data
```

## 3. Key systems

### 3.1 Visibility (`UBVStealthComponent`)

Runs every 0.1 s. Collects light actors within 20 m (cache refreshed once per
second), then for each computes:

```
contribution = clamp(intensity / 3000, 0, 1.5)
             * (1 - distance / sampleRadius)
             * shadowFactor        // 1.0 clear, 0.08 blocked by geometry
             * coneFactor          // spot lights only
```

`LightExposure` is the clamped sum plus an ambient baseline. Visibility is then:

```
visibility = lightExposure * postureMultiplier * movementMultiplier
           * (leaning ? 1.15 : 1.0)
           * (inShadow ? 0.45 : 1.0)
```

Posture: prone 0.55, crouch 0.72, hanging 0.62, in-cover 0.65, standing 1.0.
Movement: prone 0.85 up to sprint 1.30.

The AI multiplies its effective sight range by `GetDetectionMultiplier()`, which
lerps 0.25 (fully shadowed) to 1.35 (fully lit, sprinting). **This is the single
mechanism that makes lighting a gameplay system rather than decoration.**

### 3.2 Noise (`UBVNoiseSubsystem`)

All noise goes through `ReportNoise(Location, Level, Instigator, bIsGunshot, Tag)`.
Base radii: silent 0, light 600, medium 1200, heavy 2200, deafening 5000 cm
(gunshots multiply by 1.5).

Hearing checks trace from listener to source and count blockers; each blocker
halves the remaining audible radius:

```
effectiveRadius = baseRadius / 2^blockers
```

This gives believable wall attenuation without a full acoustic simulation.

### 3.3 AI awareness (`BVAIController` + `UBVAlertStateComponent`)

The controller never asks where the player is. It receives:

- **Sight** via `UAIPerceptionComponent` (sight + hearing senses), filtered by
  `ValidateSight()`, which rejects detections beyond the light-adjusted range and
  rejects shadowed targets beyond a short distance.
- **Hearing** via the hearing sense, converted into `ReportNoise()`.
- **Communication** via `UBVSquadManager::BroadcastContact()`.

Suspicion rises with sightings (55 base x confidence x vigilance) and noises
(18 x loudness) and decays continuously. Escalation thresholds move the guard
through Unaware → Suspicious → Investigating → Searching → Alerted → Combat.
Timers de-escalate back through Returning → Unaware when nothing sustains the
state.

**Body discovery:** every 1.5 s a guard checks for incapacitated, unconcealed
guards within 60 % of sight range with line of sight, and reports it, which
promotes the squad to Searching and triggers a coordinated sweep.

### 3.4 Squad coordination (`UBVSquadManager`)

A `UWorldSubsystem`. Guards register by `SquadID`.

- `BroadcastContact` shares an *approximate* location and a suggested state, and
  only within a teamwork-scaled radius (2000-6000 cm).
- `CoordinateSearch` divides a circle around the contact point into even sectors
  and gives each able guard a different one, so a search spreads instead of
  stacking.
- `ReportGuardDown` escalates the squad and sweeps the body's location.
- `CallReinforcements` raises the squad alarm; mission logic listens and spawns
  the wave.

### 3.5 Mission framework (`UBVMissionManager`)

A `UWorldSubsystem` and the runtime brain of a mission:

1. `LoadMission(index)` loads `/Game/Data/Missions/DA_Mission_%02d` and seeds
   objective state.
2. `StartMission()` activates prerequisite-free objectives, calls
   `SpawnMissionEnemies()` and `ApplyPlayerLoadout()`.
3. `AddObjectiveProgress` / `CompleteObjective` drive the state machine and
   unlock dependent objectives.
4. `CompleteMission()` builds an `FMissionScore`, computes the grade, records it,
   unlocks rewards and broadcasts.
5. Checkpoints restore player transform and mission time.

Enemy spawning is difficulty-scaled: `EnemyCountMultiplier` decides how many of
the authored guards actually spawn.

### 3.6 Interaction (`IBVInteractableInterface`)

Every interactable implements: `GetInteractionPrompt`, `GetInteractionActions`,
`ExecuteInteraction`, `CanInteract`, `SetInteractionHighlight`,
`GetInteractionType`.

`UBVInteractionComponent` scans within 2.6 m and a 45-degree aim cone, filters by
line of sight, and scores candidates by aim accuracy + category weight + distance.
Category weights put Intel (100) and Alarm Panels (90) above Doors (50), so the
player rarely fights the prompt for what they meant.

Doors expose multiple verbs (`Door.OpenQuiet`, `Door.OpenNormal`, `Door.Force`,
`Door.Lockpick`, `Door.Hack`, `Door.UseKeycard`) with different noise and lock
requirements; `R` cycles the selected verb.

### 3.7 Takedowns (`UBVTakedownComponent`)

Evaluates every 0.15 s. A target qualifies when it is within 1.6 m, alive, not
already alerted, and either behind the guard (>100 degrees off their facing),
above them (>120 cm elevation), or around a corner (>60 degrees). The best
candidate wins by angle and distance. Bodies can be picked up (which forces
silent movement), carried and concealed in hiding spots.

### 3.8 Gadgets

`UBVGadgetBase` is an actor component with charges, an ID, and a virtual
`UseGadget`. Throwables subclass `UBVGadgetThrowable`, which spawns a configured
`ABVThrowableProjectile`. Effect projectiles (noise maker, flashbang, smoke, EMP,
sticky camera) implement `Detonate()`:

- **Noise maker** pulses noise every 1.6 s for 12 s so guards commit to it.
- **Flashbang** applies `Blinded` (9 s) to guards facing the burst and `Deafened`
  (6 s) to everyone in radius; blindness cuts sight range to 8 %.
- **Smoke** spawns a tagged sight blocker.
- **EMP** iterates actors implementing `IBVElectronicDeviceInterface` and calls
  `DisableFor(30)` - cameras, terminals, alarm panels and lights all respond.
- **Sticky camera** stops on impact and provides a `USceneCaptureComponent2D`
  feed plus a remote distraction noise.

`UBVEquipmentInventory` builds gadgets on demand from an ID-to-class registry, so
a loadout is just a list of names from the mission asset.

### 3.9 Audio (`UBVAudioManager`)

A `UGameInstanceSubsystem`. Music state follows the global alert level
(Calm → Tension → Combat). Footsteps are surface-mapped (`EBVSurfaceType`) and
played spatially at the player's location. Weapon cues have separate suppressed
and unsuppressed entries so the player can hear the difference between a 7 m and
a 40 m gunshot.

## 4. Behaviour tree layout

Blackboard keys are defined once in `BVBlackboardKeys.h`.

```
BT_Guard (root selector)
├── Combat branch      [decorator: IsAlertState >= 4 (Alerted)]
│   ├── Service: Update Perception
│   ├── Task: Take Cover (EQS-free cover scoring by line-of-sight break)
│   └── Task: Fire at target (via controller)
├── Search branch      [decorator: IsAlertState >= 2 (Investigating)]
│   ├── Service: Update Perception
│   ├── Task: Investigate Location
│   └── Task: Coordinated Search (ring of navmesh-projected points)
└── Patrol branch
    ├── Service: Update Perception
    └── Task: Patrol Route
```

## 5. Data flow summary

```
Level actor ──► NoiseSubsystem ──► AI Perception (hearing) ──► AlertState
Player move ──► StealthComponent ──► ValidateSight (light/posture) ──► AlertState
AlertState ──► Blackboard ──► Behaviour Tree ──► movement/combat
AlertState ──► SquadManager ──► other guards (approximate contact only)
AlertState ──► GameMode ──► HUD alert indicator + audio music state
Objective  ──► MissionManager ──► HUD tracker ──► score on completion
```

## 6. Testing hooks

- `Tools/ValidateMissionData.py` - structural validation of all 12 missions
  (objective prerequisites, route references, gadget/weapon IDs, optional-objective
  presence, checkpoint and intel minimums). Runs without the engine.
- Automated functional tests (`FAutomationTestBase`) for critical systems are
  described in `TestReport.md` section 4; they require the editor to execute.
