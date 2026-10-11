# Mission 08 - Silent Depths

**Location:** Anvil Deep Research Station, Arctic  
**Map asset:** `/Game/Maps/M08`  
**Par time:** 1260 seconds  
**Blockout layout:** `Content/Data/Layouts/M08_Layout.json` (7 rooms)

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

## Blockout spaces

These are the actual built rooms. Coordinates are world-space cm.

| Room | Centre (X, Y) | Size W x D | Wall height | Floor Z | Ceiling |
|------|---------------|------------|-------------|---------|---------|
| Airlock | (-3400, 0) | 1800 x 1600 | 380 | 0 | yes |
| Zone1 | (-1400, 1200) | 3200 x 2200 | 420 | 0 | yes |
| Zone2 | (1100, -1000) | 3200 x 2200 | 420 | 0 | yes |
| PumpRoom | (700, -1100) | 1400 x 1200 | 320 | 0 | yes |
| Observation | (3900, -500) | 2000 x 1600 | 400 | 0 | yes |
| Zone3Flooded | (1500, 2000) | 2800 x 1800 | 400 | -60 | yes |
| CentralShaft | (-400, -2200) | 1400 x 1400 | 1600 | -400 | yes |

**Cover props: 10** (containers, machines, shelving, hedges and desks placed to break sightlines).

**Light actors: 7** across groups: Group_A, Group_B, Group_C

**Stairs: 1 runs** providing vertical connectivity.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Standard | (-3200, 0) | Squad_A | `R_AIRLOCK` | 0.45 |
| G2 | Armored | (-1200, 1200) | Squad_A | `R_ZONE1` | 0.55 |
| G3 | Technician | (800, -1000) | Squad_B | `R_PUMPS` | 0.35 |
| G4 | Standard | (2400, 1300) | Squad_B | `R_ZONE2` | 0.50 |
| G5 | Surveillance | (3600, -500) | Squad_C | `R_OBSERVATION` | 0.70 |
| G6 | Elite | (1400, 2000) | Squad_C | `R_ZONE3` | 0.85 |
| G7 | Standard | (-800, -2000) | Squad_A | `R_ZONE1` | 0.45 |
| G8 | Commander | (2500, -1600) | Squad_C | `R_COMMAND` | 0.80 |

## Patrol routes

| Route | Mode | Waypoints | Wait times | Scans | Conversations |
|-------|------|-----------|------------|-------|---------------|
| `R_AIRLOCK` | Loop | (-3200, 0) -> (-2400, 600) -> (-2800, -600) | 5s, 4s, 4s | yes | CONV_AIRLOCK |
| `R_ZONE1` | PingPong | (-1200, 1200) -> (-800, -2000) -> (-300, -100) | 5s, 5s, 4s | no | none |
| `R_PUMPS` | PingPong | (800, -1000) -> (1400, -600) | 6s, 4s | yes | none |
| `R_ZONE2` | Loop | (2400, 1300) -> (2700, 1300) -> (2500, -100) | 5s, 4s, 4s | no | none |
| `R_OBSERVATION` | OneShot | (3600, -500) | 10s | yes | none |
| `R_ZONE3` | OneShot | (1400, 2000) -> (1800, 1600) | 9s, 5s | yes | none |
| `R_COMMAND` | Loop | (2500, -1600) -> (3400, -1100) | 7s, 4s | yes | none |

## Lighting design

**Ambient:** Sub-aqua interior, emergency lighting only in flooded zones

- Airlock (white, wet reflections)
- Zone 1 (fluorescent, powered)
- Zone 2 (intermittent, half-lit)
- Zones 3-4 (red emergency, flooded, reflective)
- Central shaft (vertical, dark)

Every light is a real actor, and `UBVStealthComponent` samples them,
so each placement changes how detectable the operative is in that space.

## Intelligence

- `M08_INTEL_A` - Facility Schematic (Document)
- `M08_INTEL_B` - Incident Report (Document)
- `M08_INTEL_C` - Sorn Directive (AudioLog)

## Checkpoints

- `CP_08_AIRLOCK` at (0, -100, 0)
- `CP_08_ZONE1` at (1500, -100, 0)
- `CP_08_ZONE2` at (3100, 0, 0)

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

Flooded passages: water reduces movement speed and generates noise, and some routes require disabling the pumps to drain a compartment and open a shortcut.

## Difficulty modifiers

- Enemy count multiplier: 1.00
- Perception multiplier: 1.05
- Alarm escalation speed: 1.20
- Checkpoints disabled: no
