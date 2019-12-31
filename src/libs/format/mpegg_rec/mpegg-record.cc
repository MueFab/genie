/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "mpegg-record.h"
#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <util/exceptions.h>
#include <util/make_unique.h>
#include "alignment-container.h"
#include "external_alignment/external-alignment-none.h"
#include "meta-alignment.h"
#include "segment.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_rec {

// ---------------------------------------------------------------------------------------------------------------------

MpeggRecord::MpeggRecord()
    : number_of_template_segments(0),
      class_ID(ClassType::NONE),
      read_1_first(false),
      sharedAlignmentInfo(),
      qv_depth(0),
      read_name(),
      read_group(),
      reads(),
      alignmentInfo(0),
      flags(0),
      moreAlignmentInfo(nullptr) {}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRecord::MpeggRecord(uint8_t _number_of_template_segments, ClassType _auTypeCfg, std::string &&_read_name,
                         std::string &&_read_group, uint8_t _flags)
    : number_of_template_segments(_number_of_template_segments),
      class_ID(_auTypeCfg),
      read_1_first(true),
      sharedAlignmentInfo(),
      qv_depth(0),
      read_name(std::move(_read_name)),
      read_group(std::move(_read_group)),
      reads(),
      alignmentInfo(0),
      flags(_flags),
      moreAlignmentInfo(util::make_unique<ExternalAlignmentNone>()) {}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRecord::MpeggRecord(util::BitReader &reader) {
    number_of_template_segments = reader.read<uint8_t>();
    reads.resize(reader.read<uint8_t>());
    alignmentInfo.resize(reader.read<uint16_t>());
    class_ID = ClassType(reader.read<uint8_t>());
    read_group.resize(reader.read<uint8_t>());
    read_1_first = reader.read<uint8_t>();
    if (!alignmentInfo.empty()) {
        sharedAlignmentInfo = MetaAlignment(reader);
    }
    std::vector<uint32_t> readSizes(reads.size());
    for (auto &a : readSizes) {
        a = uint32_t(reader.read(24));
    }
    qv_depth = reader.read<uint8_t>();
    read_name.resize(reader.read<uint8_t>());
    for (auto &c : read_name) {
        c = reader.read<uint8_t>();
    }
    for (auto &c : read_group) {
        c = reader.read<uint8_t>();
    }

    size_t index = 0;
    for (auto &r : reads) {
        r = Segment(readSizes[index], qv_depth, reader);
        ++index;
    }
    for (auto &a : alignmentInfo) {
        a = AlignmentContainer(class_ID, sharedAlignmentInfo.getAsDepth(), uint8_t(number_of_template_segments),
                               reader);
    }
    flags = reader.read<uint8_t>();
    moreAlignmentInfo = ExternalAlignment::factory(reader);
}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRecord::MpeggRecord(const MpeggRecord &rec) { *this = rec; }

// ---------------------------------------------------------------------------------------------------------------------

MpeggRecord::MpeggRecord(MpeggRecord &&rec) noexcept { *this = rec; }

// ---------------------------------------------------------------------------------------------------------------------

MpeggRecord &MpeggRecord::operator=(const MpeggRecord &rec) {
    if (this == &rec) {
        return *this;
    }
    this->number_of_template_segments = rec.number_of_template_segments;
    this->class_ID = rec.class_ID;
    this->read_1_first = rec.read_1_first;
    this->sharedAlignmentInfo = rec.sharedAlignmentInfo;
    this->qv_depth = rec.qv_depth;
    this->read_name = rec.read_name;
    this->read_group = rec.read_group;
    this->reads = rec.reads;
    this->alignmentInfo = rec.alignmentInfo;
    this->flags = rec.flags;
    this->moreAlignmentInfo = rec.moreAlignmentInfo->clone();
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRecord &MpeggRecord::operator=(MpeggRecord &&rec) noexcept {
    this->number_of_template_segments = rec.number_of_template_segments;
    this->class_ID = rec.class_ID;
    this->read_1_first = rec.read_1_first;
    this->sharedAlignmentInfo = std::move(rec.sharedAlignmentInfo);
    this->qv_depth = rec.qv_depth;
    this->read_name = std::move(rec.read_name);
    this->read_group = std::move(rec.read_group);
    this->reads = std::move(rec.reads);
    this->alignmentInfo = std::move(rec.alignmentInfo);
    this->flags = rec.flags;
    this->moreAlignmentInfo = std::move(rec.moreAlignmentInfo);
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRecord::addRecordSegment(Segment &&rec) {
    if (reads.size() == number_of_template_segments) {
        UTILS_DIE("Record already full");
    }
    if (reads.empty()) {
        qv_depth = uint8_t(rec.getQualities().size());
    }
    if (!reads.empty() && rec.getQualities().size() != qv_depth) {
        UTILS_DIE("Incompatible qv depth");
    }
    reads.push_back(std::move(rec));
}

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRecord::addAlignment(uint16_t _seq_id, AlignmentContainer &&rec) {
    if (alignmentInfo.empty()) {
        sharedAlignmentInfo = MetaAlignment(_seq_id, uint8_t(rec.getAlignment().getMappingScores().size()));
    } else {
        if (rec.getAlignment().getMappingScores().size() != sharedAlignmentInfo.getAsDepth()) {
            UTILS_DIE("Incompatible AS depth");
        }
        if (rec.getNumberOfTemplateSegments() != number_of_template_segments) {
            UTILS_DIE("Incompatible number_of_template_segments");
        }
        if (_seq_id != sharedAlignmentInfo.getSeqID()) {
            UTILS_DIE("Incompatible seq id");
        }
    }
    alignmentInfo.push_back(std::move(rec));
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Segment> &MpeggRecord::getRecordSegments() const { return reads; }

// ---------------------------------------------------------------------------------------------------------------------

size_t MpeggRecord::getNumberOfTemplateSegments() const { return number_of_template_segments; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<AlignmentContainer> &MpeggRecord::getAlignments() const { return alignmentInfo; }

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRecord::write(util::BitWriter &writer) const {
    writer.write(number_of_template_segments, 8);
    writer.write(reads.size(), 8);
    writer.write(alignmentInfo.size(), 16);
    writer.write((uint8_t)class_ID, 8);
    writer.write(read_group.length(), 8);
    writer.write(uint64_t(read_1_first), 8);
    if (!alignmentInfo.empty()) {
        sharedAlignmentInfo.write(writer);
    }
    for (const auto &a : reads) {
        writer.write(a.getSequence().length(), 24);
    }
    writer.write(qv_depth, 8);
    writer.write(read_name.length(), 8);
    writer.write(read_name);
    writer.write(read_group);
    for (const auto &r : reads) {
        r.write(writer);
    }
    for (const auto &a : alignmentInfo) {
        a.write(writer);
    }
    writer.write(flags, 8);
    moreAlignmentInfo->write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<MpeggRecord> MpeggRecord::clone() const {
    auto ret = util::make_unique<MpeggRecord>();
    ret->number_of_template_segments = this->number_of_template_segments;
    ret->class_ID = this->class_ID;
    ret->read_1_first = this->read_1_first;

    ret->sharedAlignmentInfo = this->sharedAlignmentInfo;

    ret->qv_depth = this->qv_depth;
    ret->read_name = this->read_name;
    ret->read_group = this->read_group;

    for (const auto &s : reads) {
        ret->reads.push_back(s);
    }

    for (const auto &a : alignmentInfo) {
        ret->alignmentInfo.push_back(a);
    }

    ret->flags = flags;

    ret->moreAlignmentInfo = this->moreAlignmentInfo->clone();

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t MpeggRecord::getFlags() const { return flags; }

// ---------------------------------------------------------------------------------------------------------------------

ClassType MpeggRecord::getClassID() const { return class_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string &MpeggRecord::getReadName() const { return read_name; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
