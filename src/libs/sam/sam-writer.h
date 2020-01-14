#ifndef GENIE_SAM_WRITER_H
#define GENIE_SAM_WRITER_H

#include "sam-header.h"
#include "sam-record.h"

namespace genie {
namespace sam {
class SamWriter {
   private:
    sam::SamFileHeader header;
    std::ostream& file;

   public:
    SamWriter(sam::SamFileHeader&& fileHeader, std::ostream& stream);
    const sam::SamFileHeader& getHeader() const;

    void write(sam::SamRecord&& rec);
};
}  // namespace sam
}  // namespace genie

#endif  // GENIE_SAM_WRITER_H
