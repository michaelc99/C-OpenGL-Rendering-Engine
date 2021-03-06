#ifndef MODEL_H
#define MODEL_H

#include <graphics/model/model_data.h>
#include <graphics/mesh/mesh.h>
#include <graphics/texture/texture.h>
#include <string>
#include <vector>

namespace Engine {

class Model {
    public:
        Model(const std::string modelFilePath);
        Model(const ModelDataPtr modelDataPtr);
        Model(const Model& model);
        ~Model();
        
        void render() const;
        
        /*
         * Returns a ModelDataPtr to a shallow copy of the model's data in the list of (shared) loaded models.
         */
        ModelDataPtr getModelDataPtr() const;
        
        /*
         * Returns ModelDataPtr to a deep copy of this model's data.
         */
        ModelDataPtr copyModelData() const;
    private:
        unsigned int modelID = 0;
};

}

#endif //MODEL_H
