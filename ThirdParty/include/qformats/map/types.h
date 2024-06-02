#pragma once

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>

namespace qformats::map
{
    const double epsilon = 1e-5; // Used to compensate for floating point inaccuracy.
    const double scale = 128;    // Scale
    // MAP FILE

    struct QEntity
    {
        std::string classname;
        std::map<std::string, std::string> attributes;
    };

    struct QPointEntity : QEntity
    {
        glm::vec3 origin;
    };

    struct MapFileFace
    {
        MapFileFace() = default;
        std::array<glm::dvec3, 3> planePoints;
        glm::dvec3 planeNormal;
        float planeDist;
        struct StandardUV
        {
            float u;
            float v;
        };
        StandardUV standardUv;
        struct ValveUV
        {
            glm::dvec4 u;
            glm::dvec4 v;
        };
        ValveUV valveUV;
        double rotation;
        double scaleX;
        double scaleY;
        int textureID;
    };

    // GEOMETRY
    struct Vertex
    {
        glm::dvec3 point;
        glm::dvec3 normal;
        glm::vec2 uv;
        glm::vec4 tangent;

        inline bool checkLegalVertext(const std::vector<MapFileFace> &faces)
        {
            for (auto f : faces)
            {
                auto proj = glm::dot(f.planeNormal, point);
                if (proj > f.planeDist && abs(f.planeDist - proj) > 0.0008)
                    return false;
            }
            return true;
        }

        inline bool inList(const std::vector<Vertex> &list)
        {
            for (auto const &v : list)
            {
                if (v.point == point)
                    return true;
            }
            return false;
        }
    };
}
