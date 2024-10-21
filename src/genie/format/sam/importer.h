/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_IMPORTER_H_
#define SRC_GENIE_FORMAT_SAM_IMPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <array>
#include <string>
#include <vector>
#include "genie/core/format-importer.h"
#include "genie/core/record/record.h"
#include "genie/core/stats/perf-stats.h"

#include "genie/util/ordered-lock.h"
#include "genie/util/original-source.h"
#include "genie/util/runtime-exception.h"
#include "genie/util/source.h"
#include "sam/sam_to_mgrec/sorter.h"
#include "sam/sam_to_mgrec/transcoder.h"

#include <queue>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::sam {

struct CmpReaders {
  bool operator()(const genieapp::transcode_sam::sam::sam_to_mgrec::SubfileReader * a,
      genieapp::transcode_sam::sam::sam_to_mgrec::SubfileReader * b) const;
};

/**
 * @brief Module to reads fastq files and convert them into MPEGG-Record format
 */
class Importer : public core::FormatImporter {
 private:
    static constexpr size_t LINES_PER_RECORD = 4;  //!< @brief How many lines in a fastq file belong to one record
    size_t blockSize;                              //!< @brief How many records to read in one pump() run
    util::OrderedLock lock;                        //!< @brief Lock to ensure in order execution
    bool phase1_complete;
    std::string input_sam_file;
    std::string input_ref_file;
    int nref;
    std::vector<std::pair<std::string, size_t>> refs;
    std::priority_queue <genieapp::transcode_sam::sam::sam_to_mgrec::SubfileReader *,
        std::vector<genieapp::transcode_sam::sam::sam_to_mgrec::SubfileReader *>, CmpReaders> reader_prio;
    std::vector<std::unique_ptr<genieapp::transcode_sam::sam::sam_to_mgrec::SubfileReader>> readers;
    std::vector<size_t> sam_hdr_to_fasta_lut;
    size_t removed_unsupported_base = 0;
    genieapp::transcode_sam::sam::sam_to_mgrec::RefInfo refinf;


        enum Lines {
            ID = 0,
            SEQUENCE = 1,
            RESERVED = 2,
            QUALITY = 3
        };  //!< @brief FASTQ format lines
    enum Files { FIRST = 0, SECOND = 1 };                            //!< @brief File shortcuts

    /**
     * @brief Read one chunk of fastq data
     * @param _file_list Where to read data from (2 streams in paired mode)
     * @return Data extracted from the fastq files, not converted yet
     */
    static std::vector<std::array<std::string, LINES_PER_RECORD>> readData(
        const std::vector<std::istream *> &_file_list);

    /**
     * @brief Check if read record data is actually valid for fastq files or if anything went wrong
     * @param data Data read previously
     */
    static void sanityCheck(const std::array<std::string, LINES_PER_RECORD> &data);

    /**
     * @brief Do the conversion to MPEG-G records
     * @param data Raw fastq data
     * @return Finished MPEG-G record
     */
    static core::record::Record buildRecord(std::vector<std::array<std::string, LINES_PER_RECORD>> data);

 public:
    /**
     * @brief Unpaired input
     * @param _blockSize How many records to extract per pump()
     * @param _file_1 Input file
     */
    Importer(size_t _blockSize, std::string input, std::string ref);
    void setup_merge(int num_chunks);

    /**
     * @brief
     * @param _classifier
     * @return
     */
    bool pumpRetrieve(core::Classifier *_classifier) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::fastq

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_IMPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
