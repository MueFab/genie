// Copyright 2018 The genie authors

#ifndef GENIE_GENIE_FILE_FORMAT_H
#define GENIE_GENIE_FILE_FORMAT_H

#include <string>
#include <vector>
#include <map>

namespace dsg {

// Append one single file to a genie archive
void packFile(const std::string& path, const std::string& file, FILE *fout);

// Append a list of files to a genie archive
std::string packFiles(const std::vector<std::string>& list, FILE *fout);

// Retrieve one single file from a genie archive
std::string unpackFile(const std::string& path, FILE *fin);

// Retrieve all files from a genie archive
std::vector<std::string> unpackFiles(const std::string& path, FILE *fin);

// Generate a file map from a file list as required by spring
std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> createMap(const std::vector<std::string>& filelist);
}

#endif //GENIE_GENIE_FILE_FORMAT_H
