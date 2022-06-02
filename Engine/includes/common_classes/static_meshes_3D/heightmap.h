#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../shaderProgram.h"
#include "../vertexBufferObject.h"
#include "staticMeshIndexed3D.h"

namespace static_meshes_3D {


class Heightmap : public StaticMeshIndexed3D
{
public:
    static const std::string MULTILAYER_SHADER_PROGRAM_KEY; // Holds a key for multilayer heightmap shader program (used as shaders key too)

    struct ShaderConstants
    {
        DEFINE_SHADER_CONSTANT_INDEX(terrainSampler, "terrainSampler")
        DEFINE_SHADER_CONSTANT_INDEX(levels, "levels")
        DEFINE_SHADER_CONSTANT(numLevels, "numLevels")
    };

    struct HillAlgorithmParameters
    {
        HillAlgorithmParameters(int rows, int columns, int numHills, int hillRadiusMin, int hillRadiusMax, float hillMinHeight, float hillMaxHeight)
        {
            this->rows = rows;
            this->columns = columns;
            this->numHills = numHills;
            this->hillRadiusMin = hillRadiusMin;
            this->hillRadiusMax = hillRadiusMax;
            this->hillMinHeight = hillMinHeight;
            this->hillMaxHeight = hillMaxHeight;
        }

        int rows;
        int columns;
        int numHills;
        int hillRadiusMin;
        int hillRadiusMax;
        float hillMinHeight;
        float hillMaxHeight;
    };

    Heightmap(const HillAlgorithmParameters& params, bool withPositions = true, bool withTextureCoordinates = true, bool withNormals = true);
    Heightmap(const std::string& fileName, bool withPositions = true, bool withTextureCoordinates = true, bool withNormals = true);

    static void prepareMultiLayerShaderProgram();
    static ShaderProgram& getMultiLayerShaderProgram();

    void createFromHeightData(const std::vector<std::vector<float>>& heightData);

    void render() const override;

    void renderMultilayered(const std::vector<std::string>& textureKeys, const std::vector<float> levels) const;

    void renderPoints() const override;

    //Erosion
    glm::vec3 surfaceNormal(int i, int j);
    void erode(int cycles);



    int getRows() const;

    int getColumns() const;

    float getHeight(const int row, const int column) const;

    float getRenderedHeightAtPosition(const glm::vec3& renderSize, const glm::vec3& position) const;

    static std::vector<std::vector<float>> generateRandomHeightData(const HillAlgorithmParameters& params);


    static std::vector<std::vector<float>> getHeightDataFromImage(const std::string& fileName);

    std::vector<std::vector<float>> _heightData;
    

    //Erosion
    bool active = false;
    int remaining = 5;
    int erosionstep = 1;
    //Particle Properties
    float dt = 1.2;
    float density = 1.0;
    float evapRate = 0.001;
    float depositionRate = 0.1;
    float minVol = 0.01;
    float friction = 0.05;

    void set_dt(float _dt) { this->dt = _dt; }

private:

    void setUpVertices();
    void setUpTextureCoordinates();
    void setUpNormals();
    void setUpIndexBuffer();

 
    std::vector<std::vector<glm::vec3>> _vertices;
    std::vector<std::vector<glm::vec2>> _textureCoordinates;
    std::vector<std::vector<glm::vec3>> _normals;
    int _rows = 0;
    int _columns = 0;

    glm::vec2 dim;
    double scale = 60.0;
};


struct Particle {
    Particle(glm::vec2 _pos) { pos = _pos; }

    glm::vec2 pos;
    glm::vec2 speed = glm::vec2(0.0);

    float volume = 1.0;
    float sediment = 0.0;
};

}