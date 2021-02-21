//
// Created by Ricardo on 2/20/2021.
//

#include "DesktopFileSystem.h"
#include <fstream>

using namespace Eagle;


void DesktopFileSystem::init() {
    s_instance = new DesktopFileSystem();
}

std::vector<uint8_t> Eagle::DesktopFileSystem::read_bytes(const std::string &path) {
    std::ifstream is("./data/" + path, std::ios::binary | std::ios::in | std::ios::ate);

    if (!is.is_open()) {
        throw std::runtime_error("failed to open file: " + path);
    }
    std::vector<uint8_t> bytes(is.tellg());
    is.seekg(0, std::ios::beg);
    // Copy file contents into a buffer
    is.read((char*)bytes.data(), bytes.size());
    return std::move(bytes);
}

std::string DesktopFileSystem::read_text(const std::string &path) {
    std::ifstream file("./data/" + path);
    if (!file.is_open()) {
        throw std::runtime_error("failed to open file: " + path);
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}


