#ifndef GENIE_READ_ENCODER_H
#define GENIE_READ_ENCODER_H

#include <util/sam-record.h>
#include <memory>
#include <string>
#include <vector>
#include "stream-container.h"

namespace lae {
class LocalAssemblyReadEncoder {
   private:
    std::unique_ptr<StreamContainer> container;
    uint32_t pos;
    uint32_t readCounter;
    void codeVariants(const std::string& read, const std::string& cigar, const std::string& ref, bool isFirst);

   public:
    explicit LocalAssemblyReadEncoder();

    void addRead(const util::SamRecord& rec, const std::string& ref, bool isFirst = true);
    void addPair(const util::SamRecord& rec1, const std::string& ref1, const util::SamRecord& rec2,
                 const std::string& ref2);

    std::unique_ptr<StreamContainer> pollStreams();
};
}  // namespace lae

#endif  // GENIE_READ_ENCODER_H
