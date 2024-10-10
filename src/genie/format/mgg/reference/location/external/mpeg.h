/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_MPEG_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_MPEG_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/format/mgg/reference/location/external.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference::location::external {

/**
 * @brief
 */
class MPEG : public External {
 private:
    genie::core::MPEGMinorVersion version;   //!< @brief
    uint8_t external_dataset_group_id;       //!< @brief
    uint16_t external_dataset_id;            //!< @brief
    std::string ref_checksum;                //!< @brief
    std::vector<std::string> seq_checksums;  //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    std::unique_ptr<genie::core::meta::RefBase> decapsulate() override;

    /**
     * @brief
     * @param _reserved
     * @param _uri
     * @param algo
     * @param _group_id
     * @param _dataset_id
     * @param _ref_checksum
     * @param _version
     */
    MPEG(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo, uint8_t _group_id, uint16_t _dataset_id,
         std::string _ref_checksum, genie::core::MPEGMinorVersion _version);

    /**
     * @brief
     * @param reader
     * @param seq_count
     * @param _version
     */
    explicit MPEG(genie::util::BitReader& reader, size_t seq_count, genie::core::MPEGMinorVersion _version);

    /**
     * @brief
     * @param reader
     * @param _reserved
     * @param _uri
     * @param algo
     * @param seq_count
     * @param _version
     */
    explicit MPEG(genie::util::BitReader& reader, uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo,
                  size_t seq_count, genie::core::MPEGMinorVersion _version);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getExternalDatasetGroupID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint16_t getExternalDatasetID() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::string& getRefChecksum() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<std::string>& getSeqChecksums() const;

    /**
     * @brief
     * @param checksum
     */
    void addSeqChecksum(std::string checksum);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) override;

    /**
     * @brief
     * @param checksum
     */
    void addChecksum(std::string checksum) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference::location::external

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_MPEG_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
