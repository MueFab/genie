/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "record.h"
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/exceptions.h>
#include <genie/util/make-unique.h>
#include "alignment-box.h"
#include "alignment-shared-data.h"
#include "alignment_external/none.h"
#include "segment.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

Record::Record()
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

Record::Record(uint8_t _number_of_template_segments, ClassType _auTypeCfg, std::string &&_read_name,
               std::string &&_read_group, uint8_t _flags, bool _is_read_1_first)
    : number_of_template_segments(_number_of_template_segments),
      class_ID(_auTypeCfg),
      read_1_first(_is_read_1_first),
      sharedAlignmentInfo(),
      qv_depth(0),
      read_name(std::move(_read_name)),
      read_group(std::move(_read_group)),
      reads(),
      alignmentInfo(0),
      flags(_flags),
      moreAlignmentInfo(util::make_unique<alignment_external::None>()) {}

// ---------------------------------------------------------------------------------------------------------------------

Record::Record(util::BitReader &reader) {
    number_of_template_segments = reader.read<uint8_t>();
    reads.resize(reader.read<uint8_t>());
    alignmentInfo.resize(reader.read<uint16_t>());
    class_ID = ClassType(reader.read<uint8_t>());
    read_group.resize(reader.read<uint8_t>());
    read_1_first = reader.read<uint8_t>();
    if (!alignmentInfo.empty()) {
        sharedAlignmentInfo = AlignmentSharedData(reader);
    }
    std::vector<uint32_t> readSizes(reads.size());
    for (auto &a : readSizes) {
        a = reader.read<uint32_t>(24);
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
        a = AlignmentBox(class_ID, sharedAlignmentInfo.getAsDepth(), uint8_t(number_of_template_segments), reader);
    }
    flags = reader.read<uint8_t>();
    moreAlignmentInfo = AlignmentExternal::factory(reader);
}

// ---------------------------------------------------------------------------------------------------------------------

Record::Record(const Record &rec) { *this = rec; }

// ---------------------------------------------------------------------------------------------------------------------

Record::Record(Record &&rec) noexcept { *this = rec; }

// ---------------------------------------------------------------------------------------------------------------------

Record &Record::operator=(const Record &rec) {
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

Record &Record::operator=(Record &&rec) noexcept {
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

void Record::addSegment(Segment &&rec) {
    UTILS_DIE_IF(reads.size() == number_of_template_segments, "Record already full");
    if (reads.empty()) {
        qv_depth = uint8_t(rec.getQualities().size());
    }
    UTILS_DIE_IF(!reads.empty() && rec.getQualities().size() != qv_depth, "Incompatible qv depth");
    reads.push_back(std::move(rec));
}

// ---------------------------------------------------------------------------------------------------------------------

void Record::addAlignment(uint16_t _seq_id, AlignmentBox &&rec) {
    if (alignmentInfo.empty()) {
        sharedAlignmentInfo = AlignmentSharedData(_seq_id, uint8_t(rec.getAlignment().getMappingScores().size()));
    } else {
        UTILS_DIE_IF(rec.getAlignment().getMappingScores().size() != sharedAlignmentInfo.getAsDepth(),
                     "Incompatible AS depth");
        UTILS_DIE_IF(rec.getNumberOfTemplateSegments() != number_of_template_segments,
                     "Incompatible number_of_template_segments");
        UTILS_DIE_IF(_seq_id != sharedAlignmentInfo.getSeqID(), "Incompatible seq id");
    }
    alignmentInfo.push_back(std::move(rec));
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Segment> &Record::getSegments() const { return reads; }

// ---------------------------------------------------------------------------------------------------------------------

size_t Record::getNumberOfTemplateSegments() const { return number_of_template_segments; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<AlignmentBox> &Record::getAlignments() const { return alignmentInfo; }

// ---------------------------------------------------------------------------------------------------------------------

void Record::write(util::BitWriter &writer) const {
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

uint8_t Record::getFlags() const { return flags; }

// ---------------------------------------------------------------------------------------------------------------------

ClassType Record::getClassID() const { return class_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string &Record::getName() const { return read_name; }

// ---------------------------------------------------------------------------------------------------------------------

bool Record::getRead1First() const { return read_1_first; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string &Record::getGroup() const { return read_group; }

// ---------------------------------------------------------------------------------------------------------------------

const AlignmentSharedData &Record::getAlignmentSharedData() const { return sharedAlignmentInfo; }

// ---------------------------------------------------------------------------------------------------------------------

const AlignmentExternal &Record::getAlignmentExternal() const { return *moreAlignmentInfo; }

// ---------------------------------------------------------------------------------------------------------------------

void Record::setMoreAlignmentInfo(std::unique_ptr<AlignmentExternal> _more_alignment_info) {
        moreAlignmentInfo = std::move(_more_alignment_info);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
