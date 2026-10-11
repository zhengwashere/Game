"""
Black Veil - Level design document generator
===========================================

Regenerates Docs/LevelDesign/Mission_XX_*.md from the mission data assets and
the blockout layouts, so the documents always match the shipped data. Run this
after any change to Content/Data.

    python3 Tools/GenerateLevelDocs.py
"""

import json
import os

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, "..")
DATA = os.path.join(ROOT, "Content", "Data")
OUT = os.path.join(ROOT, "Docs", "LevelDesign")

GUARD_TYPES = ["Standard", "Armored", "Technician", "Surveillance",
               "Commander", "Elite"]


def load(path):
    with open(path) as handle:
        return json.load(handle)


def build_doc(index):
    m = load(os.path.join(DATA, "Missions", "DA_Mission_%02d.json" % index))
    layout = load(os.path.join(DATA, "Layouts", "M%02d_Layout.json" % index))
    rooms = layout.get("Rooms", [])

    d = []
    d.append("# Mission %02d - %s" % (index, m["MissionName"]))
    d.append("")
    d.append("**Location:** %s  " % m["LocationName"])
    d.append("**Map asset:** `%s`  " % m["MapAsset"])
    d.append("**Par time:** %d seconds  " % m["ParTimeSeconds"])
    d.append("**Blockout layout:** `Content/Data/Layouts/M%02d_Layout.json` (%d rooms)"
             % (index, len(rooms)))
    d.append("")
    d.append("**Unique mechanic:** %s" % m.get("UniqueMechanic", "See below."))
    d.append("")

    d.append("## Narrative context")
    d.append("")
    d.append(m["BriefingText"])
    d.append("")

    d.append("## Objectives")
    d.append("")
    d.append("| ID | Description | Type | Optional | Count | Prerequisites |")
    d.append("|----|-------------|------|----------|-------|---------------|")
    for o in m["Objectives"]:
        d.append("| `%s` | %s | %s | %s | %d | %s |" % (
            o["ObjectiveID"], o["Description"], o["Type"],
            "Yes" if o["bIsOptional"] else "No", o["RequiredCount"],
            ", ".join(o["Prerequisites"]) or "-"))
    d.append("")

    # --- Blockout rooms: this is the physical level the player walks through.
    d.append("## Blockout spaces")
    d.append("")
    d.append("These are the actual built rooms. Coordinates are world-space cm.")
    d.append("")
    d.append("| Room | Centre (X, Y) | Size W x D | Wall height | Floor Z | Ceiling |")
    d.append("|------|---------------|------------|-------------|---------|---------|")
    for r in rooms:
        d.append("| %s | (%.0f, %.0f) | %.0f x %.0f | %.0f | %.0f | %s |" % (
            r["ID"], r["X"], r["Y"], r["W"], r["D"], r.get("H", 400),
            r.get("FloorZ", 0), "yes" if r.get("Ceiling", True) else "open"))
    d.append("")

    if layout.get("Props"):
        d.append("**Cover props: %d** (containers, machines, shelving, hedges and"
                 " desks placed to break sightlines)." % len(layout["Props"]))
        d.append("")
    if layout.get("Lights"):
        groups = sorted({l.get("Group", "Default") for l in layout["Lights"]})
        d.append("**Light actors: %d** across groups: %s" %
                 (len(layout["Lights"]), ", ".join(groups)))
        d.append("")
    if layout.get("Stairs"):
        d.append("**Stairs: %d runs** providing vertical connectivity." % len(layout["Stairs"]))
        d.append("")

    d.append("## Guard deployment")
    d.append("")
    d.append("| Spawn | Type | Position | Squad | Patrol route | Vigilance |")
    d.append("|-------|------|----------|-------|--------------|-----------|")
    for g in m["EnemyComposition"]:
        d.append("| %s | %s | (%.0f, %.0f) | %s | `%s` | %.2f |" % (
            g["SpawnID"], GUARD_TYPES[g["GuardTypeOrdinal"]],
            g["Location"][0], g["Location"][1], g["SquadID"],
            g["PatrolRouteID"], g["Personality"]["Vigilance"]))
    d.append("")

    d.append("## Patrol routes")
    d.append("")
    d.append("| Route | Mode | Waypoints | Wait times | Scans | Conversations |")
    d.append("|-------|------|-----------|------------|-------|---------------|")
    for r in m["PatrolRoutes"]:
        pts = " -> ".join("(%.0f, %.0f)" % (p["Location"][0], p["Location"][1])
                          for p in r["Waypoints"])
        waits = ", ".join("%.0fs" % p["WaitDuration"] for p in r["Waypoints"])
        scans = "yes" if any(p["bScanWhileWaiting"] for p in r["Waypoints"]) else "no"
        convs = ", ".join(p["ConversationID"] for p in r["Waypoints"]
                          if p["bTriggerConversation"]) or "none"
        d.append("| `%s` | %s | %s | %s | %s | %s |" % (
            r["RouteID"], r["PatrolMode"], pts, waits, scans, convs))
    d.append("")

    d.append("## Lighting design")
    d.append("")
    d.append("**Ambient:** %s" % m["LightingSetup"]["Ambient"])
    d.append("")
    for z in m["LightingSetup"]["Zones"]:
        d.append("- %s" % z)
    d.append("")
    d.append("Every light is a real actor, and `UBVStealthComponent` samples them,")
    d.append("so each placement changes how detectable the operative is in that space.")
    d.append("")

    d.append("## Intelligence")
    d.append("")
    for it in m["IntelItems"]:
        d.append("- `%s` - %s (%s)" % (it["IntelID"], it["Title"], it["Category"]))
    d.append("")

    d.append("## Checkpoints")
    d.append("")
    for cp in m["Checkpoints"]:
        d.append("- `%s` at (%.0f, %.0f, %.0f)" % (
            cp["CheckpointID"], cp["Location"][0], cp["Location"][1], cp["Location"][2]))
    d.append("")

    d.append("## Recommended approaches")
    d.append("")
    d.append("**Ghost:** avoid every guard interaction, use the elevated or service")
    d.append("route, kill lights and cameras before committing to a corridor. Needs the")
    d.append("surveillance detector and patience with patrol timing.")
    d.append("")
    d.append("**Panther:** neutralise the two guards on the highest-traffic route first,")
    d.append("conceal both bodies, then move through the vacated space. Non-lethal")
    d.append("takedowns keep the score clean.")
    d.append("")
    d.append("**Assault (fallback):** if detected, break contact toward the nearest hard")
    d.append("cover, use smoke or a flashbang to reset the engagement, and withdraw along")
    d.append("a route the squad has not yet searched.")
    d.append("")

    d.append("## Set piece")
    d.append("")
    d.append(m.get("UniqueMechanic", "The mission climax."))
    d.append("")

    d.append("## Difficulty modifiers")
    d.append("")
    d.append("- Enemy count multiplier: %.2f" % m["Difficulty"]["EnemyCountMultiplier"])
    d.append("- Perception multiplier: %.2f" % m["Difficulty"]["PerceptionMultiplier"])
    d.append("- Alarm escalation speed: %.2f" % m["Difficulty"]["AlarmEscalationSpeed"])
    d.append("- Checkpoints disabled: %s" %
             ("yes" if m["Difficulty"]["bNoCheckpoints"] else "no"))
    d.append("")
    return "\n".join(d)


def main():
    if not os.path.isdir(OUT):
        os.makedirs(OUT)
    for i in range(1, 13):
        m = load(os.path.join(DATA, "Missions", "DA_Mission_%02d.json" % i))
        fname = "Mission_%02d_%s.md" % (i, m["MissionName"].replace(" ", ""))
        with open(os.path.join(OUT, fname), "w") as handle:
            handle.write(build_doc(i))
        print("wrote", fname)


if __name__ == "__main__":
    main()
