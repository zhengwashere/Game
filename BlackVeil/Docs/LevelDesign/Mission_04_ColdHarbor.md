# Mission 04 - Cold Harbor

**Location:** Port of Kalamis, Cyprus  
**Map asset:** `/Game/Maps/M04`  
**Par time:** 1080 seconds  
**Blockout layout:** `Content/Data/Layouts/M04_Layout.json` (6 rooms)

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

## Blockout spaces

These are the actual built rooms. Coordinates are world-space cm.

| Room | Centre (X, Y) | Size W x D | Wall height | Floor Z | Ceiling |
|------|---------------|------------|-------------|---------|---------|
| EntryGate | (-3600, 0) | 2000 x 1600 | 600 | 0 | open |
| Bay1 | (-1400, 1200) | 3600 x 2400 | 800 | 0 | open |
| Bay2 | (1200, -600) | 3600 x 2600 | 800 | 0 | open |
| GantryWalkway | (2600, 900) | 2800 x 400 | 320 | 900 | yes |
| DockmasterOffice | (3700, -500) | 1600 x 1400 | 380 | 0 | yes |
| MaintenanceTunnel | (-800, -2200) | 2600 x 420 | 250 | 0 | yes |

**Cover props: 20** (containers, machines, shelving, hedges and desks placed to break sightlines).

**Light actors: 7** across groups: Group_A, Group_B, Group_C

**Stairs: 2 runs** providing vertical connectivity.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Standard | (-3600, 0) | Squad_A | `R_ENTRANCE` | 0.40 |
| G2 | Standard | (-1600, 1400) | Squad_A | `R_BAY1` | 0.45 |
| G3 | Armored | (600, -1200) | Squad_B | `R_BAY2` | 0.50 |
| G4 | Standard | (2400, 900) | Squad_B | `R_GANTRY` | 0.50 |
| G5 | Surveillance | (3400, -600) | Squad_C | `R_OFFICE` | 0.70 |
| G6 | Standard | (200, 2000) | Squad_B | `R_BAY2` | 0.45 |
| G7 | Technician | (-800, -2190) | Squad_C | `R_MAINT` | 0.35 |

## Patrol routes

| Route | Mode | Waypoints | Wait times | Scans | Conversations |
|-------|------|-----------|------------|-------|---------------|
| `R_ENTRANCE` | Loop | (-3600, 0) -> (-2800, 600) -> (-3200, -600) | 4s, 3s, 4s | no | none |
| `R_BAY1` | PingPong | (-1600, 1400) -> (-400, 1800) -> (-2200, 2200) | 5s, 3s, 4s | yes | CONV_BAY |
| `R_BAY2` | Loop | (600, -1200) -> (1600, -800) -> (1400, 900) | 4s, 3s, 4s | no | none |
| `R_GANTRY` | OneShot | (2400, 900) -> (2800, 900) -> (3200, 900) | 6s, 4s, 4s | yes | none |
| `R_OFFICE` | Loop | (3400, -600) -> (3800, -200) | 7s, 4s | yes | none |
| `R_MAINT` | PingPong | (-800, -2190) -> (0, -1600) | 5s, 3s | no | none |

## Lighting design

**Ambient:** Night, sodium floodlights, sea haze

- Container yard (patchy light between stacks)
- Gantry walkways (exposed, well lit)
- Dockmaster office (interior light, blinds)
- Maintenance tunnels (unlit)
- Waterline (reflective, dark)

Every light is a real actor, and `UBVStealthComponent` samples them,
so each placement changes how detectable the operative is in that space.

## Intelligence

- `M04_INTEL_A` - Bill of Lading (Document)
- `M04_INTEL_B` - Crew Contract (Document)
- `M04_INTEL_C` - Vasic Order (AudioLog)

## Checkpoints

- `CP_04_GATE` at (0, 200, 0)
- `CP_04_BAY1` at (1500, 0, 0)
- `CP_04_GANTRY` at (2800, 0, 0)

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

Gantry crane set piece: the player can ride a moving container to bypass the yard, but the crane is lit and audible - timing matters.

## Difficulty modifiers

- Enemy count multiplier: 1.00
- Perception multiplier: 1.05
- Alarm escalation speed: 1.10
- Checkpoints disabled: no
