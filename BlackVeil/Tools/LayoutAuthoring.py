"""
Black Veil - Layout authoring helpers
=====================================

Small vocabulary for describing a blockout in code, so the 12 layouts stay
readable and consistent. Each generator script imports this and emits JSON that
BlockoutBuilder.py consumes.

Coordinate convention: X is east, Y is north, Z is up. Floor top surfaces sit at
the room's FloorZ (0 = ground level). A "corridor" is just a long narrow room.
"""

import json
import os


# ---------------------------------------------------------------- primitives

def room(rid, x, y, w, d, h=400, floorz=0.0, floor=True, ceiling=True,
         north=None, south=None, east=None, west=None):
    """A rectangular space. north/south/east/west are lists of opening() dicts.
    North is +Y, South is -Y, East is +X, West is -X."""
    return {
        "ID": rid, "X": x, "Y": y, "W": w, "D": d, "H": h, "FloorZ": floorz,
        "Floor": floor, "Ceiling": ceiling,
        "Walls": {"North": north or [], "South": south or [],
                  "East": east or [], "West": west or []},
    }


def door(center=0.0, width=280.0):
    """A full-height doorway opening in a wall."""
    return {"Center": center, "Width": width}


def window(center=0.0, width=400.0, height=200.0):
    """A high opening: shoot through it, vault through it, or see through it."""
    return {"Center": center, "Width": width, "Height": height}


def prop(kind, x, y, w, d, h, z=0.0):
    """Cover or clutter. Kinds are semantic tags, e.g. Crate, Container, Desk,
    Machine, Barrier, Shelf."""
    return {"Type": kind, "X": x, "Y": y, "Z": z, "W": w, "D": d, "H": h}


def light(x, y, z, kind="Point", intensity=3000.0, radius=1500.0, group="Default",
          cone=35.0, pitch=-90.0, yaw=0.0):
    return {"Type": kind, "X": x, "Y": y, "Z": z, "Intensity": intensity,
            "Radius": radius, "Group": group, "ConeAngle": cone,
            "Pitch": pitch, "Yaw": yaw}


def stair(x, y, z, steps=10, rise=25.0, run=35.0, width=300.0):
    return {"X": x, "Y": y, "Z": z, "Steps": steps, "Rise": rise,
            "Run": run, "Width": width}


def platform(x, y, z, w, d, h=30.0):
    return {"X": x, "Y": y, "Z": z, "W": w, "D": d, "H": h}


def layout(index, name, rooms, props=None, lights=None, stairs=None,
           platforms=None, player_start=None, bounds=None):
    data = {
        "MissionIndex": index, "Name": name,
        "Rooms": rooms, "Props": props or [], "Lights": lights or [],
        "Stairs": stairs or [], "Platforms": platforms or [],
    }
    if player_start:
        data["PlayerStart"] = player_start
    if bounds:
        data["Bounds"] = bounds
    return data


def write(data, out_dir):
    if not os.path.isdir(out_dir):
        os.makedirs(out_dir)
    path = os.path.join(out_dir, "M%02d_Layout.json" % data["MissionIndex"])
    with open(path, "w") as handle:
        json.dump(data, handle, indent=2)
    return path
