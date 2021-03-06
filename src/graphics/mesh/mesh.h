#ifndef MESH_H
#define MESH_H

#include <graphics/mesh/mesh_data.h>
#include <graphics/material/material.h>
#include <vector>

namespace Engine {

class Mesh {
    public:
        Mesh(const MeshDataPtr meshDataPtr, const TexturedMaterial texturedMaterial, const UnTexturedMaterial unTexturedMaterial, const std::string modelFilePath = "");
        Mesh(const Mesh& mesh);
        ~Mesh();
        static float myTime;
        static Math::Vec2f myMousePos;
        static Math::Vec3f myPos;
        void render() const;
        
        /*
         * Returns a MeshDataPtr to a shallow copy of the mesh's data in the list of (shared) loaded
         * mesh geometries.
         */
        MeshDataPtr getMeshDataPtr() const;
        
        /*
         * Returns MeshDataPtr to a deep copy of this mesh's data.
         */
        MeshDataPtr copyMeshData() const;
        
        TexturedMaterial getTexturedMaterial() const { return texturedMaterial; }
        void setTexturedMaterial(const TexturedMaterial texturedMaterial) { this->texturedMaterial = texturedMaterial; }
        UnTexturedMaterial getUnTexturedMaterial() const { return unTexturedMaterial; }
        void setUnTexturedMaterial(const UnTexturedMaterial unTexturedMaterial) { this->unTexturedMaterial = unTexturedMaterial; }
    private:
        unsigned int meshID = 0;
        TexturedMaterial texturedMaterial;
        UnTexturedMaterial unTexturedMaterial;
};

}

#endif //MESH_H
