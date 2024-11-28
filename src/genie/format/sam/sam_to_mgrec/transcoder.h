/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_TRANSCODER_H_
#define SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_TRANSCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/cigar-tokenizer.h"
#include "genie/core/record/record.h"
#include "genie/format/fasta/manager.h"
#include "genie/format/sam/sam_parameter.h"
#include "genie/format/sam/sam_to_mgrec/sam_record.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::sam::sam_to_mgrec {

// ---------------------------------------------------------------------------------------------------------------------

#define PHASE1_EXT ".phase1.mgrec"
#define PHASE2_EXT ".phase2.mgrec"
#define PHASE2_TMP_EXT ".phase2.tmp"
// #define PHASE1_BUFFER_SIZE 50000
#define PHASE2_BUFFER_SIZE 1000000

/**
 * @brief
 * @param mpegg_recs
 * @param bitwriters
 * @return
 */
bool save_mgrecs_by_rid(std::list<genie::core::record::Record>& mpegg_recs,
                        std::map<int32_t, genie::util::BitWriter>& bitwriters);

/**
 * @brief
 * @param options
 * @param nref
 * @return
 */
std::vector<std::pair<std::string, size_t>> sam_to_mgrec_phase1(Config& options, int& nref);

/**
 * @brief
 * @param options
 * @param rid
 * @param ifile
 * @return
 */
std::string gen_p2_tmp_fpath(Config& options, int rid, int ifile);

/**
 * @brief
 * @param options
 * @param nref
 * @return
 */
void sam_to_mgrec_phase2(Config& options, int nref,
                         const std::vector<std::pair<std::string, size_t>>& refs);

/**
 * @brief
 * @param options
 * @param nref
 */
void clean_phase1_files(Config& options, int& nref);

/**
 * @brief
 * @param options
 * @return
 */
void transcode_sam2mpg(Config& options);

/**
 *
 * @param options
 * @return
 */
void transcode_mpg2sam(Config& options);

/**
 * @brief
 * @param r1
 * @param r2
 * @return
 */
bool compare(const genie::core::record::Record& r1, const genie::core::record::Record& r2);

/**
 * @brief
 */
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
    explicit RefInfo(const std::string& fasta_name);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool isValid() const;

    /**
     * @brief
     * @return
     */
    genie::core::ReferenceManager* getMgr();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_TRANSCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
