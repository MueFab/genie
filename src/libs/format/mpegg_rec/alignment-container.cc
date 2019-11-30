#include "alignment-container.h"

#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <util/make_unique.h>

namespace format {
    namespace mpegg_rec {
        void AlignmentContainer::write(util::BitWriter *writer) const {
            writer->write(mapping_pos, 40);
            alignment->write(writer);
            for (const auto &a : splitAlignmentInfo) {
                a->write(writer);
            }
        }

        AlignmentContainer::AlignmentContainer(MpeggRecord::ClassType type, uint8_t as_depth, uint8_t number_of_template_segments,
                                               util::BitReader *reader) {
            mapping_pos = reader->read(40);
            alignment = util::make_unique<Alignment>(as_depth, reader);

            if (type == MpeggRecord::ClassType::CLASS_HM) {
                return;
            }
            for (size_t tSeg = 1; tSeg < number_of_template_segments; tSeg++) {
                splitAlignmentInfo.push_back(SplitAlignment::factory(as_depth, reader));
            }
        }

        AlignmentContainer::AlignmentContainer(
        ) : mapping_pos(0), alignment(), splitAlignmentInfo(0) {

        }

        uint32_t AlignmentContainer::getAsDepth() const {
            return alignment->getAsDepth();
        }

        std::unique_ptr<AlignmentContainer> AlignmentContainer::clone() const {
            auto ret = util::make_unique<AlignmentContainer>();
            ret->mapping_pos = this->mapping_pos;
            ret->alignment = this->alignment->clone();
            for (const auto &a : splitAlignmentInfo) {
                ret->splitAlignmentInfo.push_back(a->clone());
            }
            return ret;
        }

        void AlignmentContainer::addSplitAlignment(std::unique_ptr<SplitAlignment> _alignment) {
            if (_alignment->getType() == SplitAlignment::SplitAlignmentType::SAME_REC) {
                if (dynamic_cast<SplitAlignmentSameRec *>(_alignment.get())->getAlignment()->getAsDepth() !=
                    alignment->getAsDepth()) {
                    UTILS_DIE("AS depth incompatible");
                }
            }
            splitAlignmentInfo.push_back(std::move(_alignment));
        }

        uint8_t AlignmentContainer::getNumberOfTemplateSegments() const {
            return splitAlignmentInfo.size() + 1;
        }

        uint64_t AlignmentContainer::getPosition() const {
            return mapping_pos;
        }

        const Alignment* AlignmentContainer::getAlignment() const {
            return alignment.get();
        }

        const SplitAlignment* AlignmentContainer::getSplitAlignment(size_t index) const {
            if(index >= splitAlignmentInfo.size()) {
                UTILS_DIE("Index out of bounds");
            }
            return splitAlignmentInfo[index].get();
        }
    }
}