#ifndef GENIE_SAM_READER_H
#define GENIE_SAM_READER_H

#include <istream>
#include "sam-header.h"
#include "sam-record.h"

namespace genie {
namespace sam {
class SamReader {
   private:
    std::istream& stream;
    SamFileHeader header;
    SamRecord save;
    bool rec_saved;

   public:
    explicit SamReader(std::istream& _stream);

    const SamFileHeader& getHeader() const;

    void read(size_t num, std::vector<SamRecord>& vec);

    bool isEnd();
};
}  // namespace sam
}  // namespace genie
#endif  // GENIE_SAM_READER_H
