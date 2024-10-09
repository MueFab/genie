/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <cstring>
#include <string>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

#include "apps/genie/transcode-sam/sam/sam_to_mgrec/sam_reader.h"
#include "apps/genie/transcode-sam/sam/sam_to_mgrec/sam_record.h"
#include "util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp::transcode_sam::sam::sam_to_mgrec {

// ---------------------------------------------------------------------------------------------------------------------

SamReader::SamReader(const std::string& fpath)
    : sam_file(nullptr),           // open bam file
      sam_header(nullptr),         // read header
      sam_alignment(bam_init1()),  // initialize an alignment
      header_info(KS_INITIALIZE) {
    if (fpath.substr(0, 2) == "-.") {
        sam_file = hts_open("-", "r");
    } else {
        sam_file = hts_open(fpath.c_str(), "r");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

SamReader::~SamReader() {
    bam_destroy1(sam_alignment);
    bam_hdr_destroy(sam_header);
    if (sam_file) sam_close(sam_file);
    if (header_info.s) free(header_info.s);
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::pair<std::string, size_t>> SamReader::getRefs() {
    std::vector<std::pair<std::string, size_t>> ret;
    auto num_ref = sam_hdr_nref(sam_header);
    ret.reserve(num_ref);
    for (int i = 0; i < num_ref; ++i) {
        ret.emplace_back(sam_hdr_tid2name(sam_header, i), sam_hdr_tid2len(sam_header, i));
    }
    return ret;
}

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
    /// Find out if records are sorted by query name
    UTILS_DIE_IF(
        sam_hdr_find_tag_hd(sam_header, "SO", &header_info) != 0 || std::strcmp(header_info.s, "queryname") != 0,
        "Sam file must be ordered by read name! That ordering must be documented in the SAM header.");

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

int SamReader::readSamQuery(std::vector<SamRecord>& sr) {
    sr.clear();
    if (buffered_rec) {
        sr.push_back(std::move(buffered_rec.value()));
        buffered_rec.reset();
    } else {
        auto res = sam_read1(sam_file, sam_header, sam_alignment);
        if (res >= 0) {
            sr.emplace_back(sam_alignment);
        } else {
            return res;
        }
    }

    while (true) {
        auto res = sam_read1(sam_file, sam_header, sam_alignment);
        if (res >= 0) {
            buffered_rec = SamRecord(sam_alignment);
        } else {
            return res;
        }
        if (buffered_rec->qname != sr.front().qname) {
            return 0;
        } else {
            sr.push_back(std::move(buffered_rec.value()));
            buffered_rec.reset();
        }
    }

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genieapp::transcode_sam::sam::sam_to_mgrec

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
