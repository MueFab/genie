/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_TRANSCODER_H_
#define SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_TRANSCODER_H_

// -----------------------------------------------------------------------------

#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/cigar_tokenizer.h"
#include "genie/core/record/record.h"
#include "genie/format/fasta/manager.h"
#include "genie/format/sam/sam_parameter.h"
#include "genie/format/sam/sam_to_mgrec/sam_record.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam::sam_to_mgrec {

// -----------------------------------------------------------------------------
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
bool save_mgrecs_by_rid(std::list<core::record::Record>& mpegg_recs,
                        std::map<int32_t, util::BitWriter>& bitwriters);

/**
 * @brief
 * @param options
 * @param chunk_id
 * @return
 */
std::vector<std::pair<std::string, size_t>> sam_to_mgrec_phase1(
    const Config& options, int& chunk_id);

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
 */
void clean_phase1_files(Config& options, int& nref);

/**
 * @brief
 * @param options
 */
void TranscodeSam2Mpg(Config& options);

/**
 *
 * @param options
 */
void TranscodeMpg2Sam(Config& options);

/**
 * @brief
 * @param r1
 * @param r2
 * @return
 */
bool compare(const core::record::Record& r1, const core::record::Record& r2);

/**
 * @brief
 */
class RefInfo {
  std::unique_ptr<core::ReferenceManager> ref_mgr_;  //!< @brief
  std::unique_ptr<fasta::Manager> fasta_mgr_;        //!< @brief
  std::unique_ptr<std::istream> fasta_file_;         //!< @brief
  std::unique_ptr<std::istream> fai_file_;           //!< @brief
  std::unique_ptr<std::istream> sha_file_;           //!< @brief
  bool valid_;                                       //!< @brief

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
  [[nodiscard]] bool IsValid() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] core::ReferenceManager* GetMgr() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_TO_MGREC_TRANSCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
