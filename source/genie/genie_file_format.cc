// Copyright 2018 The genie authors

#include "genie/genie_file_format.h"

#include <iostream>

namespace dsg {

void packFile(const std::string& path, const std::string& file, FILE *fout){
    // Open output file
    FILE *fin_desc = fopen((path + file).c_str(), "rb");
    if (!fin_desc) {
        throw std::runtime_error("Could not open " + (path + file));
    }

    // Write name of input file
    uint64_t size = file.size();
    if (fwrite(&size, sizeof(uint64_t), 1, fout) != 1) {
        fclose(fin_desc);
        throw std::runtime_error("Could not write to output file");
    }
    if (fwrite(file.c_str(), 1, size, fout) != size) {
        fclose(fin_desc);
        throw std::runtime_error("Could not write to output file");
    }

    // Get and write size of input file
    fseek(fin_desc, 0, SEEK_END);
    size = ftell(fin_desc);
    if (fwrite(&size, sizeof(uint64_t), 1, fout) != 1) {
        fclose(fin_desc);
        throw std::runtime_error("Could not write to output file");
    }
    fseek(fin_desc, 0, SEEK_SET);

    // Copy input file in blocks
    uint64_t byteswritten = 0;
    std::vector<uint8_t> buffer(1000000);
    while (byteswritten < size) {
        uint64_t tmp =
                fread(buffer.data(), 1, std::min(buffer.size(), static_cast<size_t>(size - byteswritten)), fin_desc);
        // Read block
        if (tmp != std::min(buffer.size(), static_cast<size_t>(size - byteswritten))) {
            fclose(fin_desc);
            throw std::runtime_error("Could not read from " + (path + file));
        }

        // Write block
        tmp = fwrite(buffer.data(), 1, tmp, fout);
        if (tmp != std::min(buffer.size(), static_cast<size_t>(size - byteswritten))) {
            fclose(fin_desc);
            throw std::runtime_error("Could not write to output file");
        }
        byteswritten += tmp;
    }

    fclose(fin_desc);
    std::cout << "Successfully packed " << (path + file) << std::endl;

}


std::string packFiles(const std::vector<std::string>& list, FILE *fout){
    std::string path;
    for (const auto& k : list) {
        size_t pos = k.find_last_of('/') + 1;
        path = k.substr(0, pos);
        packFile(path, k.substr(pos, std::string::npos), fout);
    }

    return path;
}

std::string unpackFile(const std::string& path, FILE *fin){
    // Get name of file
    uint64_t size;
    std::vector<uint8_t> buffer(1000000);
    if (fread(&size, sizeof(uint64_t), 1, fin) != 1) {
        throw std::runtime_error("Could not read from file #1");
    }
    std::string filename;
    filename.resize(size);
    if (fread((void *) filename.data(), sizeof(uint8_t), size, fin) != size) {
        throw std::runtime_error("Could not read from file #2");
    }

    // Get size of file
    if (fread(&size, sizeof(uint64_t), 1, fin) != 1) {
        throw std::runtime_error("Could not read from file #3");
    }
    FILE *fout = fopen((path + filename).c_str(), "wb");
    if (!fout) {
        throw std::runtime_error("Could not open output file #4");
    }

    // Copy file in blocks
    uint64_t readBytes = 0;
    while (readBytes < size) {
        uint64_t tmp = fread(buffer.data(), 1, std::min(buffer.size(), static_cast<size_t>(size - readBytes)), fin);

        // Read
        if (tmp != std::min(buffer.size(), static_cast<size_t>(size - readBytes))) {
            fclose(fout);
            throw std::runtime_error("Could not read from input file #5");
        }
        readBytes += tmp;

        // Write
        if (fwrite(buffer.data(), 1, tmp, fout) != tmp) {
            fclose(fout);
            throw std::runtime_error("Could not write to output file #6");
        }
    }

    fclose(fout);

    std::cout << "Successfully unpacked " << (path + filename) << std::endl;

    return path + filename;
}


std::vector<std::string> unpackFiles(const std::string& path, FILE *fin){
    uint64_t pos = ftell(fin);
    fseek(fin, 0, SEEK_END);
    uint64_t end = ftell(fin);
    fseek(fin, pos, SEEK_SET);

    std::vector<std::string> filelist;

    while (ftell(fin) < end) {
        filelist.push_back(unpackFile(path, fin));
    }

    return filelist;
}

std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> createMap(const std::vector<std::string>& filelist){
    std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> map;
    for (const auto& f : filelist) {

        // Find ref streams
        size_t pos = std::string::npos;
        size_t tmp = 0;
        while ((tmp = f.find("ref_subseq_", tmp)) != std::string::npos) {
            pos = tmp;
            tmp++;
        }
        if (pos == std::string::npos) {
            continue;
        }

        // Extract indices from file name
        // TODO: This only works for indices < 10
        pos += std::string("ref_subseq_").length();
        if (f.size() < pos + 5) {
            continue;
        }
        uint8_t j = f[pos] - '0';
        uint8_t k = f[pos + 2] - '0';

        uint8_t i = atoi(f.substr(pos+4, f.length() - pos - 4).c_str());

        if (map.size() <= i) {
            map.resize(i + 1);
        }

        map[i][j][k] = f;
    }
    return map;
}

}
