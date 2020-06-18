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
#include <genie/core/stats/perf-stats.h>
#include <genie/util/ordered-lock.h>
#include <genie/util/watch.h>
#include <list>
#include <map>
#include "reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

/**
 *
 */
class Importer : public core::FormatImporter {
   private:
    size_t blockSize;  //!<
    Reader samReader;  //!<

   public:
    /**
     *
     * @param _blockSize
     * @param _file
     */
    Importer(size_t _blockSize, std::istream &_file);

    /**
     *
     * @param flags
     * @return
     */
    static std::tuple<bool, uint8_t> convertFlags2Mpeg(uint16_t flags);

    /**
     *
     * @param token
     * @return
     */
    static char convertCigar2ECigarChar(char token);

    /**
     *
     * @param token
     * @return
     */
    static int stepSequence(char token);

    /**
     *
     * @param cigar
     * @param seq
     * @return
     */
    static std::string convertCigar2ECigar(const std::string &cigar, const std::string &seq);

    std::map<std::string, size_t> refs;  //!<
    size_t ref_counter;                  //!<

    /**
     *
     * @param ref
     * @param _r1
     * @param _r2
     * @return
     */
    static core::record::Record convert(uint16_t ref, sam::Record &&_r1, sam::Record *_r2);

    /**
     *
     * @param _classifier
     * @return
     */
    bool pumpRetrieve(genie::core::Classifier *_classifier) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
