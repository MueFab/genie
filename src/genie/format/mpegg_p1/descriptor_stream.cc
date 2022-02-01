/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/descriptor_stream.h"
#include <sstream>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

bool DescriptorStream::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const DescriptorStream&>(info);
    return header == other.header && ds_protection == other.ds_protection && payload == other.payload;
}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStream::DescriptorStream(DescriptorStreamHeader _header) : header(std::move(_header)) {}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStream::DescriptorStream(util::BitReader& reader, const std::vector<uint64_t>& payload_sizes) {
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

// ---------------------------------------------------------------------------------------------------------------------

const DescriptorStreamHeader& DescriptorStream::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorStream::addPayload(util::DataBlock block) {
    payload.emplace_back(std::move(block));
    header.addBlock();
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<util::DataBlock>& DescriptorStream::getPayloads() const { return payload; }

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorStream::setProtection(DSProtection prot) { ds_protection = std::move(prot); }

// ---------------------------------------------------------------------------------------------------------------------

const DSProtection& DescriptorStream::getProtection() const { return *ds_protection; }

// ---------------------------------------------------------------------------------------------------------------------

bool DescriptorStream::hasProtection() const { return ds_protection != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorStream::write(util::BitWriter& writer) const {
    GenInfo::write(writer);
    header.write(writer);
    if (ds_protection != boost::none) {
        ds_protection->write(writer);
    }
    for (const auto& p : payload) {
        writer.writeBypass(p.getData(), p.getRawSize());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DescriptorStream::getSize() const {
    std::stringstream stream;
    genie::util::BitWriter writer(&stream);
    write(writer);
    return stream.str().length();
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DescriptorStream::getKey() const {
    static const std::string key = "dscn";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
