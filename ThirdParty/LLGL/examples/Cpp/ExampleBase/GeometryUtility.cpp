/*
 * GeometryUtility.cpp
 *
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "GeometryUtility.h"
#include <fstream>
#include <stdexcept>
#include <limits>


/*
 * Global helper functions
 */

std::vector<VertexPos3Norm3> LoadObjModel(const std::string& filename)
{
    std::vector<VertexPos3Norm3> vertices;
    LoadObjModel(vertices, filename);
    return vertices;
}

TriangleMesh LoadObjModel(std::vector<VertexPos3Norm3>& vertices, const std::string& filename)
{
    // Read obj file
    std::ifstream file(filename);
    if (!file.good())
        throw std::runtime_error("failed to load model from file: \"" + filename + "\"");

    // Initialize triangle mesh
    TriangleMesh mesh;
    mesh.firstVertex = static_cast<std::uint32_t>(vertices.size());

    std::vector<Gs::Vector3f> coords, normals;

    while (!file.eof())
    {
        // Read each line
        std::string line;
        std::getline(file, line);

        std::stringstream s;
        s << line;

        // Parse line
        std::string mode;
        s >> mode;

        if (mode == "v")
        {
            Gs::Vector3f v;
            s >> v.x;
            s >> v.y;
            s >> v.z;
            coords.push_back(v);
        }
        else if (mode == "vn")
        {
            Gs::Vector3f n;
            s >> n.x;
            s >> n.y;
            s >> n.z;
            normals.push_back(n);
        }
        else if (mode == "f")
        {
            unsigned int v = 0, vn = 0;

            for (int i = 0; i < 3; ++i)
            {
                s >> v;
                s.ignore(2);
                s >> vn;
                vertices.push_back({ coords[v - 1], normals[vn - 1] });
                mesh.numVertices++;
            }
        }
    }

    return mesh;
}

std::vector<Gs::Vector3f> GenerateCubeVertices()
{
    return
    {
        { -1, -1, -1 }, { -1,  1, -1 }, {  1,  1, -1 }, {  1, -1, -1 },
        { -1, -1,  1 }, { -1,  1,  1 }, {  1,  1,  1 }, {  1, -1,  1 },
    };
}

std::vector<std::uint32_t> GenerateCubeTriangleIndices()
{
    return
    {
        0, 1, 2, 0, 2, 3, // front
        3, 2, 6, 3, 6, 7, // right
        4, 5, 1, 4, 1, 0, // left
        1, 5, 6, 1, 6, 2, // top
        4, 0, 3, 4, 3, 7, // bottom
        7, 6, 5, 7, 5, 4, // back
    };
}

std::vector<std::uint32_t> GenerateCubeQuadlIndices()
{
    return
    {
        0, 1, 3, 2, // front
        3, 2, 7, 6, // right
        4, 5, 0, 1, // left
        1, 5, 2, 6, // top
        4, 0, 7, 3, // bottom
        7, 6, 4, 5, // back
    };
}

std::vector<VertexPos3Tex2> GenerateTexturedCubeVertices()
{
    return
    {
        { { -1, -1, -1 }, { 0, 1 } }, { { -1,  1, -1 }, { 0, 0 } }, { {  1,  1, -1 }, { 1, 0 } }, { {  1, -1, -1 }, { 1, 1 } }, // front
        { {  1, -1, -1 }, { 0, 1 } }, { {  1,  1, -1 }, { 0, 0 } }, { {  1,  1,  1 }, { 1, 0 } }, { {  1, -1,  1 }, { 1, 1 } }, // right
        { { -1, -1,  1 }, { 0, 1 } }, { { -1,  1,  1 }, { 0, 0 } }, { { -1,  1, -1 }, { 1, 0 } }, { { -1, -1, -1 }, { 1, 1 } }, // left
        { { -1,  1, -1 }, { 0, 1 } }, { { -1,  1,  1 }, { 0, 0 } }, { {  1,  1,  1 }, { 1, 0 } }, { {  1,  1, -1 }, { 1, 1 } }, // top
        { { -1, -1,  1 }, { 0, 1 } }, { { -1, -1, -1 }, { 0, 0 } }, { {  1, -1, -1 }, { 1, 0 } }, { {  1, -1,  1 }, { 1, 1 } }, // bottom
        { {  1, -1,  1 }, { 0, 1 } }, { {  1,  1,  1 }, { 0, 0 } }, { { -1,  1,  1 }, { 1, 0 } }, { { -1, -1,  1 }, { 1, 1 } }, // back
    };
}

std::vector<std::uint32_t> GenerateTexturedCubeTriangleIndices()
{
    return
    {
         0,  1,  2,  0,  2,  3, // front
         4,  5,  6,  4,  6,  7, // right
         8,  9, 10,  8, 10, 11, // left
        12, 13, 14, 12, 14, 15, // top
        16, 17, 18, 16, 18, 19, // bottom
        20, 21, 22, 20, 22, 23, // back
    };
}

