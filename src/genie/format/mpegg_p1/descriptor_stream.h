/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DESCRIPTOR_STREAM_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DESCRIPTOR_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <list>
#include <string>
#include <vector>
#include "boost/optional/optional.hpp"
#include "genie/format/mpegg_p1/block.h"
#include "genie/format/mpegg_p1/descriptor_stream_header.h"
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/format/mpegg_p1/master_index_table.h"
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
    std::vector<core::Payload> payload;                 //!< @brief

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

    DescriptorStream(genie::core::GenDesc descriptor, genie::core::record::ClassType classID, std::vector<format::mgb::Block> blocks) : header(false, descriptor, classID, blocks.size()){
        for (auto& b : blocks) {
            payload.emplace_back(b.movePayloadUnparsed());
        }
    }

    std::vector<format::mgb::Block> decapsulate() {
        std::vector<format::mgb::Block> ret;
        for(auto& p : payload) {
            ret.emplace_back(header.getDescriptorID(), std::move(p));
        }
        return ret;
    }

    bool isEmpty() const {
        if(payload.empty()) {
            return true;
        }

        for (const auto& p : payload) {
            if(p.getPayloadSize()) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief
     * @param reader
     * @param payload_sizes
     */
    explicit DescriptorStream(util::BitReader& reader, const MasterIndexTable& table,
                              const std::vector<MITClassConfig>& configs);

    /**
     * @brief
     * @return
     */
    const DescriptorStreamHeader& getHeader() const;

    /**
     * @brief
     * @param block
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
    DSProtection& getProtection() {
        return *ds_protection;
    }

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

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DESCRIPTOR_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
