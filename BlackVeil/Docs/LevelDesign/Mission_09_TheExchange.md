# Mission 09 - The Exchange

**Location:** Marchetti Compound, Sicily  
**Map asset:** `/Game/Maps/M09`  
**Par time:** 1200 seconds  
**Blockout layout:** `Content/Data/Layouts/M09_Layout.json` (5 rooms)

**Unique mechanic:** Branching outcome: the player can sabotage the exchange (destroy the goods), expose it (photograph and leave), or ambush it (eliminate the buyer). Each choice changes the debriefing and later mission context.

## Narrative context

Zara: Marchetti is selling something to someone tonight, and we do not know what or to whom. The compound is a villa on a hill with one road in, forty private guards, and a wine cellar that is not a wine cellar. Identify the buyer, gather evidence, and then decide how to break up the party. Your call - but I need to know which one you made.

## Objectives

| ID | Description | Type | Optional | Count | Prerequisites |
|----|-------------|------|----------|-------|---------------|
| `OBJ_09_INFILTRATE` | Infiltrate the Marchetti compound | ReachLocation | No | 1 | - |
| `OBJ_09_VAULT` | Reach the underground vault | ReachLocation | No | 1 | OBJ_09_INFILTRATE |
| `OBJ_09_IDENTIFY` | Identify the buyer | Custom | No | 1 | OBJ_09_VAULT |
| `OBJ_09_EVIDENCE` | Gather the exchange evidence | Retrieve | No | 1 | OBJ_09_VAULT |
| `OBJ_09_DISRUPT` | Disrupt the exchange | Custom | No | 1 | OBJ_09_IDENTIFY |
| `OBJ_09_EXFIL` | Escape the compound | ReachLocation | No | 1 | OBJ_09_DISRUPT |
| `OPT_09_LEDGER` | Photograph Marchetti's ledger | Interact | Yes | 1 | - |
| `OPT_09_CARS` | Plant trackers on the buyer's convoy | Interact | Yes | 3 | - |

## Blockout spaces

These are the actual built rooms. Coordinates are world-space cm.

| Room | Centre (X, Y) | Size W x D | Wall height | Floor Z | Ceiling |
|------|---------------|------------|-------------|---------|---------|
| ApproachRoad | (-3700, 0) | 2400 x 1800 | 700 | 0 | open |
| GardenTerrace | (-1600, 1400) | 3000 x 2000 | 700 | 0 | open |
| Cellar | (900, -1100) | 2800 x 2200 | 380 | -500 | yes |
| VillaInterior | (2400, 700) | 3000 x 2400 | 700 | 0 | yes |
| Vault | (4100, -500) | 2000 x 1600 | 400 | 0 | yes |

**Cover props: 14** (containers, machines, shelving, hedges and desks placed to break sightlines).

**Light actors: 7** across groups: Group_A, Group_B, Group_C

**Stairs: 1 runs** providing vertical connectivity.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Standard | (-3400, 0) | Squad_A | `R_ROAD` | 0.45 |
| G2 | Standard | (-1400, 1200) | Squad_A | `R_TERRACE` | 0.50 |
| G3 | Armored | (600, -1000) | Squad_B | `R_CELLAR` | 0.55 |
| G4 | Commander | (2200, 700) | Squad_B | `R_HOUSE` | 0.75 |
| G5 | Elite | (3700, -300) | Squad_C | `R_VAULT` | 0.85 |
| G6 | Standard | (1200, 1700) | Squad_A | `R_TERRACE` | 0.45 |
| G7 | Surveillance | (3300, -1100) | Squad_C | `R_CCTV` | 0.70 |
| G8 | Elite | (-2700, -700) | Squad_B | `R_ROAD` | 0.85 |

## Patrol routes

| Route | Mode | Waypoints | Wait times | Scans | Conversations |
|-------|------|-----------|------------|-------|---------------|
| `R_ROAD` | Loop | (-3400, 0) -> (-2700, 700) -> (-2700, -700) -> (-3000, -700) | 5s, 4s, 5s, 4s | no | none |
| `R_TERRACE` | PingPong | (-1400, 1200) -> (1200, 1700) | 5s, 5s | yes | CONV_TERRACE |
| `R_CELLAR` | Loop | (600, -1000) -> (1200, -600) -> (800, -1400) | 5s, 4s, 4s | no | none |
| `R_HOUSE` | Loop | (2200, 700) -> (2800, 1100) -> (2400, 300) | 6s, 4s, 4s | yes | none |
| `R_VAULT` | OneShot | (3700, -300) -> (4200, -100) | 10s, 5s | yes | none |
| `R_CCTV` | OneShot | (3300, -1100) | 9s | yes | none |

## Lighting design

**Ambient:** Night, warm villa lighting, sea fog

- Approach road (security floodlights)
- Terrace (string lights, dark garden)
- Cellar (bare bulbs, damp)
- House interior (chandeliers, lit)
- Vault (fluorescent, no shadow)

Every light is a real actor, and `UBVStealthComponent` samples them,
so each placement changes how detectable the operative is in that space.

## Intelligence

- `M09_INTEL_A` - Exchange Terms (Document)
- `M09_INTEL_B` - Marchetti Ledger (Document)
- `M09_INTEL_C` - Broker Recording (AudioLog)

## Checkpoints

- `CP_09_ROAD` at (0, -200, -500)
- `CP_09_TERRACE` at (1500, 0, 0)
- `CP_09_CELLAR` at (3000, 0, 0)

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

Branching outcome: the player can sabotage the exchange (destroy the goods), expose it (photograph and leave), or ambush it (eliminate the buyer). Each choice changes the debriefing and later mission context.

## Difficulty modifiers

- Enemy count multiplier: 1.15
- Perception multiplier: 1.15
- Alarm escalation speed: 1.25
- Checkpoints disabled: no
