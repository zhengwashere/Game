# Mission 01 - Dead Drop

**Location:** Kostov Steelworks, Ukraine  
**Map asset:** `/Game/Maps/M01`  
**Par time:** 900 seconds  
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

## Routes and verticality

Primary route: main approach, through the front security line. Fastest, most watched.
Alternate route A: service/maintenance passage bypassing the first checkpoint.
Alternate route B: elevated route (roof, gantry, catwalk, cliff) with better sightlines but less cover.
Alternate route C (where applicable): sub-level, tunnel or water route requiring a gadget or an interaction to open.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Standard | [-4200, 0, 0] | Squad_A | `R_GATE` | 0.35 |
| G2 | Standard | [-1800, 900, 0] | Squad_A | `R_YARD` | 0.45 |
| G3 | Standard | [600, 1200, 0] | Squad_A | `R_MILL` | 0.50 |
| G4 | Standard | [2400, -600, 0] | Squad_B | `R_LOADING` | 0.40 |
| G5 | Standard | [0, -1500, 0] | Squad_B | `R_WEST` | 0.55 |
| G6 | Technician | [1500, 0, 0] | Squad_A | `R_OFFICE` | 0.30 |

## Patrol routes

**`R_GATE`** (Loop mode)
**`R_YARD`** (PingPong mode)
**`R_MILL`** (Loop mode)
**`R_LOADING`** (Loop mode)
**`R_WEST`** (PingPong mode)
**`R_OFFICE`** (OneShot mode)

| Route | Waypoints | Wait times | Scans while waiting | Conversation triggers |
|-------|-----------|------------|---------------------|-----------------------|
| `R_GATE` | (-4200, 0) -> (-3400, 400) -> (-3600, -400) | 4s, 3s, 3s | no | none |
| `R_YARD` | (-1800, 900) -> (-1000, 1500) -> (-2200, 1600) | 5s, 3s, 4s | yes | CONV_YARD_A |
| `R_MILL` | (600, 1200) -> (1400, 900) -> (1200, 200) -> (400, 300) | 6s, 3s, 4s, 3s | yes | CONV_MILL |
| `R_LOADING` | (2400, -600) -> (3000, 200) -> (2200, 600) | 4s, 5s, 3s | yes | CONV_LOAD |
| `R_WEST` | (0, -1500) -> (-800, -1200) -> (800, -1300) | 4s, 3s, 3s | no | none |
| `R_OFFICE` | (1500, 0) -> (1600, 400) | 8s, 5s | yes | none |

## Lighting design

**Ambient:** Overcast dusk, low directional light

- Perimeter floodlights (bright, destructible)
- Yard shadows between containers (dark)
- Rolling mill interior (sparse sodium lights, flickering)
- Office block (lit, blinds)

Lighting is gameplay, not decoration: every zone above changes how far guards can see the player.

## Intelligence

- `M01_INTEL_A` - Burn Notice Memo (Document)
- `M01_INTEL_B` - Container Manifest (Document)
- `M01_INTEL_C` - Handler Note (AudioLog)

## Checkpoints

- `CP_01_GATE` at [-4500, 0, 0] (order 0)
- `CP_01_YARD` at [-1500, 700, 0] (order 1)
- `CP_01_MILL` at [900, 1000, 0] (order 2)

## Recommended approaches

**Ghost:** avoid every guard interaction, use the alternate elevated/sub-level route, kill the lights and cameras before committing to a corridor. Requires the surveillance detector and patience with patrol timing.

**Panther:** neutralise the two guards on the highest-traffic route first, conceal both bodies, then move through the vacated space. Non-lethal takedowns keep the score clean.

**Assault (fallback):** if detected, break contact toward the nearest hard cover, use smoke or a flashbang to reset the engagement, and withdraw along a route the squad has not yet searched.

## Set piece

See level design document.

## Difficulty modifiers

- Enemy count multiplier: 1.00
- Perception multiplier: 1.00
- Alarm escalation speed: 1.00
- Checkpoints disabled: no
