"""
Black Veil - Mission Objective Setup
====================================

Creates objective trigger volumes and wires optional objectives for a mission.
Reads the mission data asset JSON so the level and the runtime data never drift.

Usage (in-editor Python console):
    import MissionObjectiveSetup
    MissionObjectiveSetup.setup(3)
"""

import json
import os
import unreal

DATA_DIR = os.path.join(unreal.Paths.project_dir(), "Content", "Data", "Missions")
GENERATED_TAG = "BV_Generated"


def setup(mission_index):
    path = os.path.join(DATA_DIR, "DA_Mission_%02d.json" % mission_index)
    with open(path) as handle:
        mission = json.load(handle)

    volume_class = unreal.load_class(None, "/Script/BlackVeil.BVObjectiveVolume")
    created = 0

    for objective in mission["Objectives"]:
        location = objective.get("WorldLocation", [0, 0, 0])
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
            volume_class, unreal.Vector(*location), unreal.Rotator(0, 0, 0))
        if not actor:
            continue
        actor.tags = [GENERATED_TAG, "BV_ObjectiveVolume"]
        actor.set_actor_label("Obj_" + objective["ObjectiveID"])
        actor.set_editor_property("ObjectiveID", unreal.Name(objective["ObjectiveID"]))
        actor.set_editor_property("bIsOptional", objective["bIsOptional"])
        created += 1

    unreal.log("[BlackVeil] Created %d objective volumes for mission %d" % (created, mission_index))
    unreal.EditorLevelLibrary.save_current_level()


if __name__ == "__main__":
    setup(1)
