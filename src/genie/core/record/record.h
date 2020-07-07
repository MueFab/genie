/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_RECORD_H
#define GENIE_RECORD_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/constants.h>
#include <genie/core/record/alignment_split/same-rec.h>
#include <genie/core/stats/perf-stats.h>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <cstdint>
#include <memory>
#include <vector>
#include "alignment-box.h"
#include "alignment-external.h"
#include "alignment-shared-data.h"
#include "class-type.h"
#include "segment.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

/**
 *
 */
class Record {
   private:
    uint8_t number_of_template_segments{};    //!<
    ClassType class_ID{ClassType::NONE};      //!<
    bool read_1_first{};                      //!<
    AlignmentSharedData sharedAlignmentInfo;  //!<
    uint8_t qv_depth{};                       //!<
    std::string read_name;                    //!<
    std::string read_group;                   //!<
    std::vector<Segment> reads;               //!<
    std::vector<AlignmentBox> alignmentInfo;  //!<
    uint8_t flags{};                          //!<

    std::unique_ptr<AlignmentExternal> moreAlignmentInfo;  //!<

   public:
    /**
     *
     */
    Record();

    /**
     *
     * @param _number_of_template_segments
     * @param _auTypeCfg
     * @param _read_name
     * @param _read_group
     * @param _flags
     */
    Record(uint8_t _number_of_template_segments, ClassType _auTypeCfg, std::string&& _read_name,
           std::string&& _read_group, uint8_t _flags);

    /**
     *
     * @param rec
     */
    Record(const Record& rec);

    /**
     *
     * @param rec
     */
    Record(Record&& rec) noexcept;

    /**
     *
     */
    ~Record() = default;

    /**
     *
     * @param rec
     * @return
     */
    Record& operator=(const Record& rec);

    /**
     *
     * @param rec
     * @return
     */
    Record& operator=(Record&& rec) noexcept;

    /**
     *
     * @param reader
     */
    explicit Record(util::BitReader& reader);

    /**
     *
     * @param rec
     */
    void addSegment(Segment&& rec);

    /**
     *
     * @param _seq_id
     * @param rec
     */
    void addAlignment(uint16_t _seq_id, AlignmentBox&& rec);

    /**
     *
     * @param index
     * @return
     */
    std::vector<Segment>& getSegments();

    /**
     *
     * @return
     */
    const std::vector<Segment>& getSegments() const;

    /**
     *
     * @return
     */
    size_t getNumberOfTemplateSegments() const;

    /**
     *
     * @param index
     * @return
     */
    const std::vector<AlignmentBox>& getAlignments() const;

    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     *
     * @return
     */
    uint8_t getFlags() const;

    /**
     *
     * @return
     */
    ClassType getClassID() const;

    /**
     *
     * @return
     */
    const std::string& getName() const;

    /**
     *
     * @param _name
     */
    void setName(const std::string& _name);

    /**
     *
     * @param type
     */
    void setClassType(ClassType type);

    /**
     *
     * @return
     */
    const std::string& getGroup() const;

    /**
     *
     * @return
     */
    const AlignmentSharedData& getAlignmentSharedData() const;

    /**
     *
     * @return
     */
    const AlignmentExternal& getAlignmentExternal() const;

    /**
     *
     * @return
     */
    bool isRead1First() const;

    static uint64_t getLengthOfCigar(const std::string& cigar) {
        std::string digits;
        size_t length = 0;
        for (const auto& c : cigar) {
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

    size_t getMappedLength(size_t alignment, size_t split) const {
        if (split == 0) {
            return getLengthOfCigar(getAlignments()[alignment].getAlignment().getECigar());
        }
        auto& s2 = dynamic_cast<record::alignment_split::SameRec&>(
            *getAlignments()[alignment].getAlignmentSplits()[split - 1]);
        return getLengthOfCigar(s2.getAlignment().getECigar());
    }

    std::pair<size_t, size_t> getTemplatePosition() const {
        std::pair<size_t, size_t> ret = {getPosition(0, 0), getPosition(0, 0) + getMappedLength(0, 0)};
        for (size_t i = 0; i < getAlignments().front().getAlignmentSplits().size(); ++i) {
            auto pos = getPosition(0, i);
            ret.first = std::min(ret.first, pos);
            ret.second = std::max(ret.second, pos + getMappedLength(0, i));
        }
        return ret;
    }

    size_t getPosition(size_t alignment, size_t split) const {
        if (split == 0) {
            return getAlignments()[alignment].getPosition();
        }
        auto& s2 = dynamic_cast<record::alignment_split::SameRec&>(
            *getAlignments()[alignment].getAlignmentSplits()[split - 1]);
        return getAlignments()[alignment].getPosition() + s2.getDelta();
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_RECORD_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
