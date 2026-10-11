# Mission 02 - Blackout

**Location:** Vasiliev Power Station, Georgia  
**Map asset:** `/Game/Maps/M02`  
**Par time:** 960 seconds  
**Blockout layout:** `Content/Data/Layouts/M02_Layout.json` (6 rooms)

**Unique mechanic:** See level design document.

## Narrative context

Leon: The station is running extra night shifts and nobody filed the paperwork. We think they are moving cargo through the northern corridor while the grid is down. Storm hits in ninety minutes - when the alert sounds, every non-essential system shuts off for ninety seconds. That is your window. Kill the surveillance before it comes back up, or you will be walking into a lit room.

## Objectives

| ID | Description | Type | Optional | Count | Prerequisites |
|----|-------------|------|----------|-------|---------------|
| `OBJ_02_INFILTRATE` | Enter the power station | ReachLocation | No | 1 | - |
| `OBJ_02_CONTROL` | Reach the control room | ReachLocation | No | 1 | OBJ_02_INFILTRATE |
| `OBJ_02_DISABLE` | Disable the surveillance grid | Hack | No | 1 | OBJ_02_CONTROL |
| `OBJ_02_EXFIL` | Extract with the grid data | ReachLocation | No | 1 | OBJ_02_DISABLE |
| `OPT_02_BREAKER` | Sabotage the northern breaker yard | Sabotage | Yes | 1 | - |
| `OPT_02_ROSTER` | Recover the falsified shift roster | Retrieve | Yes | 1 | - |

## Blockout spaces

These are the actual built rooms. Coordinates are world-space cm.

| Room | Centre (X, Y) | Size W x D | Wall height | Floor Z | Ceiling |
|------|---------------|------------|-------------|---------|---------|
| PerimeterYard | (-3300, 0) | 2600 x 2600 | 600 | 0 | open |
| TurbineHall | (-1200, 800) | 3400 x 2400 | 1200 | 0 | yes |
| ControlMezzanine | (500, 0) | 2000 x 1600 | 400 | 500 | yes |
| CableTunnel | (0, -1750) | 3800 x 500 | 240 | 0 | yes |
| BreakerYard | (2500, 600) | 2400 x 2000 | 600 | 0 | open |
| NorthCorridor | (1800, -900) | 2400 x 420 | 320 | 0 | yes |

**Cover props: 14** (containers, machines, shelving, hedges and desks placed to break sightlines).

**Light actors: 7** across groups: Group_A, Group_B, Group_C

**Stairs: 2 runs** providing vertical connectivity.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Standard | (-3000, 0) | Squad_A | `R_PERIMETER` | 0.40 |
| G2 | Standard | (-1200, 800) | Squad_A | `R_TURBINE` | 0.45 |
| G3 | Technician | (300, 0) | Squad_B | `R_CONTROL` | 0.35 |
| G4 | Standard | (1600, -900) | Squad_B | `R_NORTH` | 0.50 |
| G5 | Armored | (2400, 600) | Squad_C | `R_YARD` | 0.55 |
| G6 | Surveillance | (800, -1700) | Squad_B | `R_CCTV` | 0.70 |
| G7 | Standard | (-2200, -1100) | Squad_A | `R_PERIMETER` | 0.40 |

## Patrol routes

| Route | Mode | Waypoints | Wait times | Scans | Conversations |
|-------|------|-----------|------------|-------|---------------|
| `R_PERIMETER` | Loop | (-3000, 0) -> (-2200, -800) -> (-2600, -1100) -> (-3400, -700) | 4s, 3s, 4s, 3s | no | none |
| `R_TURBINE` | PingPong | (-1200, 800) -> (-300, 1200) -> (-900, 1800) | 5s, 3s, 4s | yes | CONV_TURBINE |
| `R_CONTROL` | Loop | (300, 0) -> (900, 400) -> (700, -400) | 7s, 4s, 4s | yes | none |
| `R_NORTH` | Loop | (1600, -900) -> (2600, -200) -> (2800, 300) | 4s, 3s, 5s | yes | CONV_NORTH |
| `R_YARD` | Loop | (2400, 600) -> (1800, 1000) -> (2200, 1400) | 4s, 3s, 4s | no | none |
| `R_CCTV` | OneShot | (800, -1700) | 10s | yes | none |
| `R_PERIMETER` | Loop | (-2200, -1100) -> (-1400, -1800) -> (-2600, -1100) | 4s, 3s, 4s | no | none |

## Lighting design

**Ambient:** Storm night, heavy rain, lightning flashes

- Exterior yard (floodlit, high contrast)
- Turbine hall (harsh mercury vapour, strobing under load)
- Control room (lit, glass partitions)
- Cable tunnels (near total darkness)
- Emergency red lighting activates during brownouts

Every light is a real actor, and `UBVStealthComponent` samples them,
so each placement changes how detectable the operative is in that space.

## Intelligence

- `M02_INTEL_A` - Grid Sabotage Log (Document)
- `M02_INTEL_B` - Shift Roster (Document)
- `M02_INTEL_C` - Storm Protocol (Document)

## Checkpoints

- `CP_02_GATE` at (-3200, 0, 0)
- `CP_02_TURBINE` at (-1000, 600, 0)
- `CP_02_CONTROL` at (300, -200, 0)

## Recommended approaches

**Ghost:** avoid every guard interaction, use the elevated or service
route, kill lights and cameras before committing to a corridor. Needs the
surveillance detector and patience with patrol timing.

**Panther:** neutralise the two guards on the highest-traffic route first,
conceal both bodies, then move through the vacated space. Non-lethal
takedowns keep the score clean.

**Assault (fallback):** if detected, break contact toward the nearest hard
cover, use smoke or a flashbang to reset the engagement, and withdraw along
a route the squad has not yet searched.

## Set piece

See level design document.

## Difficulty modifiers

- Enemy count multiplier: 1.00
- Perception multiplier: 1.05
- Alarm escalation speed: 1.10
- Checkpoints disabled: no
