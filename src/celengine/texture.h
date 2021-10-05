// texture.h
//
// Copyright (C) 2001-2003, Chris Laurel
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <celutil/color.h>
#include <celcompat/filesystem.h>
#include <celengine/image.h>
#include <celutil/array_view.h>

typedef void (*ProceduralTexEval)(float, float, float, std::uint8_t*);


struct TextureTile
{
    TextureTile(unsigned int _texID) :
        u(0.0f), v(0.0f), du(1.0f), dv(1.0f), texID(_texID) {};
    TextureTile(unsigned int _texID, float _u, float _v) :
        u(_u), v(_v), du(1.0f), dv(1.0f), texID(_texID) {};
    TextureTile(unsigned int _texID, float _u, float _v, float _du, float _dv) :
        u(_u), v(_v), du(_du), dv(_dv), texID(_texID) {};

    float u, v;
    float du, dv;
    unsigned int texID;
};


class TexelFunctionObject
{
 public:
    TexelFunctionObject() {};
    virtual ~TexelFunctionObject() {};
    virtual void operator()(float u, float v, float w,
                            std::uint8_t* pixel) = 0;
};


class Texture
{
 public:
    Texture(int w, int h, int d = 1);
    virtual ~Texture() = default;

    virtual const TextureTile getTile(int lod, int u, int v) = 0;
    virtual void bind() = 0;

    virtual int getLODCount() const;
    virtual int getUTileCount(int lod) const;
    virtual int getVTileCount(int lod) const;
    virtual int getWTileCount(int lod) const;

    // Currently, these methods are only implemented by virtual textures; they
    // may be useful later when a more sophisticated texture management scheme
    // is implemented.
    virtual void beginUsage() {};
    virtual void endUsage() {};

    virtual void setBorderColor(Color);

    int getWidth() const;
    int getHeight() const;
    int getDepth() const;

    bool hasAlpha() const { return alpha; }
    bool isCompressed() const { return compressed; }

    /*! Identical formats may need to be treated in slightly different
     *  fashions. One (and currently the only) example is the DXT5 compressed
     *  normal map format, which is an ordinary DXT5 texture but requires some
     *  shader tricks to be used correctly.
     */
    unsigned int getFormatOptions() const;

    //! Set the format options.
    void setFormatOptions(unsigned int opts);

    enum AddressMode
    {
        Wrap        = 0,
        BorderClamp = 1,
        EdgeClamp   = 2,
    };

    enum MipMapMode
    {
        DefaultMipMaps = 0,
        NoMipMaps      = 1,
    };

    // Format option flags
    enum {
        DXT5NormalMap = 1
    };

 protected:
    bool alpha{ false };
    bool compressed{ false };

 private:
    int width;
    int height;
    int depth;

    unsigned int formatOptions{ 0 };
};


class ImageTexture : public Texture
{
 public:
    ImageTexture(const Image& img, AddressMode, MipMapMode);
    ~ImageTexture();

    const TextureTile getTile(int lod, int u, int v) override;
    void bind() override;
    void setBorderColor(Color) override;

    unsigned int getName() const;

 private:
    unsigned int glName;
};


class TiledTexture : public Texture
{
 public:
    TiledTexture(const Image& img, int _uSplit, int _vSplit, MipMapMode);
    ~TiledTexture();

    const TextureTile getTile(int lod, int u, int v) override;
    void bind() override;
    void setBorderColor(Color) override;

    int getUTileCount(int lod) const override;
    int getVTileCount(int lod) const override;

 private:
    int uSplit;
    int vSplit;
    unsigned int* glNames;
};


class CubeMap : public Texture
{
 public:
    explicit CubeMap(celestia::util::array_view<const Image*>);
    ~CubeMap();

    const TextureTile getTile(int lod, int u, int v) override;
    void bind() override;
    void setBorderColor(Color) override;

 private:
    unsigned int glName;
};


std::unique_ptr<Texture>
CreateProceduralTexture(int width, int height,
                        celestia::PixelFormat format,
                        ProceduralTexEval func,
                        Texture::AddressMode addressMode = Texture::EdgeClamp,
                        Texture::MipMapMode mipMode = Texture::DefaultMipMaps);

std::unique_ptr<Texture>
CreateProceduralTexture(int width, int height,
                        celestia::PixelFormat format,
                        TexelFunctionObject& func,
                        Texture::AddressMode addressMode = Texture::EdgeClamp,
                        Texture::MipMapMode mipMode = Texture::DefaultMipMaps);

std::unique_ptr<Texture>
CreateProceduralCubeMap(int size, celestia::PixelFormat format,
                        ProceduralTexEval func);

std::unique_ptr<Texture>
LoadTextureFromFile(const fs::path& filename,
                    Texture::AddressMode addressMode = Texture::EdgeClamp,
                    Texture::MipMapMode mipMode = Texture::DefaultMipMaps);

std::unique_ptr<Texture>
LoadHeightMapFromFile(const fs::path& filename,
                      float height,
                      Texture::AddressMode addressMode = Texture::EdgeClamp);
