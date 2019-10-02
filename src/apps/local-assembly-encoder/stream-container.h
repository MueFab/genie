#ifndef GENIE_STREAM_CONTAINER_H
#define GENIE_STREAM_CONTAINER_H

#include <vector>
#include <cstdint>

struct StreamContainer {
    std::vector<uint64_t> pos_0;
    std::vector<uint64_t> rcomp_0;
    std::vector<uint64_t> mmpos_0;
    std::vector<uint64_t> mmpos_1;
    std::vector<uint64_t> mmtype_0;
    std::vector<uint64_t> mmtype_1;
    std::vector<uint64_t> mmtype_2;
    std::vector<uint64_t> clips_0;
    std::vector<uint64_t> clips_1;
    std::vector<uint64_t> clips_2;
    std::vector<uint64_t> clips_3;
    std::vector<uint64_t> rlen_0;
};

#endif //GENIE_STREAM_CONTAINER_H
