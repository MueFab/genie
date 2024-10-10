/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/record.h"
#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/record/alignment-box.h"
#include "genie/core/record/alignment-shared-data.h"
#include "genie/core/record/alignment_external/none.h"
#include "genie/core/record/segment.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

// ---------------------------------------------------------------------------------------------------------------------

void Record::patchRefID(size_t refID) {
    sharedAlignmentInfo = AlignmentSharedData(static_cast<uint16_t>(refID), sharedAlignmentInfo.getAsDepth());
}

// ---------------------------------------------------------------------------------------------------------------------

Record::Record()
    : number_of_template_segments(0),
      reads(),
      alignmentInfo(0),
      class_ID(ClassType::NONE),
      read_group(),
      read_1_first(false),
      sharedAlignmentInfo(),
      qv_depth(0),
      read_name(),
      flags(0),
      moreAlignmentInfo(nullptr) {}

// ---------------------------------------------------------------------------------------------------------------------

Record::Record(uint8_t _number_of_template_segments, ClassType _auTypeCfg, std::string &&_read_name,
               std::string &&_read_group, uint8_t _flags, bool _is_read_1_first)
    : number_of_template_segments(_number_of_template_segments),
      reads(),
      alignmentInfo(0),
      class_ID(_auTypeCfg),
      read_group(std::move(_read_group)),
      read_1_first(_is_read_1_first),
      sharedAlignmentInfo(),
      qv_depth(0),
      read_name(std::move(_read_name)),
      flags(_flags),
      moreAlignmentInfo(std::make_unique<alignment_external::None>()) {}

// ---------------------------------------------------------------------------------------------------------------------

Record::Record(util::BitReader &reader)
    : number_of_template_segments(reader.readAlignedInt<uint8_t>()),
      reads(reader.readAlignedInt<uint8_t>()),
      alignmentInfo(reader.readAlignedInt<uint16_t>()),
      class_ID(reader.readAlignedInt<ClassType>()),
      read_group(reader.readAlignedInt<uint8_t>(), 0),
      read_1_first(reader.readAlignedInt<uint8_t>()),
      sharedAlignmentInfo(!alignmentInfo.empty() ? AlignmentSharedData(reader) : AlignmentSharedData()) {
    std::vector<uint32_t> readSizes(reads.size());
    for (auto &s : readSizes) {
        s = reader.readAlignedInt<uint32_t, 3>();
    }
    qv_depth = reader.readAlignedInt<uint8_t>();
    read_name.resize(reader.readAlignedInt<uint8_t>());
    reader.readAlignedBytes(&read_name[0], read_name.size());
    reader.readAlignedBytes(&read_group[0], read_group.size());

    size_t index = 0;
    for (auto &r : reads) {
        r = Segment(readSizes[index], qv_depth, reader);
        ++index;
    }
    for (auto &a : alignmentInfo) {
        a = AlignmentBox(class_ID, sharedAlignmentInfo.getAsDepth(), uint8_t(number_of_template_segments), reader);
    }
    flags = reader.readAlignedInt<uint8_t>();
    moreAlignmentInfo = AlignmentExternal::factory(reader);
}

// ---------------------------------------------------------------------------------------------------------------------

Record::Record(const Record &rec) { *this = rec; }

// ---------------------------------------------------------------------------------------------------------------------

Record::Record(Record &&rec) noexcept { *this = std::move(rec); }

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

std::vector<Segment> &Record::getSegments() { return reads; }

// ---------------------------------------------------------------------------------------------------------------------

void Record::swapSegmentOrder() {
    if (reads.size() != 2) {
        return;
    }
    auto s_tmp = std::move(reads[0]);
    reads[0] = std::move(reads[1]);
    reads[1] = std::move(s_tmp);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t Record::getNumberOfTemplateSegments() const { return number_of_template_segments; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<AlignmentBox> &Record::getAlignments() const { return alignmentInfo; }

// ---------------------------------------------------------------------------------------------------------------------

void Record::write(util::BitWriter &writer) const {
    writer.writeAlignedInt(number_of_template_segments);
    writer.writeAlignedInt<uint8_t>(static_cast<uint8_t>(reads.size()));
    writer.writeAlignedInt<uint16_t>(static_cast<uint16_t>(alignmentInfo.size()));
    writer.writeAlignedInt(class_ID);
    writer.writeAlignedInt<uint8_t>(static_cast<uint8_t>(read_group.length()));
    writer.writeAlignedInt(read_1_first);
    if (!alignmentInfo.empty()) {
        sharedAlignmentInfo.write(writer);
    }
    for (const auto &a : reads) {
        writer.writeAlignedInt<uint32_t, 3>(static_cast<uint32_t>(a.getSequence().length()));
    }
    writer.writeAlignedInt(qv_depth);
    writer.writeAlignedInt<uint8_t>(static_cast<uint8_t>(read_name.length()));
    writer.writeAlignedBytes(read_name.data(), read_name.length());
    writer.writeAlignedBytes(read_group.data(), read_group.length());
    for (const auto &r : reads) {
        r.write(writer);
    }
    for (const auto &a : alignmentInfo) {
        a.write(writer);
    }
    writer.writeAlignedInt(flags);
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

void Record::setQVDepth(uint8_t depth) { qv_depth = depth; }

// ---------------------------------------------------------------------------------------------------------------------

void Record::setRead1First(bool val) { this->read_1_first = val; }

// ---------------------------------------------------------------------------------------------------------------------

void Record::setName(const std::string &_name) { read_name = _name; }

// ---------------------------------------------------------------------------------------------------------------------

void Record::setClassType(ClassType type) { this->class_ID = type; }

// ---------------------------------------------------------------------------------------------------------------------

bool Record::isRead1First() const { return read_1_first; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Record::getLengthOfCigar(const std::string &cigar) {
    std::string digits;
    size_t length = 0;
    for (const auto &c : cigar) {
        if (isdigit(c)) {
            digits += c;
            continue;
        }
        if (getAlphabetProperties(core::AlphabetID::ACGTN).isIncluded(c)) {
            length++;
            digits.clear();
            continue;
        }
        if (c == '=' || c == '-' || c == '*' || c == '/' || c == '%') {
            length += std::stoi(digits);
            digits.clear();
        } else {
            digits.clear();
        }
    }
    return length;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t Record::getMappedLength(size_t alignment, size_t split) const {
    if (split == 0) {
        return static_cast<size_t>(getLengthOfCigar(getAlignments()[alignment].getAlignment().getECigar()));
    }
    auto &s2 =
        dynamic_cast<record::alignment_split::SameRec &>(*getAlignments()[alignment].getAlignmentSplits()[split - 1]);
    return static_cast<size_t>(getLengthOfCigar(s2.getAlignment().getECigar()));
}

// ---------------------------------------------------------------------------------------------------------------------

void Record::setAlignment(size_t id, AlignmentBox &&b) { this->alignmentInfo[id] = std::move(b); }

// ---------------------------------------------------------------------------------------------------------------------

std::pair<size_t, size_t> Record::getTemplatePosition() const {
    std::pair<size_t, size_t> ret = {getPosition(0, 0), getPosition(0, 0) + getMappedLength(0, 0)};
    for (size_t i = 0; i < getAlignments().front().getAlignmentSplits().size(); ++i) {
        auto pos = getPosition(0, i);
        ret.first = std::min(ret.first, pos);
        ret.second = std::max(ret.second, pos + getMappedLength(0, i));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t Record::getPosition(size_t alignment, size_t split) const {
    if (split == 0) {
        return static_cast<size_t>(getAlignments()[alignment].getPosition());
    }
    auto &s2 =
        dynamic_cast<record::alignment_split::SameRec &>(*getAlignments()[alignment].getAlignmentSplits()[split - 1]);
    return static_cast<size_t>(getAlignments()[alignment].getPosition() + s2.getDelta());
}

// ---------------------------------------------------------------------------------------------------------------------

void Record::setMoreAlignmentInfo(std::unique_ptr<AlignmentExternal> _more_alignment_info) {
    moreAlignmentInfo = std::move(_more_alignment_info);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
