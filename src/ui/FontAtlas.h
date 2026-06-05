#pragma once
#include <glad/glad.h>
#include <stb_truetype.h>
#include <string>

struct StbttCharData {
    stbtt_bakedchar data[96];
};

class FontAtlas {
public:
    GLuint textureID = 0;
    StbttCharData charData;
    int atlasW = 0, atlasH = 0;

    FontAtlas() = default;
    FontAtlas(const std::string& filename, float pxSize, int width = 2048, int height = 2048);
    ~FontAtlas();
    
    FontAtlas(const FontAtlas&) = delete;
    FontAtlas& operator=(const FontAtlas&) = delete;
};