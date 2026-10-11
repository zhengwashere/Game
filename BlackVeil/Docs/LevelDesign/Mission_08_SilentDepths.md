# Mission 08 - Silent Depths

**Location:** Anvil Deep Research Station, Arctic  
**Map asset:** `/Game/Maps/M08`  
**Par time:** 1260 seconds  
**Unique mechanic:** Flooded passages: water reduces movement speed and generates noise, and some routes require disabling the pumps to drain a compartment and open a shortcut.

## Narrative context

Zara: Anvil Deep is four pressure zones stacked under the ice. Zones three and four flooded in March and nobody filed an incident report. The central shaft connects everything, but it is sealed by two blast doors and the water is rising. Whatever they were doing down there, they did not want it found. Find it anyway.

## Objectives

| ID | Description | Type | Optional | Count | Prerequisites |
|----|-------------|------|----------|-------|---------------|
| `OBJ_08_INFILTRATE` | Enter the research station | ReachLocation | No | 1 | - |
| `OBJ_08_ZONE2` | Reach the second pressure zone | ReachLocation | No | 1 | OBJ_08_INFILTRATE |
| `OBJ_08_LAB` | Access the flooded laboratory | ReachLocation | No | 1 | OBJ_08_ZONE2 |
| `OBJ_08_SAMPLES` | Recover the specimen data | Retrieve | No | 1 | OBJ_08_LAB |
| `OBJ_08_EXFIL` | Reach the escape submersible | ReachLocation | No | 1 | OBJ_08_SAMPLES |
| `OPT_08_SURVIVOR` | Locate the surviving researcher | Custom | Yes | 1 | - |
| `OPT_08_PUMPS` | Restart the drainage pumps | Sabotage | Yes | 1 | - |

## Routes and verticality

Primary route: main approach, through the front security line. Fastest, most watched.
Alternate route A: service/maintenance passage bypassing the first checkpoint.
Alternate route B: elevated route (roof, gantry, catwalk, cliff) with better sightlines but less cover.
Alternate route C (where applicable): sub-level, tunnel or water route requiring a gadget or an interaction to open.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Standard | [-3200, 0, 0] | Squad_A | `R_AIRLOCK` | 0.45 |
| G2 | Armored | [-1200, 1200, 0] | Squad_A | `R_ZONE1` | 0.55 |
| G3 | Technician | [800, -1000, 0] | Squad_B | `R_PUMPS` | 0.35 |
| G4 | Standard | [2400, 700, 0] | Squad_B | `R_ZONE2` | 0.50 |
| G5 | Surveillance | [3600, -500, 0] | Squad_C | `R_OBSERVATION` | 0.70 |
| G6 | Elite | [1400, 2000, 0] | Squad_C | `R_ZONE3` | 0.85 |
| G7 | Standard | [-800, -2000, 0] | Squad_A | `R_ZONE1` | 0.45 |
| G8 | Commander | [3000, -1600, 0] | Squad_C | `R_COMMAND` | 0.80 |

## Patrol routes

**`R_AIRLOCK`** (Loop mode)
**`R_ZONE1`** (PingPong mode)
**`R_PUMPS`** (PingPong mode)
**`R_ZONE2`** (Loop mode)
**`R_OBSERVATION`** (OneShot mode)
**`R_ZONE3`** (OneShot mode)
**`R_COMMAND`** (Loop mode)

| Route | Waypoints | Wait times | Scans while waiting | Conversation triggers |
|-------|-----------|------------|---------------------|-----------------------|
| `R_AIRLOCK` | (-3200, 0) -> (-2400, 600) -> (-2800, -600) | 5s, 4s, 4s | yes | CONV_AIRLOCK |
| `R_ZONE1` | (-1200, 1200) -> (-800, -2000) -> (-400, 0) | 5s, 5s, 4s | no | none |
| `R_PUMPS` | (800, -1000) -> (1400, -600) | 6s, 4s | yes | none |
| `R_ZONE2` | (2400, 700) -> (3000, 1100) -> (2600, 300) | 5s, 4s, 4s | no | none |
| `R_OBSERVATION` | (3600, -500) | 10s | yes | none |
| `R_ZONE3` | (1400, 2000) -> (1800, 1600) | 9s, 5s | yes | none |
| `R_COMMAND` | (3000, -1600) -> (3400, -1200) | 7s, 4s | yes | none |

## Lighting design

**Ambient:** Sub-aqua interior, emergency lighting only in flooded zones

- Airlock (white, wet reflections)
- Zone 1 (fluorescent, powered)
- Zone 2 (intermittent, half-lit)
- Zones 3-4 (red emergency, flooded, reflective)
- Central shaft (vertical, dark)

Lighting is gameplay, not decoration: every zone above changes how far guards can see the player.

## Intelligence

- `M08_INTEL_A` - Facility Schematic (Document)
- `M08_INTEL_B` - Incident Report (Document)
- `M08_INTEL_C` - Sorn Directive (AudioLog)

## Checkpoints

- `CP_08_AIRLOCK` at [0, 0, 0] (order 0)
- `CP_08_ZONE1` at [1500, 0, 0] (order 1)
- `CP_08_ZONE2` at [3000, 0, 0] (order 2)

## Recommended approaches

**Ghost:** avoid every guard interaction, use the alternate elevated/sub-level route, kill the lights and cameras before committing to a corridor. Requires the surveillance detector and patience with patrol timing.

**Panther:** neutralise the two guards on the highest-traffic route first, conceal both bodies, then move through the vacated space. Non-lethal takedowns keep the score clean.

**Assault (fallback):** if detected, break contact toward the nearest hard cover, use smoke or a flashbang to reset the engagement, and withdraw along a route the squad has not yet searched.

## Set piece

Flooded passages: water reduces movement speed and generates noise, and some routes require disabling the pumps to drain a compartment and open a shortcut.

## Difficulty modifiers

- Enemy count multiplier: 1.00
- Perception multiplier: 1.05
- Alarm escalation speed: 1.20
- Checkpoints disabled: no
