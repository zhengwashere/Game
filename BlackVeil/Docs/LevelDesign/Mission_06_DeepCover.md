# Mission 06 - Deep Cover

**Location:** Verdant Gala, Monaco  
**Map asset:** `/Game/Maps/M06`  
**Par time:** 900 seconds  
**Unique mechanic:** Social stealth: guards do not attack on sight. Suspicion rises if the player runs, crouches in public, or enters staff-only areas. The player must behave like a guest.

## Narrative context

Zara: This one is different. You are going in as a guest, not a ghost. Diplomatic cover, tailored suit, and a face that has been in the guest list database for three weeks. Blend, do not hide. The restricted wing is behind the ballroom. Smile, and nobody will ask what you are doing there.

## Objectives

| ID | Description | Type | Optional | Count | Prerequisites |
|----|-------------|------|----------|-------|---------------|
| `OBJ_06_INFILTRATE` | Gain entry to the gala | ReachLocation | No | 1 | - |
| `OBJ_06_BALLROOM` | Reach the ballroom undetected as suspicious | ReachLocation | No | 1 | OBJ_06_INFILTRATE |
| `OBJ_06_WING` | Enter the restricted east wing | ReachLocation | No | 1 | OBJ_06_BALLROOM |
| `OBJ_06_TARGET` | Identify the broker's contact | Custom | No | 1 | OBJ_06_WING |
| `OBJ_06_EXFIL` | Extract with the guest list | ReachLocation | No | 1 | OBJ_06_TARGET |
| `OPT_06_LISTEN` | Eavesdrop on three conversations | Custom | Yes | 3 | - |
| `OPT_06_DRESSING` | Plant a listening device in the study | Interact | Yes | 1 | - |

## Routes and verticality

Primary route: main approach, through the front security line. Fastest, most watched.
Alternate route A: service/maintenance passage bypassing the first checkpoint.
Alternate route B: elevated route (roof, gantry, catwalk, cliff) with better sightlines but less cover.
Alternate route C (where applicable): sub-level, tunnel or water route requiring a gadget or an interaction to open.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Commander | [-3000, 0, 0] | Squad_A | `R_ENTRANCE` | 0.60 |
| G2 | Elite | [-1400, 1200, 0] | Squad_A | `R_BALLROOM` | 0.70 |
| G3 | Commander | [600, -800, 0] | Squad_B | `R_WESTWING` | 0.65 |
| G4 | Elite | [2000, 600, 0] | Squad_B | `R_EASTWING` | 0.80 |
| G5 | Surveillance | [3200, 0, 0] | Squad_C | `R_STUDY` | 0.70 |
| G6 | Commander | [-2000, -1400, 0] | Squad_C | `R_TERRACE` | 0.60 |

## Patrol routes

**`R_ENTRANCE`** (Loop mode)
**`R_BALLROOM`** (PingPong mode)
**`R_WESTWING`** (Loop mode)
**`R_EASTWING`** (OneShot mode)
**`R_STUDY`** (OneShot mode)
**`R_TERRACE`** (PingPong mode)

| Route | Waypoints | Wait times | Scans while waiting | Conversation triggers |
|-------|-----------|------------|---------------------|-----------------------|
| `R_ENTRANCE` | (-3000, 0) -> (-2200, 600) -> (-2600, -600) | 6s, 4s, 4s | yes | CONV_GALA_A |
| `R_BALLROOM` | (-1400, 1200) -> (-200, 1800) | 8s, 5s | yes | CONV_GALA_B |
| `R_WESTWING` | (600, -800) -> (1200, -400) -> (800, -1200) | 4s, 4s, 4s | no | none |
| `R_EASTWING` | (2000, 600) -> (2600, 200) | 9s, 5s | yes | none |
| `R_STUDY` | (3200, 0) | 10s | yes | none |
| `R_TERRACE` | (-2000, -1400) -> (-1000, -1800) | 5s, 4s | no | none |

## Lighting design

**Ambient:** Evening, warm interior, sea terrace

- Foyer (bright, chandeliers)
- Ballroom (warm, crowded, noisy)
- Corridors (dim, sconces)
- Restricted wing (white, clinical)
- Terrace (moonlit, open)

Lighting is gameplay, not decoration: every zone above changes how far guards can see the player.

## Intelligence

- `M06_INTEL_A` - Guest List (Document)
- `M06_INTEL_B` - Ballroom Seating (Document)
- `M06_INTEL_C` - Sorn Speech Draft (AudioLog)

## Checkpoints

- `CP_06_ENTRANCE` at [0, 0, 0] (order 0)
- `CP_06_BALLROOM` at [1500, 0, 0] (order 1)
- `CP_06_EASTWING` at [3000, 0, 0] (order 2)

## Recommended approaches

**Ghost:** avoid every guard interaction, use the alternate elevated/sub-level route, kill the lights and cameras before committing to a corridor. Requires the surveillance detector and patience with patrol timing.

**Panther:** neutralise the two guards on the highest-traffic route first, conceal both bodies, then move through the vacated space. Non-lethal takedowns keep the score clean.

**Assault (fallback):** if detected, break contact toward the nearest hard cover, use smoke or a flashbang to reset the engagement, and withdraw along a route the squad has not yet searched.

## Set piece

Social stealth: guards do not attack on sight. Suspicion rises if the player runs, crouches in public, or enters staff-only areas. The player must behave like a guest.

## Difficulty modifiers

- Enemy count multiplier: 0.85
- Perception multiplier: 1.00
- Alarm escalation speed: 1.15
- Checkpoints disabled: no
