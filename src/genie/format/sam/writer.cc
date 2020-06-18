/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

Writer::Writer(header::Header&& fileHeader, std::ostream& stream) : header(std::move(fileHeader)), file(stream) {
    header.print(file);
}

// ---------------------------------------------------------------------------------------------------------------------

const header::Header& Writer::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

void Writer::write(sam::Record&& rec) {
    sam::Record v = std::move(rec);
    auto str = v.toString();
    str += '\n';
    file.write(str.data(), str.length());
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------