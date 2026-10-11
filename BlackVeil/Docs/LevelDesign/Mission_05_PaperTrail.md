# Mission 05 - Paper Trail

**Location:** Directorate Archive, Vienna  
**Map asset:** `/Game/Maps/M05`  
**Par time:** 1140 seconds  
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

## Routes and verticality

Primary route: main approach, through the front security line. Fastest, most watched.
Alternate route A: service/maintenance passage bypassing the first checkpoint.
Alternate route B: elevated route (roof, gantry, catwalk, cliff) with better sightlines but less cover.
Alternate route C (where applicable): sub-level, tunnel or water route requiring a gadget or an interaction to open.

## Guard deployment

| Spawn | Type | Position | Squad | Patrol route | Vigilance |
|-------|------|----------|-------|--------------|-----------|
| G1 | Elite | [-2800, 0, 0] | Squad_A | `R_ENTRY` | 0.85 |
| G2 | Elite | [-1000, 1000, 0] | Squad_A | `R_READING` | 0.80 |
| G3 | Standard | [1000, -800, 0] | Squad_B | `R_STACKS` | 0.50 |
| G4 | Elite | [2600, 600, 0] | Squad_B | `R_VAULT` | 0.90 |
| G5 | Surveillance | [3600, 0, 0] | Squad_C | `R_CCTV` | 0.75 |
| G6 | Standard | [1800, 1600, 0] | Squad_B | `R_STACKS` | 0.50 |
| G7 | Commander | [-2000, -1600, 0] | Squad_C | `R_COMMAND` | 0.80 |
| G8 | Elite | [400, 2200, 0] | Squad_A | `R_READING` | 0.85 |

## Patrol routes

**`R_ENTRY`** (Loop mode)
**`R_READING`** (PingPong mode)
**`R_STACKS`** (Loop mode)
**`R_VAULT`** (OneShot mode)
**`R_CCTV`** (OneShot mode)
**`R_COMMAND`** (Loop mode)

| Route | Waypoints | Wait times | Scans while waiting | Conversation triggers |
|-------|-----------|------------|---------------------|-----------------------|
| `R_ENTRY` | (-2800, 0) -> (-2000, 500) -> (-2400, -500) | 6s, 4s, 4s | yes | none |
| `R_READING` | (-1000, 1000) -> (400, 2200) -> (-600, 1800) | 5s, 5s, 4s | yes | CONV_READ |
| `R_STACKS` | (1000, -800) -> (1800, 1600) -> (2600, 400) | 4s, 5s, 4s | no | none |
| `R_VAULT` | (2600, 600) -> (3000, 200) | 10s, 6s | yes | none |
| `R_CCTV` | (3600, 0) | 10s | yes | none |
| `R_COMMAND` | (-2000, -1600) -> (-1200, -2000) | 7s, 4s | yes | none |

## Lighting design

**Ambient:** Night, interior climate-controlled

- Reading room (warm task lighting)
- Stacks (motion-triggered, long sightlines)
- Vault corridor (harsh white, no shadows)
- Loading bay (unlit, roller doors)
- Server room (blue LED wash)

Lighting is gameplay, not decoration: every zone above changes how far guards can see the player.

## Intelligence

- `M05_INTEL_A` - Directorate Memo (Document)
- `M05_INTEL_B` - Vienna Station File (Document)
- `M05_INTEL_C` - Analyst Note (AudioLog)

## Checkpoints

- `CP_05_ENTRY` at [0, 0, 0] (order 0)
- `CP_05_READING` at [1500, 0, 0] (order 1)
- `CP_05_STACKS` at [3000, 0, 0] (order 2)

## Recommended approaches

**Ghost:** avoid every guard interaction, use the alternate elevated/sub-level route, kill the lights and cameras before committing to a corridor. Requires the surveillance detector and patience with patrol timing.

**Panther:** neutralise the two guards on the highest-traffic route first, conceal both bodies, then move through the vacated space. Non-lethal takedowns keep the score clean.

**Assault (fallback):** if detected, break contact toward the nearest hard cover, use smoke or a flashbang to reset the engagement, and withdraw along a route the squad has not yet searched.

## Set piece

Biometric vault door: cannot be picked or hacked. The player must acquire a matching biometric from a senior archivist - either by following one to the door or by finding the enrolment terminal.

## Difficulty modifiers

- Enemy count multiplier: 1.10
- Perception multiplier: 1.15
- Alarm escalation speed: 1.25
- Checkpoints disabled: no
