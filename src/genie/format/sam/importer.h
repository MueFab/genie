/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_IMPORTER_H_
#define SRC_GENIE_FORMAT_SAM_IMPORTER_H_

#define PHASE1_EXT ".phase1.mgrec"
#define PHASE2_BUFFER_SIZE 1000000

// ---------------------------------------------------------------------------------------------------------------------


#include <genie/format/fasta/manager.h>
#include <genie/format/sam/sam_parameter.h>

#include <array>
#include <queue>
#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "genie/core/format-importer.h"
#include "genie/core/record/record.h"
#include "genie/core/stats/perf-stats.h"

#include "genie/util/ordered-lock.h"
#include "genie/util/original-source.h"
#include "genie/util/runtime-exception.h"
#include "genie/util/source.h"
#include "sam_to_mgrec/sorter.h"


// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::sam {

class RefInfo {
 private:
    std::unique_ptr<genie::core::ReferenceManager> refMgr;    //!< @brief
    std::unique_ptr<genie::format::fasta::Manager> fastaMgr;  //!< @brief
    std::unique_ptr<std::istream> fastaFile;                  //!< @brief
    std::unique_ptr<std::istream> faiFile;                    //!< @brief
    std::unique_ptr<std::istream> shaFile;                    //!< @brief
    bool valid;                                               //!< @brief

 public:
    /**
     * @brief
     * @param fasta_name
     */
    explicit RefInfo(const std::string &fasta_name);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool isValid() const;

    /**
     * @brief
     * @return
     */
    genie::core::ReferenceManager *getMgr();
};

struct CmpReaders {
    bool operator()(const sam_to_mgrec::SubfileReader *a, sam_to_mgrec::SubfileReader *b) const;
};

/**
 * @brief Module to reads fastq files and convert them into MPEGG-Record format
 */
class Importer : public core::FormatImporter {
 private:
    size_t blockSize;                              //!< @brief How many records to read in one pump() run
    // util::OrderedLock lock;                        //!< @brief Lock to ensure in order execution
    std::string input_sam_file;
    std::string input_ref_file;
    int nref;
    bool phase1_complete;
    std::vector<std::pair<std::string, size_t>> refs;
    std::priority_queue<sam_to_mgrec::SubfileReader *, std::vector<sam_to_mgrec::SubfileReader *>, CmpReaders>
        reader_prio;
    std::vector<std::unique_ptr<sam_to_mgrec::SubfileReader>> readers;
    std::vector<size_t> sam_hdr_to_fasta_lut;
    size_t removed_unsupported_base = 0;
    RefInfo refinf;

 public:
    Importer(size_t _blockSize, std::string input, std::string ref);

    std::vector<std::pair<std::string, size_t>> sam_to_mgrec_phase1(format::sam::Config &options,
                                                                    int &chunk_id);

    void setup_merge(int num_chunks);

    /**
     * @brief
     * @param _classifier
     * @return
     */
    bool pumpRetrieve(core::Classifier *_classifier) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::sam

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_IMPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
