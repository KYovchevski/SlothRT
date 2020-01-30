#pragma once
#include "Triangle.h"

#include <vector>
#include <string>
#include <memory>
#include <map>

class BVHNode;
class Triangle;

struct Mesh
{
    std::unique_ptr<BVHNode> m_BVHRoot;
    std::vector<std::unique_ptr<Hitable>> m_Hitables;
    mat4 m_Transform;
};

class ModelLoader
{
public:

    Mesh* LoadMesh(std::string a_FilePath, mat4 Transform);

private:

    std::map<std::string, std::unique_ptr<Mesh>> m_LoadedMeshes;
};

