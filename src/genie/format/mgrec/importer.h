/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGREC_IMPORTER_H_
#define SRC_GENIE_FORMAT_MGREC_IMPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/classifier.h"
#include "genie/core/format-importer.h"
#include "genie/core/record/chunk.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/util/bitreader.h"
#include "genie/util/ordered-lock.h"
#include "genie/util/original-source.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgrec {

/**
 *
 */
class Importer : public core::FormatImporter {
 private:
    size_t blockSize;        //!<
    util::BitReader reader;  //!<

 public:
    /**
     *
     * @param _blockSize
     * @param _file_1
     */
    Importer(size_t _blockSize, std::istream& _file_1);

    /**
     *
     * @param _classifier
     * @return
     */
    bool pumpRetrieve(core::Classifier* _classifier) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgrec
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGREC_IMPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
