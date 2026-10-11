"""
Black Veil - Layout / Mission cross-validator
============================================

Guards that spawn inside a wall, or float over a room that does not exist, are
the most common way a blockout looks broken. This checks every gameplay
position authored in a mission data asset against the blockout layout for the
same mission.

Checks per mission:
  * every enemy spawn sits inside some room footprint
  * every enemy spawn sits at (or very near) that room's floor level
  * every checkpoint and objective location sits inside some room
  * every enemy's patrol route waypoints sit inside some room

Run standalone, no editor required:
    python3 Tools/ValidateLayouts.py
"""

import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
DATA = os.path.join(HERE, "..", "Content", "Data")
LAYOUTS = os.path.join(DATA, "Layouts")
MISSIONS = os.path.join(DATA, "Missions")

FLOOR_TOLERANCE = 250.0   # cm: spawn must be within this of the room's floor Z
EDGE_MARGIN = 0.0         # spawn must be strictly inside the footprint


def load(path):
    with open(path) as handle:
        return json.load(handle)


def find_room(rooms, x, y, z):
    """Return the room whose footprint contains (x, y) and whose floor level is
    closest to z, or None."""
    best, best_dz = None, 1e9
    for r in rooms:
        cx, cy = float(r["X"]), float(r["Y"])
        w, d = float(r["W"]), float(r["D"])
        if not (cx - w / 2.0 - EDGE_MARGIN <= x <= cx + w / 2.0 + EDGE_MARGIN):
            continue
        if not (cy - d / 2.0 - EDGE_MARGIN <= y <= cy + d / 2.0 + EDGE_MARGIN):
            continue
        dz = abs(z - float(r.get("FloorZ", 0.0)))
        if dz < best_dz:
            best, best_dz = r, dz
    return (best, best_dz) if best else (None, None)


def check_mission(index):
    problems = []
    layout = load(os.path.join(LAYOUTS, "M%02d_Layout.json" % index))
    mission = load(os.path.join(MISSIONS, "DA_Mission_%02d.json" % index))
    rooms = layout.get("Rooms", [])

    # 1. Enemy spawns.
    for e in mission.get("EnemyComposition", []):
        loc = e.get("Location", [0, 0, 0])
        x, y, z = float(loc[0]), float(loc[1]), float(loc[2])
        room, dz = find_room(rooms, x, y, z)
        if room is None:
            problems.append("guard %s at (%.0f, %.0f) is outside every room footprint"
                            % (e["SpawnID"], x, y))
        elif dz > FLOOR_TOLERANCE:
            problems.append("guard %s at (%.0f, %.0f) is %.0fcm off the floor of room '%s'"
                            % (e["SpawnID"], x, y, dz, room["ID"]))

    # 2. Patrol waypoints.
    routes = {r["RouteID"]: r for r in mission.get("PatrolRoutes", [])}
    for e in mission.get("EnemyComposition", []):
        rid = e.get("PatrolRouteID")
        route = routes.get(rid)
        if not route:
            continue
        for wi, wp in enumerate(route.get("Waypoints", [])):
            loc = wp["Location"]
            x, y, z = float(loc[0]), float(loc[1]), float(loc[2])
            room, dz = find_room(rooms, x, y, z)
            if room is None:
                problems.append("route %s waypoint %d at (%.0f, %.0f) is outside every room"
                                % (rid, wi, x, y))
            elif dz > FLOOR_TOLERANCE:
                problems.append("route %s waypoint %d is %.0fcm off the floor of '%s'"
                                % (rid, wi, dz, room["ID"]))

    # 3. Checkpoints.
    for cp in mission.get("Checkpoints", []):
        loc = cp["Location"]
        x, y, z = float(loc[0]), float(loc[1]), float(loc[2])
        room, _ = find_room(rooms, x, y, z)
        if room is None:
            problems.append("checkpoint %s at (%.0f, %.0f) is outside every room"
                            % (cp["CheckpointID"], x, y))

    # 4. Objectives with a world location.
    for o in mission.get("Objectives", []):
        loc = o.get("WorldLocation", [0, 0, 0])
        if abs(float(loc[0])) < 1.0 and abs(float(loc[1])) < 1.0:
            continue   # unplaced in the data asset; designers place it in-editor
        x, y, z = float(loc[0]), float(loc[1]), float(loc[2])
        room, _ = find_room(rooms, x, y, z)
        if room is None:
            problems.append("objective %s at (%.0f, %.0f) is outside every room"
                            % (o["ObjectiveID"], x, y))

    return layout.get("Name", "?"), rooms, problems


def main():
    total_problems = 0
    for i in range(1, 13):
        name, rooms, problems = check_mission(i)
        if problems:
            total_problems += len(problems)
            print("M%02d %-14s %d rooms, %d PROBLEM(S)" % (i, name, len(rooms), len(problems)))
            for p in problems[:12]:
                print("      - " + p)
            if len(problems) > 12:
                print("      ... and %d more" % (len(problems) - 12))
        else:
            print("M%02d %-14s %d rooms, OK" % (i, name, len(rooms)))

    print("\n%d placement problem(s) across 12 missions." % total_problems)
    return 1 if total_problems else 0


if __name__ == "__main__":
    sys.exit(main())
