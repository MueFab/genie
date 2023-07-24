/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_GENOTYPEPARAMETERS_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_GENOTYPEPARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/core/writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

enum class BinarizationID { BIT_PLANE = 0, ROW_SPLIT };

enum class ConcatAxis { CONCAT_ROW_DIR, CONCAT_COL_DIR, DO_NOT_CONCAT };

class GenotypeParameters {
 private:
    uint8_t max_ploidy;
    bool no_reference_flag;
    bool not_available_flag;
    BinarizationID binarization_ID;
    uint8_t num_bit_plane;
    ConcatAxis concat_axis;
    uint8_t num_variants_payloads;
    std::vector<bool> sort_variants_rows_flag;
    std::vector<bool> sort_variants_cols_flag;
    std::vector<bool> transpose_variants_mat_flag;
    std::vector<uint8_t> variants_codec_ID;

    bool encode_phases_data_flag;
    bool sort_phases_rows_flag;
    bool sort_phases_cols_flag;
    bool transpose_phases_mat_flag;
    bool phases_codec_ID;
    bool phases_value;

 public:
    GenotypeParameters();
    explicit GenotypeParameters(util::BitReader& reader);
    GenotypeParameters(uint8_t max_ploidy, bool no_reference_flag, bool not_available_flag,
                       annotation_parameter_set::BinarizationID binarization_ID, uint8_t num_bit_plane,
                       ConcatAxis concat_axis, std::vector<bool> sort_variants_rows_flag,
                       std::vector<bool> sort_variants_cols_flag, std::vector<bool> transpose_variants_mat_flag,
                       std::vector<uint8_t> variants_codec_ID, bool encode_phases_data_flag, bool sort_phases_rows_flag,
                       bool sort_phases_cols_flag, bool transpose_phases_mat_flag, bool phases_codec_ID,
                       bool phases_value);

    void read(util::BitReader& reader);
    void write(util::BitWriter& writer) const;
    void write(std::ostream& outputfile) const;
    void write(core::Writer& writer) const;
    size_t getSize(core::Writer& writesize) const;

    uint8_t getMaxPloidy() const { return max_ploidy; }
    bool isNoReference() const { return no_reference_flag; }
    bool isNotAvailable() const { return not_available_flag; }
    BinarizationID getBinarizationID() const { return binarization_ID; }
    uint8_t getNumberOfBitPlane() const { return num_bit_plane; }
    ConcatAxis getConcatAxis() const { return concat_axis; }
    std::vector<bool> getSortVariantsRowsFlags() const { return sort_variants_rows_flag; }
    std::vector<bool> getSortVariantsColsFlags() const { return sort_variants_cols_flag; }
    std::vector<bool> getTransposeVariantsMatFlags() const { return transpose_variants_mat_flag; }
    std::vector<uint8_t> getVariantsCodecIDs() const { return variants_codec_ID; }

    bool isEncodePhaseData() const { return encode_phases_data_flag; }
    bool isSortPhasesRows() const { return sort_phases_rows_flag; }
    bool isSortPhasesCols() const { return sort_phases_cols_flag; }
    bool isTransposePhaseMat() const { return transpose_phases_mat_flag; }
    bool isPhasesCodecID() const { return phases_codec_ID; }
    bool isPhasesValue() const { return phases_value; }
};
// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_GENOTYPEPARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
