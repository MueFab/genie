#ifndef GENIE_READ_ENCODER_H
#define GENIE_READ_ENCODER_H

#include <memory>
#include <string>
#include <vector>
#include <format/sam/sam-record.h>
#include "format/mpegg_rec/mpegg-record.h"
#include "stream-container.h"

namespace lae {
    class LocalAssemblyReadEncoder {
    private:
        std::unique_ptr<StreamContainer> container;
        int32_t pos;
        uint32_t readCounter;
        void codeVariants(const std::string &read, const std::string &cigar, const std::string& ref, format::mpegg_rec::MpeggRecord::ClassType type, bool isFirst);

        format::mpegg_rec::MpeggRecord::ClassType getClass(const std::string &read, const std::string &cigar, const std::string &ref);

        void addSingleRead(const format::sam::SamRecord& rec, format::mpegg_rec::MpeggRecord::ClassType type);
    public:
        explicit LocalAssemblyReadEncoder();

        format::mpegg_rec::MpeggRecord::ClassType addRead(const format::sam::SamRecord& rec, const std::string& ref);
        format::mpegg_rec::MpeggRecord::ClassType addPair(const format::sam::SamRecord& rec1, const std::string& ref1, const format::sam::SamRecord& rec2, const std::string& ref2);

        std::unique_ptr<StreamContainer> pollStreams();
    };
}

#endif //GENIE_READ_ENCODER_H
