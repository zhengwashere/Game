# Mission 12 - Black Veil

**Location:** Sorn Estate, Black Sea Coast  
**Map asset:** `/Game/Maps/M12`  
**Par time:** 1500 seconds  
**Unique mechanic:** Layered finale: the vault requires a keycard and a biometric match, the estate reacts to the alarm with a full lockdown, and the confrontation with Sorn can end in elimination, capture, or exposure - each producing a different ending epilogue.

## Narrative context

Zara: This is the last one. Sorn is at the estate, the ledger is in the vault under the boathouse, and in eleven hours he transmits the first batch of names to fourteen governments. After that, the blackmail becomes permanent. You can go in through the cliffs, the marina, or the front gate. You can bring the ledger home, or you can burn it. Whatever you choose, choose it as yourself.

## Objectives

| ID | Description | Type | Optional | Count | Prerequisites |
|----|-------------|------|----------|-------|---------------|
| `OBJ_12_INFILTRATE` | Breach the Sorn estate | ReachLocation | No | 1 | - |
| `OBJ_12_VAULT` | Reach the boathouse vault | ReachLocation | No | 1 | OBJ_12_INFILTRATE |
| `OBJ_12_LEDGER` | Secure the ledger | Retrieve | No | 1 | OBJ_12_VAULT |
| `OBJ_12_SORN` | Confront Aldric Sorn | Custom | No | 1 | OBJ_12_LEDGER |
| `OBJ_12_EXFIL` | Extract from the estate | ReachLocation | No | 1 | OBJ_12_SORN |
| `OPT_12_KESTREL` | Extract Yuki Tanaka alive | Escort | Yes | 1 | - |
| `OPT_12_SERVERS` | Destroy the UMBRA transmission array | Sabotage | Yes | 1 | - |
| `OPT_12_GHOST` | Complete the operation without a single detection | Custom | Yes | 1 | - |

## Routes and verticality

Primary route: main approach, through the front security line. Fastest, most watched.
Alternate route A: service/maintenance passage bypassing the first checkpoint.
Alternate route B: elevated route (roof, gantry, catwalk, cliff) with better sightlines but less cover.
Alternate route C (where applicable): sub-level, tunnel or water route requiring a gadget or an interaction to open.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Elite | [-4600, 0, 0] | Squad_A | `R_CLIFF` | 0.90 |
| G2 | Elite | [-2400, 1600, 0] | Squad_A | `R_GARDEN` | 0.85 |
| G3 | Armored | [-400, -1400, 0] | Squad_B | `R_MARINA` | 0.65 |
| G4 | Elite | [1600, 900, 0] | Squad_B | `R_HOUSE` | 0.90 |
| G5 | Commander | [3400, -700, 0] | Squad_C | `R_COMMAND` | 0.85 |
| G6 | Elite | [1000, 2600, 0] | Squad_B | `R_GARDEN` | 0.90 |
| G7 | Surveillance | [3000, -2000, 0] | Squad_C | `R_CCTV` | 0.80 |
| G8 | Elite | [-1400, -2600, 0] | Squad_A | `R_CLIFF` | 0.90 |
| G9 | Elite | [2400, 100, 0] | Squad_B | `R_HOUSE` | 0.90 |
| G10 | Commander | [500, -2400, 0] | Squad_C | `R_MARINA` | 0.85 |
| G11 | Elite | [3800, -1400, 0] | Squad_C | `R_BOATHOUSE` | 0.95 |
| G12 | Elite | [-3200, -1200, 0] | Squad_A | `R_CLIFF` | 0.90 |

## Patrol routes

**`R_CLIFF`** (Loop mode)
**`R_GARDEN`** (PingPong mode)
**`R_MARINA`** (Loop mode)
**`R_HOUSE`** (Loop mode)
**`R_COMMAND`** (Loop mode)
**`R_CCTV`** (OneShot mode)
**`R_BOATHOUSE`** (OneShot mode)

| Route | Waypoints | Wait times | Scans while waiting | Conversation triggers |
|-------|-----------|------------|---------------------|-----------------------|
| `R_CLIFF` | (-4600, 0) -> (-3200, -1200) -> (-1400, -2600) -> (-3800, -1600) | 6s, 5s, 5s, 4s | yes | none |
| `R_GARDEN` | (-2400, 1600) -> (1000, 2600) | 6s, 5s | yes | CONV_ESTATE_A |
| `R_MARINA` | (-400, -1400) -> (500, -2400) -> (200, -600) | 5s, 5s, 4s | no | none |
| `R_HOUSE` | (1600, 900) -> (2400, 100) -> (1800, 1700) | 6s, 5s, 4s | yes | none |
| `R_COMMAND` | (3400, -700) -> (500, -2400) | 7s, 6s | yes | none |
| `R_CCTV` | (3000, -2000) | 10s | yes | none |
| `R_BOATHOUSE` | (3800, -1400) -> (4200, -1000) | 12s, 6s | yes | none |

## Lighting design

**Ambient:** Night, coastal storm, mixed architectural lighting

- Cliff path (moonlight, wind, deep shadow)
- Gardens (landscape uplighting, hedges)
- Marina (dock lights, reflective water)
- Main house (warm interior, large windows)
- Command suite (lit, glass)
- Boathouse vault (floodlit, no shadow, layered sensors)

Lighting is gameplay, not decoration: every zone above changes how far guards can see the player.

## Intelligence

- `M12_INTEL_A` - Sorn Estate Plans (Document)
- `M12_INTEL_B` - The Ledger (Document)
- `M12_INTEL_C` - Zara Final Transmission (AudioLog)

## Checkpoints

- `CP_12_CLIFF` at [0, 0, 0] (order 0)
- `CP_12_GARDEN` at [1500, 0, 0] (order 1)
- `CP_12_HOUSE` at [3000, 0, 0] (order 2)

## Recommended approaches

**Ghost:** avoid every guard interaction, use the alternate elevated/sub-level route, kill the lights and cameras before committing to a corridor. Requires the surveillance detector and patience with patrol timing.

**Panther:** neutralise the two guards on the highest-traffic route first, conceal both bodies, then move through the vacated space. Non-lethal takedowns keep the score clean.

**Assault (fallback):** if detected, break contact toward the nearest hard cover, use smoke or a flashbang to reset the engagement, and withdraw along a route the squad has not yet searched.

## Set piece

Layered finale: the vault requires a keycard and a biometric match, the estate reacts to the alarm with a full lockdown, and the confrontation with Sorn can end in elimination, capture, or exposure - each producing a different ending epilogue.

## Difficulty modifiers

- Enemy count multiplier: 1.30
- Perception multiplier: 1.30
- Alarm escalation speed: 1.50
- Checkpoints disabled: no
