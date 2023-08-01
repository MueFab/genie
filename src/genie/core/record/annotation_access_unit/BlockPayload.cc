/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "BlockPayload.h"
#include <algorithm>
#include <sstream>
#include <string>
#include <utility>

#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

BlockPayload::BlockPayload()
    : descriptor_ID(AnnotDesc::GENOTYPE),
      num_chrs(0),
      genotype_payload{},
      likelihood_payload{},
      cm_bin_payload{},
      cm_mat_payload{},
      block_payload_size(0),
      generic_payload{} {}

BlockPayload::BlockPayload(AnnotDesc descriptorID,
                           uint32_t block_payload_size, const std::vector<uint8_t>& generic_payload)
    : descriptor_ID(descriptorID),
      num_chrs(0),
      genotype_payload{},
      likelihood_payload{},
      cm_bin_payload{},
      cm_mat_payload{},
      block_payload_size(block_payload_size),
      generic_payload(generic_payload) {}

BlockPayload::BlockPayload(util::BitReader& reader,
                           AnnotDesc descriptorID, uint8_t numChrs) {
    descriptor_ID = descriptorID;
    num_chrs = numChrs;
    read(reader);
}

BlockPayload::BlockPayload(AnnotDesc descriptorID, uint8_t numChrs,
                           GenotypePayload genotype_payload, LikelihoodPayload likelihood_payload,
                           std::vector<ContactMatrixBinPayload> cm_bin_payload, ContactMatrixMatPayload cm_mat_payload,
                           uint32_t block_payload_size, const std::vector<uint8_t>& genericPayload)
    : descriptor_ID(descriptorID),
      num_chrs(numChrs),
      genotype_payload(genotype_payload),
      likelihood_payload(likelihood_payload),
      cm_bin_payload(cm_bin_payload),
      cm_mat_payload(cm_mat_payload),
      block_payload_size(block_payload_size),
      generic_payload(genericPayload) {}

void BlockPayload::read(util::BitReader& reader) {
    if (descriptor_ID == AnnotDesc::GENOTYPE) {
        genotype_payload.read(reader);
    } else if (descriptor_ID == AnnotDesc::LIKELIHOOD) {
        likelihood_payload.read(reader);
    } else if (descriptor_ID == AnnotDesc::CONTACT) {
        for (auto i = 0; i < num_chrs; ++i) {
            ContactMatrixBinPayload cmBinPayload;
            cmBinPayload.read(reader);
            cm_bin_payload.push_back(cmBinPayload);
        }
        cm_mat_payload.read(reader);
    } else {
        generic_payload.push_back(static_cast<uint8_t>(reader.read_b(8)));
    }
    reader.flush();
}

void BlockPayload::read(util::BitReader& reader,
                        AnnotDesc descriptorID, uint8_t numChrs) {
    descriptor_ID = descriptorID;
    num_chrs = numChrs;
    read(reader);
}

void BlockPayload::write(core::Writer& writer) const {
    if (descriptor_ID == AnnotDesc::GENOTYPE) {
        genotype_payload.write(writer);
    } else if (descriptor_ID == AnnotDesc::LIKELIHOOD) {
        likelihood_payload.write(writer);
    } else if (descriptor_ID == AnnotDesc::CONTACT) {
        for (auto binPayload : cm_bin_payload) binPayload.write(writer);
        cm_mat_payload.write(writer);
    } else {
        for (const auto& byte : generic_payload) writer.write(byte, 8, true);
    }
    writer.flush();
}

size_t BlockPayload::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.getBitsWritten();
}


}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
