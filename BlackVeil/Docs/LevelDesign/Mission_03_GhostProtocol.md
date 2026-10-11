# Mission 03 - Ghost Protocol

**Location:** Halcyon Tower, Singapore  
**Map asset:** `/Game/Maps/M03`  
**Par time:** 1020 seconds  
**Unique mechanic:** Motion-triggered corridor lights turn on ahead of the player, revealing them - the player must either wait for them to cycle or shoot out the sensors.

## Narrative context

Zara: Halcyon Tower after hours. Fourteen floors of glass and three of them do not appear on the building plans. The documents we need are on the sub-level, encrypted, and the whole floor is watched by a camera grid we cannot see. You have two ways in: the service lift or the facade rig. I would take the rig. Nobody looks up.

## Objectives

| ID | Description | Type | Optional | Count | Prerequisites |
|----|-------------|------|----------|-------|---------------|
| `OBJ_03_INFILTRATE` | Enter Halcyon Tower | ReachLocation | No | 1 | - |
| `OBJ_03_SECURITY` | Reach the security sub-level | ReachLocation | No | 1 | OBJ_03_INFILTRATE |
| `OBJ_03_DOCS` | Steal the encrypted documents | Retrieve | No | 1 | OBJ_03_SECURITY |
| `OBJ_03_EXFIL` | Exit without triggering the alarm | ReachLocation | No | 1 | OBJ_03_DOCS |
| `OPT_03_CAMERAS` | Disable all cameras on floors 12-14 | Hack | Yes | 1 | - |
| `OPT_03_SAFE` | Crack the executive safe | Interact | Yes | 1 | - |

## Routes and verticality

Primary route: main approach, through the front security line. Fastest, most watched.
Alternate route A: service/maintenance passage bypassing the first checkpoint.
Alternate route B: elevated route (roof, gantry, catwalk, cliff) with better sightlines but less cover.
Alternate route C (where applicable): sub-level, tunnel or water route requiring a gadget or an interaction to open.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Surveillance | [-2000, 0, 0] | Squad_A | `R_LOBBY` | 0.70 |
| G2 | Standard | [-600, 1200, 0] | Squad_A | `R_ATRIUM` | 0.50 |
| G3 | Standard | [800, -900, 0] | Squad_A | `R_CORRIDOR` | 0.45 |
| G4 | Surveillance | [2200, 0, 0] | Squad_B | `R_SUBLEVEL` | 0.75 |
| G5 | Elite | [3000, 1000, 0] | Squad_B | `R_EXEC` | 0.85 |
| G6 | Standard | [1400, 1600, 0] | Squad_A | `R_SERVICE` | 0.40 |

## Patrol routes

**`R_LOBBY`** (Loop mode)
**`R_ATRIUM`** (PingPong mode)
**`R_CORRIDOR`** (Loop mode)
**`R_SUBLEVEL`** (OneShot mode)
**`R_EXEC`** (Loop mode)
**`R_SERVICE`** (PingPong mode)

| Route | Waypoints | Wait times | Scans while waiting | Conversation triggers |
|-------|-----------|------------|---------------------|-----------------------|
| `R_LOBBY` | (-2000, 0) -> (-1200, 600) -> (-1600, -600) | 5s, 3s, 4s | yes | CONV_LOBBY |
| `R_ATRIUM` | (-600, 1200) -> (400, 1600) -> (-1200, 1800) | 5s, 3s, 4s | no | none |
| `R_CORRIDOR` | (800, -900) -> (1600, -500) -> (1200, -1500) | 4s, 3s, 4s | no | none |
| `R_SUBLEVEL` | (2200, 0) -> (2600, -400) | 9s, 5s | yes | none |
| `R_EXEC` | (3000, 1000) -> (3600, 600) -> (3200, 200) | 6s, 4s, 4s | yes | CONV_EXEC |
| `R_SERVICE` | (1400, 1600) -> (1800, 1200) | 4s, 3s | no | none |

## Lighting design

**Ambient:** Night city glow through glass

- Lobby (lit, reflective marble)
- Corridors (motion-triggered strip lights)
- Sub-level (server blue, cold)
- Executive floor (warm lamps, blinds)
- Stairwells (emergency lighting only)

Lighting is gameplay, not decoration: every zone above changes how far guards can see the player.

## Intelligence

- `M03_INTEL_A` - Project Halcyon (Document)
- `M03_INTEL_B` - Board Meeting Minutes (Document)
- `M03_INTEL_C` - Zara Intercept (AudioLog)

## Checkpoints

- `CP_03_SERVICE` at [0, 0, 0] (order 0)
- `CP_03_ATRIUM` at [1500, 0, 0] (order 1)
- `CP_03_SUBLEVEL` at [3000, 0, 0] (order 2)

## Recommended approaches

**Ghost:** avoid every guard interaction, use the alternate elevated/sub-level route, kill the lights and cameras before committing to a corridor. Requires the surveillance detector and patience with patrol timing.

**Panther:** neutralise the two guards on the highest-traffic route first, conceal both bodies, then move through the vacated space. Non-lethal takedowns keep the score clean.

**Assault (fallback):** if detected, break contact toward the nearest hard cover, use smoke or a flashbang to reset the engagement, and withdraw along a route the squad has not yet searched.

## Set piece

Motion-triggered corridor lights turn on ahead of the player, revealing them - the player must either wait for them to cycle or shoot out the sensors.

## Difficulty modifiers

- Enemy count multiplier: 1.00
- Perception multiplier: 1.10
- Alarm escalation speed: 1.20
- Checkpoints disabled: no
