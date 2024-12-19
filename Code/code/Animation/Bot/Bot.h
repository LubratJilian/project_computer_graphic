#ifndef BOT_H
#define BOT_H


#include <../external/tinygltf-2.9.3/tiny_gltf.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <../render/shader.h>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include<iostream>
#include<Lights.h>

struct PrimitiveObject {
    GLuint vao;
    std::map<int, GLuint> vbos;
};
struct SkinObject {
    // Transforms the geometry into the space of the respective joint
    std::vector<glm::mat4> inverseBindMatrices;

    // Transforms the geometry following the movement of the joints
    std::vector<glm::mat4> globalJointTransforms;

    // Combined transforms
    std::vector<glm::mat4> jointMatrices;
};
// Animation
struct SamplerObject {
    std::vector<float> input;
    std::vector<glm::vec4> output;
    int interpolation;
};
struct ChannelObject {
    int sampler;
    std::string targetPath;
    int targetNode;
};
struct AnimationObject {
    std::vector<SamplerObject> samplers;	// Animation data
};

class Bot{
  public:
    void initialize();
    void update(float time);
    void updateSkinning(const std::vector<glm::mat4> &nodeTransforms);
    void updateAnimation(
    const tinygltf::Model &model,
    const tinygltf::Animation &anim,
    const AnimationObject &animationObject,
    float time,
    std::vector<glm::mat4> &nodeTransforms);
    std::vector<AnimationObject> prepareAnimation(const tinygltf::Model &model);
    int findKeyframeIndex(const std::vector<float>& times, float animationTime);
    std::vector<SkinObject> prepareSkinning(const tinygltf::Model &model);
    void computeGlobalNodeTransform(const tinygltf::Model& model,
    const std::vector<glm::mat4> &localTransforms,
    int nodeIndex, const glm::mat4& parentTransform,
    std::vector<glm::mat4> &globalTransforms);
    void computeLocalNodeTransform(const tinygltf::Model& model,
    int nodeIndex,
    std::vector<glm::mat4> &localTransforms);
    glm::mat4 getNodeTransform(const tinygltf::Node& node);
    void computechild(const tinygltf::Model& model,
        const std::vector<glm::mat4> &localTransforms,
        int nodeIndex, const glm::mat4& parentTransform,
        std::vector<glm::mat4> &globalTransforms);
    bool loadModel(tinygltf::Model &model, const char *filename);
    void bindMesh(std::vector<PrimitiveObject> &primitiveObjects,
            tinygltf::Model &model, tinygltf::Mesh &mesh);
    void bindModelNodes(std::vector<PrimitiveObject> &primitiveObjects,
                    tinygltf::Model &model,
                    tinygltf::Node &node);
    std::vector<PrimitiveObject> bindModel(tinygltf::Model &model);
    void drawMesh(const std::vector<PrimitiveObject> &primitiveObjects,tinygltf::Model &model, tinygltf::Mesh &mesh);
    void drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects,tinygltf::Model &model, tinygltf::Node &node);
    void drawModel(const std::vector<PrimitiveObject>& primitiveObjects,			tinygltf::Model &model);
    void render(glm::mat4 projectionMatrix, glm::vec3 cameraPosition, Lights lights);
    void cleanup();

    private:

    GLuint mvpMatrixID;
    GLuint jointMatricesID;
    GLuint CameraPositionID;
    GLuint nbLightsID;
    GLuint programID;
    GLuint textureSampler3DID;
    GLuint blockIndex;

    tinygltf::Model model;

    // Each VAO corresponds to each mesh primitive in the GLTF model

    std::vector<PrimitiveObject> primitiveObjects;

    // Skinning

    std::vector<SkinObject> skinObjects;

    std::vector<AnimationObject> animationObjects;
  };

#endif //BOT_H
