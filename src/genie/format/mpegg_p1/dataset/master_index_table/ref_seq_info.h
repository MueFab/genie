#ifndef GENIE_PART1_MIT_REF_SEQ_INFO_H
#define GENIE_PART1_MIT_REF_SEQ_INFO_H

#include <cstdint>

#include <genie/util/bitwriter.h>
#include <genie/format/mpegg_p1/dataset/dataset_header.h>

namespace genie {
namespace format {
namespace mpegg_p1 {

class MITReferenceSequenceInfo {
   private:
    uint16_t ref_sequence_id;
    uint64_t ref_start_position;
    uint64_t ref_end_position;

    /** ------------------------------------------------------------------------------------------------------------
     *  Internal
     *  ------------------------------------------------------------------------------------------------------------ */
    DatasetHeader::Pos40SizeFlag pos_40_flag;

   public:
    /**
     *
     * @param reader
     */
    MITReferenceSequenceInfo(util::BitReader& reader);
    /**
     *
     * @param seq_id
     * @param start_pos
     * @param end_pos
     * @param flag
     */
    MITReferenceSequenceInfo(uint16_t seq_id, uint64_t start_pos, uint64_t end_pos, DatasetHeader::Pos40SizeFlag flag);
    /**
     *
     * @return
     */
    uint64_t getBitLength() const;
    /**
     *
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_MIT_REF_SEQ_INFO_H
