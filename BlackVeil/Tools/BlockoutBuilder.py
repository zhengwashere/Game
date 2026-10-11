"""
Black Veil - Blockout Builder
=============================

Builds actual, walkable level geometry (a greybox blockout) from a compact
layout description, then leaves the gameplay layer to LevelSetup.py.

This is the missing half of the pipeline. LevelSetup.py places *gameplay*
actors (objective volumes, spawn markers, cameras, intel). This script places
the *space* they live in: floors, walls, ceilings, doorways, stairs, cover,
platforms, lights and the NavMesh volume.

A stealth level does not need art to be playable. Sightlines, shadow, cover and
patrol timing are all fully readable in a blockout, which is exactly how this
genre is built in production before the art pass.

USAGE (Unreal Editor Python console, with the target map already open):

    import sys
    sys.path.append(r"<path>/BlackVeil/Tools")
    import BlockoutBuilder

    BlockoutBuilder.build(1)          # build mission 1's blockout
    BlockoutBuilder.build(1, True)    # rebuild from scratch

Then, on the same map:

    import LevelSetup, SecuritySystemSetup, MissionObjectiveSetup
    LevelSetup.build_mission(1)
    SecuritySystemSetup.build(1)
    MissionObjectiveSetup.setup(1)

The layout format is documented in Docs/BlockoutFormat.md.

Everything this script creates is tagged BV_Blockout, so a rebuild replaces
geometry instead of duplicating it, and so the art pass can find and replace it.
"""

import json
import os
import unreal

BLOCKOUT_TAG = "BV_Blockout"
LAYOUT_DIR = os.path.join(unreal.Paths.project_dir(), "Content", "Data", "Layouts")

# Engine primitive: a 100x100x100 cube centred on its origin, so world size is
# simply scale * 100.
CUBE_MESH = "/Engine/BasicShapes/Cube.Cube"
CUBE_SIZE = 100.0

WALL_THICKNESS = 20.0      # cm
FLOOR_THICKNESS = 30.0     # cm
DEFAULT_ROOM_HEIGHT = 400.0


def log(msg):
    unreal.log("[BlackVeil/Blockout] " + str(msg))


def _actor_subsystem():
    """UE5 prefers EditorActorSubsystem; fall back for older builds."""
    try:
        return unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    except Exception:
        return None


def _spawn(actor_class, location, rotation=(0.0, 0.0, 0.0)):
    eas = _actor_subsystem()
    loc = unreal.Vector(float(location[0]), float(location[1]), float(location[2]))
    rot = unreal.Rotator(float(rotation[0]), float(rotation[1]), float(rotation[2]))
    if eas:
        return eas.spawn_actor_from_class(actor_class, loc, rot)
    return unreal.EditorLevelLibrary.spawn_actor_from_class(actor_class, loc, rot)


def clear_blockout():
    """Destroy previously generated blockout geometry so rebuilds are clean."""
    removed = 0
    try:
        actors = unreal.EditorLevelLibrary.get_all_level_actors()
    except Exception:
        actors = []
    for actor in actors:
        if actor and BLOCKOUT_TAG in actor.tags:
            try:
                unreal.EditorLevelLibrary.destroy_actor(actor)
                removed += 1
            except Exception:
                pass
    log("Cleared %d blockout actors." % removed)


def make_box(center, size, label="", tags=None, mobility_static=True):
    """
    Spawn one scaled cube.

    center : (x, y, z) world centre of the box
    size   : (x, y, z) world size in cm
    """
    actor = _spawn(unreal.StaticMeshActor, center)
    if not actor:
        return None

    smc = actor.static_mesh_component
    mesh = unreal.load_asset(CUBE_MESH)
    if mesh and smc:
        smc.set_static_mesh(mesh)

    actor.set_actor_scale3d(unreal.Vector(
        float(size[0]) / CUBE_SIZE,
        float(size[1]) / CUBE_SIZE,
        float(size[2]) / CUBE_SIZE))

    if mobility_static and smc:
        try:
            smc.set_mobility(unreal.ComponentMobility.STATIC)
        except Exception:
            pass

    actor.tags = [BLOCKOUT_TAG]
    if tags:
        actor.tags += list(tags)
    if label:
        actor.set_actor_label(label)
    return actor


def build_wall(center, length, height, axis, openings, base_z, label=""):
    """
    Build a wall, cut by door/window openings.

    center  : (x, y, base_z) is the wall's mid-point on its run and its floor level
    length  : extent along the run axis
    axis    : 'X' (runs along world X) or 'Y' (runs along world Y)
    openings: list of dicts {Center, Width, Height} where Center is the offset
              from the wall midpoint along the run axis. Height defaults to the
              full wall height (a doorway); a smaller value makes a window.

    A wall with openings is built as solid segments either side of each opening,
    plus a lintel above it. That produces real doorways rather than gaps.
    """
    cx, cy, _ = center
    half = length / 2.0

    # Normalise and sort openings by position along the run.
    ops = []
    for o in openings or []:
        w = float(o.get("Width", 250.0))
        c = float(o.get("Center", 0.0))
        h = float(o.get("Height", height))
        left = max(-half, c - w / 2.0)
        right = min(half, c + w / 2.0)
        if right > left:
            ops.append((left, right, min(h, height)))
    ops.sort()

    # Solid segments between openings.
    cursor = -half
    for left, right, _h in ops:
        if left > cursor:
            seg = left - cursor
            mid = cursor + seg / 2.0
            if axis == "X":
                make_box((cx + mid, cy, base_z + height / 2.0),
                         (seg, WALL_THICKNESS, height), label=label)
            else:
                make_box((cx, cy + mid, base_z + height / 2.0),
                         (WALL_THICKNESS, seg, height), label=label)
        cursor = max(cursor, right)

    # Trailing segment after the last opening.
    if cursor < half:
        seg = half - cursor
        mid = cursor + seg / 2.0
        if axis == "X":
            make_box((cx + mid, cy, base_z + height / 2.0),
                     (seg, WALL_THICKNESS, height), label=label)
        else:
            make_box((cx, cy + mid, base_z + height / 2.0),
                     (WALL_THICKNESS, seg, height), label=label)

    # Lintels above every opening (so doorways are framed, not open-to-sky).
    for left, right, oh in ops:
        lintel_h = height - oh
        if lintel_h > 1.0:
            seg = right - left
            mid = left + seg / 2.0
            if axis == "X":
                make_box((cx + mid, cy, base_z + oh + lintel_h / 2.0),
                         (seg, WALL_THICKNESS, lintel_h), label=label + "_Lintel")
            else:
                make_box((cx, cy + mid, base_z + oh + lintel_h / 2.0),
                         (WALL_THICKNESS, seg, lintel_h), label=label + "_Lintel")


def build_room(room):
    """
    Build one rectangular room: floor, optional ceiling, and four walls with
    openings.

    Room keys:
      ID, X, Y            centre of the room on the ground plane
      W, D                width (X) and depth (Y)
      H                   wall height (default 400)
      FloorZ              floor top surface Z (default 0)
      Floor, Ceiling      booleans (default True)
      Walls               {"North"|"South"|"East"|"West": [opening, ...]}
                          North = +Y side, South = -Y side,
                          East = +X side, West = -X side
    """
    rid = room.get("ID", "Room")
    cx = float(room["X"])
    cy = float(room["Y"])
    w = float(room["W"])
    d = float(room["D"])
    h = float(room.get("H", DEFAULT_ROOM_HEIGHT))
    fz = float(room.get("FloorZ", 0.0))

    if room.get("Floor", True):
        make_box((cx, cy, fz - FLOOR_THICKNESS / 2.0),
                 (w, d, FLOOR_THICKNESS), label="Floor_" + rid)

    if room.get("Ceiling", True):
        make_box((cx, cy, fz + h + FLOOR_THICKNESS / 2.0),
                 (w, d, FLOOR_THICKNESS), label="Ceil_" + rid)

    walls = room.get("Walls", {}) or {}

    # North (+Y) and South (-Y) walls run along X.
    build_wall((cx, cy + d / 2.0, fz), w, h, "X",
               walls.get("North", []), fz, label="Wall_%s_N" % rid)
    build_wall((cx, cy - d / 2.0, fz), w, h, "X",
               walls.get("South", []), fz, label="Wall_%s_S" % rid)

    # East (+X) and West (-X) walls run along Y.
    build_wall((cx + w / 2.0, cy, fz), d, h, "Y",
               walls.get("East", []), fz, label="Wall_%s_E" % rid)
    build_wall((cx - w / 2.0, cy, fz), d, h, "Y",
               walls.get("West", []), fz, label="Wall_%s_W" % rid)

    log("Room %s (%.0fx%.0f) built." % (rid, w, d))


def build_stairs(stair):
    """A straight run of steps. Direction is +Y unless Yaw is set."""
    cx = float(stair["X"])
    cy = float(stair["Y"])
    cz = float(stair.get("Z", 0.0))
    steps = int(stair.get("Steps", 10))
    rise = float(stair.get("Rise", 25.0))
    run = float(stair.get("Run", 35.0))
    width = float(stair.get("Width", 300.0))

    for i in range(steps):
        make_box((cx, cy + i * run, cz + i * rise + rise / 2.0),
                 (width, run, rise), label="Step_%d" % i)

    log("Stairs: %d steps." % steps)


def build_props(props):
    """Cover, crates, containers, barriers. These are what make a room a
    stealth problem rather than an open floor."""
    for p in props or []:
        kind = p.get("Type", "Crate")
        make_box((float(p["X"]), float(p["Y"]), float(p.get("Z", 0.0)) + float(p["H"]) / 2.0),
                 (float(p["W"]), float(p["D"]), float(p["H"])),
                 label="Prop_%s" % kind, tags=["BV_Cover", "Cover_" + kind])


def build_lights(lights):
    """Real light actors, so the visibility system has something to read.

    A light here is not decoration: BVStealthComponent samples these actors, so
    placing one directly changes how detectable the player is in that space.
    """
    for i, lt in enumerate(lights or []):
        kind = lt.get("Type", "Point")
        loc = (float(lt["X"]), float(lt["Y"]), float(lt["Z"]))
        rot = (float(lt.get("Pitch", -90.0)), float(lt.get("Yaw", 0.0)), 0.0)

        if kind == "Spot":
            cls = unreal.SpotLight
        elif kind == "Rect":
            cls = unreal.RectLight
        else:
            cls = unreal.PointLight

        actor = _spawn(cls, loc, rot)
        if not actor:
            continue

        intensity = float(lt.get("Intensity", 3000.0))
        radius = float(lt.get("Radius", 1500.0))
        group = str(lt.get("Group", "Default"))

        try:
            comp = actor.light_component
            comp.set_intensity(intensity)
            comp.set_attenuation_radius(radius)
            comp.set_mobility(unreal.ComponentMobility.STATIC)
            if kind == "Spot":
                comp.set_outer_cone_angle(float(lt.get("ConeAngle", 35.0)))
        except Exception as exc:
            log("Light %d: could not set properties (%s)" % (i, exc))

        actor.tags = [BLOCKOUT_TAG, "BV_Light", "LightGroup_" + group]
        actor.set_actor_label("Light_%s_%d" % (group, i))

    log("%d lights placed." % len(lights or []))


def build_navmesh(bounds):
    """
    Spawn a NavMeshBoundsVolume over the whole level. Without this the guards
    have no navigation data and will stand still, which is the single most
    common 'the AI is broken' report on a fresh blockout.

    bounds: {"Min": [x, y, z], "Max": [x, y, z]}
    """
    mn = bounds["Min"]
    mx = bounds["Max"]
    cx = (mn[0] + mx[0]) / 2.0
    cy = (mn[1] + mx[1]) / 2.0
    cz = (mn[2] + mx[2]) / 2.0

    vol = _spawn(unreal.NavMeshBoundsVolume, (cx, cy, cz))
    if not vol:
        log("WARNING: could not spawn NavMeshBoundsVolume; place one manually.")
        return None

    # A brush volume is scaled by its 200x200x200 default extent.
    try:
        vol.set_actor_scale3d(unreal.Vector(
            (mx[0] - mn[0]) / 200.0,
            (mx[1] - mn[1]) / 200.0,
            (mx[2] - mn[2]) / 200.0))
    except Exception:
        pass

    vol.tags = [BLOCKOUT_TAG, "BV_NavMesh"]
    vol.set_actor_label("BV_NavMeshBounds")
    log("NavMeshBoundsVolume placed. If it looks wrong, select it and set the brush size in the Details panel.")
    return vol


def build_player_start(pos):
    actor = _spawn(unreal.PlayerStart,
                   (float(pos[0]), float(pos[1]), float(pos.get("Z", 100.0)) if isinstance(pos, dict) else float(pos[2])),
                   (0.0, float(pos.get("Yaw", 0.0)) if isinstance(pos, dict) else 0.0, 0.0))
    if actor:
        actor.tags = [BLOCKOUT_TAG, "BV_PlayerStart"]
        actor.set_actor_label("BV_PlayerStart")
    return actor


def load_layout(mission_index):
    path = os.path.join(LAYOUT_DIR, "M%02d_Layout.json" % mission_index)
    if not os.path.exists(path):
        raise RuntimeError("No layout file at " + path)
    with open(path, "r") as handle:
        return json.load(handle)


def compute_bounds(layout):
    """Derive a bounding box from the rooms plus a margin, for the NavMesh."""
    xs, ys, zs = [], [], []
    for room in layout.get("Rooms", []):
        cx, cy = float(room["X"]), float(room["Y"])
        w, d = float(room["W"]), float(room["D"])
        fz = float(room.get("FloorZ", 0.0))
        h = float(room.get("H", DEFAULT_ROOM_HEIGHT))
        xs += [cx - w / 2.0, cx + w / 2.0]
        ys += [cy - d / 2.0, cy + d / 2.0]
        zs += [fz - 200.0, fz + h + 200.0]

    for p in layout.get("Props", []):
        xs.append(float(p["X"]))
        ys.append(float(p["Y"]))

    if not xs:
        return {"Min": [-1000, -1000, -500], "Max": [1000, 1000, 1000]}

    margin = 800.0
    return {
        "Min": [min(xs) - margin, min(ys) - margin, min(zs) - 400.0],
        "Max": [max(xs) + margin, max(ys) + margin, max(zs) + 400.0],
    }


def build(mission_index, rebuild=True):
    """Build the full blockout for one mission onto the currently open level."""
    layout = load_layout(mission_index)
    log("Building blockout for mission %d: %s" % (mission_index, layout.get("Name", "?")))

    if rebuild:
        clear_blockout()

    # 1. Spaces. Rooms are the only primitive: a corridor is just a long, narrow
    #    room, which keeps the format small and the generator simple.
    for room in layout.get("Rooms", []):
        build_room(room)

    # 2. Verticality.
    for stair in layout.get("Stairs", []):
        build_stairs(stair)
    for plat in layout.get("Platforms", []):
        make_box((float(plat["X"]), float(plat["Y"]), float(plat["Z"])),
                 (float(plat["W"]), float(plat["D"]), float(plat.get("H", 30.0))),
                 label="Platform")

    # 3. Cover and clutter.
    build_props(layout.get("Props", []))

    # 4. Lighting - gameplay input, not decoration.
    build_lights(layout.get("Lights", []))

    # 5. Navigation.
    build_navmesh(layout.get("Bounds") or compute_bounds(layout))

    # 6. Player entry.
    if "PlayerStart" in layout:
        build_player_start(layout["PlayerStart"])

    try:
        unreal.EditorLevelLibrary.save_current_level()
    except Exception:
        pass

    log("Blockout for mission %d complete. %d rooms, %d props, %d lights." % (
        mission_index,
        len(layout.get("Rooms", [])),
        len(layout.get("Props", [])),
        len(layout.get("Lights", []))))

    log("Next: run LevelSetup.build_mission(%d), SecuritySystemSetup.build(%d), "
        "MissionObjectiveSetup.setup(%d) on this same map." % (mission_index, mission_index, mission_index))


if __name__ == "__main__":
    build(1)
