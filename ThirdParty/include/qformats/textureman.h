#pragma once

#include <vector>
#include <string>
#include <functional>

using std::string, std::vector;

namespace qformats::textures
{

    class ITexture
    {
    public:
        ITexture(){};
        ITexture(string name, int width, int height) : name(name), width(width), height(height){};
        ITexture(string name) : name(name){};

        void SetWidth(int w) { width = w; }
        void SetHeight(int h) { height = h; }
        const std::string &Name() { return name; }
        int Width() { return width; }
        int Height() { return height; }

    protected:
        std::string name;
        int width;
        int height;

        friend class TextureMan;
    };

    template <class T>
    class Texture : public ITexture
    {
    public:
        Texture(){};
        void SetData(T d)
        {
            data = d;
        }
        const T &Data() { return data; }

    protected:
        T data;
    };

    using textureRequestCb = std::function<ITexture *(string)>;

    class TextureMan
    {
    public:
        int GetOrAddTexture(string name);
        void OnTextureRequested(textureRequestCb);
        ITexture *GetTexture(int texID);
        int GetTextureID(std::string name);

        const std::vector<ITexture *> &GetTextures() { return textures; }

    private:
        ITexture *invokeTextureRequest(string name);
        textureRequestCb reqCb;
        std::vector<ITexture *> textures;
    };
}