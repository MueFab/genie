/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_WRITER_H_
#define SRC_GENIE_FORMAT_SAM_WRITER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <ostream>
#include "genie/format/sam/header/header.h"
#include "genie/format/sam/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

class Writer {
 private:
    header::Header header;
    std::ostream& file;

 public:
    Writer(header::Header&& fileHeader, std::ostream& stream);
    const header::Header& getHeader() const;

    void write(sam::Record&& rec);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_WRITER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
