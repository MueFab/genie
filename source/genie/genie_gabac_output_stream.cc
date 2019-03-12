// Copyright 2018 The genie authors

#include "genie/genie_gabac_output_stream.h"
namespace dsg {

    GenieGabacOutputStream::GenieGabacOutputStream() : bytes(0){
    }

    size_t GenieGabacOutputStream::writeStream(gabac::DataBlock *inbuffer){
        size_t currSize = inbuffer->size() * inbuffer->getWordSize();
        bytes += currSize;
        streams.emplace_back(currSize, static_cast<uint8_t *>(malloc(currSize)));
        memcpy(streams.back().second, inbuffer->getData(), currSize);
        return currSize;
    }

    size_t GenieGabacOutputStream::writeBytes(gabac::DataBlock *){
        throw std::runtime_error("WriteBytes not supported by gabac-genie-outputstream!");
    }

    size_t GenieGabacOutputStream::bytesWritten(){
        return bytes;
    }

    void GenieGabacOutputStream::flush(std::vector <std::pair<size_t, uint8_t *>> *dat){
        dat->swap(streams);
        bytes = 0;
    }

}