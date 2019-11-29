#include "mpegg-record.h"

#include "external_alignment/external-alignment-none.h"

#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <util/exceptions.h>
#include <util/make_unique.h>

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
                r = util::make_unique<Segment>(readSizes[index], reader);
                ++index;
            }
            for (auto &a : alignmentInfo) {
                a = util::make_unique<AlignmentContainer>(reader);
            }
            flags = reader->read(8);
            moreAlignmentInfo = ExternalAlignment::factory(reader);
        }

        void MpeggRecord::addRecordSegment(std::unique_ptr <Segment> rec) {
            if (reads.size() == number_of_template_segments) {
                UTILS_DIE("Record already full");
            }
            if (!reads.empty() && rec->getQvDepth() != qv_depth) {
                UTILS_DIE("Incompatible qv depth");
            }
            reads.push_back(std::move(rec));
        }

        void MpeggRecord::addAlignment(uint16_t _seq_id, std::unique_ptr <AlignmentContainer> rec) {
            if (sharedAlignmentInfo) {
                if (rec->getAsDepth() != sharedAlignmentInfo->getAsDepth()) {
                    UTILS_DIE("Incompatible as depth");
                }
                if (_seq_id != sharedAlignmentInfo->getSeqID()) {
                    UTILS_DIE("Incompatible seq id");
                }
            } else {
                sharedAlignmentInfo = util::make_unique<MetaAlignment>(_seq_id, rec->getAsDepth());
            }
            alignmentInfo.push_back(std::move(rec));
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
            std::unique_ptr <MpeggRecord> ret;

            return ret;
        }
    }
}