// Copyright 2018 The genie authors

#include "genie-gabac-output-stream.h"

namespace lae {

GenieGabacOutputBuffer::GenieGabacOutputBuffer() : bytesLeft(0) {}

int GenieGabacOutputBuffer::overflow(int c) {
    char c2 = c;
    xsputn(&c2, sizeof(char));
    return c;
}

std::streamsize GenieGabacOutputBuffer::xsputn(const char *s, std::streamsize n) {
    std::streamsize curPos = 0;
    while (curPos < n) {
        // Fill size buffer
        while (bytesLeft == 0 && sizeBuf.size() != sizeof(uint32_t) && curPos < n) {
            sizeBuf.push_back(s[curPos++]);
        }

        // Size buffer full, create new stream
        if (sizeBuf.size() == sizeof(uint32_t)) {
            bytesLeft = *reinterpret_cast<uint32_t *>(sizeBuf.data());
            sizeBuf.clear();
            streams.emplace_back(bytesLeft, sizeof(uint8_t));
        }

        // Copy data if possible
        size_t l = std::min(n - curPos, bytesLeft);
        memcpy(static_cast<uint8_t *>(streams.back().getData()) + (streams.back().size() - bytesLeft), s + curPos, l);
        curPos += l;
        bytesLeft -= l;
    }
    return n;
}

void GenieGabacOutputBuffer::flush_blocks(std::vector<gabac::DataBlock> *dat) {
    if (bytesLeft) {
        GABAC_DIE("Stream not in flushable state, bytes left");
    }
    if (!sizeBuf.empty()) {
        GABAC_DIE("Stream not in flushable state, expecting size");
    }
    dat->clear();
    dat->swap(streams);
}
}  // namespace lae