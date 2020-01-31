#include "precomp.h"
#include "ModelLoader.h"

#include "gltf.h"
#include "BVHBranch.h"

struct ImageInfo
{
    std::string fileName;

    unsigned int binarySize;
    const uint8_t* binaryData;
};

/*
 * VERTEX PARSING
 */

 // checkt he size of the components used by the accessor
uint8_t ParseComponentSize(const fx::gltf::Accessor& a_Accessor)
{
    switch (a_Accessor.componentType)
    {
    case fx::gltf::Accessor::ComponentType::Byte:
    case fx::gltf::Accessor::ComponentType::UnsignedByte:
        return 1;
        break;
    case fx::gltf::Accessor::ComponentType::Short:
    case fx::gltf::Accessor::ComponentType::UnsignedShort:
        return 2;
        break;
    case fx::gltf::Accessor::ComponentType::Float:
    case fx::gltf::Accessor::ComponentType::UnsignedInt:
        return 4;
        break;
    }
}

// check the number of the components used by the accessor
uint8_t ParseComponentNumber(const fx::gltf::Accessor& a_Accessor)
{
    switch (a_Accessor.type)
    {
    case fx::gltf::Accessor::Type::Scalar:
        return 1;
        break;
    case fx::gltf::Accessor::Type::Vec2:
        return 2;
        break;
    case fx::gltf::Accessor::Type::Vec3:
        return 3;
        break;
    case fx::gltf::Accessor::Type::Mat2:
    case fx::gltf::Accessor::Type::Vec4:
        return 4;
        break;
    case fx::gltf::Accessor::Type::Mat3:
        return 9;
        break;
    case fx::gltf::Accessor::Type::Mat4:
        return 16;
        break;
    }
}

struct BufferInfo
{
    fx::gltf::Accessor const* Accessor;

    uint8_t const* Data;
    uint32_t DataStride;
    uint32_t TotalSize;

    bool HasData() const noexcept
    {
        return Data != nullptr;
    }
};

// extract the data from the document based on the accessor
BufferInfo GetData(const fx::gltf::Document& a_Doc, const fx::gltf::Accessor& a_Accessor)
{

    fx::gltf::BufferView const& bufferView = a_Doc.bufferViews[a_Accessor.bufferView];
    fx::gltf::Buffer const& buffer = a_Doc.buffers[bufferView.buffer];

    unsigned short int stride;
    if (bufferView.byteStride)
    {
        stride = bufferView.byteStride;
    }
    else
    {
        stride = ParseComponentNumber(a_Accessor) * ParseComponentSize(a_Accessor);
    }

    return { &a_Accessor, &buffer.data[static_cast<uint64_t>(bufferView.byteOffset) + a_Accessor.byteOffset], stride, a_Accessor.count * stride };

}

// this function is purely for debugging convenience
// the graphics API doesn't care whether you are passing floats or bytes, it treats the data the same way
// because of that we might want to just undefine it for release
template <typename T>
std::vector<T> ParseBufferInfo(const BufferInfo& a_bufferInfo)
{
    unsigned int numElements = a_bufferInfo.TotalSize / ParseComponentSize(*a_bufferInfo.Accessor);
    std::vector<T> output;
    output.reserve(2048);
    auto accessor = a_bufferInfo.Accessor;
    for (size_t i = 0; i < a_bufferInfo.Accessor->count; i++)
    {
        uint8_t numComponents = ParseComponentNumber(*a_bufferInfo.Accessor);
        for (uint8_t k = 0; k < numComponents; k++)
        {
            unsigned int readIndex = i * a_bufferInfo.DataStride + k * sizeof(T);

            union
            {
                T f;
                uint8_t bytes[sizeof(T)];
            } number;

            for (unsigned int j = 0; j < sizeof(T); j++)
            {
                number.bytes[j] = a_bufferInfo.Data[readIndex + j];
            }

            output.push_back(number.f);
        }
    }
    return output;
}

Mesh* ModelLoader::LoadMesh(std::string a_FilePath, mat4 a_Transform)
{

    auto iter = m_LoadedMeshes.find(a_FilePath);

    if (iter != m_LoadedMeshes.end())
    {
        return (*iter).second.get();
    }

    auto doc = fx::gltf::LoadFromText(a_FilePath);

    fx::gltf::Document gltfDoc = fx::gltf::LoadFromText(a_FilePath);

    BufferInfo finalInfo;
    fx::gltf::Mesh gltfMesh = gltfDoc.meshes[0];
    std::vector<float> verticesPositions;
    std::vector<float> verticesUVs;
    std::vector<float> verticesNormals;
    std::vector<float> verticesTangents;

    std::vector<unsigned short int> indices;




    // iterate through all of the attributes and extract position, normal and UV coordinates
    // TODO: needs some reworking this one
    for (auto& attribute : gltfMesh.primitives[0].attributes)
    {
        if (attribute.first == "POSITION")
        {
            fx::gltf::Accessor accessor = gltfDoc.accessors[attribute.second];
            // this contains the buffer for the position in bytes
            // to use this, the bytes need to be merged into floats
            finalInfo = GetData(gltfDoc, accessor);

            verticesPositions = ParseBufferInfo<float>(finalInfo);
        }
        else if (attribute.first == "NORMAL")
        {
            fx::gltf::Accessor accessor = gltfDoc.accessors[attribute.second];
            // this contains the buffer for the position in bytes
            // to use this, the bytes need to be merged into floats
            finalInfo = GetData(gltfDoc, accessor);

            verticesNormals = ParseBufferInfo<float>(finalInfo);
        }
        else if (attribute.first == "TEXCOORD_0")
        {

            fx::gltf::Accessor accessor = gltfDoc.accessors[attribute.second];
            // this contains the buffer for the position in bytes
            // to use this, the bytes need to be merged into floats
            finalInfo = GetData(gltfDoc, accessor);

            verticesUVs = ParseBufferInfo<float>(finalInfo);
        }
        else if (attribute.first == "TANGENT")
        {

            fx::gltf::Accessor accessor = gltfDoc.accessors[attribute.second];
            // this contains the buffer for the position in bytes
            // to use this, the bytes need to be merged into floats
            finalInfo = GetData(gltfDoc, accessor);

            verticesTangents = ParseBufferInfo<float>(finalInfo);
        }

    }


    // get the index buffer
    fx::gltf::Accessor indexAccessor = gltfDoc.accessors[gltfMesh.primitives[0].indices];
    BufferInfo indexInfo = GetData(gltfDoc, indexAccessor);
    indices = ParseBufferInfo<unsigned short int>(indexInfo);

    std::vector<float3> positions;

    for (size_t i = 0; i < finalInfo.Accessor->count; i++)
    {
        positions.push_back({ verticesPositions[i * 3 + 0], verticesPositions[i * 3 + 1], verticesPositions[i * 3 + 2] });
        positions.back() = a_Transform.TransformPoint(positions.back());
    }

    Mesh* m = new Mesh();

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        m->m_Hitables.push_back(std::make_unique<Triangle>(positions[indices[i + 0]], positions[indices[i + 1]], positions[indices[i + 2]]));
    }

    //m.m_FilePath = a_FilePath;
    m->m_BVHRoot = std::make_unique<BVHBranch>();
    m->m_BVHRoot->Construct(m->m_Hitables);
    //auto pair = std::make_pair<std::string, Mesh>(std::move(a_FilePath), std::move(m);
    m_LoadedMeshes.emplace(a_FilePath, std::unique_ptr<Mesh>(m));

    auto& p = m_LoadedMeshes[a_FilePath];
    return p.get();
}
