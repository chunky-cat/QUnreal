#pragma once
#include <string>
#include <map>
#include <array>
#include <vector>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include "types.h"
#include "brush.h"

namespace qformats::map
{
    const int STANDARD_VERSION = 100;
    const int VALVE_VERSION = 220;

    struct QBrushEntity : QEntity
    {
        std::vector<Brush> brushes;
        std::string tbType;
        std::string tbName;
        bool hasPhongShading;
    };

    class QMapFile
    {
    public:
        QMapFile() { worldSpawn = nullptr; };
        void Parse(const std::string &filename);
        const std::string &VersionString() { return mapVersionStr; };
        const int Version() { return mapVersion; };

    private:
        void parse_entity_attributes(std::string l, QEntity *ent);
        void parse_entity_planes(std::stringstream &lines, QBrushEntity *ent);
        void parse_wad_string(const std::string &wads);

    private:
        int getOrAddTexture(std::string texture);

        int mapVersion = STANDARD_VERSION;
        std::string mapVersionStr = "100";
        QBrushEntity *worldSpawn;
        std::vector<QBrushEntity *> brushEntities;
        std::vector<QPointEntity *> pointEntities;
        std::vector<std::string> textures;
        std::vector<std::string> wads;
        friend class QMap;
    };
}
