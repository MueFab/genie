#ifndef GENIE_GABACFILE_HPP
#define GENIE_GABACFILE_HPP

#include <string>
#include <stdexcept>
#include <vector>
#include "gabac/configuration.h"
#include "gabac/gabac.h"

class GabacFile {
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

    void unpackFile(const std::string& fileName, const gabac::Configuration& conf) {
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

    void packFile(const std::string& fileName, const gabac::Configuration& conf) {
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

 private:

    uint64_t retrieveBlock(const gabac::Configuration& conf, std::vector<uint64_t>* symbols) {
        uint32_t blockSize;
        fread(&blockSize, sizeof(uint32_t), 1,  file);

        std::vector<uint8_t> buffer(blockSize);
        fread(buffer.data(), blockSize, 1, file);

        gabac::decode(&buffer, conf, {&std::cout, gabac::LogInfo::LogLevel::WARNING}, symbols);

        return blockSize + sizeof(uint32_t);
    }

    uint64_t appendBlock(std::vector<uint64_t>& symbols, const gabac::Configuration& conf) {
        std::vector<uint8_t> outBuffer;
        gabac::encode(conf, {&std::cout, gabac::LogInfo::LogLevel::WARNING}, &symbols,&outBuffer);
        uint64_t size = outBuffer.size();
        fwrite(&size, sizeof(uint32_t), 1, file);
        fwrite(outBuffer.data(), size, sizeof(uint8_t), file);
        return size + sizeof(uint32_t);
    }


    void generateSymbolStream(
            const std::vector<unsigned char>& buffer,
            unsigned int wordSize,
            std::vector<uint64_t> * const symbols
    ){
        assert((wordSize == 1) || (wordSize == 2) || (wordSize == 4) || (wordSize == 8));
        assert((buffer.size() % wordSize) == 0);
        assert(symbols != nullptr);

        // Note: as buffer is a vector of unsigned chars no masks (i.e. 0xff) need
        // to be applied before shifting the bits to the right position within a
        // symbol.

        // Prepare the (output) symbols vector
        symbols->clear();
        size_t symbolsSize = buffer.size() / wordSize;

        // Because we resize the symbols vector we have to get out here if the
        // buffer is empty.
        if (buffer.empty())
        {
            return;
        }
        symbols->resize(symbolsSize);

        // Multiplex every wordSize bytes into one symbol
        size_t symbolsIdx = 0;
        for (size_t i = 0; i < buffer.size(); i += wordSize)
        {
            uint64_t symbol = 0;
            switch (wordSize)
            {
                case 1:
                {
                    symbol = buffer[i];
                    break;
                }
                case 2:
                {
                    symbol = static_cast<uint16_t>(buffer[i + 1]) << 8u;
                    symbol |= static_cast<uint16_t>(buffer[i]);
                    break;
                }
                case 4:
                {
                    symbol = static_cast<uint32_t>(buffer[i + 3]) << 24u;
                    symbol |= static_cast<uint32_t>(buffer[i + 2]) << 16u;
                    symbol |= static_cast<uint32_t>(buffer[i + 1]) << 8u;
                    symbol |= static_cast<uint32_t>(buffer[i]);
                    break;
                }
                case 8:
                {
                    symbol = static_cast<uint64_t>(buffer[i + 7]) << 56u;
                    symbol |= static_cast<uint64_t>(buffer[i + 6]) << 48u;
                    symbol |= static_cast<uint64_t>(buffer[i + 5]) << 40u;
                    symbol |= static_cast<uint64_t>(buffer[i + 4]) << 32u;
                    symbol |= static_cast<uint64_t>(buffer[i + 3]) << 24u;
                    symbol |= static_cast<uint64_t>(buffer[i + 2]) << 16u;
                    symbol |= static_cast<uint64_t>(buffer[i + 1]) << 8u;
                    symbol |= static_cast<uint64_t>(buffer[i]);
                    break;
                }
                default:
                {
                    // The default case can happen if assertions are disabled.
                    // However, it should never happen and there is nothing we
                    // could do. So we bluntly abort the process.
                    abort();
                }
            }
            (*symbols)[symbolsIdx++] = symbol;
        }
    }


    void generateByteBuffer(
            const std::vector<uint64_t>& symbols,
            unsigned int wordSize,
            std::vector<unsigned char> * const buffer
    ){
        assert((wordSize == 1) || (wordSize == 2) || (wordSize == 4) || (wordSize == 8));
        assert(buffer != nullptr);

        // Prepare the (output) buffer
        buffer->clear();

        switch (wordSize)
        {
            case 1:
            {
                for (const auto& symbol : symbols)
                {
                    buffer->push_back(symbol & 0xff);
                }
                break;
            }
            case 2:
            {
                for (const auto& symbol : symbols)
                {
                    buffer->push_back(symbol & 0xff);
                    buffer->push_back((symbol >> 8u) & 0xff);
                }
                break;
            }
            case 4:
            {
                for (const auto& symbol : symbols)
                {
                    buffer->push_back(symbol & 0xff);
                    buffer->push_back((symbol >> 8u) & 0xff);
                    buffer->push_back((symbol >> 16u) & 0xff);
                    buffer->push_back((symbol >> 24u) & 0xff);
                }
                break;
            }
            case 8:
            {
                for (const auto& symbol : symbols)
                {
                    buffer->push_back(symbol & 0xff);
                    buffer->push_back((symbol >> 8u) & 0xff);
                    buffer->push_back((symbol >> 16u) & 0xff);
                    buffer->push_back((symbol >> 24u) & 0xff);
                    buffer->push_back((symbol >> 32u) & 0xff);
                    buffer->push_back((symbol >> 40u) & 0xff);
                    buffer->push_back((symbol >> 48u) & 0xff);
                    buffer->push_back((symbol >> 56u) & 0xff);
                }
                break;
            }
            default:
            {
                // The default case can happen if assertions are disabled.
                // However, it should never happen and there is nothing we
                // could do. So we bluntly abort the process.
                abort();
            }
        }
    }
};

#endif //GENIE_GABACFILE_HPP
