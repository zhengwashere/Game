"""
Black Veil - Layout / Mission reconciliation
===========================================

The mission data assets (guard spawns, patrol routes, checkpoints, objectives)
and the blockout layouts were authored independently, so many gameplay
positions sat outside the built geometry. This snaps every gameplay position
into the nearest valid room and reports every move it makes.

Direction of the fix matters: the blockout is the physical space, so the
*positions* move to the space, not the other way round. That is also what a
level designer would do on opening the blockout and finding a guard in the void.

Rules:
  * A point already inside a room keeps its X/Y and is set to that room's floor Z.
  * A point outside every room is clamped into the nearest room, inset by MARGIN.
  * Room choice prefers the smallest XY distance; ties break toward the room
    whose floor level is closest to the point's authored Z.

Usage:
    python3 Tools/ReconcileLayouts.py           # dry run, prints moves
    python3 Tools/ReconcileLayouts.py --apply   # rewrite the mission JSONs
"""

import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
DATA = os.path.join(HERE, "..", "Content", "Data")
LAYOUTS = os.path.join(DATA, "Layouts")
MISSIONS = os.path.join(DATA, "Missions")

MARGIN = 200.0   # keep snapped points this far inside a room's walls


def load(path):
    with open(path) as handle:
        return json.load(handle)


def rect_distance(room, x, y):
    """Distance from (x, y) to the room rectangle: 0 when inside."""
    cx, cy = float(room["X"]), float(room["Y"])
    w, d = float(room["W"]), float(room["D"])
    dx = max(abs(x - cx) - w / 2.0, 0.0)
    dy = max(abs(y - cy) - d / 2.0, 0.0)
    return (dx * dx + dy * dy) ** 0.5


def snap_point(rooms, x, y, z):
    """Return (new_x, new_y, new_z, room_id, moved_distance) or None."""
    if not rooms:
        return None

    # Rank rooms by distance, then by floor-level proximity.
    ranked = sorted(rooms, key=lambda r: (round(rect_distance(r, x, y), 1),
                                          abs(float(r.get("FloorZ", 0.0)) - z)))
    room = ranked[0]

    cx, cy = float(room["X"]), float(room["Y"])
    w, d = float(room["W"]), float(room["D"])
    half_w = max(w / 2.0 - MARGIN, 0.0)
    half_d = max(d / 2.0 - MARGIN, 0.0)

    nx = min(max(x, cx - half_w), cx + half_w)
    ny = min(max(y, cy - half_d), cy + half_d)
    nz = float(room.get("FloorZ", 0.0))

    moved = ((nx - x) ** 2 + (ny - y) ** 2) ** 0.5
    return nx, ny, nz, room["ID"], moved


def reconcile(index, apply_changes):
    layout = load(os.path.join(LAYOUTS, "M%02d_Layout.json" % index))
    mpath = os.path.join(MISSIONS, "DA_Mission_%02d.json" % index)
    mission = load(mpath)
    rooms = layout.get("Rooms", [])

    moves = []

    def fix(loc, label):
        x, y, z = float(loc[0]), float(loc[1]), float(loc[2])
        result = snap_point(rooms, x, y, z)
        if not result:
            return loc
        nx, ny, nz, rid, moved = result
        if moved > 1.0 or abs(nz - z) > 1.0:
            moves.append((label, x, y, z, nx, ny, nz, rid, moved))
        return [round(nx, 1), round(ny, 1), round(nz, 1)]

    for e in mission.get("EnemyComposition", []):
        e["Location"] = fix(e["Location"], "guard " + e["SpawnID"])

    for r in mission.get("PatrolRoutes", []):
        for wi, wp in enumerate(r.get("Waypoints", [])):
            wp["Location"] = fix(wp["Location"], "route %s wp%d" % (r["RouteID"], wi))

    for cp in mission.get("Checkpoints", []):
        cp["Location"] = fix(cp["Location"], "checkpoint " + cp["CheckpointID"])

    for o in mission.get("Objectives", []):
        loc = o.get("WorldLocation", [0, 0, 0])
        if abs(float(loc[0])) < 1.0 and abs(float(loc[1])) < 1.0:
            continue
        o["WorldLocation"] = fix(loc, "objective " + o["ObjectiveID"])

    if apply_changes:
        with open(mpath, "w") as handle:
            json.dump(mission, handle, indent=2)

    return mission.get("MissionName", "?"), moves


def main():
    apply_changes = "--apply" in sys.argv
    total = 0
    for i in range(1, 13):
        name, moves = reconcile(i, apply_changes)
        total += len(moves)
        print("M%02d %-14s %d position(s) reconciled" % (i, name, len(moves)))
        for label, x, y, z, nx, ny, nz, rid, moved in moves[:6]:
            print("      %-22s (%.0f, %.0f, %.0f) -> (%.0f, %.0f, %.0f) in '%s'  [%.0fcm]"
                  % (label, x, y, z, nx, ny, nz, rid, moved))
        if len(moves) > 6:
            print("      ... and %d more" % (len(moves) - 6))

    print("\n%d position(s) %s." % (total, "rewritten" if apply_changes else "would be rewritten (dry run)"))
    if not apply_changes and total:
        print("Re-run with --apply to write the changes.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
