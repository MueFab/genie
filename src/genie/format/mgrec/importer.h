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
#include "genie/core/record/alignment/chunk.h"
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
 * @brief
 */
class Importer : public core::FormatImporter {
 private:
    size_t blockSize;        //!< @brief
    util::BitReader reader;  //!< @brief
    util::BitWriter writer;  //!< @brief

    size_t discarded_splices{};              //!< @brief
    size_t discarded_long_distance{};        //!< @brief
    size_t discarded_HM{};                   //!< @brief
    size_t discarded_missing_pair_U{};       //!< @brief
    size_t missing_additional_alignments{};  //!< @brief

    boost::optional<core::record::Record> bufferedRecord;  //!< @brief
    bool checkSupport;                                     //!< @brief

    bool isRecordSupported(const core::record::Record& rec);  //!< @brief

 public:
    /**
     * @brief
     * @param _blockSize
     * @param _file_1
     * @param _unsupported
     * @param checkSupport
     */
    Importer(size_t _blockSize, std::istream& _file_1, std::ostream& _unsupported, bool checkSupport = true);

    /**
     * @brief
     * @param _classifier
     * @return
     */
    bool pumpRetrieve(core::Classifier* _classifier) override;

    /**
     * @brief
     */
    void printStats() const;

    /**
     * @brief
     * @param pos
     */
    void flushIn(uint64_t& pos) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgrec
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGREC_IMPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
