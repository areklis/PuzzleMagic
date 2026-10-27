# KENNEY PUMPKIN: imports the carved pumpkin of Kenney's Graveyard Kit (CC0, www.kenney.nl; the files and the licence text are in
# RawMeshes/Kenney) as /Game/Event/SM_PumpkinCarved, with its colour map (the dark-insides version made by
# Tools/darken_pumpkin_colormap.py) as /Game/Event/T_KenneyColormap and a plain lit material
# /Game/Event/M_KenneyColormap on it (the colour map sampled into Base Color: it compiles on phones too). AEventSpots puts it on the
# floor of the Halloween event for every Trick! (it builds a pumpkin from basic shapes when this asset is missing).
# Run: UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script=<this file>   (the editor must be closed)
import unreal

eal = unreal.EditorAssetLibrary
tools = unreal.AssetToolsHelpers.get_asset_tools()
mel = unreal.MaterialEditingLibrary
SOURCE = unreal.SystemLibrary.get_project_directory() + "RawMeshes/Kenney/"
FOLDER = "/Game/Event"


def import_task(filename, name, options=None):
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", filename)
    task.set_editor_property("destination_path", FOLDER)
    task.set_editor_property("destination_name", name)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("replace_existing_settings", True)
    task.set_editor_property("automated", True)
    task.set_editor_property("save", True)
    if options is not None:
        task.set_editor_property("options", options)
    tools.import_asset_tasks([task])
    return list(task.get_editor_property("imported_object_paths"))


# the colour map: flat colours, so no mip maps and no filtering (colours must not bleed into each other)
if not import_task(SOURCE + "colormap_dark.png", "T_KenneyColormap"):
    unreal.log_error("import_kenney_pumpkin.py: the colour map could not be imported")
texture = eal.load_asset(FOLDER + "/T_KenneyColormap")
texture.set_editor_property("filter", unreal.TextureFilter.TF_NEAREST)
texture.set_editor_property("mip_gen_settings", unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS)
eal.save_loaded_asset(texture)

# the material
path = FOLDER + "/M_KenneyColormap"
if eal.does_asset_exist(path):
    material = eal.load_asset(path)
    mel.delete_all_material_expressions(material)
else:
    material = tools.create_asset("M_KenneyColormap", FOLDER, unreal.Material, unreal.MaterialFactoryNew())
sample = mel.create_material_expression(material, unreal.MaterialExpressionTextureSample, -400, 0)
sample.set_editor_property("texture", texture)
mel.connect_material_property(sample, "RGB", unreal.MaterialProperty.MP_BASE_COLOR)
rough = mel.create_material_expression(material, unreal.MaterialExpressionConstant, -400, 220)
rough.set_editor_property("r", 0.85)
mel.connect_material_property(rough, "", unreal.MaterialProperty.MP_ROUGHNESS)
mel.recompile_material(material)
eal.save_loaded_asset(material)

# the mesh, without the materials of the file
ui = unreal.FbxImportUI()
ui.set_editor_property("import_mesh", True)
ui.set_editor_property("import_as_skeletal", False)
ui.set_editor_property("import_materials", False)
ui.set_editor_property("import_textures", False)
ui.set_editor_property("import_animations", False)
ui.static_mesh_import_data.set_editor_property("combine_meshes", True)
ui.static_mesh_import_data.set_editor_property("auto_generate_collision", False)
if not import_task(SOURCE + "pumpkin-carved.fbx", "SM_PumpkinCarved", ui):
    unreal.log_error("import_kenney_pumpkin.py: the pumpkin could not be imported")
mesh = eal.load_asset(FOLDER + "/SM_PumpkinCarved")
mesh.set_material(0, material)
eal.save_loaded_asset(mesh)
bounds = mesh.get_bounds()
unreal.log("import_kenney_pumpkin.py: SM_PumpkinCarved done, bounds origin %s extent %s" % (bounds.origin, bounds.box_extent))
