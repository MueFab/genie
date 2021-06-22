/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_RECORD_H_
#define SRC_GENIE_CORE_RECORD_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/record/alignment-box.h"
#include "genie/core/record/alignment-external.h"
#include "genie/core/record/alignment-shared-data.h"
#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/core/record/class-type.h"
#include "genie/core/record/segment.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

/**
 *
 */
class Record {
 private:
    uint8_t number_of_template_segments{};    //!< @brief
    ClassType class_ID{ClassType::NONE};      //!< @brief
    bool read_1_first{};                      //!< @brief
    AlignmentSharedData sharedAlignmentInfo;  //!< @brief
    uint8_t qv_depth{};                       //!< @brief
    std::string read_name;                    //!< @brief
    std::string read_group;                   //!< @brief
    std::vector<Segment> reads;               //!< @brief
    std::vector<AlignmentBox> alignmentInfo;  //!< @brief
    uint8_t flags{};                          //!< @brief

    std::unique_ptr<AlignmentExternal> moreAlignmentInfo;  //!< @brief

 public:
    /**
     * @brief
     */
    Record();

    /**
     * @brief
     * @param _number_of_template_segments
     * @param _auTypeCfg
     * @param _read_name
     * @param _read_group
     * @param _flags
     * @param _is_read_1_first
     */
    Record(uint8_t _number_of_template_segments, ClassType _auTypeCfg, std::string&& _read_name,
           std::string&& _read_group, uint8_t _flags, bool _is_read_1_first = true);

    /**
     * @brief
     * @param rec
     */
    Record(const Record& rec);

    /**
     * @brief
     * @param rec
     */
    Record(Record&& rec) noexcept;

    /**
     * @brief
     */
    ~Record() = default;

    /**
     * @brief
     * @param rec
     * @return
     */
    Record& operator=(const Record& rec);

    /**
     * @brief
     * @param rec
     * @return
     */
    Record& operator=(Record&& rec) noexcept;

    /**
     * @brief
     * @param reader
     */
    explicit Record(util::BitReader& reader);

    /**
     * @brief
     * @param rec
     */
    void addSegment(Segment&& rec);

    /**
     * @brief
     * @param _seq_id
     * @param rec
     */
    void addAlignment(uint16_t _seq_id, AlignmentBox&& rec);

    /**
     * @brief
     * @param index
     * @return
     */
    std::vector<Segment>& getSegments();

    /**
     * @brief
     * @return
     */
    const std::vector<Segment>& getSegments() const;

    /**
     * @brief
     * @return
     */
    size_t getNumberOfTemplateSegments() const;

    /**
     * @brief
     * @param index
     * @return
     */
    const std::vector<AlignmentBox>& getAlignments() const;

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    uint8_t getFlags() const;

    /**
     * @brief
     * @return
     */
    ClassType getClassID() const;

    /**
     * @brief
     * @return
     */
    const std::string& getName() const;

    /**
     * @brief
     * @param _name
     */
    void setName(const std::string& _name);

    /**
     * @brief
     * @param depth
     */
    void setQVDepth(uint8_t depth);

    /**
     * @brief
     * @param type
     */
    void setClassType(ClassType type);

    /**
     * @brief
     * @return
     */
    const std::string& getGroup() const;

    /**
     * @brief
     * @return
     */
    const AlignmentSharedData& getAlignmentSharedData() const;

    /**
     * @brief
     * @return
     */
    const AlignmentExternal& getAlignmentExternal() const;

    /**
     * @brief
     * @return
     */
    bool isRead1First() const;

    /**
     * @brief
     * @param cigar
     * @return
     */
    static uint64_t getLengthOfCigar(const std::string& cigar);

    /**
     * @brief
     * @param alignment
     * @param split
     * @return
     */
    size_t getMappedLength(size_t alignment, size_t split) const;

    /**
     * @brief
     * @return
     */
    std::pair<size_t, size_t> getTemplatePosition() const;

    /**
     * @brief
     * @param alignment
     * @param split
     * @return
     */
    size_t getPosition(size_t alignment, size_t split) const;

    /**
     * @brief
     * @return
     */
    bool getRead1First() const;

    /**
     * @brief
     * @return
     */
    void setMoreAlignmentInfo(std::unique_ptr<AlignmentExternal> _more_alignment_info);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_RECORD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
