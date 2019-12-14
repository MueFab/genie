#include "mpegg-record.h"

#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <util/exceptions.h>
#include <util/make_unique.h>

#include "alignment-container.h"
#include "external_alignment/external-alignment-none.h"
#include "meta-alignment.h"
#include "segment.h"

namespace format {
    namespace mpegg_rec {
        MpeggRecord::MpeggRecord(
        ) : number_of_template_segments(0),
            class_ID(ClassType::NONE),
            read_1_first(0),
            sharedAlignmentInfo(nullptr),
            qv_depth(0),
            read_name(nullptr),
            read_group(nullptr),
            reads(0),
            alignmentInfo(0),
            flags(0),
            moreAlignmentInfo(nullptr) {

        }

        MpeggRecord::MpeggRecord(
                uint8_t _number_of_template_segments,
                ClassType _auTypeCfg,
                std::unique_ptr <std::string> _read_name,
                std::unique_ptr <std::string> _read_group,
                uint8_t _flags
        ) : number_of_template_segments(_number_of_template_segments),
            class_ID(_auTypeCfg),
            read_1_first(true),
            sharedAlignmentInfo(nullptr),
            qv_depth(0),
            read_name(std::move(_read_name)),
            read_group(std::move(_read_group)),
            reads(0),
            alignmentInfo(0),
            flags(_flags),
            moreAlignmentInfo(util::make_unique<ExternalAlignmentNone>()) {

        }

        MpeggRecord::MpeggRecord(util::BitReader *reader) {
            number_of_template_segments = reader->read(8);
            reads.resize(reader->read(8));
            alignmentInfo.resize(reader->read(16));
            class_ID = ClassType(reader->read(8));
            read_group->resize(reader->read(8));
            read_1_first = reader->read(8);
            if (!alignmentInfo.empty()) {
                sharedAlignmentInfo = util::make_unique<MetaAlignment>(reader);
            }
            std::vector <uint32_t> readSizes(reads.size());
            for (auto &a : readSizes) {
                a = reader->read(24);
            }
            qv_depth = reader->read(8);
            read_name->resize(reader->read(8));
            for (auto &c : *read_name) {
                c = reader->read(8);
            }
            for (auto &c : *read_group) {
                c = reader->read(8);
            }

            size_t index = 0;
            for (auto &r : reads) {
                r = util::make_unique<Segment>(readSizes[index], uint8_t (qv_depth), reader);
                ++index;
            }
            for (auto &a : alignmentInfo) {
                a = util::make_unique<AlignmentContainer>(class_ID,  sharedAlignmentInfo->getAsDepth(), uint8_t (number_of_template_segments), reader);
            }
            flags = reader->read(8);
            moreAlignmentInfo = ExternalAlignment::factory(reader);
        }

        void MpeggRecord::addRecordSegment(std::unique_ptr <Segment> rec) {
            if (reads.size() == number_of_template_segments) {
                UTILS_DIE("Record already full");
            }
            if(reads.empty()) {
                qv_depth = rec->getQvDepth();
            }
            if (!reads.empty() && rec->getQvDepth() != qv_depth) {
                UTILS_DIE("Incompatible qv depth");
            }
            reads.push_back(std::move(rec));
        }

        void MpeggRecord::addAlignment(uint16_t _seq_id, std::unique_ptr <AlignmentContainer> rec) {
            if (sharedAlignmentInfo) {
                if (rec->getAsDepth() != sharedAlignmentInfo->getAsDepth()) {
                    UTILS_DIE("Incompatible AS depth");
                }
                if (rec->getNumberOfTemplateSegments() != number_of_template_segments) {
                    UTILS_DIE("Incompatible number_of_template_segments");
                }
                if (_seq_id != sharedAlignmentInfo->getSeqID()) {
                    UTILS_DIE("Incompatible seq id");
                }
            } else {
                sharedAlignmentInfo = util::make_unique<MetaAlignment>(_seq_id, rec->getAsDepth());
            }
            alignmentInfo.push_back(std::move(rec));
        }

        const Segment* MpeggRecord::getRecordSegment(size_t index) const {
            if (index >= reads.size()) {
                UTILS_DIE("Index out of bounds");
            }
            return reads[index].get();
        }

        size_t MpeggRecord::getNumberOfRecords() const {
            return reads.size();
        }

        size_t MpeggRecord::getNumberOfAlignments() const {
            return alignmentInfo.size();
        }

        size_t MpeggRecord::getNumberOfTemplateSegments() const {
            return number_of_template_segments;
        }

        const AlignmentContainer* MpeggRecord::getAlignment(size_t index) const {
            if (index != alignmentInfo.size()) {
                UTILS_DIE("Index out of bounds");
            }
            return alignmentInfo[index].get();
        }

        void MpeggRecord::write(util::BitWriter *writer) const {
            writer->write(number_of_template_segments, 8);
            writer->write(reads.size(), 8);
            writer->write(alignmentInfo.size(), 16);
            writer->write((uint8_t) class_ID, 8);
            writer->write(read_group->length(), 8);
            writer->write(read_1_first, 8);
            if (sharedAlignmentInfo) {
                sharedAlignmentInfo->write(writer);
            }
            for (const auto &a : reads) {
                writer->write(a->getLength(), 24);
            }
            writer->write(qv_depth, 8);
            writer->write(read_name->length(), 8);
            for (const auto &c : *read_name) {
                writer->write(c, 8);
            }
            for (const auto &c : *read_group) {
                writer->write(c, 8);
            }
            for (const auto &r : reads) {
                r->write(writer);
            }
            for (const auto &a : alignmentInfo) {
                a->write(writer);
            }
            writer->write(flags, 8);
            moreAlignmentInfo->write(writer);
        }

        std::unique_ptr <MpeggRecord> MpeggRecord::clone() const {
            auto ret = util::make_unique<MpeggRecord>();
            ret->number_of_template_segments = this->number_of_template_segments;
            ret->class_ID = this->class_ID;
            ret->read_1_first = this->read_1_first;

            if(this->sharedAlignmentInfo) {
                ret->sharedAlignmentInfo = this->sharedAlignmentInfo->clone();
            }

            ret->qv_depth = this->qv_depth;
            ret->read_name = util::make_unique<std::string>(*this->read_name);
            ret->read_group = util::make_unique<std::string>(*this->read_group);

            for(const auto& s : reads) {
                ret->reads.push_back(s->clone());
            }

            for(const auto& a : alignmentInfo) {
                ret->alignmentInfo.push_back(a->clone());
            }

            ret->flags = flags;

            ret->moreAlignmentInfo = this->moreAlignmentInfo->clone();

            return ret;
        }

        uint8_t MpeggRecord::getFlags() const {
            return flags;
        }

        MpeggRecord::ClassType MpeggRecord::getClassID() const {
            return class_ID;
        }

        const std::string& MpeggRecord::getReadName() const {
            return *read_name;
        }
    }
}