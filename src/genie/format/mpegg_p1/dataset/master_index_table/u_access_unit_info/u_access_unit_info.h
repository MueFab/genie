#ifndef GENIE_PART1_MIT_U_ACCESS_UNIT_INFO_H
#define GENIE_PART1_MIT_U_ACCESS_UNIT_INFO_H

#include <cstdint>
#include <vector>
#include <genie/util/bitwriter.h>

#include "genie/format/mpegg_p1/dataset/dataset_header.h"
#include "genie/format/mpegg_p1/dataset/master_index_table/access_unit_info/access_unit_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class MITUAccessUnitInfo {
   private:
    //uint32_t num_U_clusters;
    uint32_t multiple_signature_base;
    uint8_t U_signature_size;
    bool U_signature_constant_length;
    uint8_t U_signature_length;

    std::vector<uint8_t> U_cluster_signature;
    uint16_t num_signatures;

    DatasetHeader* datasetHeader;

    uint16_t U_ref_sequence_id;
    uint64_t U_ref_start_position;
    uint64_t U_ref_end_position;

    /** ------------------------------------------------------------------------------------------------------------
     *  Internal
     *  ------------------------------------------------------------------------------------------------------------ */
    DatasetHeader::ByteOffsetSizeFlag byte_offset_size_flag;
    DatasetHeader::Pos40SizeFlag pos_40_size_flag;
   public:
    /**
     *
     */
    MITUAccessUnitInfo();

    /**
     *
     * @param _num_U_clusters
     */
    explicit MITUAccessUnitInfo(uint32_t _num_U_clusters);

    /**
     *
     * @param reader
     * @param length
     */
    MITUAccessUnitInfo(util::BitReader& reader);
    /**
     *
     * @param base
     * @param size
     */
    void setMultipleSignature(uint32_t base, uint8_t size);
    /**
     *
     * @param sign_length
     */
    void setConstantSignature(uint8_t sign_length);
    /**
     *
     * @return
     */
    uint8_t getUSignatureSize() const;
    /**
     *
     * @return
     */
    uint32_t getMultipleSignatureBase() const;

    /**
     *
     * @return
     */
    uint64_t getBitLength() const;
    /**
     * Write to bit_writer
     *
     * @param bit_writer
     */
    //void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie


#endif  // GENIE_PART1_MIT_U_ACCESS_UNIT_INFO_H
