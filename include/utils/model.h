#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <limits>
#include <iostream>

#include <tuple>
#include <glm/glm.hpp>
#include <unordered_map>
#include <random>

#include <functional>


float distance(const aiVector3D& v1, const aiVector3D& v2) {
    return std::sqrt(std::pow(v2.x - v1.x, 2) + std::pow(v2.y - v1.y, 2) + std::pow(v2.z - v1.z, 2));
}

std::vector<GLuint> processFaces(const aiMesh* mesh, const std::unordered_map<unsigned int, unsigned int>& vertexMapping, 
                                        const std::vector<aiVector3D>& vertices, int vertexSampleRate) {
    std::vector<GLuint> newIndices;

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        std::vector<unsigned int> faceIndices;

        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            unsigned int originalIndex = face.mIndices[j];
            
            // Check if the original index is in the map (i.e., if it was sampled)
            if (vertexMapping.find(originalIndex) != vertexMapping.end()) {
                // Add the mapped reduced index for this face
                faceIndices.push_back(vertexMapping.at(originalIndex));
            } else {
                // Find the nearest sampled vertex and add that index
                float minDist = std::numeric_limits<float>::max();
                unsigned int closestIndex = 0;

                // Search for the closest sampled vertex
                for (unsigned int k = 0; k < mesh->mNumVertices; k++) {
                    if (k % vertexSampleRate == 0) {
                        float dist = distance(mesh->mVertices[originalIndex], mesh->mVertices[k]);
                        if (dist < minDist) {
                            minDist = dist;
                            closestIndex = vertexMapping.at(k);  // Get the reduced index for this vertex
                        }
                        if(minDist <= 0.001f){
                          break;
                        }
                    }
                }
                faceIndices.push_back(closestIndex);
            }
        }

        // Add the face to newIndices if it has 3 indices (valid triangle)
        if (faceIndices.size() == 3) {
            newIndices.push_back(faceIndices[0]);
            newIndices.push_back(faceIndices[1]);
            newIndices.push_back(faceIndices[2]);
        }
    }

    return newIndices;
}

void loadModel(std::vector<GLfloat>& unrotatedVertices, std::vector<GLuint>& indices, const std::string modelPath, float scale, int vertexSampleRate){

    // Initialize Assimp importer
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        modelPath,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes
    );

    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return;
    }

    // Process the first mesh in the scene
    aiMesh* mesh = scene->mMeshes[0];
    
    std::unordered_map<unsigned int, unsigned int> vertexMapping;  // Maps original index to reduced index
    int reducedIndex = 0;

    // Extract and scale vertices, sampling as needed
    int index = 0;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        if (index % vertexSampleRate == 0) {
            // Store the mapping from original index to reduced index
            vertexMapping[i] = reducedIndex++;

            // Push scaled vertex positions
            unrotatedVertices.push_back(mesh->mVertices[i].x * scale);
            unrotatedVertices.push_back(mesh->mVertices[i].y * scale);
            unrotatedVertices.push_back(mesh->mVertices[i].z * scale);

            // Push normals (no scaling needed)
            unrotatedVertices.push_back(mesh->mNormals[i].x);
            unrotatedVertices.push_back(mesh->mNormals[i].y);
            unrotatedVertices.push_back(mesh->mNormals[i].z);
        }
        index++;
    }

    std::vector<aiVector3D> aiVertices;
    for (int i = 0; i < unrotatedVertices.size(); i += 6) {
      // Extract position (x, y, z) from unrotatedVertices and convert them to aiVector3D
      aiVector3D vertex(
          unrotatedVertices[i],       // x
          unrotatedVertices[i + 1],   // y
          unrotatedVertices[i + 2]    // z
      );
      aiVertices.push_back(vertex);
    }
    indices = processFaces(mesh, vertexMapping, aiVertices, vertexSampleRate);
}


#endif // !MODEL_H
