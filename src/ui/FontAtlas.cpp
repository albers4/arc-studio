#include "FontAtlas.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include "../utils/FileHelper.h"
#include <iostream>

FontAtlas::~FontAtlas() {
  if (textureID)
    glDeleteTextures(1, &textureID);
}

FontAtlas::FontAtlas(const std::string &filename, float targetSize,
                     float atlasSize, int width, int height)
    : atlasW(width), atlasH(height) {
  std::vector<unsigned char> bitmap(atlasW * atlasH, 0);

  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::cerr << "CRITICAL: Failed to open font file: " << filename
              << std::endl;
    return;
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<unsigned char> buffer(size);
  if (!file.read((char *)buffer.data(), size)) {
    std::cerr << "CRITICAL: Failed to read font file data." << std::endl;
    return;
  }

  stbtt_BakeFontBitmap(buffer.data(), 0, atlasSize, bitmap.data(), atlasW,
                       atlasH, 32, 96, charData.data);

  scale = targetSize / atlasSize;

  stbtt_fontinfo info;
  if (stbtt_InitFont(&info, buffer.data(), 0)) {
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

    float tempScale = stbtt_ScaleForPixelHeight(&info, atlasSize);
    baselineOffset = (ascent + descent) / 2.0f * tempScale;

    baselineOffset *= scale;
  }

  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, atlasW, atlasH, 0, GL_RED,
               GL_UNSIGNED_BYTE, bitmap.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
}