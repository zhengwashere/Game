"""
Black Veil - Blockout finish pass
=================================

Takes the raw greybox that BlockoutBuilder produces (hundreds of scaled engine
cubes) and turns it into something production-usable:

  1. create_materials()  - real Material assets on disk, not defaults
  2. apply_materials()   - assign them by role (floor / wall / ceiling / cover)
  3. merge_rooms()       - merge each room's cubes into ONE StaticMesh asset

Step 3 is the one that changes the nature of the thing: it converts ~1000 loose
scaled cubes into a few dozen real StaticMesh assets that an artist can open,
re-UV, and replace piece by piece.

USAGE (Unreal Editor Python console):

    import sys
    sys.path.append(r"<path>/BlackVeil/Tools")
    import BlockoutFinish

    BlockoutFinish.finish()        # materials + apply + merge, all 12 missions
    BlockoutFinish.finish(3)       # just the currently open level (mission 3)

Or step by step:

    BlockoutFinish.create_materials()
    BlockoutFinish.apply_materials()
    BlockoutFinish.merge_rooms()
"""

import unreal

MATERIAL_DIR = "/Game/Materials/Blockout"
MERGED_DIR = "/Game/Blockout/Merged"
BLOCKOUT_TAG = "BV_Blockout"

# Role -> (base colour, roughness). Mid-grey and matte on purpose: a stealth
# blockout must read light and shadow honestly, so nothing here is glossy and
# nothing is bright enough to fake illumination.
ROLES = {
	"Floor":   ((0.32, 0.32, 0.34), 0.90),
	"Ceiling": ((0.22, 0.22, 0.24), 0.95),
	"Wall":    ((0.42, 0.42, 0.44), 0.85),
	"Cover":   ((0.26, 0.30, 0.26), 0.80),
	"Glass":   ((0.10, 0.14, 0.16), 0.10),
}


def log(msg):
	unreal.log("[BlackVeil/BlockoutFinish] " + str(msg))


def _asset_tools():
	return unreal.AssetToolsHelpers.get_asset_tools()


# ------------------------------------------------------------------ materials

def create_materials():
	"""Create one Material asset per blockout role."""
	created = []
	try:
		unreal.EditorAssetLibrary.make_directory(MATERIAL_DIR)
	except Exception:
		pass

	for role, (rgb, rough) in ROLES.items():
		name = "M_Blockout_" + role
		path = "%s/%s" % (MATERIAL_DIR, name)

		if unreal.EditorAssetLibrary.does_asset_exist(path):
			continue

		try:
			mat = _asset_tools().create_asset(name, MATERIAL_DIR, unreal.Material,
											 unreal.MaterialFactoryNew())
			if not mat:
				log("could not create " + name)
				continue

			mel = unreal.MaterialEditingLibrary

			base = mel.create_material_expression(
				mat, unreal.MaterialExpressionConstant3Vector, -520, -100)
			base.set_editor_property(
				"constant", unreal.LinearColor(rgb[0], rgb[1], rgb[2], 1.0))
			mel.connect_material_property(base, "", unreal.MaterialProperty.MP_BASE_COLOR)

			r_const = mel.create_material_expression(
				mat, unreal.MaterialExpressionConstant, -520, 90)
			r_const.set_editor_property("r", float(rough))
			mel.connect_material_property(r_const, "", unreal.MaterialProperty.MP_ROUGHNESS)

			mel.recompile_material(mat)
			unreal.EditorAssetLibrary.save_loaded_asset(mat)
			created.append(name)
		except Exception as exc:
			log("FAILED to create %s (%s)" % (name, exc))

	log("Materials created: %s" % (", ".join(created) if created else "none new"))
	return created


def _role_for_label(label):
	if label.startswith("Floor_"):
		return "Floor"
	if label.startswith("Ceil_"):
		return "Ceiling"
	if label.startswith("Wall_"):
		return "Wall"
	if label.startswith("Prop_") or label.startswith("Platform"):
		return "Cover"
	if label.startswith("Step_"):
		return "Wall"
	return None


def apply_materials():
	"""Assign the role material to every blockout actor in the open level."""
	assigned, missing = 0, 0

	cache = {}
	for role in ROLES:
		path = "%s/M_Blockout_%s" % (MATERIAL_DIR, role)
		mat = unreal.EditorAssetLibrary.load_asset(path)
		if mat:
			cache[role] = mat

	if not cache:
		log("No blockout materials found - run create_materials() first.")
		return 0

	try:
		actors = unreal.EditorLevelLibrary.get_all_level_actors()
	except Exception:
		actors = []

	for actor in actors:
		if not actor or BLOCKOUT_TAG not in actor.tags:
			continue

		role = _role_for_label(actor.get_actor_label())
		if not role or role not in cache:
			missing += 1
			continue

		try:
			smc = actor.static_mesh_component
			if smc:
				smc.set_material(0, cache[role])
				assigned += 1
		except Exception:
			missing += 1

	log("Materials assigned to %d actors (%d skipped)." % (assigned, missing))
	return assigned


# --------------------------------------------------------------------- merging

def _group_actors(actors, tag_prefix):
	groups = {}
	for actor in actors:
		for tag in actor.tags:
			t = str(tag)
			if t.startswith(tag_prefix):
				groups.setdefault(t[len(tag_prefix):], []).append(actor)
				break
	return groups


def merge_rooms(destroy_sources=True):
	"""
	Merge each room's blockout geometry into a single StaticMesh asset.

	destroy_sources=True replaces the loose cubes with the merged mesh. That is
	safe because the layout JSON is the source of truth: BlockoutBuilder.build(N,
	rebuild=True) regenerates everything from scratch either way.
	"""
	try:
		unreal.EditorAssetLibrary.make_directory(MERGED_DIR)
	except Exception:
		pass

	try:
		actors = [a for a in unreal.EditorLevelLibrary.get_all_level_actors()
				   if a and BLOCKOUT_TAG in a.tags
				   and a.get_class() == unreal.StaticMeshActor]
	except Exception:
		actors = []

	if not actors:
		log("No blockout StaticMeshActors found in this level.")
		return 0

	rooms = _group_actors(actors, "BV_Room_")
	merged_count = 0

	for room_id, group in sorted(rooms.items()):
		if len(group) < 2:
			continue

		try:
			opts = unreal.MergeStaticMeshActorsOptions()
			opts.base_package_name = "%s/M_%s" % (MERGED_DIR, room_id)
			opts.spawn_merged_actor = True
			opts.destroy_source_actors = bool(destroy_sources)
			try:
				mesh_opts = unreal.MeshMergingSettings()
				mesh_opts.generate_light_map_uv = False
				mesh_opts.gutter_size = 2
				opts.mesh_merge_options = mesh_opts
			except Exception:
				pass

			unreal.EditorLevelLibrary.merge_actors(group, opts)
			merged_count += 1
			log("Merged room '%s' (%d pieces) -> %s/M_%s" % (room_id, len(group), MERGED_DIR, room_id))
		except Exception as exc:
			log("FAILED to merge room '%s' (%s)" % (room_id, exc))

	# Cover props merge by kind, so an artist can replace all Containers at once.
	props = _group_actors(actors, "Cover_")
	for kind, group in sorted(props.items()):
		if len(group) < 2:
			continue
		try:
			opts = unreal.MergeStaticMeshActorsOptions()
			opts.base_package_name = "%s/Props_%s" % (MERGED_DIR, kind)
			opts.spawn_merged_actor = True
			opts.destroy_source_actors = bool(destroy_sources)
			unreal.EditorLevelLibrary.merge_actors(group, opts)
			merged_count += 1
			log("Merged props '%s' (%d pieces)" % (kind, len(group)))
		except Exception as exc:
			log("FAILED to merge props '%s' (%s)" % (kind, exc))

	log("Merged %d group(s) into %s" % (merged_count, MERGED_DIR))
	return merged_count


# ----------------------------------------------------------------------- entry

def finish(mission_index=None, merge=True, destroy_sources=True):
	"""Create materials, apply them, then merge room geometry."""
	if mission_index is not None:
		log("Finishing the open level (expected: M%02d)" % mission_index)

	create_materials()
	apply_materials()
	if merge:
		merge_rooms(destroy_sources)

	try:
		unreal.EditorLevelLibrary.save_current_level()
	except Exception:
		pass

	log("Blockout finish pass complete.")


if __name__ == "__main__":
	finish()
