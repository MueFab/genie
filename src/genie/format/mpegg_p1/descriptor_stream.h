/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_H_

#include <memory>
#include <string>
#include <vector>
#include "dataset_header.h"
#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/core/record/class-type.h"
#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgb/au_type_cfg.h"
#include "genie/format/mgb/mm_cfg.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
#include "genie/format/mpegg_p1/descriptor_stream_header.h"
#include "signature_cfg.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DescriptorStream : public GenInfo {
 private:
    DescriptorStreamHeader header;
    boost::optional<DSProtection> ds_protection;
    std::vector<util::DataBlock> payload;
 public:

    explicit DescriptorStream(DescriptorStreamHeader _header) : header(std::move(_header)) {

    }

    explicit DescriptorStream(util::BitReader& reader, const std::vector<uint64_t>& payload_sizes ) {
        auto length = reader.read<uint64_t>();
        header = DescriptorStreamHeader(reader);
        uint64_t total_payload_size = 0;
        for (const auto& p : payload_sizes) {
            total_payload_size += p;
        }
        if (length - header.getSize() - 4 < total_payload_size) {
            ds_protection = DSProtection(reader);
        }
        for (const auto& p : payload_sizes) {
            payload.emplace_back(p, 1);
            reader.readBypass(payload.back().getData(), payload.back().getRawSize());
        }
    }

    const DescriptorStreamHeader& getHeader() const {
        return header;
    }

    void addPayload(util::DataBlock block) {
        payload.emplace_back(std::move(block));
        header.addBlock();
    }

    const std::vector<util::DataBlock>& getPayloads() const {
        return payload;
    }

    void setProtection(DSProtection prot) {
        ds_protection = std::move(prot);
    }


    const DSProtection& getProtection() const {
        return *ds_protection;
    }

    bool hasProtection() const {
        return ds_protection != boost::none;
    };

    void write(util::BitWriter& writer) const override {
        GenInfo::write(writer);
        header.write(writer);
        if(ds_protection != boost::none) {
            ds_protection->write(writer);
        }
        for(const auto& p : payload) {
            writer.writeBypass(p.getData(), p.getRawSize());
        }
    }

    uint64_t getSize() const override {
        std::stringstream stream;
        genie::util::BitWriter writer(&stream);
        write(writer);
        return stream.str().length();
    }

    const std::string& getKey() const override {
        static const std::string key = "dscn";
        return key;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
