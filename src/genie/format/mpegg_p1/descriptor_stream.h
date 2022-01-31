/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <boost/optional/optional.hpp>
#include "genie/format/mpegg_p1/descriptor_stream_header.h"
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/util/bitreader.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DescriptorStream : public GenInfo {
 private:
    DescriptorStreamHeader header;                //!< @brief
    boost::optional<DSProtection> ds_protection;  //!< @brief
    std::vector<util::DataBlock> payload;         //!< @brief

 public:
    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     * @param _header
     */
    explicit DescriptorStream(DescriptorStreamHeader _header);

    /**
     * @brief
     * @param reader
     * @param payload_sizes
     */
    explicit DescriptorStream(util::BitReader& reader, const std::vector<uint64_t>& payload_sizes);

    /**
     * @brief
     * @return
     */
    const DescriptorStreamHeader& getHeader() const;

    /**
     * @brief
     * @param block
     */
    void addPayload(util::DataBlock block);

    /**
     * @brief
     * @return
     */
    const std::vector<util::DataBlock>& getPayloads() const;

    /**
     * @brief
     * @param prot
     */
    void setProtection(DSProtection prot);

    /**
     * @brief
     * @return
     */
    const DSProtection& getProtection() const;

    /**
     * @brief
     * @return
     */
    bool hasProtection() const;

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter& writer) const override;

    /**
     * @brief
     * @return
     */
    uint64_t getSize() const override;

    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
