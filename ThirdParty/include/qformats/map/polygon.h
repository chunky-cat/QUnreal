#pragma once

#include "types.h"
#include <memory>

namespace qformats::map
{

    class Polygon
    {
    public:
        enum eCP
        {
            FRONT = 0,
            BACK,
            ONPLANE,
            SPLIT,
        };

    public:
        Polygon(){};
        Polygon::eCP ClassifyPoly(const Polygon *other);
        void RecalcNormals();
        std::shared_ptr<Polygon> Copy();

        MapFileFace faceRef;
        std::vector<Vertex> vertices;
        std::vector<ushort> indices;
        glm::vec3 center;
        const bool operator==(const Polygon &arg_) const;

    private:
        bool last = false;
    };
}