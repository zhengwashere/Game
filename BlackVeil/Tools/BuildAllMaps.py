"""
Black Veil - Build all campaign maps
====================================

Creates the 14 levels the game expects and builds each mission map end to end:
geometry, then gameplay, then security, then objectives.

This is the one-command answer to "how do I make a level per map". Run it once
and you have 12 playable blockout missions plus the front-end maps.

USAGE (Unreal Editor Python console):

    import sys
    sys.path.append(r"<path>/BlackVeil/Tools")
    import BuildAllMaps

    BuildAllMaps.build_all()        # M01..M12, end to end
    BuildAllMaps.build_one(3)       # just mission 3
    BuildAllMaps.build_frontend()   # MainMenu + Briefing

Expect this to take a few minutes: it is building real geometry, saving a level
12 times, and letting the navigation system rebuild after each save.

MAP NAMES (these are referenced by the mission data assets and the C++, so they
are not free choices):

    /Game/Maps/MainMenu     main menu level
    /Game/Maps/Briefing     pre-mission briefing level
    /Game/Maps/M01 .. M12   the twelve missions
"""

import os
import unreal

MAPS_DIR = "/Game/Maps"


def log(msg):
    unreal.log("[BlackVeil/BuildAllMaps] " + str(msg))


def _level_subsystem():
    try:
        return unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    except Exception:
        return None


def new_level(asset_path):
    """Create an empty level at /Game/Maps/<name>. Returns True on success."""
    sub = _level_subsystem()
    try:
        if sub:
            return bool(sub.new_level(asset_path))
        return bool(unreal.EditorLevelLibrary.new_level(asset_path))
    except Exception as exc:
        log("Could not create %s (%s)" % (asset_path, exc))
        return False


def open_level(asset_path):
    sub = _level_subsystem()
    try:
        if sub:
            return bool(sub.load_level(asset_path))
        return bool(unreal.EditorLevelLibrary.load_level(asset_path))
    except Exception as exc:
        log("Could not open %s (%s)" % (asset_path, exc))
        return False


def save_level():
    sub = _level_subsystem()
    try:
        if sub:
            sub.save_current_level()
        else:
            unreal.EditorLevelLibrary.save_current_level()
    except Exception as exc:
        log("Save failed (%s)" % exc)


def exists(asset_path):
    return unreal.EditorAssetLibrary.does_asset_exist(asset_path)


def build_frontend():
    """MainMenu and Briefing are empty levels; the UMG widgets fill them."""
    for name in ("MainMenu", "Briefing"):
        path = "%s/%s" % (MAPS_DIR, name)
        if exists(path):
            log("%s already exists, skipping." % name)
            continue
        if new_level(path):
            save_level()
            log("Created %s" % path)
        else:
            log("FAILED to create %s" % path)


def build_one(index, rebuild=True, finish=True, merge_geometry=True):
    """Build one mission map end to end."""
    import BlockoutBuilder
    import LevelSetup
    import SecuritySystemSetup
    import MissionObjectiveSetup

    name = "M%02d" % index
    path = "%s/%s" % (MAPS_DIR, name)

    if not exists(path):
        if not new_level(path):
            log("ABORT: could not create %s" % path)
            return False
        log("Created %s" % path)

    if not open_level(path):
        log("ABORT: could not open %s" % path)
        return False

    log("--- Building %s ---" % name)

    # 1. The space: floors, walls, doorways, stairs, cover, lights, navigation.
    BlockoutBuilder.build(index, rebuild)

    # 2. Gameplay: objective volumes, guard spawn points, patrol routes, intel,
    #    security actors.
    LevelSetup.build_mission(index)

    # 3. Security network: camera groups wired to terminals and alarm panels.
    SecuritySystemSetup.build(index)

    # 4. Objective trigger volumes.
    MissionObjectiveSetup.setup(index)

    # 5. Finish the blockout: real materials, then merge each room's loose cubes
    #    into one StaticMesh asset so the result is editable by an artist.
    if finish:
        import BlockoutFinish
        BlockoutFinish.finish(index, merge=merge_geometry)

    save_level()
    log("--- %s complete ---" % name)
    return True


def build_all(rebuild=True, finish=True, merge_geometry=True):
    build_frontend()
    ok, failed = 0, []
    for i in range(1, 13):
        if build_one(i, rebuild, finish, merge_geometry):
            ok += 1
        else:
            failed.append("M%02d" % i)

    log("Built %d/12 mission maps." % ok)
    if failed:
        log("Failed: " + ", ".join(failed))
    log("Next: open M01, press Play, and check the NavMesh covers the level "
        "(press P in the viewport). Guards need navigation data to move.")
    return ok


if __name__ == "__main__":
    build_all()
