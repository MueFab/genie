#ifndef GENIE_STREAM_CONTAINER_H
#define GENIE_STREAM_CONTAINER_H

#include <vector>
#include <cstdint>
#include <gabac/gabac.h>

struct StreamContainer {
    gabac::DataBlock pos_0;
    gabac::DataBlock rcomp_0;
    gabac::DataBlock flags_0;
    gabac::DataBlock flags_1;
    gabac::DataBlock flags_2;
    gabac::DataBlock mmpos_0;
    gabac::DataBlock mmpos_1;
    gabac::DataBlock mmtype_0;
    gabac::DataBlock mmtype_1;
    gabac::DataBlock mmtype_2;
    gabac::DataBlock clips_0;
    gabac::DataBlock clips_1;
    gabac::DataBlock clips_2;
    gabac::DataBlock clips_3;
    gabac::DataBlock pair_0;
    gabac::DataBlock pair_1;
    gabac::DataBlock mscore_0;
    gabac::DataBlock rlen_0;
    gabac::DataBlock rtype_0;
    gabac::DataBlock ureads_0;

    StreamContainer()
            : pos_0(0, 4),
              rcomp_0(0, 4),
              flags_0(0, 4),
              flags_1(0, 4),
              flags_2(0, 4),
              mmpos_0(0, 4),
              mmpos_1(0, 4),
              mmtype_0(0, 4),
              mmtype_1(0, 4),
              mmtype_2(0, 4),
              clips_0(0, 4),
              clips_1(0, 4),
              clips_2(0, 4),
              clips_3(0, 4),
              pair_0(0, 4),
              pair_1(0,4),
              mscore_0(0,4),
              rlen_0(0, 4),
              rtype_0(0, 4),
              ureads_0(0, 4){

    }
};

#endif //GENIE_STREAM_CONTAINER_H
