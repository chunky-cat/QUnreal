//
// Created by tgoeh on 10.11.2023.
//
#pragma once

#include <string>
#include <vector>
#include <functional>

#include <glm/glm.hpp>
#include "types.h"
#include "map_file.h"
#include "brush.h"
#include "../textureman.h"

namespace qformats::map
{
    class SolidEntity
    {
    public:
        QBrushEntity *entityRef;
        std::vector<Brush> geoBrushes;
        std::vector<int> textureIDs;
    };

    using polygonGatherCb = std::function<void(std::vector<PolygonPtr>, int)>;

    class QMap
    {
    public:
        QMap() = default;
        ~QMap() = default;

        void LoadTextures(textures::textureRequestCb cb);
        void LoadFile(const std::string &filename);
        void GenerateGeometry();
        std::vector<PolygonPtr> GetPolygonsByTexture(int entityID, std::string texName);
        void GatherPolygons(int entityID, polygonGatherCb);
        const std::vector<string> &Wads() { return map_file->wads; };
        bool HasWads() { return map_file->wads.size() > 0; };
        textures::ITexture *GetTextureByID(int id) { return texMan.GetTexture(id); };
        const std::vector<textures::ITexture *> GetTextures() { return texMan.GetTextures(); };
        QMapFile *MapData() { return map_file; };
        std::vector<QBrushEntity *> &BrushEntities() { return map_file->brushEntities; };
        std::vector<SolidEntity> solidEntities;

    private:
        bool getPolygonsByTextureID(int entityID, int texID, std::vector<PolygonPtr> &list);
        textures::TextureMan texMan;
        QMapFile *map_file;
    };
}
