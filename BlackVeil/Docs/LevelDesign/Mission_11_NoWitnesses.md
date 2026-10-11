# Mission 11 - No Witnesses

**Location:** UMBRA Operations Center, Prague  
**Map asset:** `/Game/Maps/M11`  
**Par time:** 1440 seconds  
**Blockout layout:** `Content/Data/Layouts/M11_Layout.json` (6 rooms)

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

## Blockout spaces

These are the actual built rooms. Coordinates are world-space cm.

| Room | Centre (X, Y) | Size W x D | Wall height | Floor Z | Ceiling |
|------|---------------|------------|-------------|---------|---------|
| LoadingEntry | (-4300, 0) | 2400 x 1800 | 500 | 0 | yes |
| Atrium | (-2200, 1500) | 3200 x 2400 | 1400 | 0 | yes |
| Gallery | (100, -1200) | 3600 x 800 | 420 | 0 | yes |
| OpsFloor | (2100, 800) | 3600 x 2400 | 600 | 0 | yes |
| CommandSuite | (3900, -600) | 2200 x 1800 | 420 | 0 | yes |
| ServerHall | (1300, 2600) | 2600 x 1600 | 420 | 0 | yes |

**Cover props: 16** (containers, machines, shelving, hedges and desks placed to break sightlines).

**Light actors: 9** across groups: Group_A, Group_B, Group_C

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Elite | (-4200, 0) | Squad_A | `R_ENTRY` | 0.90 |
| G2 | Elite | (-2200, 1400) | Squad_A | `R_ATRIUM` | 0.85 |
| G3 | Armored | (-200, -1200) | Squad_B | `R_GALLERY` | 0.65 |
| G4 | Elite | (1800, 800) | Squad_B | `R_OPS` | 0.90 |
| G5 | Commander | (3600, -600) | Squad_C | `R_COMMAND` | 0.85 |
| G6 | Elite | (1200, 2400) | Squad_B | `R_GALLERY` | 0.90 |
| G7 | Surveillance | (3200, -1300) | Squad_C | `R_CCTV` | 0.80 |
| G8 | Elite | (-1200, -1400) | Squad_A | `R_ENTRY` | 0.90 |
| G9 | Elite | (2600, 0) | Squad_B | `R_OPS` | 0.90 |
| G10 | Commander | (600, -1400) | Squad_C | `R_COMMAND` | 0.85 |

## Patrol routes

| Route | Mode | Waypoints | Wait times | Scans | Conversations |
|-------|------|-----------|------------|-------|---------------|
| `R_ENTRY` | Loop | (-4200, 0) -> (-3400, 700) -> (-1200, -1400) -> (-3300, -700) | 6s, 4s, 5s, 4s | yes | none |
| `R_ATRIUM` | PingPong | (-2200, 1400) -> (-800, 2000) | 6s, 5s | yes | CONV_OPS_A |
| `R_GALLERY` | Loop | (-200, -1200) -> (1200, 2400) -> (800, 600) | 5s, 5s, 4s | no | none |
| `R_OPS` | Loop | (1800, 800) -> (2600, 0) -> (2000, 1600) | 6s, 5s, 4s | yes | none |
| `R_COMMAND` | Loop | (3600, -600) -> (600, -1400) | 7s, 6s | yes | none |
| `R_CCTV` | OneShot | (3200, -1300) | 10s | yes | none |

## Lighting design

**Ambient:** Night, brutalist concrete interior, mixed fluorescent

- Loading entry (service lighting)
- Atrium (high windows, city glow)
- Gallery (spotlit exhibits, deep shadow between)
- Operations floor (bright, open-plan, no cover)
- Command suite (lit, glass walls)
- Server hall (cold blue)

Every light is a real actor, and `UBVStealthComponent` samples them,
so each placement changes how detectable the operative is in that space.

## Intelligence

- `M11_INTEL_A` - Operations Directory (Document)
- `M11_INTEL_B` - Burn Protocol (Document)
- `M11_INTEL_C` - Yuki Confession (AudioLog)

## Checkpoints

- `CP_11_ENTRY` at (500, 0, 0)
- `CP_11_ATRIUM` at (1500, 0, 0)
- `CP_11_GALLERY` at (3000, 0, 0)

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

Coordinated response: UMBRA guards share contacts aggressively, assign distinct search sectors, and use flanking. Combat escalation is faster here than anywhere else in the campaign.

## Difficulty modifiers

- Enemy count multiplier: 1.25
- Perception multiplier: 1.25
- Alarm escalation speed: 1.40
- Checkpoints disabled: no
