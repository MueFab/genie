/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_CONTACTMATRIXPARAMETERS_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_CONTACTMATRIXPARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------

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
namespace core {
namespace record {
namespace annotation_parameter_set {

class ContactMatrixParameters {
 private:
    uint8_t num_samples{};
    std::vector<uint8_t> sample_ID{};
    std::vector<std::string> sample_name{};
    uint8_t num_chrs{};
    std::vector<uint8_t> chr_ID{};
    std::vector<std::string> chr_name{};
    std::vector<uint64_t> chr_length{};

    uint32_t interval{};
    uint32_t tile_size{};
    uint8_t num_interval_multipliers{};
    std::vector<uint32_t> interval_multiplier{};

    uint8_t num_norm_methods{};
    std::vector<uint8_t> norm_method_ID{};
    std::vector<std::string> norm_method_name{};
    std::vector<bool> norm_method_mult_flag{};

    uint8_t num_norm_matrices{};
    std::vector<uint8_t> norm_matrix_ID{};
    std::vector<std::string> norm_matrix_name{};

 public:
    ContactMatrixParameters();
    ContactMatrixParameters(uint8_t num_samples, std::vector<uint8_t> sample_ID, std::vector<std::string> sample_name,
                            uint8_t num_chrs, std::vector<uint8_t> chr_ID, std::vector<std::string> chr_name,
                            std::vector<uint64_t> chr_length,

                            uint32_t interval, uint32_t tile_size, uint8_t num_interval_multipliers,
                            std::vector<uint32_t> interval_multiplier,

                            uint8_t num_norm_methods, std::vector<uint8_t> norm_method_ID,
                            std::vector<std::string> norm_method_name, std::vector<bool> norm_method_mult_flag,

                            uint8_t num_norm_matrices, std::vector<uint8_t> norm_matrix_ID,
                            std::vector<std::string> norm_matrix_name);

    void read(util::BitReader& reader);

    void write(core::Writer& writer) const;
    size_t getSize(core::Writer& writesize) const;

    uint8_t getNumberOfSamples() const { return num_samples; }
    std::vector<uint8_t> getSampleIDs() const { return sample_ID; }
    std::vector<std::string> getSampleNames() const { return sample_name; }
    uint8_t getNumberOfChromosomes() const { return num_chrs; }
    std::vector<uint8_t> getChromosomeIDs() const { return chr_ID; }
    std::vector<std::string> getChromosomeNames() const { return chr_name; }
    std::vector<uint64_t> getChromsomeLength() const { return chr_length; }

    uint32_t getInterval() const { return interval; }
    uint32_t getTileSize() const { return tile_size; }
    uint8_t getNumberOfIntervalMultipliers() const { return num_interval_multipliers; }
    std::vector<uint32_t> getIntervalMultipliers() const { return interval_multiplier; }

    uint8_t getNumberOfNormalizationMethods() const { return num_norm_methods; }
    std::vector<uint8_t> getNormalizationMethodIDs() const { return norm_method_ID; }
    std::vector<std::string> getNormalizationMethodNames() const { return norm_method_name; }
    std::vector<bool> getNormalizationMethodMultFlag() const { return norm_method_mult_flag; }

    uint8_t getNumberOfNormalizationMatrices() const { return num_norm_matrices; }
    std::vector<uint8_t> getNormalizationMatrixIDs() const { return norm_matrix_ID; }
    std::vector<std::string> getNormalizationMatrixNames() const { return norm_matrix_name; }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_CONTACTMATRIXPARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
