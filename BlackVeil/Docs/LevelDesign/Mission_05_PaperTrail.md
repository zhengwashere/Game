# Mission 05 - Paper Trail

**Location:** Directorate Archive, Vienna  
**Map asset:** `/Game/Maps/M05`  
**Par time:** 1140 seconds  
**Blockout layout:** `Content/Data/Layouts/M05_Layout.json` (6 rooms)

**Unique mechanic:** Biometric vault door: cannot be picked or hacked. The player must acquire a matching biometric from a senior archivist - either by following one to the door or by finding the enrolment terminal.

## Narrative context

Zara: The archive is where the Directorate keeps what it cannot destroy. Originals only, no digital copies. Elite guards, biometric doors, and a records clerk who goes home at 2300. The file we need is from 1998 and it names everyone who built the Directorate's black budget. Get it and the whole structure becomes visible.

## Objectives

| ID | Description | Type | Optional | Count | Prerequisites |
|----|-------------|------|----------|-------|---------------|
| `OBJ_05_INFILTRATE` | Enter the archive | ReachLocation | No | 1 | - |
| `OBJ_05_ARCHIVE` | Reach the 1998 records vault | ReachLocation | No | 1 | OBJ_05_INFILTRATE |
| `OBJ_05_FILE` | Recover the black budget file | Retrieve | No | 1 | OBJ_05_ARCHIVE |
| `OBJ_05_EXFIL` | Exit through the loading bay | ReachLocation | No | 1 | OBJ_05_FILE |
| `OPT_05_KESTREL` | Find the KESTREL station file | Retrieve | Yes | 1 | - |
| `OPT_05_BIOMETRIC` | Obtain a biometric bypass | Custom | Yes | 1 | - |

## Blockout spaces

These are the actual built rooms. Coordinates are world-space cm.

| Room | Centre (X, Y) | Size W x D | Wall height | Floor Z | Ceiling |
|------|---------------|------------|-------------|---------|---------|
| EntryHall | (-3000, 0) | 2000 x 1800 | 420 | 0 | yes |
| ReadingRoom | (-800, 1200) | 3000 x 2000 | 420 | 0 | yes |
| StacksA | (1700, -900) | 3400 x 900 | 420 | 0 | yes |
| StacksB | (1700, 700) | 3400 x 900 | 420 | 0 | yes |
| VaultCorridor | (3900, 0) | 1800 x 700 | 380 | 0 | yes |
| LoadingBay | (1700, 2500) | 2600 x 1600 | 600 | 0 | yes |

**Cover props: 22** (containers, machines, shelving, hedges and desks placed to break sightlines).

**Light actors: 7** across groups: Group_A, Group_B, Group_C

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Elite | (-2800, 0) | Squad_A | `R_ENTRY` | 0.85 |
| G2 | Elite | (-1000, 1000) | Squad_A | `R_READING` | 0.80 |
| G3 | Standard | (1000, -800) | Squad_B | `R_STACKS` | 0.50 |
| G4 | Elite | (2600, 600) | Squad_B | `R_VAULT` | 0.90 |
| G5 | Surveillance | (3600, 0) | Squad_C | `R_CCTV` | 0.75 |
| G6 | Standard | (1800, 1900) | Squad_B | `R_STACKS` | 0.50 |
| G7 | Commander | (-2200, -700) | Squad_C | `R_COMMAND` | 0.80 |
| G8 | Elite | (400, 2000) | Squad_A | `R_READING` | 0.85 |

## Patrol routes

| Route | Mode | Waypoints | Wait times | Scans | Conversations |
|-------|------|-----------|------------|-------|---------------|
| `R_ENTRY` | Loop | (-2800, 0) -> (-2200, 500) -> (-2400, -500) | 6s, 4s, 4s | yes | none |
| `R_READING` | PingPong | (-1000, 1000) -> (400, 2000) -> (-600, 1800) | 5s, 5s, 4s | yes | CONV_READ |
| `R_STACKS` | Loop | (1000, -800) -> (1800, 1900) -> (2600, 450) | 4s, 5s, 4s | no | none |
| `R_VAULT` | OneShot | (2600, 600) -> (3200, 150) | 10s, 6s | yes | none |
| `R_CCTV` | OneShot | (3600, 0) | 10s | yes | none |
| `R_COMMAND` | Loop | (-2200, -700) -> (-2200, -700) | 7s, 4s | yes | none |

## Lighting design

**Ambient:** Night, interior climate-controlled

- Reading room (warm task lighting)
- Stacks (motion-triggered, long sightlines)
- Vault corridor (harsh white, no shadows)
- Loading bay (unlit, roller doors)
- Server room (blue LED wash)

Every light is a real actor, and `UBVStealthComponent` samples them,
so each placement changes how detectable the operative is in that space.

## Intelligence

- `M05_INTEL_A` - Directorate Memo (Document)
- `M05_INTEL_B` - Vienna Station File (Document)
- `M05_INTEL_C` - Analyst Note (AudioLog)

## Checkpoints

- `CP_05_ENTRY` at (0, 400, 0)
- `CP_05_READING` at (1500, 450, 0)
- `CP_05_STACKS` at (3200, 0, 0)

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

Biometric vault door: cannot be picked or hacked. The player must acquire a matching biometric from a senior archivist - either by following one to the door or by finding the enrolment terminal.

## Difficulty modifiers

- Enemy count multiplier: 1.10
- Perception multiplier: 1.15
- Alarm escalation speed: 1.25
- Checkpoints disabled: no
