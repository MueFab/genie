/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MPEGG_RECORD_H
#define GENIE_MPEGG_RECORD_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace util {
class BitWriter;
class BitReader;
}  // namespace util

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_rec {
class MetaAlignment;
class Segment;
class AlignmentContainer;
class ExternalAlignment;

/**
 *
 */
class MpeggRecord {
   public:
    /**
     *
     */
    enum class ClassType : uint8_t {
        NONE = 0,      //!<
        CLASS_P = 1,   //!<
        CLASS_N = 2,   //!<
        CLASS_M = 3,   //!<
        CLASS_I = 4,   //!<
        CLASS_HM = 5,  //!<
        CLASS_U = 6    //!<
    };

   private:
    uint8_t number_of_template_segments : 8;  //!<
    // uint8_t number_of_record_segments : 8; //!<
    // uint16_t number_of_alignments : 16; //!<
    ClassType class_ID;  //!<
    // uint8_t read_group_len : 8; //!<
    uint8_t read_1_first : 8;  //!<
    // if(number_of_alignments > 0) //!<
    std::unique_ptr<MetaAlignment> sharedAlignmentInfo;  //!<

    //  for (rSeg=0; rSeg < number_of_record_segments; rSeg++) //!<
    // std::vector<uint32_t> read_len; //: 24;  //!<

    uint8_t qv_depth : 8;  //!<
    // uint8_t read_name_len : 8; //!<
    std::unique_ptr<std::string> read_name;   //!<
    std::unique_ptr<std::string> read_group;  //!<
    // for (rSeg=0; rSeg < number_of_record_segments; rSeg++) //!<
    std::vector<std::unique_ptr<Segment>> reads;  //!<

    // for (noa=0; noa < number_of_alignments; noa++) //!<
    std::vector<std::unique_ptr<AlignmentContainer>> alignmentInfo;  //!<
    uint8_t flags : 8;                                               //!<

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
    MpeggRecord(uint8_t _number_of_template_segments, ClassType _auTypeCfg, std::unique_ptr<std::string> _read_name,
                std::unique_ptr<std::string> _read_group, uint8_t _flags);

    /**
     *
     * @param reader
     */
    explicit MpeggRecord(util::BitReader* reader);

    /**
     *
     * @param rec
     */
    void addRecordSegment(std::unique_ptr<Segment> rec);

    /**
     *
     * @param _seq_id
     * @param rec
     */
    void addAlignment(uint16_t _seq_id, std::unique_ptr<AlignmentContainer> rec);

    /**
     *
     * @param index
     * @return
     */
    const Segment* getRecordSegment(size_t index) const;

    /**
     *
     * @return
     */
    size_t getNumberOfRecords() const;

    /**
     *
     * @return
     */
    size_t getNumberOfAlignments() const;

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
    const AlignmentContainer* getAlignment(size_t index) const;

    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter* writer) const;

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
};

/**
 *
 */
typedef std::vector<std::unique_ptr<MpeggRecord>> MpeggChunk;

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MPEGG_RECORD_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
