#ifndef TRANSCODER_SAM_READER_H
#define TRANSCODER_SAM_READER_H

#include <htslib/sam.h>
#include <string>

#include "sam_record.h"

namespace sam_transcoder {

class SamReader{
   private:
    samFile *sam_file;
    bam_hdr_t *sam_header;
    bam1_t *sam_alignment;
   public:

    explicit SamReader(const char* fpath);

    explicit SamReader(std::string& fpath);

    ~SamReader();

    int getNumRef();

    bool ready();

    int readSamRecord(SamRecord& sr);

};

}

#endif  // TRANSCODER_SAM_READER_H
