/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_GENIE_TRANSCODE_SAM_SAM_SAM_TO_MGREC_TRANSCODER_H_
#define SRC_APPS_GENIE_TRANSCODE_SAM_SAM_SAM_TO_MGREC_TRANSCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "apps/genie/transcode-sam/sam/sam_to_mgrec/sam_record.h"
#include "apps/genie/transcode-sam/transcoding.h"
#include "genie/core/cigar_tokenizer.h"
#include "genie/core/record/alignment/record.h"
#include "genie/format/fasta/manager.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace transcode_sam {
namespace sam {
namespace sam_to_mgrec {

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
void sam_to_mgrec_phase2(Config& options, int nref, const std::vector<std::pair<std::string, size_t>>& refs);

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
    bool isValid() const;

    /**
     * @brief
     * @return
     */
    genie::core::ReferenceManager* getMgr();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcode_sam
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_TRANSCODE_SAM_SAM_SAM_TO_MGREC_TRANSCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
