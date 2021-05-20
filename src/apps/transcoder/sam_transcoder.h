#ifndef SAM_TRANSCODER_H
#define SAM_TRANSCODER_H

#include <vector>
#include <list>
#include <genie/core/record/record.h>
#include <genie/util/bitwriter.h>

#include "sam_record.h"

namespace sam_transcoder {

#define PHASE1_EXT ".phase1.mgrec"
#define PHASE2_EXT ".phase2.mgrec"
#define PHASE2_TMP_EXT ".phase2.tmp"
#define PHASE1_BUFFER_SIZE 50000
#define PHASE2_BUFFER_SIZE 500000

bool save_mgrecs_by_rid(std::list<genie::core::record::Record>& mpegg_recs,
                        std::map<int32_t,
                        genie::util::BitWriter>& bitwriters);

uint8_t sam_to_mgrec_phase1(transcoder::ProgramOptions& options,
                            int& nref);

std::string gen_p2_tmp_fpath(transcoder::ProgramOptions& options,
                             int rid,
                             int ifile);

uint8_t sam_to_mgrec_phase2(transcoder::ProgramOptions& options,
                            int& nref);

uint8_t sam_to_mgrec(transcoder::ProgramOptions& options);

}

#endif  // SAM_TRANSCODER_H
