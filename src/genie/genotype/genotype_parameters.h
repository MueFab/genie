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
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

// ---------------------------------------------------------------------------------------------------------------------

enum class BinarizationID : uint8_t {
    BIT_PLANE = 0,
    ROW_BIN = 1,  // should be ROW_SPLIT?
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
    GenotypeBinMatParameters()
        : sort_rows_flag(false),
          sort_cols_flag(false),
          transpose_mat_flag(false),
          variants_codec_ID(genie::core::AlgoID::BSC) {}
};

// ---------------------------------------------------------------------------------------------------------------------

class GenotypeParameters {
 private:
    uint8_t max_ploidy_;
    bool no_reference_flag_;
    bool not_available_flag_;
    BinarizationID binarization_ID_;

    uint8_t num_bit_plane_;
    ConcatAxis concat_axis_;

    std::vector<GenotypeBinMatParameters> variants_payload_params_;

    bool encode_phases_data_flag_;
    GenotypeBinMatParameters phases_payload_params_;
    bool phasing_value_;

 public:
    // Default constructor
    GenotypeParameters();

    // Parameterized constructor
    GenotypeParameters(uint8_t max_ploidy, bool no_reference_flag, bool not_available_flag,
                       BinarizationID binarization_ID, uint8_t num_bit_planes, ConcatAxis concat_axis,
                       std::vector<GenotypeBinMatParameters>&& variants_payload_params, bool encode_phases_data_flag,
                       GenotypeBinMatParameters phases_payload_params, bool phases_value);

    // Move constructor
    GenotypeParameters(GenotypeParameters&& other) noexcept;

    // Copy constructor
    GenotypeParameters(const GenotypeParameters& other);

    // Move assignment operator
    GenotypeParameters& operator=(GenotypeParameters&& other) noexcept;

    // Copy assignment operator
    GenotypeParameters& operator=(const GenotypeParameters& other);

    // Getters
    uint8_t GetMaxPloidy() const;
    bool GetNoReferenceFlag() const;
    bool GetNotAvailableFlag() const;
    BinarizationID GetBinarizationID() const;
    uint8_t GetNumBitPlanes() const;
    ConcatAxis GetConcatAxis() const;
    uint8_t GetNumVariantsPayloads() const;
    const std::vector<GenotypeBinMatParameters>& GetVariantsPayloadParams() const;
    bool IsPhaseEncoded() const;
    const GenotypeBinMatParameters& GetPhasesPayloadParams() const;
    bool GetPhaseValue() const;

    // Setters
    void SetMaxPloidy(uint8_t value);
    void SetNoReferenceFlag(bool value);
    void SetNotAvailableFlag(bool value);
    void SetBinarizationID(BinarizationID value);
    void SetNumBitPlanes(uint8_t value);
    void SetConcatAxis(ConcatAxis value);
    void SetVariantsPayloadParams(std::vector<GenotypeBinMatParameters>&& value);
    void SetEncodePhasesDataFlag(bool value);
    void SetPhasesPayloadParams(GenotypeBinMatParameters&& value);
    void SetPhaseValue(bool value);

    void Write(core::Writer& writer) const;
    size_t GetSize(core::Writer& writesize) const;

    //TODO: Why we have read function here instead of a constructor?
    void read(util::BitReader& reader);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GENOTYPE_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------
