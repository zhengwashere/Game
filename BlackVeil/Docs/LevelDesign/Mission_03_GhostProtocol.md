# Mission 03 - Ghost Protocol

**Location:** Halcyon Tower, Singapore  
**Map asset:** `/Game/Maps/M03`  
**Par time:** 1020 seconds  
**Blockout layout:** `Content/Data/Layouts/M03_Layout.json` (5 rooms)

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

## Blockout spaces

These are the actual built rooms. Coordinates are world-space cm.

| Room | Centre (X, Y) | Size W x D | Wall height | Floor Z | Ceiling |
|------|---------------|------------|-------------|---------|---------|
| Lobby | (0, 0) | 2600 x 2200 | 600 | 0 | yes |
| Atrium | (0, 0) | 2600 x 2200 | 1400 | 600 | open |
| CorridorRing | (0, 0) | 3400 x 3000 | 420 | 1800 | yes |
| ServerSubLevel | (0, -1900) | 2600 x 1800 | 420 | -700 | yes |
| ExecutiveFloor | (1400, 0) | 2600 x 2000 | 450 | 2400 | yes |

**Cover props: 12** (containers, machines, shelving, hedges and desks placed to break sightlines).

**Light actors: 6** across groups: Group_A, Group_B, Group_C

**Stairs: 3 runs** providing vertical connectivity.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Surveillance | (-1500, 0) | Squad_A | `R_LOBBY` | 0.70 |
| G2 | Standard | (-600, 1200) | Squad_A | `R_ATRIUM` | 0.50 |
| G3 | Standard | (800, -900) | Squad_A | `R_CORRIDOR` | 0.45 |
| G4 | Surveillance | (2200, 0) | Squad_B | `R_SUBLEVEL` | 0.75 |
| G5 | Elite | (2500, 800) | Squad_B | `R_EXEC` | 0.85 |
| G6 | Standard | (1400, 1300) | Squad_A | `R_SERVICE` | 0.40 |

## Patrol routes

| Route | Mode | Waypoints | Wait times | Scans | Conversations |
|-------|------|-----------|------------|-------|---------------|
| `R_LOBBY` | Loop | (-1500, 0) -> (-1100, 600) -> (-1500, -600) | 5s, 3s, 4s | yes | CONV_LOBBY |
| `R_ATRIUM` | PingPong | (-600, 1200) -> (400, 1300) -> (-1200, 1300) | 5s, 3s, 4s | no | none |
| `R_CORRIDOR` | Loop | (800, -900) -> (1500, -500) -> (1100, -1500) | 4s, 3s, 4s | no | none |
| `R_SUBLEVEL` | OneShot | (2200, 0) -> (2500, -400) | 9s, 5s | yes | none |
| `R_EXEC` | Loop | (2500, 800) -> (2500, 600) -> (2500, 200) | 6s, 4s, 4s | yes | CONV_EXEC |
| `R_SERVICE` | PingPong | (1400, 1300) -> (1500, 1200) | 4s, 3s | no | none |

## Lighting design

**Ambient:** Night city glow through glass

- Lobby (lit, reflective marble)
- Corridors (motion-triggered strip lights)
- Sub-level (server blue, cold)
- Executive floor (warm lamps, blinds)
- Stairwells (emergency lighting only)

Every light is a real actor, and `UBVStealthComponent` samples them,
so each placement changes how detectable the operative is in that space.

## Intelligence

- `M03_INTEL_A` - Project Halcyon (Document)
- `M03_INTEL_B` - Board Meeting Minutes (Document)
- `M03_INTEL_C` - Zara Intercept (AudioLog)

## Checkpoints

- `CP_03_SERVICE` at (0, 0, 0)
- `CP_03_ATRIUM` at (1500, 0, 1800)
- `CP_03_SUBLEVEL` at (2500, 0, 2400)

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

Motion-triggered corridor lights turn on ahead of the player, revealing them - the player must either wait for them to cycle or shoot out the sensors.

## Difficulty modifiers

- Enemy count multiplier: 1.00
- Perception multiplier: 1.10
- Alarm escalation speed: 1.20
- Checkpoints disabled: no
