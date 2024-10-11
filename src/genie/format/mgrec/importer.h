/**
 * @file importer.h
 * @brief Header file for the MGREC format importer module in GENIE.
 *
 * This file defines the `Importer` class used for reading MGREC formatted files
 * and converting them into an internal record structure for use in the GENIE framework.
 * The importer handles various record validation, supports performance tracking,
 * and manages unsupported records.
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGREC_IMPORTER_H_
#define SRC_GENIE_FORMAT_MGREC_IMPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/classifier.h"
#include "genie/core/format-importer.h"
#include "genie/core/record/chunk.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/util/bit-reader.h"
#include "genie/util/ordered-lock.h"
#include "genie/util/original-source.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgrec {

/**
 * @class Importer
 * @brief MGREC format importer for reading and processing records.
 *
 * The `Importer` class is designed to handle the import of MGREC formatted files,
 * processing them into the GENIE framework's internal record structure. It provides
 * functionality for validating record types, tracking performance metrics, and
 * handling unsupported records.
 */
class Importer : public core::FormatImporter {
 private:
    size_t blockSize;        //!< Block size for processing chunks
    util::BitReader reader;  //!< Bit reader for the input stream
    util::BitWriter writer;  //!< Bit writer for unsupported record output

    size_t discarded_splices{};              //!< Number of discarded spliced records
    size_t discarded_long_distance{};        //!< Number of discarded long distance records
    size_t discarded_HM{};                   //!< Number of discarded hard mask records
    size_t discarded_missing_pair_U{};       //!< Number of discarded unmatched pair records
    size_t missing_additional_alignments{};  //!< Number of records missing additional alignments

    std::optional<core::record::Record> bufferedRecord;  //!< Buffer to hold the current record being processed
    bool checkSupport;                                   //!< Flag to enable/disable record support checking

    /**
     * @brief Check if a given record is supported by the importer.
     * @param rec Record to be validated.
     * @return `true` if the record is supported, `false` otherwise.
     */
    bool isRecordSupported(const core::record::Record& rec);

 public:
    /**
     * @brief Construct an MGREC importer.
     * @param _blockSize Size of blocks to read from the input.
     * @param _file_1 Input stream for the MGREC file.
     * @param _unsupported Output stream for unsupported records.
     * @param checkSupport Flag to enable/disable support checking.
     */
    Importer(size_t _blockSize, std::istream& _file_1, std::ostream& _unsupported, bool checkSupport = true);

    /**
     * @brief Retrieve and classify records from the input stream.
     * @param _classifier Classifier for managing records.
     * @return `true` if records were successfully retrieved, `false` otherwise.
     */
    bool pumpRetrieve(core::Classifier* _classifier) override;

    /**
     * @brief Print statistics about discarded records.
     */
    void printStats() const;

    /**
     * @brief Flush any remaining records into the classifier.
     * @param pos Current position indicator.
     */
    void flushIn(uint64_t& pos) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgrec

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGREC_IMPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
