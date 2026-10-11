# Mission 07 - Dead Signal

**Location:** Kestrel Relay Station, Caucasus  
**Map asset:** `/Game/Maps/M07`  
**Par time:** 1200 seconds  
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

## Routes and verticality

Primary route: main approach, through the front security line. Fastest, most watched.
Alternate route A: service/maintenance passage bypassing the first checkpoint.
Alternate route B: elevated route (roof, gantry, catwalk, cliff) with better sightlines but less cover.
Alternate route C (where applicable): sub-level, tunnel or water route requiring a gadget or an interaction to open.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Standard | [-4000, 0, 0] | Squad_A | `R_APPROACH` | 0.45 |
| G2 | Standard | [-1800, 1400, 0] | Squad_A | `R_RIDGE` | 0.50 |
| G3 | Armored | [400, -1200, 0] | Squad_B | `R_PAD` | 0.55 |
| G4 | Surveillance | [2200, 800, 0] | Squad_B | `R_DISH` | 0.75 |
| G5 | Commander | [3800, -600, 0] | Squad_C | `R_COMMAND` | 0.80 |
| G6 | Standard | [1000, 2200, 0] | Squad_B | `R_PAD` | 0.45 |
| G7 | Elite | [2800, -1800, 0] | Squad_C | `R_MAST` | 0.85 |
| G8 | Technician | [-600, -2200, 0] | Squad_A | `R_GENERATOR` | 0.35 |

## Patrol routes

**`R_APPROACH`** (Loop mode)
**`R_RIDGE`** (PingPong mode)
**`R_PAD`** (Loop mode)
**`R_DISH`** (OneShot mode)
**`R_COMMAND`** (Loop mode)
**`R_MAST`** (OneShot mode)
**`R_GENERATOR`** (PingPong mode)

| Route | Waypoints | Wait times | Scans while waiting | Conversation triggers |
|-------|-----------|------------|---------------------|-----------------------|
| `R_APPROACH` | (-4000, 0) -> (-3200, 700) -> (-3600, -700) | 5s, 4s, 4s | no | none |
| `R_RIDGE` | (-1800, 1400) -> (-600, 1800) | 5s, 4s | yes | CONV_RIDGE |
| `R_PAD` | (400, -1200) -> (1000, 2200) -> (1600, 0) | 5s, 5s, 4s | no | none |
| `R_DISH` | (2200, 800) -> (2600, 1200) | 10s, 5s | yes | none |
| `R_COMMAND` | (3800, -600) -> (4200, -200) | 7s, 5s | yes | none |
| `R_MAST` | (2800, -1800) | 9s | yes | none |
| `R_GENERATOR` | (-600, -2200) -> (200, -1800) | 6s, 4s | no | none |

## Lighting design

**Ambient:** High altitude night, wind, snow haze

- Approach road (headlights, exposed)
- Ridge path (moonlit, deep shadows)
- Helipad (floodlit, high contrast)
- Dish platform (work lights, exposed)
- Command bunker (interior, lit)
- Generator shed (unlit)

Lighting is gameplay, not decoration: every zone above changes how far guards can see the player.

## Intelligence

- `M07_INTEL_A` - Relay Manifest (Document)
- `M07_INTEL_B` - Reinforcement Order (Document)
- `M07_INTEL_C` - Yuki Message (AudioLog)

## Checkpoints

- `CP_07_APPROACH` at [0, 0, 0] (order 0)
- `CP_07_RIDGE` at [1500, 0, 0] (order 1)
- `CP_07_PAD` at [3000, 0, 0] (order 2)

## Recommended approaches

**Ghost:** avoid every guard interaction, use the alternate elevated/sub-level route, kill the lights and cameras before committing to a corridor. Requires the surveillance detector and patience with patrol timing.

**Panther:** neutralise the two guards on the highest-traffic route first, conceal both bodies, then move through the vacated space. Non-lethal takedowns keep the score clean.

**Assault (fallback):** if detected, break contact toward the nearest hard cover, use smoke or a flashbang to reset the engagement, and withdraw along a route the squad has not yet searched.

## Set piece

Reinforcement timer: the moment the alarm sounds, a 12-minute countdown begins. The player can extend it by destroying the radio mast and can eliminate it entirely by also disabling the uplink.

## Difficulty modifiers

- Enemy count multiplier: 1.00
- Perception multiplier: 1.10
- Alarm escalation speed: 1.30
- Checkpoints disabled: no
