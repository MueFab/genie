/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_TRANSCODER_SAM_SAM_TO_MGREC_SAM_READER_H_
#define SRC_APPS_TRANSCODER_SAM_SAM_TO_MGREC_SAM_READER_H_

#include <htslib/sam.h>
#include <string>

#include "apps/transcoder/sam/sam_to_mgrec/sam_record.h"

namespace genie {
namespace transcoder {
namespace sam {
namespace sam_to_mgrec {

class SamReader {
 private:
    samFile* sam_file;
    bam_hdr_t* sam_header;
    bam1_t* sam_alignment;
    kstring_t header_info;

 public:
    explicit SamReader(const char* fpath);

    explicit SamReader(std::string& fpath);

    ~SamReader();

    int getNumRef();

    bool isReady();

    bool isValid();

    int readSamRecord(SamRecord& sr);
};

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcoder
}  // namespace genie

#endif  // SRC_APPS_TRANSCODER_SAM_SAM_TO_MGREC_SAM_READER_H_
