#include "saveMap.h"
#include <asserts.h>
#include <fstream>
#include <iostream>


struct BlockSaveRepresentation1
{

	std::uint16_t type = 0;

	Block toBlock()
	{
		Block b;
		b.type = type;
		return b;
	}

};

const int VERSION = 1;


BlockSaveRepresentation1 toBlockRepresentation(Block b)
{
	BlockSaveRepresentation1 rez;
	rez.type = b.type;
	return rez;
}


bool saveBlockDataToFile(const std::vector<Block> &blocks, int w, int h, const char *fileName)
{

	std::ofstream f(fileName, std::ios::binary);

	if (!f.is_open()) { return false; }

	if (blocks.size() != w * h) { return false; }

	f.write((const char *)&VERSION, sizeof(VERSION));
	f.write((const char *)&w, sizeof(w));
	f.write((const char *)&h, sizeof(h));

	for (int i = 0; i < blocks.size(); i++)
	{
		auto b = toBlockRepresentation(blocks[i]);
		f.write((const char *)&b, sizeof(b));
	}

	f.close();

	return true;
}

bool loadBlockDataFromFile(std::vector<Block> &blocks, int &w, int &h, const char *fileName)
{

	blocks.clear();
	w = 0;
	h = 0;

	std::ifstream f(fileName, std::ios::binary);

	if (!f.is_open()) { return false; }

	int readVersion = 0;

	f.read((char *)&readVersion, sizeof(readVersion));
	f.read((char *)&w, sizeof(w));
	f.read((char *)&h, sizeof(h));

	if (!f || w <= 0 || h <= 0)
	{
		f.close();
		return false;
	}

	if (w > 10000) { f.close(); return false; } // probably corrupt data
	if (h > 10000) { f.close(); return false; } // probably corrupt data

	// Read block data

	switch (readVersion)
	{
		case 1:
		{

			size_t blockCount = w * h;
			blocks.resize(blockCount);

			for (int i = 0; i < blockCount; i++)
			{
				BlockSaveRepresentation1 read;
				f.read((char *)&read, sizeof(read));

				if (!f)
				{
					blocks.clear();
					w = 0;
					h = 0;
					f.close();
					return false;
				}

				blocks[i] = read.toBlock();
			}



			break;
		}

		default:
		{
			// incorrect version
			w = 0;
			h = 0;
			f.close();
			return false;
		}
	}



	for (int i = 0; i < blocks.size(); i++)
	{
		blocks[i].sanitize();
	}

	f.close();
	return true;

}

bool getFileSize(std::ifstream& file, std::size_t& size)
{
    // Move the read position to the end of the file
    file.seekg(0, std::ios::end);

    if (!file)
        return false;

    // Get the current position
    std::streampos endPosition = file.tellg();

    if (endPosition == std::streampos(-1))
        return false;

    // Move the read position back to the beginning
    file.seekg(0, std::ios::beg);

    if (!file)
        return false;

    size = static_cast<std::size_t>(endPosition);

    return true;
}


bool readEntireFile(
    const char* fileName,
    std::vector<unsigned char>& data
)
{
    // Open the file in binary mode
    std::ifstream f(fileName, std::ios::binary);

    if (!f.is_open())
    {
        std::cerr << "Could not open file: " << fileName << '\n';
        return false;
    }

    std::size_t size;

    // Determine its size
    if (!getFileSize(f, size))
    {
        std::cerr << "Error determining file size\n";
        return false;
    }

    std::vector<unsigned char> temp;
    temp.resize(size);

    // Read the file contents into the vector
    if (size > 0)
        f.read(reinterpret_cast<char*>(temp.data()), static_cast<std::streamsize>(size));

    if (!f)
    {
        std::cerr << "Error reading the file!\n";
        return false;
    }

    // Move temp into the actual data array on read success
    data.swap(temp);

    return true;
}


bool readEntireFile(
    const char* fileName,
    std::string& data
)
{
    // Open the file in binary mode
    std::ifstream f(fileName, std::ios::binary);

    if (!f.is_open())
    {
        std::cerr << "Could not open file: " << fileName << '\n';
        return false;
    }

    std::size_t size;

    // Determine its size
    if (!getFileSize(f, size))
    {
        std::cerr << "Error determining file size\n";
        return false;
    }

    std::string temp;
    temp.resize(size);

    // Read the file contents into the string
    if (size > 0)
        f.read(temp.data(), static_cast<std::streamsize>(size));

    if (!f)
    {
        std::cerr << "Error reading the file!\n";
        return false;
    }

    // Move temp into the actual data string on read success
    data.swap(temp);

    return true;
}


bool readEntireFile(
    const char* fileName,
    void* buffer,
    std::size_t bufferSize,
    std::size_t& bytesRead
)
{
    bytesRead = 0;

    // Open the file in binary mode
    std::ifstream f(fileName, std::ios::binary);

    if (!f.is_open())
    {
        std::cerr << "Could not open file: " << fileName << '\n';
        return false;
    }

    std::size_t size;

    // Determine its size
    if (!getFileSize(f, size))
    {
        std::cerr << "Error determining file size\n";
        return false;
    }

    if (size > bufferSize)
    {
        std::cerr << "File contents too big for supplied buffer\n";
        return false;
    }

    // Read the file contents into the buffer
    if (size > 0)
        f.read(static_cast<char*>(buffer), static_cast<std::streamsize>(size));

    if (!f)
    {
        std::cerr << "Error reading the file!\n";
        return false;
    }

    bytesRead = size;

    return true;
}


bool writeEntireFile(
    const char* fileName,
    const void* data,
    std::size_t size
)
{
    std::ofstream f(fileName, std::ios::binary);

    if (!f.is_open())
        return false;

    f.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));

    if (!f)
        return false;

    f.close();

    return !f.fail();
}