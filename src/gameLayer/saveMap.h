#pragma once
#include <vector>
#include <blocks.h>
#include <fstream>
#include <cstddef>


bool saveBlockDataToFile(const std::vector<Block> &blocks, int w, int h, const char *fileName);

bool loadBlockDataFromFile(std::vector<Block> &blocks, int &w, int &h, const char *fileName);

bool writeEntireFile(const char *fileName, const void *data, size_t size);

bool readEntireFile(
    const char* fileName,
    std::vector<unsigned char>& data
);

bool readEntireFile(
    const char* fileName,
    std::string& data
);

bool readEntireFile(
    const char* fileName,
    void* buffer,
    size_t bufferSize,
    size_t& bytesRead
);