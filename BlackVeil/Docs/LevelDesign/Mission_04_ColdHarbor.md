# Mission 04 - Cold Harbor

**Location:** Port of Kalamis, Cyprus  
**Map asset:** `/Game/Maps/M04`  
**Par time:** 1080 seconds  
**Unique mechanic:** Gantry crane set piece: the player can ride a moving container to bypass the yard, but the crane is lit and audible - timing matters.

## Narrative context

Leon: Twelve containers came through customs with a stamp from an officer who retired eight months ago. Tonight they load onto a freighter that is not on any schedule. The port is a maze of containers and gantry cranes - vertical, exposed, and full of places to be seen. Move with the shifts, not against them.

## Objectives

| ID | Description | Type | Optional | Count | Prerequisites |
|----|-------------|------|----------|-------|---------------|
| `OBJ_04_INFILTRATE` | Enter the port facility | ReachLocation | No | 1 | - |
| `OBJ_04_MANIFEST` | Identify the twelve unlisted containers | Interact | No | 12 | OBJ_04_INFILTRATE |
| `OBJ_04_OFFICE` | Access the dockmaster's office | Hack | No | 1 | OBJ_04_INFILTRATE |
| `OBJ_04_EXFIL` | Extract the cargo data | ReachLocation | No | 1 | OBJ_04_OFFICE |
| `OPT_04_CRANE` | Sabotage the loading crane | Sabotage | Yes | 1 | - |
| `OPT_04_CREW` | Eavesdrop on the dock crew | Custom | Yes | 1 | - |

## Routes and verticality

Primary route: main approach, through the front security line. Fastest, most watched.
Alternate route A: service/maintenance passage bypassing the first checkpoint.
Alternate route B: elevated route (roof, gantry, catwalk, cliff) with better sightlines but less cover.
Alternate route C (where applicable): sub-level, tunnel or water route requiring a gadget or an interaction to open.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Standard | [-3600, 0, 0] | Squad_A | `R_ENTRANCE` | 0.40 |
| G2 | Standard | [-1600, 1400, 0] | Squad_A | `R_BAY1` | 0.45 |
| G3 | Armored | [600, -1200, 0] | Squad_B | `R_BAY2` | 0.50 |
| G4 | Standard | [2400, 800, 0] | Squad_B | `R_GANTRY` | 0.50 |
| G5 | Surveillance | [3400, -600, 0] | Squad_C | `R_OFFICE` | 0.70 |
| G6 | Standard | [1200, 2000, 0] | Squad_B | `R_BAY2` | 0.45 |
| G7 | Technician | [-800, -2000, 0] | Squad_C | `R_MAINT` | 0.35 |

## Patrol routes

**`R_ENTRANCE`** (Loop mode)
**`R_BAY1`** (PingPong mode)
**`R_BAY2`** (Loop mode)
**`R_GANTRY`** (OneShot mode)
**`R_OFFICE`** (Loop mode)
**`R_MAINT`** (PingPong mode)

| Route | Waypoints | Wait times | Scans while waiting | Conversation triggers |
|-------|-----------|------------|---------------------|-----------------------|
| `R_ENTRANCE` | (-3600, 0) -> (-2800, 700) -> (-3200, -700) | 4s, 3s, 4s | no | none |
| `R_BAY1` | (-1600, 1400) -> (-400, 1800) -> (-2200, 2200) | 5s, 3s, 4s | yes | CONV_BAY |
| `R_BAY2` | (600, -1200) -> (1600, -800) -> (1200, 1800) | 4s, 3s, 4s | no | none |
| `R_GANTRY` | (2400, 800) -> (2800, 1200) -> (3200, 600) | 6s, 4s, 4s | yes | none |
| `R_OFFICE` | (3400, -600) -> (3800, -200) | 7s, 4s | yes | none |
| `R_MAINT` | (-800, -2000) -> (0, -1600) | 5s, 3s | no | none |

## Lighting design

**Ambient:** Night, sodium floodlights, sea haze

- Container yard (patchy light between stacks)
- Gantry walkways (exposed, well lit)
- Dockmaster office (interior light, blinds)
- Maintenance tunnels (unlit)
- Waterline (reflective, dark)

Lighting is gameplay, not decoration: every zone above changes how far guards can see the player.

## Intelligence

- `M04_INTEL_A` - Bill of Lading (Document)
- `M04_INTEL_B` - Crew Contract (Document)
- `M04_INTEL_C` - Vasic Order (AudioLog)

## Checkpoints

- `CP_04_GATE` at [0, 0, 0] (order 0)
- `CP_04_BAY1` at [1500, 0, 0] (order 1)
- `CP_04_GANTRY` at [3000, 0, 0] (order 2)

## Recommended approaches

**Ghost:** avoid every guard interaction, use the alternate elevated/sub-level route, kill the lights and cameras before committing to a corridor. Requires the surveillance detector and patience with patrol timing.

**Panther:** neutralise the two guards on the highest-traffic route first, conceal both bodies, then move through the vacated space. Non-lethal takedowns keep the score clean.

**Assault (fallback):** if detected, break contact toward the nearest hard cover, use smoke or a flashbang to reset the engagement, and withdraw along a route the squad has not yet searched.

## Set piece

Gantry crane set piece: the player can ride a moving container to bypass the yard, but the crane is lit and audible - timing matters.

## Difficulty modifiers

- Enemy count multiplier: 1.00
- Perception multiplier: 1.05
- Alarm escalation speed: 1.10
- Checkpoints disabled: no
