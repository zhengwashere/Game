# Mission 07 - Dead Signal

**Location:** Kestrel Relay Station, Caucasus  
**Map asset:** `/Game/Maps/M07`  
**Par time:** 1200 seconds  
**Blockout layout:** `Content/Data/Layouts/M07_Layout.json` (6 rooms)

**Unique mechanic:** Reinforcement timer: the moment the alarm sounds, a 12-minute countdown begins. The player can extend it by destroying the radio mast and can eliminate it entirely by also disabling the uplink.

## Narrative context

Leon: The station sits on a ridge above the valley. On alarm, two helicopter teams launch from the staging ground within twelve minutes - and the launch order goes out by radio, not by data. Killing the uplink buys you time, but not safety. You need to disable the uplink and the radio mast, and you need to do it before they notice you are there.

## Objectives

| ID | Description | Type | Optional | Count | Prerequisites |
|----|-------------|------|----------|-------|---------------|
| `OBJ_07_INFILTRATE` | Reach the relay station | ReachLocation | No | 1 | - |
| `OBJ_07_UPLINK` | Disable the communications uplink | Sabotage | No | 1 | OBJ_07_INFILTRATE |
| `OBJ_07_RADIO` | Destroy the reinforcement radio mast | Sabotage | No | 1 | OBJ_07_INFILTRATE |
| `OBJ_07_DATA` | Extract the relay traffic logs | Retrieve | No | 1 | OBJ_07_UPLINK |
| `OBJ_07_EXFIL` | Escape before the helicopters arrive | ReachLocation | No | 1 | OBJ_07_RADIO, OBJ_07_DATA |
| `OPT_07_TRUCK` | Steal the valley transport truck | Interact | Yes | 1 | - |
| `OPT_07_ANTENNA` | Realign the dish for a clean intercept | Interact | Yes | 1 | - |

## Blockout spaces

These are the actual built rooms. Coordinates are world-space cm.

| Room | Centre (X, Y) | Size W x D | Wall height | Floor Z | Ceiling |
|------|---------------|------------|-------------|---------|---------|
| ApproachRoad | (-4300, 0) | 2400 x 1800 | 800 | 0 | open |
| RidgePath | (-2100, 1700) | 2800 x 1400 | 800 | 300 | open |
| Helipad | (500, -1300) | 2600 x 2200 | 800 | 0 | open |
| DishPlatform | (2500, 1000) | 2200 x 1600 | 500 | 700 | yes |
| CommandBunker | (4200, -600) | 2000 x 1600 | 420 | 0 | yes |
| GeneratorShed | (-700, -2400) | 1600 x 1200 | 380 | 0 | yes |

**Cover props: 11** (containers, machines, shelving, hedges and desks placed to break sightlines).

**Light actors: 7** across groups: Group_A, Group_B, Group_C

**Stairs: 2 runs** providing vertical connectivity.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Standard | (-4000, 0) | Squad_A | `R_APPROACH` | 0.45 |
| G2 | Standard | (-1800, 1400) | Squad_A | `R_RIDGE` | 0.50 |
| G3 | Armored | (400, -1200) | Squad_B | `R_PAD` | 0.55 |
| G4 | Surveillance | (2200, 800) | Squad_B | `R_DISH` | 0.75 |
| G5 | Commander | (3800, -600) | Squad_C | `R_COMMAND` | 0.80 |
| G6 | Standard | (1600, 1600) | Squad_B | `R_PAD` | 0.45 |
| G7 | Elite | (3400, -1200) | Squad_C | `R_MAST` | 0.85 |
| G8 | Technician | (-600, -2200) | Squad_A | `R_GENERATOR` | 0.35 |

## Patrol routes

| Route | Mode | Waypoints | Wait times | Scans | Conversations |
|-------|------|-----------|------------|-------|---------------|
| `R_APPROACH` | Loop | (-4000, 0) -> (-3300, 700) -> (-3600, -700) | 5s, 4s, 4s | no | none |
| `R_RIDGE` | PingPong | (-1800, 1400) -> (-900, 1800) | 5s, 4s | yes | CONV_RIDGE |
| `R_PAD` | Loop | (400, -1200) -> (1600, 1600) -> (1600, -400) | 5s, 5s, 4s | no | none |
| `R_DISH` | OneShot | (2200, 800) -> (2600, 1200) | 10s, 5s | yes | none |
| `R_COMMAND` | Loop | (3800, -600) -> (4200, -200) | 7s, 5s | yes | none |
| `R_MAST` | OneShot | (3400, -1200) | 9s | yes | none |
| `R_GENERATOR` | PingPong | (-600, -2200) -> (200, -1800) | 6s, 4s | no | none |

## Lighting design

**Ambient:** High altitude night, wind, snow haze

- Approach road (headlights, exposed)
- Ridge path (moonlit, deep shadows)
- Helipad (floodlit, high contrast)
- Dish platform (work lights, exposed)
- Command bunker (interior, lit)
- Generator shed (unlit)

Every light is a real actor, and `UBVStealthComponent` samples them,
so each placement changes how detectable the operative is in that space.

## Intelligence

- `M07_INTEL_A` - Relay Manifest (Document)
- `M07_INTEL_B` - Reinforcement Order (Document)
- `M07_INTEL_C` - Yuki Message (AudioLog)

## Checkpoints

- `CP_07_APPROACH` at (0, -400, 0)
- `CP_07_RIDGE` at (1500, -400, 0)
- `CP_07_PAD` at (3400, 0, 0)

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

Reinforcement timer: the moment the alarm sounds, a 12-minute countdown begins. The player can extend it by destroying the radio mast and can eliminate it entirely by also disabling the uplink.

## Difficulty modifiers

- Enemy count multiplier: 1.00
- Perception multiplier: 1.10
- Alarm escalation speed: 1.30
- Checkpoints disabled: no
