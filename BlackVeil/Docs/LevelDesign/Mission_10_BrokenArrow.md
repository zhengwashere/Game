# Mission 10 - Broken Arrow

**Location:** Site 14 Research Complex, Kazakhstan  
**Map asset:** `/Game/Maps/M10`  
**Par time:** 1320 seconds  
**Blockout layout:** `Content/Data/Layouts/M10_Layout.json` (6 rooms)

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

## Blockout spaces

These are the actual built rooms. Coordinates are world-space cm.

| Room | Centre (X, Y) | Size W x D | Wall height | Floor Z | Ceiling |
|------|---------------|------------|-------------|---------|---------|
| PerimeterYard | (-4000, 0) | 2400 x 1800 | 900 | 0 | open |
| VehicleHangar | (-1800, 1500) | 3200 x 2400 | 1200 | 0 | yes |
| BunkerCorridor | (700, -1200) | 3400 x 700 | 400 | 0 | yes |
| Laboratory | (2500, 900) | 3000 x 2200 | 500 | 0 | yes |
| CommandSuite | (4100, -600) | 2200 x 1800 | 420 | 0 | yes |
| PrototypeChamber | (1600, 100) | 2000 x 1800 | 800 | 0 | yes |

**Cover props: 16** (containers, machines, shelving, hedges and desks placed to break sightlines).

**Light actors: 9** across groups: Group_A, Group_B, Group_C

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Elite | (-3800, 0) | Squad_A | `R_PERIMETER` | 0.85 |
| G2 | Armored | (-1600, 1400) | Squad_A | `R_HANGAR` | 0.60 |
| G3 | Elite | (600, -1200) | Squad_B | `R_CORRIDOR` | 0.80 |
| G4 | Elite | (2400, 800) | Squad_B | `R_LAB` | 0.90 |
| G5 | Commander | (3800, -600) | Squad_C | `R_COMMAND` | 0.80 |
| G6 | Armored | (1200, 1800) | Squad_A | `R_HANGAR` | 0.60 |
| G7 | Surveillance | (3200, -1300) | Squad_C | `R_CCTV` | 0.75 |
| G8 | Elite | (-800, -1350) | Squad_A | `R_PERIMETER` | 0.85 |
| G9 | Elite | (1600, 0) | Squad_B | `R_CORRIDOR` | 0.85 |

## Patrol routes

| Route | Mode | Waypoints | Wait times | Scans | Conversations |
|-------|------|-----------|------------|-------|---------------|
| `R_PERIMETER` | Loop | (-3800, 0) -> (-3000, 700) -> (-800, -1350) -> (-3000, -700) | 6s, 4s, 5s, 4s | yes | none |
| `R_HANGAR` | PingPong | (-1600, 1400) -> (1200, 1800) | 5s, 5s | yes | CONV_HANGAR |
| `R_CORRIDOR` | Loop | (600, -1200) -> (1600, 0) -> (1000, -600) | 5s, 5s, 4s | no | none |
| `R_LAB` | OneShot | (2400, 800) -> (2800, 1200) | 10s, 5s | yes | none |
| `R_COMMAND` | Loop | (3800, -600) -> (4200, -200) | 7s, 5s | yes | none |
| `R_CCTV` | OneShot | (3200, -1300) | 9s | yes | none |

## Lighting design

**Ambient:** Bunker interior plus surface yard, harsh industrial

- Surface yard (floodlit, guard towers)
- Vehicle hangar (work lights)
- Bunker corridors (strip lights, long sightlines)
- Laboratory (bright, clinical)
- Prototype chamber (caged, spotlights)

Every light is a real actor, and `UBVStealthComponent` samples them,
so each placement changes how detectable the operative is in that space.

## Intelligence

- `M10_INTEL_A` - Site 14 Program (Document)
- `M10_INTEL_B` - Test Log (Document)
- `M10_INTEL_C` - Sorn Personal Log (AudioLog)

## Checkpoints

- `CP_10_PERIMETER` at (-400, 500, 0)
- `CP_10_HANGAR` at (1500, 0, 0)
- `CP_10_LAB` at (3000, 0, 0)

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

Prototype sabotage: the directed-energy prototype must be disabled via a three-stage process (coolant, capacitor, control core) under a hardening security response - the longer the player takes, the more guards converge.

## Difficulty modifiers

- Enemy count multiplier: 1.20
- Perception multiplier: 1.20
- Alarm escalation speed: 1.30
- Checkpoints disabled: no
