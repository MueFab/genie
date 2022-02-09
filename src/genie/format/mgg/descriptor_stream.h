/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_H_
#define SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <list>
#include <string>
#include <vector>
#include "boost/optional/optional.hpp"
#include "genie/format/mgg/block.h"
#include "genie/format/mgg/descriptor_stream_header.h"
#include "genie/format/mgg/descriptor_stream_protection.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/format/mgg/master_index_table.h"
#include "genie/util/bitreader.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

/**
 * @brief
 */
class DescriptorStream : public GenInfo {
 private:
    DescriptorStreamHeader header;                              //!< @brief
    boost::optional<DescriptorStreamProtection> ds_protection;  //!< @brief
    std::vector<core::Payload> payload;                         //!< @brief

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
     * @param descriptor
     * @param classID
     * @param blocks
     */
    DescriptorStream(genie::core::GenDesc descriptor, genie::core::record::ClassType classID,
                     std::vector<format::mgb::Block> blocks);

    /**
     * @brief
     * @return
     */
    std::vector<format::mgb::Block> decapsulate();

    /**
     * @brief
     * @return
     */
    bool isEmpty() const;

    /**
     * @brief
     * @param reader
     * @param table
     * @param configs
     */
    explicit DescriptorStream(util::BitReader& reader, const MasterIndexTable& table,
                              const std::vector<dataset_header::MITClassConfig>& configs);

    /**
     * @brief
     * @return
     */
    const DescriptorStreamHeader& getHeader() const;

    /**
     * @brief
     * @param p
     */
    void addPayload(core::Payload p);

    /**
     * @brief
     * @return
     */
    const std::vector<core::Payload>& getPayloads() const;

    /**
     * @brief
     * @param prot
     */
    void setProtection(DescriptorStreamProtection prot);

    /**
     * @brief
     * @return
     */
    const DescriptorStreamProtection& getProtection() const;

    /**
     * @brief
     * @return
     */
    DescriptorStreamProtection& getProtection();

    /**
     * @brief
     * @return
     */
    bool hasProtection() const;

    /**
     * @brief
     * @param writer
     */
    void box_write(util::BitWriter& writer) const override;

    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DESCRIPTOR_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
