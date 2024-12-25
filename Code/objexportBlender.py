import bpy

# Export OBJ with Vertex Colors
output_path = "D:/Documents/Etudes/Polytech/2024-2025/Trinity college/CSU44052 - COMPUTER GRAPHICS/project_computer_graphic/Code/code/Objects/street_lamps.objoriented"

with open(output_path, "w") as file:
    objs = bpy.context.selected_objects

    # Si l'objet est en mode maillage (mesh)
    for obj in objs:
        if obj.type != 'MESH':
            raise TypeError("L'objet doit être un maillage (mesh).")

        # Matrice de transformation globale (si l'objet est déplacé)
        transform_matrix = obj.matrix_world

        # Récupérer les matériaux pour attribuer des couleurs
        materials = obj.data.materials

        # Parcourir les sommets sélectionnés de l'objet
        for vertex in obj.data.vertices:
            if vertex.select:  # Seulement les sommets sélectionnés
                # Appliquer la transformation globale pour obtenir les coordonnées mondiales
                global_position = transform_matrix @ vertex.co

                # Par défaut, utiliser une couleur si aucun matériau n'est spécifié
                material_color = (1.0, 1.0, 1.0)  # Couleur blanche par défaut

                # Vérifier si le sommet a une couleur via un matériau (si applicable)
                if materials:
                    # Appliquer une couleur en fonction du matériau de l'objet (1er matériau)
                    mat = materials[0]  # Prendre le 1er matériau (s'il existe)
                    if mat.use_nodes:
                        principled = mat.node_tree.nodes.get("Principled BSDF")
                        if principled:
                            color = principled.inputs["Base Color"].default_value
                            material_color = (color[0], color[1], color[2])  # R, G, B
                    else:
                        # Si pas de nœuds, utiliser la couleur du matériau classique
                        material_color = mat.diffuse_color[:3]

                # Écrire la ligne de sommet avec la couleur
                file.write(f"v {global_position.x} {global_position.z} {-global_position.y} {material_color[0]}\n")

print(f"Fichier exporté avec les couleurs des sommets : {output_path}")