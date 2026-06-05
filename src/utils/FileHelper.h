#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Utils {
    inline std::string readFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filepath << std::endl;
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    inline std::vector<unsigned char> readBinaryFile(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) return {};
        
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<unsigned char> buffer(size);
        if (!file.read((char*)buffer.data(), size)) return {};
        return buffer;
    }
}