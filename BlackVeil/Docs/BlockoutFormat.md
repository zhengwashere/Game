# Blockout Format Reference

Black Veil levels are built in two passes, and the split matters:

| Pass | Tool | Produces |
|------|------|----------|
| **1. Space** | `Tools/BlockoutBuilder.py` | Floors, walls, ceilings, doorways, stairs, platforms, cover, lights, NavMesh volume |
| **2. Gameplay** | `Tools/LevelSetup.py`, `SecuritySystemSetup.py`, `MissionObjectiveSetup.py` | Objective volumes, guard spawn points, patrol routes, cameras, terminals, alarm panels, intel pickups |

The layout JSON feeds pass 1. The mission data asset feeds pass 2. `ReconcileLayouts.py` keeps the two consistent.

---

## 1. Why a blockout is a real level

A stealth level does not need art to be playable. Sightlines, shadow, cover and
patrol timing are all fully readable in untextured geometry, which is how this
genre is actually built in production before the art pass. A finished blockout is
a playable mission; it is not a placeholder for one.

---

## 2. Coordinate convention

- **X** east, **Y** north, **Z** up, all in centimetres.
- A room's `FloorZ` is the **top surface** of its floor. Ground level is `0`.
- `North` = +Y wall, `South` = -Y wall, `East` = +X wall, `West` = -X wall.

---

## 3. Layout file structure

```json
{
  "MissionIndex": 1,
  "Name": "Dead Drop",
  "Rooms": [ ... ],
  "Stairs": [ ... ],
  "Platforms": [ ... ],
  "Props": [ ... ],
  "Lights": [ ... ],
  "PlayerStart": [-5200, 0, 100],
  "Bounds": { "Min": [...], "Max": [...] }
}
```

`Bounds` is optional; when absent the builder derives it from the rooms plus a margin.

---

## 4. Rooms

A room is a rectangular volume. **A corridor is just a long narrow room** - there
is only one primitive, which keeps the format small and the builder simple.

```json
{
  "ID": "RollingMill",
  "X": 900, "Y": 900,
  "W": 3200, "D": 2200,
  "H": 900,
  "FloorZ": 0,
  "Floor": true,
  "Ceiling": true,
  "Walls": {
    "North": [],
    "South": [ { "Center": 600, "Width": 280 } ],
    "East":  [ { "Center": 0,   "Width": 280 } ],
    "West":  [ { "Center": 0,   "Width": 280 } ]
  }
}
```

| Field | Meaning |
|-------|---------|
| `W` / `D` | Width along X / depth along Y |
| `H` | Wall height above `FloorZ` |
| `FloorZ` | Floor top surface. Use non-zero for upper floors and sub-levels |
| `Ceiling` | `false` for outdoor or open-to-above spaces |
| `Walls` | Openings per wall; omit a wall key for a solid wall |

### Openings

An opening is cut into the wall, and a **lintel is built above it**, so a doorway
is a framed hole rather than a missing wall segment.

| Field | Meaning |
|-------|---------|
| `Center` | Offset along the wall from the room's centre, in cm |
| `Width` | Opening width |
| `Height` | Opening height. Omit for a full-height door; set it lower for a window |

A wall with openings is built as solid segments either side, plus the lintel above.

---

## 5. Verticality

```json
"Stairs":    [ { "X": 1600, "Y": -400, "Z": 0, "Steps": 20,
                 "Rise": 25, "Run": 32, "Width": 350 } ],
"Platforms": [ { "X": 1500, "Y": 1600, "Z": 60,
                 "W": 2400, "D": 300, "H": 40 } ]
```

Stairs run in +Y unless you rotate them in-editor. For a stacked-floor level
(the tower in mission 3, the sub-aqua station in mission 8), give the upper room
a `FloorZ` equal to the lower room's `H`, and set the lower room's `Ceiling` to
`false` where you want a double-height void.

---

## 6. Cover

```json
{ "Type": "Container", "X": -3400, "Y": 700, "Z": 0,
  "W": 600, "D": 240, "H": 260 }
```

`Z` is the **bottom** of the prop, so `H` stacks work (`Z: 260` on a second
container puts it on top of the first). `Type` is a semantic tag - the builder
also tags each prop `BV_Cover` and `Cover_<Type>` so the art pass can find and
replace them by kind. Kinds in use: `Container`, `Crate`, `Barrier`, `Machine`,
`Vehicle`, `Shelf`, `Rack`, `Console`, `Desk`, `DeskCluster`, `Table`, `Sofa`,
`Hedge`, `Rock`, `Planter`, `Pedestal`, `Bench`, `Safe`, `Cage`, `Dish`.

---

## 7. Lights

```json
{ "Type": "Spot", "X": -2400, "Y": 1300, "Z": 700,
  "Intensity": 3500, "Radius": 1800, "Group": "Perimeter",
  "ConeAngle": 40, "Pitch": -90, "Yaw": 0 }
```

`Type` is `Point`, `Spot` or `Rect`. **These are not decoration.**
`UBVStealthComponent` samples real light components to compute player visibility,
so adding or removing one directly changes how detectable the operative is. Lights
are tagged `BV_Light` and `LightGroup_<Group>` so security terminals and EMP
effects can address them by group.

---

## 8. Building a level

With the target map open in the editor:

```python
import sys
sys.path.append(r"<path>/BlackVeil/Tools")
import BlockoutBuilder, LevelSetup, SecuritySystemSetup, MissionObjectiveSetup

BlockoutBuilder.build(1)        # 1. the space
LevelSetup.build_mission(1)     # 2. objectives, spawns, routes, intel, security
SecuritySystemSetup.build(1)    # 3. camera / terminal / alarm network
MissionObjectiveSetup.setup(1)  # 4. objective trigger volumes
```

Everything is tagged `BV_Blockout` or `BV_Generated`, so re-running replaces
rather than duplicates, and the art pass can find every generated actor.

### Navigation

`BlockoutBuilder` places a `NavMeshBoundsVolume` sized to the level. If it looks
wrong, select it and set the brush size in the Details panel - the Python API for
brush sizing is inconsistent across engine versions, and this is the one step
worth checking by eye. **Without navigation data the guards will not move**, which
is the most common false "the AI is broken" report on a fresh blockout.

---

## 9. Keeping data and geometry in sync

Gameplay positions live in the mission data asset; the space lives in the layout.
These drift apart easily, so two tools police them:

```bash
python3 Tools/ValidateLayouts.py        # report gameplay positions outside geometry
python3 Tools/ReconcileLayouts.py       # dry run: show the moves needed
python3 Tools/ReconcileLayouts.py --apply   # snap positions into the nearest room
```

When a guard is found in the void, the **position moves to the space**, not the
other way round - the blockout is the physical reality, and this is also what a
designer would do by hand. The reconciler prints every move it makes with the
distance, so anything that moved a long way is visible as a placement that needs
design attention rather than a mechanical nudge.

---

## 10. Adding a new mission

1. Write a layout (copy an existing generator in `Tools/_gen_layouts_*.py` as a template, or hand-write the JSON).
2. `python3 Tools/ValidateLayouts.py` - confirm no placements are outside geometry.
3. In the editor: `BlockoutBuilder.build(N)` then the three gameplay tools.
4. `python3 Tools/ValidateMissionData.py` and `python3 Tools/GenerateLevelDocs.py`.

No C++ changes are required.
