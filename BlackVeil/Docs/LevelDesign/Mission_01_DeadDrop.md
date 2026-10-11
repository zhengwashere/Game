# Mission 01 - Dead Drop

**Location:** Kostov Steelworks, Ukraine  
**Map asset:** `/Game/Maps/M01`  
**Par time:** 900 seconds  
**Blockout layout:** `Content/Data/Layouts/M01_Layout.json` (6 rooms)

**Unique mechanic:** See level design document.

## Narrative context

Zara: The informant went silent six days ago. His last transmission said the ledger was in the old rolling mill. The works shut down in 2019 but the lights are still on. Someone is paying the bills. Get in, find the ledger, get out. Do not be seen - if they know we came, the ledger becomes worthless and the informant becomes a liability.

## Objectives

| ID | Description | Type | Optional | Count | Prerequisites |
|----|-------------|------|----------|-------|---------------|
| `OBJ_01_INFILTRATE` | Infiltrate the steelworks | ReachLocation | No | 1 | - |
| `OBJ_01_LEDGER` | Recover the informant's ledger | Retrieve | No | 1 | OBJ_01_INFILTRATE |
| `OBJ_01_EXFIL` | Reach the extraction point | ReachLocation | No | 1 | OBJ_01_LEDGER |
| `OPT_01_CONTAINER` | Photograph the unlisted rail manifest | Interact | Yes | 1 | - |
| `OPT_01_NOALARM` | Complete without raising the alarm | Custom | Yes | 1 | - |

## Blockout spaces

These are the actual built rooms. Coordinates are world-space cm.

| Room | Centre (X, Y) | Size W x D | Wall height | Floor Z | Ceiling |
|------|---------------|------------|-------------|---------|---------|
| Gatehouse | (-4600, 0) | 1200 x 1200 | 380 | 0 | yes |
| PerimeterYard | (-2450, 300) | 3300 x 2800 | 500 | 0 | open |
| RollingMill | (900, 900) | 3200 x 2200 | 900 | 0 | yes |
| LoadingDock | (3000, -500) | 2000 x 1800 | 500 | 0 | yes |
| OfficeBlock | (1100, -1550) | 2200 x 1200 | 350 | 0 | yes |
| ServiceTunnel | (-300, -1750) | 2600 x 420 | 260 | 0 | yes |

**Cover props: 19** (containers, machines, shelving, hedges and desks placed to break sightlines).

**Light actors: 8** across groups: Dock, Mill, Office, Perimeter

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Standard | (-4200, 0) | Squad_A | `R_GATE` | 0.35 |
| G2 | Standard | (-1800, 900) | Squad_A | `R_YARD` | 0.45 |
| G3 | Standard | (600, 1200) | Squad_A | `R_MILL` | 0.50 |
| G4 | Standard | (2400, -600) | Squad_B | `R_LOADING` | 0.40 |
| G5 | Standard | (200, -1500) | Squad_B | `R_WEST` | 0.55 |
| G6 | Technician | (1500, 0) | Squad_A | `R_OFFICE` | 0.30 |

## Patrol routes

| Route | Mode | Waypoints | Wait times | Scans | Conversations |
|-------|------|-----------|------------|-------|---------------|
| `R_GATE` | Loop | (-4200, 0) -> (-3400, 400) -> (-3600, -400) | 4s, 3s, 3s | no | none |
| `R_YARD` | PingPong | (-1800, 900) -> (-1000, 1500) -> (-2200, 1500) | 5s, 3s, 4s | yes | CONV_YARD_A |
| `R_MILL` | Loop | (600, 1200) -> (1400, 900) -> (1200, 200) -> (400, 300) | 6s, 3s, 4s, 3s | yes | CONV_MILL |
| `R_LOADING` | Loop | (2400, -600) -> (3000, 200) -> (2200, 600) | 4s, 5s, 3s | yes | CONV_LOAD |
| `R_WEST` | PingPong | (200, -1500) -> (-1000, -900) -> (800, -1300) | 4s, 3s, 3s | no | none |
| `R_OFFICE` | OneShot | (1500, 0) -> (1600, 400) | 8s, 5s | yes | none |

## Lighting design

**Ambient:** Overcast dusk, low directional light

- Perimeter floodlights (bright, destructible)
- Yard shadows between containers (dark)
- Rolling mill interior (sparse sodium lights, flickering)
- Office block (lit, blinds)

Every light is a real actor, and `UBVStealthComponent` samples them,
so each placement changes how detectable the operative is in that space.

## Intelligence

- `M01_INTEL_A` - Burn Notice Memo (Document)
- `M01_INTEL_B` - Container Manifest (Document)
- `M01_INTEL_C` - Handler Note (AudioLog)

## Checkpoints

- `CP_01_GATE` at (-4500, 0, 0)
- `CP_01_YARD` at (-1500, 700, 0)
- `CP_01_MILL` at (900, 1000, 0)

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

See level design document.

## Difficulty modifiers

- Enemy count multiplier: 1.00
- Perception multiplier: 1.00
- Alarm escalation speed: 1.00
- Checkpoints disabled: no
