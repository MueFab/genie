/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_RECORD_H
#define GENIE_RECORD_H

// ---------------------------------------------------------------------------------------------------------------------

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
    std::unique_ptr<Record> clone() const;

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

    void setName(const std::string& _name) { read_name = _name; }

    /**
     *
     * @return
     */
    const std::string& getGroup() const;

    const AlignmentSharedData& getAlignmentSharedData() const;

    const AlignmentExternal& getAlignmentExternal() const;

    void setClassType(ClassType type) { this->class_ID = type; }
};

/**
 *
 */
typedef std::vector<Record> Chunk;

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_RECORD_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
