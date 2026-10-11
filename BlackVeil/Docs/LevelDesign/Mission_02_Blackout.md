# Mission 02 - Blackout

**Location:** Vasiliev Power Station, Georgia  
**Map asset:** `/Game/Maps/M02`  
**Par time:** 960 seconds  
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

## Routes and verticality

Primary route: main approach, through the front security line. Fastest, most watched.
Alternate route A: service/maintenance passage bypassing the first checkpoint.
Alternate route B: elevated route (roof, gantry, catwalk, cliff) with better sightlines but less cover.
Alternate route C (where applicable): sub-level, tunnel or water route requiring a gadget or an interaction to open.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Standard | [-3000, 0, 0] | Squad_A | `R_PERIMETER` | 0.40 |
| G2 | Standard | [-1200, 800, 0] | Squad_A | `R_TURBINE` | 0.45 |
| G3 | Technician | [400, 0, 0] | Squad_B | `R_CONTROL` | 0.35 |
| G4 | Standard | [1600, -900, 0] | Squad_B | `R_NORTH` | 0.50 |
| G5 | Armored | [2400, 600, 0] | Squad_C | `R_YARD` | 0.55 |
| G6 | Surveillance | [800, -1600, 0] | Squad_B | `R_CCTV` | 0.70 |
| G7 | Standard | [-2200, -1400, 0] | Squad_A | `R_PERIMETER` | 0.40 |

## Patrol routes

**`R_PERIMETER`** (Loop mode)
**`R_TURBINE`** (PingPong mode)
**`R_CONTROL`** (Loop mode)
**`R_NORTH`** (Loop mode)
**`R_YARD`** (Loop mode)
**`R_CCTV`** (OneShot mode)
**`R_PERIMETER`** (Loop mode)

| Route | Waypoints | Wait times | Scans while waiting | Conversation triggers |
|-------|-----------|------------|---------------------|-----------------------|
| `R_PERIMETER` | (-3000, 0) -> (-2000, -800) -> (-2600, -1400) -> (-3400, -700) | 4s, 3s, 4s, 3s | no | none |
| `R_TURBINE` | (-1200, 800) -> (-300, 1200) -> (-900, 1800) | 5s, 3s, 4s | yes | CONV_TURBINE |
| `R_CONTROL` | (400, 0) -> (900, 400) -> (700, -400) | 7s, 4s, 4s | yes | none |
| `R_NORTH` | (1600, -900) -> (2600, -500) -> (2800, 300) | 4s, 3s, 5s | yes | CONV_NORTH |
| `R_YARD` | (2400, 600) -> (1800, 1000) -> (2200, 1400) | 4s, 3s, 4s | no | none |
| `R_CCTV` | (800, -1600) | 10s | yes | none |
| `R_PERIMETER` | (-2200, -1400) -> (-1400, -1800) -> (-2600, -1900) | 4s, 3s, 4s | no | none |

## Lighting design

**Ambient:** Storm night, heavy rain, lightning flashes

- Exterior yard (floodlit, high contrast)
- Turbine hall (harsh mercury vapour, strobing under load)
- Control room (lit, glass partitions)
- Cable tunnels (near total darkness)
- Emergency red lighting activates during brownouts

Lighting is gameplay, not decoration: every zone above changes how far guards can see the player.

## Intelligence

- `M02_INTEL_A` - Grid Sabotage Log (Document)
- `M02_INTEL_B` - Shift Roster (Document)
- `M02_INTEL_C` - Storm Protocol (Document)

## Checkpoints

- `CP_02_GATE` at [-3200, 0, 0] (order 0)
- `CP_02_TURBINE` at [-1000, 600, 0] (order 1)
- `CP_02_CONTROL` at [300, -200, 0] (order 2)

## Recommended approaches

**Ghost:** avoid every guard interaction, use the alternate elevated/sub-level route, kill the lights and cameras before committing to a corridor. Requires the surveillance detector and patience with patrol timing.

**Panther:** neutralise the two guards on the highest-traffic route first, conceal both bodies, then move through the vacated space. Non-lethal takedowns keep the score clean.

**Assault (fallback):** if detected, break contact toward the nearest hard cover, use smoke or a flashbang to reset the engagement, and withdraw along a route the squad has not yet searched.

## Set piece

See level design document.

## Difficulty modifiers

- Enemy count multiplier: 1.00
- Perception multiplier: 1.05
- Alarm escalation speed: 1.10
- Checkpoints disabled: no
