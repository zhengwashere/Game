"""
Black Veil - Security System Setup
==================================

Wires camera groups, alarm panels and security terminals into consistent
networks so that hacking a terminal disables the correct cameras and a camera
detection raises the correct alarm group.

Usage (in-editor Python console):
    import SecuritySystemSetup
    SecuritySystemSetup.build(3, groups=["Group_A", "Group_B"])
"""

import unreal

GENERATED_TAG = "BV_Generated"


def build(mission_index, groups=None, cameras_per_group=2):
    groups = groups or ["Group_A", "Group_B", "Group_C"]

    camera_class = unreal.load_class(None, "/Script/BlackVeil.BVSecurityCamera")
    terminal_class = unreal.load_class(None, "/Script/BlackVeil.BVSecurityTerminal")
    alarm_class = unreal.load_class(None, "/Script/BlackVeil.BVAlarmPanel")

    for g, group in enumerate(groups):
        # Cameras sweep their arc and report to the group alarm panel.
        for c in range(cameras_per_group):
            location = [-2000 + g * 2200 + c * 900, 1500 - c * 500, 380]
            cam = unreal.EditorLevelLibrary.spawn_actor_from_class(
                camera_class, unreal.Vector(*location), unreal.Rotator(0, 180 + c * 45, -25))
            if cam:
                cam.tags = [GENERATED_TAG, "BV_Camera"]
                cam.set_actor_label("Camera_%s_%d" % (group, c))
                cam.set_editor_property("AlarmGroupID", unreal.Name(group))
                cam.set_editor_property("SweepHalfAngle", 45.0 + c * 10.0)

        # One terminal per group, able to kill that group's cameras.
        term = unreal.EditorLevelLibrary.spawn_actor_from_class(
            terminal_class, unreal.Vector(-1500 + g * 2200, -900, 100), unreal.Rotator(0, 0, 0))
        if term:
            term.tags = [GENERATED_TAG, "BV_Terminal"]
            term.set_actor_label("Terminal_" + group)
            term.set_editor_property("CameraGroupToDisable", unreal.Name(group))
            term.set_editor_property("LightGroupToKill", unreal.Name(group))
            term.set_editor_property("SecurityTier", 2 + g)

        # One alarm panel per group, sabotagable by the player.
        panel = unreal.EditorLevelLibrary.spawn_actor_from_class(
            alarm_class, unreal.Vector(-2500 + g * 2200, 0, 150), unreal.Rotator(0, 0, 0))
        if panel:
            panel.tags = [GENERATED_TAG, "BV_AlarmPanel"]
            panel.set_actor_label("Alarm_" + group)
            panel.set_editor_property("AlarmGroupID", unreal.Name(group))

    unreal.log("[BlackVeil] Security network built for mission %d" % mission_index)
    unreal.EditorLevelLibrary.save_current_level()


if __name__ == "__main__":
    build(1)
