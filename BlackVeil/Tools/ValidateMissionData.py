"""
Black Veil - Mission Data Validator
===================================

Checks every mission JSON for the structural requirements the runtime depends on:
  * at least one mandatory objective and one optional objective
  * every objective has a unique ID and valid prerequisites
  * at least one checkpoint and at least two intel items
  * every enemy references a patrol route that exists
  * every referenced gadget/weapon ID is registered in the code registry

Run standalone (no editor required):
    python3 Tools/ValidateMissionData.py
"""

import json
import os
import sys

KNOWN_GADGETS = {
    "NoiseMaker", "Flashbang", "SmokeGrenade", "EMP", "StickyCamera", "Lockpick",
    "HackingDevice", "SurveillanceDetector", "FiberOpticCamera", "TearGasGrenade",
    "SuppressedPistol",
}
KNOWN_WEAPONS = {"SuppressedPistol", "SuppressedRifle", "NonLethalProjector"}


def validate(path):
    errors = []
    with open(path) as handle:
        mission = json.load(handle)

    name = mission.get("MissionName", os.path.basename(path))
    objectives = mission.get("Objectives", [])

    mandatory = [o for o in objectives if not o["bIsOptional"]]
    optional = [o for o in objectives if o["bIsOptional"]]
    if not mandatory:
        errors.append("no mandatory objectives")
    if not optional:
        errors.append("no optional objective")

    ids = [o["ObjectiveID"] for o in objectives]
    if len(ids) != len(set(ids)):
        errors.append("duplicate objective IDs")
    for o in objectives:
        for prereq in o.get("Prerequisites", []):
            if prereq not in ids:
                errors.append("objective %s references unknown prerequisite %s" % (o["ObjectiveID"], prereq))

    if len(mission.get("Checkpoints", [])) < 1:
        errors.append("no checkpoints")
    if len(mission.get("IntelItems", [])) < 2:
        errors.append("fewer than two intel items")

    route_ids = {r["RouteID"] for r in mission.get("PatrolRoutes", [])}
    for guard in mission.get("EnemyComposition", []):
        if guard["PatrolRouteID"] and guard["PatrolRouteID"] not in route_ids:
            errors.append("guard %s references unknown route %s" % (guard["SpawnID"], guard["PatrolRouteID"]))

    for gid in mission.get("StartingGadgets", []):
        if gid not in KNOWN_GADGETS:
            errors.append("unknown gadget ID " + gid)
    for wid in mission.get("StartingWeapons", []):
        if wid not in KNOWN_WEAPONS:
            errors.append("unknown weapon ID " + wid)

    return name, errors


def main():
    data_dir = os.path.join(os.path.dirname(__file__), "..", "Content", "Data", "Missions")
    files = sorted(f for f in os.listdir(data_dir) if f.startswith("DA_Mission_"))

    failed = 0
    for filename in files:
        name, errors = validate(os.path.join(data_dir, filename))
        if errors:
            failed += 1
            print("FAIL  %-14s %s" % (name, "; ".join(errors)))
        else:
            print("OK    %s" % name)

    print("\n%d/%d missions valid" % (len(files) - failed, len(files)))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
