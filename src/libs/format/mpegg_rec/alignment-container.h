#ifndef GENIE_ALIGNMENT_CONTAINER_H
#define GENIE_ALIGNMENT_CONTAINER_H

#include <cstdint>
#include <memory>
#include <vector>
#include <util/make_unique.h>
#include <format/mpegg_rec/split_alignment/split-alignment-unpaired.h>
#include <util/exceptions.h>
#include <format/mpegg_rec/split_alignment/split-alignment-same-rec.h>

#include "alignment.h"
#include "split-alignment.h"
#include "mpegg-record.h"

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

            explicit AlignmentContainer(
                    MpeggRecord::ClassType type,
                    uint8_t as_depth,
                    uint8_t number_of_template_segments,
                    util::BitReader *reader
            );

            AlignmentContainer(
                    uint64_t _mapping_pos,
                    std::unique_ptr<Alignment> _alignment
            ) : mapping_pos(_mapping_pos),
                alignment(std::move(_alignment)) {
            }

            void addSplitAlignment(std::unique_ptr<SplitAlignment> _alignment);

            AlignmentContainer();

            uint32_t getAsDepth() const;

            uint8_t getNumberOfTemplateSegments() const;

            uint64_t getPosition() const;

            const Alignment* getAlignment() const;

            const SplitAlignment* getSplitAlignment(size_t index) const;

            virtual std::unique_ptr<AlignmentContainer> clone() const;
        };
    }
}


#endif //GENIE_ALIGNMENT_CONTAINER_H
