#pragma once

#include "types.h"
#include "polygon.h"
#include <vector>
#include <memory>

namespace qformats::map
{
    template <class T>
    using boolRet = std::pair<bool, T>;
    using PolygonPtr = std::shared_ptr<Polygon>;
    using PolygonIter = std::__1::vector<PolygonPtr>::iterator;
    class Brush
    {
    public:
        Brush(){};
        const bool DoesIntersect(Brush &other);
        std::vector<PolygonPtr> polygons;

    private:
        std::vector<MapFileFace> faces;
        bool hasValveUV;

        glm::dvec3 min;
        glm::dvec3 max;

        void generatePolygons();
        void windFaceVertices();
        void indexFaceVertices();
        void calculateAABB();
        void buildGeometry();
        glm::vec4 calcStandardTangent(const MapFileFace &face);
        glm::vec4 calcValveTangent(const MapFileFace &face);
        glm::vec2 calcStandardUV(glm::vec3 vertex, const MapFileFace &face, int texW, int texH);
        glm::vec2 calcValveUV(glm::vec3 vertex, const MapFileFace &face, int texW, int texH);
        Vertex mergeDuplicate(int from, Vertex &v);
        boolRet<Vertex> intersectPlanes(const MapFileFace &a, const MapFileFace &b, const MapFileFace &c);

        friend class QMap;
        friend class QMapFile;
    };
}