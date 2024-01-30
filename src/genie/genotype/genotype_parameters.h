/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENOTYPE_PARAMETERS_H
#define GENIE_GENOTYPE_PARAMETERS_H

#include <boost/optional/optional.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

// ---------------------------------------------------------------------------------------------------------------------

enum class BinarizationID : uint8_t {
    BIT_PLANE = 0,
    ROW_BIN = 1, // should be ROW_SPLIT?
    UNDEFINED = 2,
};

// ---------------------------------------------------------------------------------------------------------------------

enum class ConcatAxis : uint8_t {
    CONCAT_ROW_DIR = 0,
    CONCAT_COL_DIR = 1,
    DO_NOT_CONCAT = 2,
    UNDEFINED = 3,
};

// ---------------------------------------------------------------------------------------------------------------------

struct GenotypeBinMatParameters {
    bool sort_rows_flag;
    bool sort_cols_flag;
    bool transpose_mat_flag;
    genie::core::AlgoID variants_codec_ID;
};

// ---------------------------------------------------------------------------------------------------------------------

class GenotypeParameters {
 private:
    uint8_t max_ploidy;
    bool no_reference_flag;
    bool not_available_flag;
    BinarizationID binarization_ID;

    ConcatAxis concat_axis;

    std::vector<GenotypeBinMatParameters> variants_payload_params;

    bool encode_phases_data_flag;
    GenotypeBinMatParameters phases_payload_params;
    bool phasing_value;

 public:
    GenotypeParameters();

    GenotypeParameters(uint8_t max_ploidy, bool no_reference_flag, bool not_available_flag,
                       BinarizationID binarization_ID, ConcatAxis concat_axis,
                       std::vector<GenotypeBinMatParameters>&& variants_payload_params, bool encode_phases_data_flag,
                       GenotypeBinMatParameters phases_payload_params, bool phases_value);

//    GenotypeParameters(GenotypeParameters&& other);

    uint8_t getMaxPloidy() const;
    bool getNoRerefernceFlag() const;
    bool getNotAvailableFlag() const;
    BinarizationID getBinarizationID() const;
    uint8_t getNumBitPlanes() const;
    ConcatAxis isConcatenated() const;
    const std::vector<GenotypeBinMatParameters>& getVariantsPayloadParams() const;

    bool isPhaseEncoded() const;
    // IF encode_phases_data_flag
    const GenotypeBinMatParameters& getPhasesPayloadParams() const;
    uint8_t getNumVariantsPayloads() const;
    // ELSE
    bool getPhaseValue() const;

    void write(core::Writer& writer) const;
    size_t getSize(core::Writer& writesize) const;
    void read(util::BitReader& reader) { (void)reader; }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GENOTYPE_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------
