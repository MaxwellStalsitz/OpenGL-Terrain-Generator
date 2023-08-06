#ifndef MESHGENERATION_H
#define MESHGENERATION_H

#include "Source.h"
#include "perlinNoise.h"

struct Mesh {
    int seed;
    glm::vec2 size;
    std::vector<glm::vec3> vertices;
    std::vector<int> indices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> colors;
};

Mesh terrainMesh;

struct colorRange {
    std::string name;
    float startHeight;
    glm::vec3 color;
};

colorRange water = {
    "water",
    -1.0f, // terrain realistically starts at 0
    glm::vec3(5, 73, 181)
};

colorRange sand = {
    "sand",
    0.0f,
    glm::vec3(209, 205, 180)
};

colorRange dirt = {
    "dirt",
    0.1f,
    glm::vec3(112, 84, 43)
};

colorRange grass = {
    "grass",
    0.2f,
    glm::vec3(115, 237, 88)
};

colorRange stone = {
    "stone",
    0.5f,
    glm::vec3(138, 138, 138)
};

colorRange snow = {
    "snow",
    0.8f,
    glm::vec3(255, 255, 255)
};

std::vector<colorRange> defaultColorSet = { water, sand, dirt, grass, stone, snow };

std::vector<glm::vec3> generateNormals(std::vector<glm::vec3> vertices, std::vector<int> indices) {
    std::vector<glm::vec3> normals(vertices.size(), glm::vec3(0.0f));

    for (int i = 0; i < indices.size(); i += 3) {
        unsigned int idx1 = indices[i];
        unsigned int idx2 = indices[i + 1];
        unsigned int idx3 = indices[i + 2];

        glm::vec3 v1 = vertices[idx1];
        glm::vec3 v2 = vertices[idx2];
        glm::vec3 v3 = vertices[idx3];

        glm::vec3 faceNormal = glm::normalize(glm::cross(v2 - v1, v3 - v1));

        normals[idx1] += faceNormal;
        normals[idx2] += faceNormal;
        normals[idx3] += faceNormal;
    }

    for (int i = 0; i < normals.size(); i++) { //normalizing the normals
        normals[i] = glm::normalize(normals[i]);
    }

    return normals;
}

std::vector<int> generateIndicies(int sizeX, int sizeY) {
    std::vector<int> indices;

    for (int y = 0; y < sizeX - 1; ++y) {
        for (int x = 0; x < sizeY - 1; ++x) {
            unsigned int topLeft = y * sizeX + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (y + 1) * sizeX + x;
            unsigned int bottomRight = bottomLeft + 1;
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    return indices;
}

glm::vec3 getColor(glm::vec3 color) {
    return glm::vec3(color.x / 255, color.y / 255, color.z / 255);
}

std::vector<glm::vec3> generateColors(std::vector<glm::vec3> vertices, std::vector<colorRange> colorSet) {
    glm::vec3 color;
    std::vector<glm::vec3> colors;

    for (int i = 0; i < vertices.size(); i++) {
        for (int j = 0; j < colorSet.size(); j++) {
            float scaleMultiplier = amplitude * meshScale;
            float height = vertices[i].y;

            if (height >= (colorSet[j].startHeight * scaleMultiplier)) {
                color = colorSet[j].color;
            }
        }

        colors.push_back(getColor(color));
    }

    return colors;
}

void bindBuffers(std::vector<glm::vec3> vertices, std::vector<int> indices, std::vector<glm::vec3> normals, std::vector<glm::vec3> colors) {
    //bind vertices to buffer
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);

    //bind indices to buffer (but we don't send it over to the vertex shader)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_DYNAMIC_DRAW);

    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(0);

    //bind normals to buffer
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_DYNAMIC_DRAW);

    //normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(1);

    //bind colors to buffer
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_DYNAMIC_DRAW);

    //color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(2);
}

Mesh initializeMesh(int sizeX, int sizeY) {
    Mesh mesh;

    mesh.size = glm::vec2(sizeX, sizeY);

    std::vector<glm::vec3> vertices = mapPerlinNoise(mesh.size.x, mesh.size.y);
    mesh.vertices = vertices;

    //populating index array
    std::vector<int> indices = generateIndicies(mesh.size.x, mesh.size.y);
    mesh.indices = indices;

    std::vector<glm::vec3> normals = generateNormals(mesh.vertices, mesh.indices); // terrain normals
    mesh.normals = normals;

    std::vector<glm::vec3> meshColors = generateColors(mesh.vertices, defaultColorSet);

    mesh.colors = meshColors;

    bindBuffers(vertices, indices, normals, meshColors);

    return mesh;
}

Mesh updateMesh(Mesh mesh) {

    Mesh newMesh;
    newMesh = mesh;

    // (we don't need to change indices unless the size is different)
    std::vector<int> newIndices = newMesh.indices;
    if (glm::vec2(gridX, gridY) != mesh.size) {
        newIndices = generateIndicies(gridX, gridY);
        newMesh.size = glm::vec2(gridX, gridY);
        newMesh.indices = newIndices;
    }

    std::vector<glm::vec3> newVertices = mapPerlinNoise(newMesh.size.x, newMesh.size.y);
    newMesh.vertices = newVertices;

    std::vector<glm::vec3> newNormals = generateNormals(newMesh.vertices, newMesh.indices);
    newMesh.normals = newNormals;

    std::vector<glm::vec3> newColors = generateColors(newMesh.vertices, defaultColorSet);
    newMesh.colors = newColors;

    // update buffers
    // ------------------------------------------------------------------------

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, newMesh.vertices.data(), newMesh.vertices.size() * sizeof(glm::vec3));
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    ptr = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, newMesh.indices.data(), newMesh.indices.size() * sizeof(int));
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NBO);
    ptr = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, newMesh.normals.data(), newMesh.normals.size() * sizeof(glm::vec3));
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CBO);
    ptr = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, newMesh.colors.data(), newMesh.colors.size() * sizeof(glm::vec3));
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    // ------------------------------------------------------------------------

    return newMesh;    
}

#endif