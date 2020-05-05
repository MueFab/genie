/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SAM_IMPORTER_H
#define GENIE_SAM_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/format-importer.h>
#include <genie/core/record/record.h>
#include <genie/util/ordered-lock.h>
#include <map>
#include "reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

class Importer : public core::FormatImporter {
   private:
    size_t blockSize;
    Reader samReader;
    ReadTemplateGroup rtg;
    util::OrderedLock lock;  //!< @brief Lock to ensure in order execution

   public:
    Importer(size_t _blockSize, std::istream &_file);

    static std::tuple<bool, uint8_t> convertFlags2Mpeg(uint16_t flags);

    static char convertCigar2ECigarChar(char token);

    static int stepSequence(char token);

    static std::string convertCigar2ECigar(const std::string &cigar, const std::string &seq);

    std::map<std::string, size_t> refs;
    size_t ref_counter;

    static core::record::Record convert(uint16_t ref, sam::Record &&_r1, sam::Record *_r2);

    bool pump(size_t id) override;

    void dryIn() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------