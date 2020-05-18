/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_READER_H
#define GENIE_READER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <istream>
#include "header/header.h"
#include "record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

class Reader {
   private:
    std::istream& stream;
    header::Header header;
    Record save;
    bool rec_saved;

   public:
    explicit Reader(std::istream& _stream);

    const header::Header& getHeader() const;

    void read(size_t num, std::vector<Record>& vec, core::stats::PerfStats& stats);

    bool isEnd();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_READER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
