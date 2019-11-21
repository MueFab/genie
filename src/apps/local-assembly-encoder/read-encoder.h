#ifndef GENIE_READ_ENCODER_H
#define GENIE_READ_ENCODER_H

#include <memory>
#include <string>
#include <vector>
#include <util/sam-record.h>
#include "format/part2/data_unit.h"
#include "stream-container.h"

namespace lae {
    class LocalAssemblyReadEncoder {
    private:
        std::unique_ptr<StreamContainer> container;
        int32_t pos;
        uint32_t readCounter;
        void codeVariants(const std::string &read, const std::string &cigar, const std::string& ref, format::DataUnit::AuType type, bool isFirst);

        format::DataUnit::AuType getClass(const std::string &read, const std::string &cigar, const std::string &ref);

        void addSingleRead(const util::SamRecord& rec, format::DataUnit::AuType type);
    public:
        explicit LocalAssemblyReadEncoder();

        format::DataUnit::AuType addRead(const util::SamRecord& rec, const std::string& ref);
        format::DataUnit::AuType addPair(const util::SamRecord& rec1, const std::string& ref1, const util::SamRecord& rec2, const std::string& ref2);

        std::unique_ptr<StreamContainer> pollStreams();
    };
}

#endif //GENIE_READ_ENCODER_H
