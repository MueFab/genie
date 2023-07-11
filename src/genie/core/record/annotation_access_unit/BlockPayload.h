/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCKPAYLOAD_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCKPAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/core/record/annotation_parameter_set/GenotypeParameters.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

class VariantsPayload {
 public:
    void read(genie::util::BitReader& reader) { reader.read_b(1); }
};

class RowColIDsPayload {
 private:
    uint8_t nbits_per_elem;
    uint8_t nelements;
    std::vector<uint8_t> row_col_ids_elements;

 public:
    RowColIDsPayload();
    void read(genie::util::BitReader& reader) { reader.read_b(1); }
};

class AmaxPayload {
 private:
    uint32_t nelems;
    uint8_t nbits_per_elem;
    uint8_t nelements;
    std::vector<bool> is_one_flag;
    std::vector<uint32_t> amax_elements;

 public:
    void read(genie::util::BitReader& reader) { reader.read_b(1); }
};

class BinaryMatrixPayload {
 private:
    uint8_t codec_ID;  // unknown, can be JBIG or CABAC
    uint32_t payload;  // unknown, described in ISO/IEC 11544 or ISO/IEC 23092-2
    uint32_t nrows;
    uint32_t ncols;

 public:
    void read(genie::util::BitReader& reader) { reader.read_b(1); }
};

class GenotypePayload {
 private:
    uint8_t num_variants_payloads;
    std::vector<uint32_t> variants_payload_size;
    std::vector<VariantsPayload> variants_payload;
    std::vector<bool> sort_variants_row_flag;
    std::vector<uint32_t> sort_variants_row_ids_payload_size;
    std::vector<RowColIDsPayload> sort_variants_row_ids_payload;
    std::vector<bool> sort_variants_col_flags;
    std::vector<uint32_t> sort_variants_col_ids_payload_size;
    std::vector<RowColIDsPayload> sort_variants_col_ids_payload;
    genie::core::record::annotation_parameter_set::BinarizationID binarization_id;
    uint32_t variant_amax_payload_size;
    AmaxPayload variant_amax_payload;
    bool encode_phase_data;
    uint32_t phases_payload_size;
    BinaryMatrixPayload phases_payload;

 public:
    GenotypePayload();
    GenotypePayload(util::BitReader& reader,
                    genie::core::record::annotation_parameter_set::GenotypeParameters& genotypeParameters);
    void read(util::BitReader& reader,
              genie::core::record::annotation_parameter_set::GenotypeParameters& genotypeParameters);
    void read(util::BitReader& reader);
    void write(core::Writer& writer) const { writer.write(0, 1); }
};

class LikelihoodPayload {
 public:
    void read(util::BitReader& reader) { reader.read_b(1); }
    void write(core::Writer& writer) const { writer.write(0, 1); }
};
class ContactMatrixBinPayload {
 public:
    void read(util::BitReader& reader) { reader.read_b(1); }
    void write(core::Writer& writer) const { writer.write(0, 1); }
};
class ContactMatrixMatPayload {
 public:
    void read(util::BitReader& reader) { reader.read_b(1); }
    void write(core::Writer& writer) const { writer.write(0, 1); }
};

//----------------------------------------------------------------------------------//

class BlockPayload {
 private:
    genie::core::record::annotation_parameter_set::DescriptorID descriptor_ID;
    uint8_t num_chrs;
    GenotypePayload genotype_payload;
    LikelihoodPayload likelihood_payload;
    std::vector<ContactMatrixBinPayload> cm_bin_payload;
    ContactMatrixMatPayload cm_mat_payload;
    uint32_t block_payload_size;
    std::vector<uint8_t> generic_payload;

 public:
    BlockPayload();
    BlockPayload(util::BitReader& reader,
                       genie::core::record::annotation_parameter_set::DescriptorID descriptorID,
                 uint8_t numChrs);

    BlockPayload(genie::core::record::annotation_parameter_set::DescriptorID descriptorID, uint8_t numChrs,
                 GenotypePayload genotype_payload, LikelihoodPayload likelihood_payload,
                 std::vector<ContactMatrixBinPayload> cm_bin_payload, ContactMatrixMatPayload cm_mat_payload,
                 uint32_t block_payload_size, const std::vector<uint8_t>& generic_payload);
 
    void read(util::BitReader& reader);
    void read(util::BitReader& reader, genie::core::record::annotation_parameter_set::DescriptorID descriptorID,
              uint8_t numChrs);
    void write(core::Writer& writer);
};



// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCKPAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
