import bpy
from bpy import context
from bpy_extras.io_utils import axis_conversion
import os
import shutil

filepath = bpy.path.abspath("//") + "myscene.scene"
abs_folderpath = bpy.path.abspath("//")

# Deselect all objects
bpy.ops.object.select_all(action='DESELECT')

export_scale = 1.0

# Delete already existing scene folder
if os.path.exists(abs_folderpath + "scene/"):
    shutil.rmtree(abs_folderpath + "scene/")

with open(filepath, 'w') as f:
    
    f.write("#NAME MODEL\n")
         
    for object in context.visible_objects:
        
        if object.type == 'MESH':
                        
            folderpath = "scene/" + object.data.name + "/"
            abs_folderpath = bpy.path.abspath("//") + folderpath
            
            name = object.data.name + ".obj"
            
            # Check whether the specified path exists or not
            if not os.path.exists(abs_folderpath):
            # Create a new directory because it does not exist
                os.makedirs(abs_folderpath)

            # Export OBJ
            object.select_set(True)
            print(folderpath+name)
            bpy.ops.wm.obj_export(filepath=abs_folderpath+name, filter_glob="*.obj;*.mtl", path_mode="COPY",
                export_selected_objects=True, export_triangulated_mesh=True, global_scale=export_scale)

            object.select_set(False)
            
        
        elif object.type == 'EMPTY':

            folderpath = ""
            name = object.name
            
        else:
            continue

        # Store original mat        
        original_mat = object.matrix_world.copy()
        
        # Set identity to export in 0,0,0 and no rotations
        object.matrix_world.identity()
        
        # Restore transform
        object.matrix_world = original_mat.copy()
        
        # Export object in scene file
        f.write(folderpath + name + " ")

        round_val = 5

        # Export matrix data (Change Y and Z axis and scale translation by global scale)
        
        correction_matrix = axis_conversion(to_forward='-Z', to_up='Y',).to_4x4()
        correction_matrix_inv = correction_matrix.inverted()
        
        original_mat = correction_matrix @ original_mat @ correction_matrix_inv

        f.write( str( round(original_mat[0][0], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[1][0], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[2][0], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[3][0], round_val) ) )
        f.write(",")
        
        f.write( str( round(original_mat[0][1], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[1][1], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[2][1], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[3][1], round_val) ) )
        f.write(",")
        
        f.write( str( round(original_mat[0][2], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[1][2], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[2][2], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[3][2], round_val) ) )
        f.write(",")
        
        f.write( str( round(original_mat[0][3] * export_scale, round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[1][3] * export_scale, round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[2][3] * export_scale, round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[3][3], round_val) ) )
        
        f.write("\n")

            
            