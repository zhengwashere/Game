# Mission 11 - No Witnesses

**Location:** UMBRA Operations Center, Prague  
**Map asset:** `/Game/Maps/M11`  
**Par time:** 1440 seconds  
**Unique mechanic:** Coordinated response: UMBRA guards share contacts aggressively, assign distinct search sectors, and use flanking. Combat escalation is faster here than anywhere else in the campaign.

## Narrative context

Zara: This is the building where it all comes together. Nine countries, one operations floor, and a burn protocol that wipes everything in forty minutes if they think they are compromised. There is no clean way in. There are four dirty ways. Pick one, and remember that the guards here are the best they have - they will coordinate, they will flank, and they will not walk into the same trap twice.

## Objectives

| ID | Description | Type | Optional | Count | Prerequisites |
|----|-------------|------|----------|-------|---------------|
| `OBJ_11_INFILTRATE` | Breach the operations center | ReachLocation | No | 1 | - |
| `OBJ_11_OPS` | Reach the operations floor | ReachLocation | No | 1 | OBJ_11_INFILTRATE |
| `OBJ_11_PROTOCOL` | Disable the burn protocol | Hack | No | 1 | OBJ_11_OPS |
| `OBJ_11_DIRECTORY` | Recover the operations directory | Retrieve | No | 1 | OBJ_11_PROTOCOL |
| `OBJ_11_EXFIL` | Escape before the building locks down | ReachLocation | No | 1 | OBJ_11_DIRECTORY |
| `OPT_11_KESTREL` | Locate Yuki Tanaka | Custom | Yes | 1 | - |
| `OPT_11_ARCHIVE` | Wipe the UMBRA archive server | Sabotage | Yes | 1 | - |

## Routes and verticality

Primary route: main approach, through the front security line. Fastest, most watched.
Alternate route A: service/maintenance passage bypassing the first checkpoint.
Alternate route B: elevated route (roof, gantry, catwalk, cliff) with better sightlines but less cover.
Alternate route C (where applicable): sub-level, tunnel or water route requiring a gadget or an interaction to open.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Elite | [-4200, 0, 0] | Squad_A | `R_ENTRY` | 0.90 |
| G2 | Elite | [-2200, 1400, 0] | Squad_A | `R_ATRIUM` | 0.85 |
| G3 | Armored | [-200, -1200, 0] | Squad_B | `R_GALLERY` | 0.65 |
| G4 | Elite | [1800, 800, 0] | Squad_B | `R_OPS` | 0.90 |
| G5 | Commander | [3600, -600, 0] | Squad_C | `R_COMMAND` | 0.85 |
| G6 | Elite | [1200, 2400, 0] | Squad_B | `R_GALLERY` | 0.90 |
| G7 | Surveillance | [3200, -1800, 0] | Squad_C | `R_CCTV` | 0.80 |
| G8 | Elite | [-1200, -2400, 0] | Squad_A | `R_ENTRY` | 0.90 |
| G9 | Elite | [2600, 0, 0] | Squad_B | `R_OPS` | 0.90 |
| G10 | Commander | [600, -2200, 0] | Squad_C | `R_COMMAND` | 0.85 |

## Patrol routes

**`R_ENTRY`** (Loop mode)
**`R_ATRIUM`** (PingPong mode)
**`R_GALLERY`** (Loop mode)
**`R_OPS`** (Loop mode)
**`R_COMMAND`** (Loop mode)
**`R_CCTV`** (OneShot mode)

| Route | Waypoints | Wait times | Scans while waiting | Conversation triggers |
|-------|-----------|------------|---------------------|-----------------------|
| `R_ENTRY` | (-4200, 0) -> (-3400, 800) -> (-1200, -2400) -> (-3000, -900) | 6s, 4s, 5s, 4s | yes | none |
| `R_ATRIUM` | (-2200, 1400) -> (-600, 2000) | 6s, 5s | yes | CONV_OPS_A |
| `R_GALLERY` | (-200, -1200) -> (1200, 2400) -> (800, 600) | 5s, 5s, 4s | no | none |
| `R_OPS` | (1800, 800) -> (2600, 0) -> (2000, 1600) | 6s, 5s, 4s | yes | none |
| `R_COMMAND` | (3600, -600) -> (600, -2200) | 7s, 6s | yes | none |
| `R_CCTV` | (3200, -1800) | 10s | yes | none |

## Lighting design

**Ambient:** Night, brutalist concrete interior, mixed fluorescent

- Loading entry (service lighting)
- Atrium (high windows, city glow)
- Gallery (spotlit exhibits, deep shadow between)
- Operations floor (bright, open-plan, no cover)
- Command suite (lit, glass walls)
- Server hall (cold blue)

Lighting is gameplay, not decoration: every zone above changes how far guards can see the player.

## Intelligence

- `M11_INTEL_A` - Operations Directory (Document)
- `M11_INTEL_B` - Burn Protocol (Document)
- `M11_INTEL_C` - Yuki Confession (AudioLog)

## Checkpoints

- `CP_11_ENTRY` at [0, 0, 0] (order 0)
- `CP_11_ATRIUM` at [1500, 0, 0] (order 1)
- `CP_11_GALLERY` at [3000, 0, 0] (order 2)

## Recommended approaches

**Ghost:** avoid every guard interaction, use the alternate elevated/sub-level route, kill the lights and cameras before committing to a corridor. Requires the surveillance detector and patience with patrol timing.

**Panther:** neutralise the two guards on the highest-traffic route first, conceal both bodies, then move through the vacated space. Non-lethal takedowns keep the score clean.

**Assault (fallback):** if detected, break contact toward the nearest hard cover, use smoke or a flashbang to reset the engagement, and withdraw along a route the squad has not yet searched.

## Set piece

Coordinated response: UMBRA guards share contacts aggressively, assign distinct search sectors, and use flanking. Combat escalation is faster here than anywhere else in the campaign.

## Difficulty modifiers

- Enemy count multiplier: 1.25
- Perception multiplier: 1.25
- Alarm escalation speed: 1.40
- Checkpoints disabled: no
