#ifndef GENIE_GABACFILE_HPP
#define GENIE_GABACFILE_HPP

#include <string>
#include <stdexcept>
#include <vector>
// #include "gabac/gabac.h"

/*class GabacFile {
    FILE* file;
    bool reader;
 public:
    explicit GabacFile(const std::string& fileName, bool read) : reader(read){
        if(!read) {
            file = fopen(fileName.c_str(), "wb");
        } else {
            file = fopen(fileName.c_str(), "rb");
        }
        if(!file)
            throw std::runtime_error("Could not open file for gabac: " + fileName);
    }

    void unpackFile(const std::string& fileName, const gabac::EncodingConfiguration& conf) {
        if(!reader){
            throw std::runtime_error("Gabac unpacking in reader mode only" + fileName);
        }
        FILE* outFile;
        outFile = fopen(fileName.c_str(), "wb");

        if(!outFile)
            throw std::runtime_error("Could not write file for gabac: " + fileName);

        uint64_t fileSize = 0;
        fread(&fileSize, sizeof(uint64_t), 1, file);
        uint64_t currentSize = 0;

        while(currentSize < fileSize) {
            std::vector<uint64_t> buffer;
            currentSize += retrieveBlock(conf, &buffer);
            std::vector<uint8_t> outbuffer;
            generateByteBuffer(buffer, conf.wordSize, &outbuffer);
            fwrite(outbuffer.data(), outbuffer.size(), 1, outFile);
        }

        fclose(outFile);


    }

    void packFile(const std::string& fileName, const gabac::EncodingConfiguration& conf) {
        if(reader){
            throw std::runtime_error("Gabac packing in writer mode only" + fileName);
        }
        FILE* inFile;
        inFile = fopen(fileName.c_str(), "rb");

        if(!inFile)
            throw std::runtime_error("Could not open file for gabac: " + fileName);

        const size_t BLOCKSIZE = 10000000;
        std::vector<uint8_t> buffer(BLOCKSIZE);
        size_t readSize = 0;

        long fileSizePos = ftell(file);

        uint64_t fileSize = 0;

        fwrite(&fileSize, 1, sizeof(uint64_t), file);

        do {
            readSize = fread(buffer.data(), sizeof(uint8_t), BLOCKSIZE, inFile);
            if(ferror(inFile)) {
                perror("Error");
            }
            std::vector<uint64_t> symbols;
            buffer.resize(readSize);
            generateSymbolStream(buffer, conf.wordSize, &symbols);
            fileSize += appendBlock(symbols, conf);
        } while (readSize == BLOCKSIZE);

        fseek(file, fileSizePos, SEEK_SET);
        fwrite(&fileSize, 1, sizeof(uint64_t), file);
        fseek(file, 0, SEEK_END);
        fclose(inFile);
    }

    ~GabacFile(){
        fclose(file);
    }
};*/

#endif //GENIE_GABACFILE_HPP
