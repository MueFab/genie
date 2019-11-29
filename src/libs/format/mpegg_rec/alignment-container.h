#ifndef GENIE_ALIGNMENT_CONTAINER_H
#define GENIE_ALIGNMENT_CONTAINER_H

#include <cstdint>
#include <memory>
#include <vector>

#include "alignment.h"
#include "split-alignment.h"

namespace util {
    class BitWriter;

    class BitReader;
}

namespace format {
    namespace mpegg_rec {
        class AlignmentContainer {
            uint64_t mapping_pos : 40;
            std::unique_ptr<Alignment> alignment;

            // if (class_ID != Class_HM)
            // for (tSeg=1; tSeg < number_of_template_segments; tSeg++)
            std::vector<std::unique_ptr<SplitAlignment>> splitAlignmentInfo;

        public:
            virtual void write(util::BitWriter *writer) const;

            explicit AlignmentContainer(util::BitReader *reader);

            uint32_t getAsDepth() const;
        };
    }
}


#endif //GENIE_ALIGNMENT_CONTAINER_H
