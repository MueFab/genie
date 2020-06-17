/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_WRITER_H
#define GENIE_WRITER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <ostream>
#include "header/header.h"
#include "record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

/**
 *
 */
class Writer {
   private:
    header::Header header; //!<
    std::ostream& file;//!<

   public:
    /**
     *
     * @param fileHeader
     * @param stream
     */
    Writer(header::Header&& fileHeader, std::ostream& stream);

    /**
     *
     * @return
     */
    const header::Header& getHeader() const;

    /**
     *
     * @param rec
     */
    void write(sam::Record&& rec);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_WRITER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------