/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DATASET_HEADER_H
#define GENIE_DATASET_HEADER_H

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bitwriter.h"

#include <cstdint>
#include <iostream>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 *
 */
class DatasetHeader {
   public:
    /**
     *
     * @param x_datasetID
     */
    explicit DatasetHeader(const uint16_t x_datasetID);

    /**
     *
     * @param datasetGroupId
     * @param datasetId
     */
    DatasetHeader(uint8_t datasetGroupId, uint16_t datasetId/*, char *version, uint8_t byteOffsetSizeFlag,
                  uint8_t nonOverlappingAuRangeFlag, uint8_t pos40BitsFlag, uint8_t blockHeaderFlag, uint8_t mitFlag,
                  uint8_t ccModeFlag, uint8_t orderedBlocksFlag, uint16_t seqCount, uint8_t referenceId,
                  const std::vector<uint16_t> &seqId, const std::vector<uint32_t> &seqBlocks, uint8_t datasetType,
                  uint8_t numClasses, const std::vector<uint8_t> &clid, const std::vector<uint8_t> &numDescriptors,
                  const std::vector<std::vector<uint8_t>> &descriptorId, uint8_t alphabetId, uint32_t numUAccessUnits,
                  uint32_t numUClusters, uint32_t multipleSignatureBase, uint8_t uSignatureSize,
                  uint8_t uSignatureConstantLength, uint8_t uSignatureLength, const std::vector<uint8_t> &tflag,
                  const std::vector<uint32_t> &thres*/);

    /**
     *
     * @param datasetGroupId
     */
    void setDatasetGroupId(uint8_t datasetGroupId);

    /**
     *
     * @return
     */
    uint16_t getDatasetId() const;

    /**
     *
     * @return
     */
    uint8_t getDatasetGroupId() const;

    /**
     *
     * @return
     */
    uint64_t getLength() const;

    /**
     *
     * @param bitWriter
     */
    void writeToFile(genie::util::BitWriter& bitWriter) const;

   private:
    uint8_t dataset_group_ID : 8;                     //!<
    uint16_t dataset_ID : 16;                         //!<
    std::string version;                              //!<
    uint8_t byte_offset_size_flag : 1;                //!<
    uint8_t non_overlapping_AU_range_flag : 1;        //!<
    uint8_t pos_40_bits_flag : 1;                     //!<
    uint8_t block_header_flag : 1;                    //!<
    uint8_t MIT_flag : 1;                             //!<
    uint8_t CC_mode_flag : 1;                         //!<
    uint8_t ordered_blocks_flag : 1;                  //!<
    uint16_t seq_count : 16;                          //!<
    uint8_t reference_ID : 8;                         //!<
    std::vector<uint16_t> seq_ID;                     //!<
    std::vector<uint32_t> seq_blocks;                 //!<
    uint8_t dataset_type : 4;                         //!<
    uint8_t num_classes : 4;                          //!<
    std::vector<uint8_t> clid;                        //!<
    std::vector<uint8_t> num_descriptors;             //!<
    std::vector<std::vector<uint8_t>> descriptor_ID;  //!<
    uint8_t alphabet_ID : 8;                          //!<
    uint32_t num_U_access_units : 32;                 //!<
    uint32_t num_U_clusters : 32;                     //!<
    uint32_t multiple_signature_base : 31;            //!<
    uint8_t U_signature_size : 6;                     //!<
    uint8_t U_signature_constant_length : 1;          //!<
    uint8_t U_signature_length : 8;                   //!<
    std::vector<uint8_t> tflag;                       //!<
    std::vector<uint32_t> thres;                      //!<
    // TODO: nesting_zero_bit:
    // while(!writer.byte_aligned()) {
    //     writer->write(0);
    // }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DATASET_HEADER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------