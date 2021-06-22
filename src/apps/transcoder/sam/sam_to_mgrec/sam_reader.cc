/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <cstring>

// ---------------------------------------------------------------------------------------------------------------------

#include "apps/transcoder/sam/sam_to_mgrec/sam_reader.h"
#include "apps/transcoder/sam/sam_to_mgrec/sam_record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace transcoder {
namespace sam {
namespace sam_to_mgrec {

// ---------------------------------------------------------------------------------------------------------------------

SamReader::SamReader(const char* fpath)
    : sam_file(hts_open(fpath, "r")),  // open bam file
      sam_header(nullptr),             // read header
      sam_alignment(bam_init1()),      // initialize an alignment
      header_info(KS_INITIALIZE) {}

// ---------------------------------------------------------------------------------------------------------------------

SamReader::SamReader(std::string& fpath) : SamReader(fpath.c_str()) {}

// ---------------------------------------------------------------------------------------------------------------------

SamReader::~SamReader() {
    bam_destroy1(sam_alignment);
    bam_hdr_destroy(sam_header);
    if (sam_file) sam_close(sam_file);
    if (header_info.s) free(header_info.s);
}

// ---------------------------------------------------------------------------------------------------------------------

int SamReader::getNumRef() { return sam_hdr_nref(sam_header); }

// ---------------------------------------------------------------------------------------------------------------------

bool SamReader::isReady() {
    if (!sam_file) {
        return false;
    }

    sam_header = sam_hdr_read(sam_file);  // read header
    if (!sam_header) {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

bool SamReader::isValid() {
    /// Find Tag HD with key "SO" to find out the ordering
    if (sam_hdr_find_tag_hd(sam_header, "SO", &header_info) != 0) {
        return false;
    }

    /// Find out if records are sorted by query name
    if (std::strcmp(header_info.s, "queryname") != 0) {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

int SamReader::readSamRecord(SamRecord& sr) {
    auto res = sam_read1(sam_file, sam_header, sam_alignment);

    if (res >= 0) {
        sr = SamRecord(sam_alignment);
    }

    return res;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcoder
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
