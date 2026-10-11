# Mission 10 - Broken Arrow

**Location:** Site 14 Research Complex, Kazakhstan  
**Map asset:** `/Game/Maps/M10`  
**Par time:** 1320 seconds  
**Unique mechanic:** Prototype sabotage: the directed-energy prototype must be disabled via a three-stage process (coolant, capacitor, control core) under a hardening security response - the longer the player takes, the more guards converge.

## Narrative context

Leon: Site 14 is a Soviet-era bunker with a modern coat of paint and a very modern security contract. Elite guards, motion sensors, and a research program called BROKEN ARROW that should not exist. Two prototypes were built. One is still here. Get the data, sabotage the prototype, and find out who signed the order.

## Objectives

| ID | Description | Type | Optional | Count | Prerequisites |
|----|-------------|------|----------|-------|---------------|
| `OBJ_10_INFILTRATE` | Breach Site 14 | ReachLocation | No | 1 | - |
| `OBJ_10_LAB` | Reach the research laboratory | ReachLocation | No | 1 | OBJ_10_INFILTRATE |
| `OBJ_10_DATA` | Recover the BROKEN ARROW project data | Retrieve | No | 1 | OBJ_10_LAB |
| `OBJ_10_SABOTAGE` | Sabotage the directed-energy prototype | Sabotage | No | 1 | OBJ_10_LAB |
| `OBJ_10_EXFIL` | Escape the complex | ReachLocation | No | 1 | OBJ_10_DATA, OBJ_10_SABOTAGE |
| `OPT_10_SORN` | Find Sorn's personal log | Retrieve | Yes | 1 | - |
| `OPT_10_PRISONER` | Free the detained technician | Interact | Yes | 1 | - |

## Routes and verticality

Primary route: main approach, through the front security line. Fastest, most watched.
Alternate route A: service/maintenance passage bypassing the first checkpoint.
Alternate route B: elevated route (roof, gantry, catwalk, cliff) with better sightlines but less cover.
Alternate route C (where applicable): sub-level, tunnel or water route requiring a gadget or an interaction to open.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Elite | [-3800, 0, 0] | Squad_A | `R_PERIMETER` | 0.85 |
| G2 | Armored | [-1600, 1400, 0] | Squad_A | `R_HANGAR` | 0.60 |
| G3 | Elite | [600, -1200, 0] | Squad_B | `R_CORRIDOR` | 0.80 |
| G4 | Elite | [2400, 800, 0] | Squad_B | `R_LAB` | 0.90 |
| G5 | Commander | [3800, -600, 0] | Squad_C | `R_COMMAND` | 0.80 |
| G6 | Armored | [1200, 2200, 0] | Squad_A | `R_HANGAR` | 0.60 |
| G7 | Surveillance | [3000, -1800, 0] | Squad_C | `R_CCTV` | 0.75 |
| G8 | Elite | [-800, -2200, 0] | Squad_A | `R_PERIMETER` | 0.85 |
| G9 | Elite | [1600, 0, 0] | Squad_B | `R_CORRIDOR` | 0.85 |

## Patrol routes

**`R_PERIMETER`** (Loop mode)
**`R_HANGAR`** (PingPong mode)
**`R_CORRIDOR`** (Loop mode)
**`R_LAB`** (OneShot mode)
**`R_COMMAND`** (Loop mode)
**`R_CCTV`** (OneShot mode)

| Route | Waypoints | Wait times | Scans while waiting | Conversation triggers |
|-------|-----------|------------|---------------------|-----------------------|
| `R_PERIMETER` | (-3800, 0) -> (-3000, 800) -> (-800, -2200) -> (-2600, -900) | 6s, 4s, 5s, 4s | yes | none |
| `R_HANGAR` | (-1600, 1400) -> (1200, 2200) | 5s, 5s | yes | CONV_HANGAR |
| `R_CORRIDOR` | (600, -1200) -> (1600, 0) -> (1000, -800) | 5s, 5s, 4s | no | none |
| `R_LAB` | (2400, 800) -> (2800, 1200) | 10s, 5s | yes | none |
| `R_COMMAND` | (3800, -600) -> (4200, -200) | 7s, 5s | yes | none |
| `R_CCTV` | (3000, -1800) | 9s | yes | none |

## Lighting design

**Ambient:** Bunker interior plus surface yard, harsh industrial

- Surface yard (floodlit, guard towers)
- Vehicle hangar (work lights)
- Bunker corridors (strip lights, long sightlines)
- Laboratory (bright, clinical)
- Prototype chamber (caged, spotlights)

Lighting is gameplay, not decoration: every zone above changes how far guards can see the player.

## Intelligence

- `M10_INTEL_A` - Site 14 Program (Document)
- `M10_INTEL_B` - Test Log (Document)
- `M10_INTEL_C` - Sorn Personal Log (AudioLog)

## Checkpoints

- `CP_10_PERIMETER` at [0, 0, 0] (order 0)
- `CP_10_HANGAR` at [1500, 0, 0] (order 1)
- `CP_10_LAB` at [3000, 0, 0] (order 2)

## Recommended approaches

**Ghost:** avoid every guard interaction, use the alternate elevated/sub-level route, kill the lights and cameras before committing to a corridor. Requires the surveillance detector and patience with patrol timing.

**Panther:** neutralise the two guards on the highest-traffic route first, conceal both bodies, then move through the vacated space. Non-lethal takedowns keep the score clean.

**Assault (fallback):** if detected, break contact toward the nearest hard cover, use smoke or a flashbang to reset the engagement, and withdraw along a route the squad has not yet searched.

## Set piece

Prototype sabotage: the directed-energy prototype must be disabled via a three-stage process (coolant, capacitor, control core) under a hardening security response - the longer the player takes, the more guards converge.

## Difficulty modifiers

- Enemy count multiplier: 1.20
- Perception multiplier: 1.20
- Alarm escalation speed: 1.30
- Checkpoints disabled: no
