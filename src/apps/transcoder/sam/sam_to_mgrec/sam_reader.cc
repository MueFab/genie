#include "sam_reader.h"
#include "sam_record.h"

namespace genie {
namespace transcoder {
namespace sam {
namespace sam_to_mgrec {

SamReader::SamReader(const char* fpath)
    : sam_file(hts_open(fpath, "r")),  // open bam file
      sam_header(nullptr),             // read header
      sam_alignment(bam_init1())       // initialize an alignment
{}

SamReader::SamReader(std::string& fpath) : SamReader(fpath.c_str()) {}

SamReader::~SamReader() {
    bam_destroy1(sam_alignment);
    bam_hdr_destroy(sam_header);
    if (sam_file) sam_close(sam_file);
}

int SamReader::getNumRef() { return sam_hdr_nref(sam_header); }

bool SamReader::ready() {
    if (!sam_file) {
        return false;
    }

    sam_header = sam_hdr_read(sam_file);  // read header
    if (!sam_header) {
        return false;
    }

    return true;
}

int SamReader::readSamRecord(SamRecord& sr) {
    auto res = sam_read1(sam_file, sam_header, sam_alignment);

    if (res >= 0) {
        sr = SamRecord(sam_alignment);
    }

    return res;
}

}
}
}
}

