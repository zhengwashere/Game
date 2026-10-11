"""
Black Veil - Level Setup Tool
=============================

Builds the actor scaffolding for a mission level from its data asset JSON.
Run inside the Unreal Editor's Python console (Tools > Execute Python Script) or
via the commandlet:

    UnrealEditor-Cmd.exe BlackVeil.uproject -run=pythonscript \
        -script="Tools/LevelSetup.py"

The script is idempotent: re-running it replaces previously generated actors
(tagged BV_Generated) rather than duplicating them.

It creates:
  * Patrol route actors (BVPatrolRoute) with waypoints from the mission JSON
  * Guard spawn points (BVGuardSpawn) tagged with squad, type and route
  * Objective volumes (BVObjectiveVolume) for each objective
  * Checkpoint volumes (BVCheckpoint)
  * Intel pickup actors (BVIntelActor)
  * Security cameras, terminals and alarm panels per security group

All actors are placed on the currently open level, so open the target map first.
"""

import json
import os
import unreal

GENERATED_TAG = "BV_Generated"
DATA_DIR = os.path.join(unreal.Paths.project_dir(), "Content", "Data", "Missions")


def log(msg):
    unreal.log("[BlackVeil] " + msg)


def clear_generated():
    """Remove actors created by a previous run so the script is repeatable."""
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        if actor and GENERATED_TAG in actor.tags:
            unreal.EditorLevelLibrary.destroy_actor(actor)
    log("Cleared previously generated actors.")


def spawn(actor_class, location, rotation=(0.0, 0.0, 0.0), label=""):
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        actor_class, unreal.Vector(*location), unreal.Rotator(*rotation))
    if actor:
        actor.tags = [GENERATED_TAG]
        if label:
            actor.set_actor_label(label)
    return actor


def load_mission(index):
    path = os.path.join(DATA_DIR, "DA_Mission_%02d.json" % index)
    if not os.path.exists(path):
        raise RuntimeError("Mission data not found: " + path)
    with open(path, "r") as handle:
        return json.load(handle)


def build_patrol_routes(mission):
    route_class = unreal.load_class(None, "/Script/BlackVeil.BVPatrolRoute")
    for route in mission.get("PatrolRoutes", []):
        points = route.get("Waypoints", [])
        if not points:
            continue
        first = points[0]["Location"]
        actor = spawn(route_class, first, label="Route_" + route["RouteID"])
        if not actor:
            continue
        actor.set_editor_property("RouteID", unreal.Name(route["RouteID"]))
        log("Route %s with %d waypoints" % (route["RouteID"], len(points)))


def build_guards(mission):
    """Guard spawn markers; the mission manager spawns the actual pawns at
    runtime so difficulty scaling can change the population."""
    marker_class = unreal.load_class(None, "/Script/Engine.TargetPoint")
    for guard in mission.get("EnemyComposition", []):
        actor = spawn(marker_class, guard["Location"], guard["Rotation"],
                      label="Guard_" + guard["SpawnID"])
        if actor:
            actor.tags = [GENERATED_TAG, "BV_GuardSpawn", "Route_" + guard["PatrolRouteID"],
                          "Squad_" + guard["SquadID"]]


def build_objectives(mission):
    volume_class = unreal.load_class(None, "/Script/BlackVeil.BVObjectiveVolume")
    for objective in mission.get("Objectives", []):
        actor = spawn(volume_class, objective.get("WorldLocation", [0, 0, 0]),
                      label="Obj_" + objective["ObjectiveID"])
        if actor:
            actor.set_editor_property("ObjectiveID", unreal.Name(objective["ObjectiveID"]))


def build_checkpoints(mission):
    cp_class = unreal.load_class(None, "/Script/BlackVeil.BVCheckpoint")
    for cp in mission.get("Checkpoints", []):
        actor = spawn(cp_class, cp["Location"], label="CP_" + cp["CheckpointID"])
        if actor:
            actor.set_editor_property("CheckpointID", unreal.Name(cp["CheckpointID"]))


def build_intel(mission):
    intel_class = unreal.load_class(None, "/Script/BlackVeil.BVIntelActor")
    for index, item in enumerate(mission.get("IntelItems", [])):
        # Spread intel along the level's X axis; designers reposition in-editor.
        location = [-3000 + index * 2500, 800, 100]
        actor = spawn(intel_class, location, label="Intel_" + item["IntelID"])
        if actor:
            actor.set_editor_property("IntelID", unreal.Name(item["IntelID"]))


def build_security(mission):
    camera_class = unreal.load_class(None, "/Script/BlackVeil.BVSecurityCamera")
    terminal_class = unreal.load_class(None, "/Script/BlackVeil.BVSecurityTerminal")
    alarm_class = unreal.load_class(None, "/Script/BlackVeil.BVAlarmPanel")

    groups = ["Group_A", "Group_B", "Group_C"]
    for index, group in enumerate(groups):
        cam = spawn(camera_class, [-2000 + index * 2200, 1500, 400], [0, 180, -25],
                    label="Camera_" + group)
        if cam:
            cam.set_editor_property("AlarmGroupID", unreal.Name(group))

        term = spawn(terminal_class, [-1500 + index * 2200, -900, 100],
                     label="Terminal_" + group)
        if term:
            term.set_editor_property("CameraGroupToDisable", unreal.Name(group))
            term.set_editor_property("SecurityTier", 2 + index)

        panel = spawn(alarm_class, [-2500 + index * 2200, 0, 150], label="Alarm_" + group)
        if panel:
            panel.set_editor_property("AlarmGroupID", unreal.Name(group))


def build_mission(index):
    mission = load_mission(index)
    log("Building mission %d: %s" % (index, mission["MissionName"]))

    clear_generated()
    build_patrol_routes(mission)
    build_guards(mission)
    build_objectives(mission)
    build_checkpoints(mission)
    build_intel(mission)
    build_security(mission)

    unreal.EditorLevelLibrary.save_current_level()
    log("Mission %d build complete." % index)


if __name__ == "__main__":
    # Edit this value (or drive it from the command line) to target a mission.
    MISSION_INDEX = 1
    build_mission(MISSION_INDEX)
