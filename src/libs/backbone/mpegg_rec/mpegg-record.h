/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MPEGG_RECORD_H
#define GENIE_MPEGG_RECORD_H

// ---------------------------------------------------------------------------------------------------------------------

#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <cstdint>
#include <memory>
#include <vector>
#include "alignment-container.h"
#include "class-type.h"
#include "external-alignment.h"
#include "meta-alignment.h"
#include "segment.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace mpegg_rec {

/**
 *
 */
class MpeggRecord {
   private:
    uint8_t number_of_template_segments{};          //!<
    ClassType class_ID{ClassType::NONE};            //!<
    bool read_1_first{};                            //!<
    MetaAlignment sharedAlignmentInfo;              //!<
    uint8_t qv_depth{};                             //!<
    std::string read_name;                          //!<
    std::string read_group;                         //!<
    std::vector<Segment> reads;                     //!<
    std::vector<AlignmentContainer> alignmentInfo;  //!<
    uint8_t flags{};                                //!<

    std::unique_ptr<ExternalAlignment> moreAlignmentInfo;  //!<

   public:
    /**
     *
     */
    MpeggRecord();

    /**
     *
     * @param _number_of_template_segments
     * @param _auTypeCfg
     * @param _read_name
     * @param _read_group
     * @param _flags
     */
    MpeggRecord(uint8_t _number_of_template_segments, ClassType _auTypeCfg, std::string&& _read_name,
                std::string&& _read_group, uint8_t _flags);

    /**
     *
     * @param rec
     */
    MpeggRecord(const MpeggRecord& rec);

    /**
     *
     * @param rec
     */
    MpeggRecord(MpeggRecord&& rec) noexcept;

    /**
     *
     */
    ~MpeggRecord() = default;

    /**
     *
     * @param rec
     * @return
     */
    MpeggRecord& operator=(const MpeggRecord& rec);

    /**
     *
     * @param rec
     * @return
     */
    MpeggRecord& operator=(MpeggRecord&& rec) noexcept;

    /**
     *
     * @param reader
     */
    explicit MpeggRecord(util::BitReader& reader);

    /**
     *
     * @param rec
     */
    void addRecordSegment(Segment&& rec);

    /**
     *
     * @param _seq_id
     * @param rec
     */
    void addAlignment(uint16_t _seq_id, AlignmentContainer&& rec);

    /**
     *
     * @param index
     * @return
     */
    const std::vector<Segment>& getRecordSegments() const;

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
    const std::vector<AlignmentContainer>& getAlignments() const;

    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     *
     * @return
     */
    std::unique_ptr<MpeggRecord> clone() const;

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
    const std::string& getReadName() const;

    /**
     *
     * @return
     */
    const std::string& getGroupName() const { return read_group; }

    const MetaAlignment& getMetaAlignment() const { return sharedAlignmentInfo; }

    const ExternalAlignment& getExternalAlignmentInfo() const { return *moreAlignmentInfo; }
};

// ---------------------------------------------------------------------------------------------------------------------

/**
 *
 */
typedef std::vector<MpeggRecord> MpeggChunk;

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MPEGG_RECORD_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
