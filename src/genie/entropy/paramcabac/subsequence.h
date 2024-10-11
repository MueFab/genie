/**
 * @file
 * @brief Defines the `Subsequence` class, which handles transformation configurations for subsequences in CABAC entropy coding.
 * @details The `Subsequence` class manages the configurations and parameters required to encode or decode a subsequence
 *          using various transformations such as equality coding, match coding, RLE coding, and merge coding.
 *          The class also provides methods for serialization, deserialization, and comparison.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_SUBSEQUENCE_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_SUBSEQUENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include <optional>
#include "genie/entropy/paramcabac/transformed-subseq.h"
#include "genie/entropy/paramcabac/transformed_parameters.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

/**
 * @brief Manages the parameters and configurations for a CABAC encoded subsequence.
 * @details This class stores the transformations applied to a subsequence in the context of CABAC coding. It handles
 *          different transformation types and their parameters, provides methods to access and modify the transformation
 *          configurations, and can serialize/deserialize its content to and from binary and JSON formats.
 */
class Subsequence {
 public:
    /**
     * @brief Default constructor. Initializes a blank `Subsequence` object.
     */
    Subsequence();

    /**
     * @brief Constructor that initializes a `Subsequence` with a descriptor ID and token type flag.
     * @param _descriptor_subsequence_ID ID of the subsequence descriptor.
     * @param _tokentypeFlag Flag indicating whether the subsequence uses a token type.
     */
    Subsequence(uint16_t _descriptor_subsequence_ID, bool _tokentypeFlag);

    /**
     * @brief Constructor to initialize a `Subsequence` with parameters and transformation configurations.
     * @param _transform_subseq_parameters The transformation parameters applied to this subsequence.
     * @param _descriptor_subsequence_ID ID of the subsequence descriptor.
     * @param _tokentypeFlag Flag indicating whether the subsequence uses a token type.
     * @param transformSubseq_cfgs List of transformation configurations for subsequence symbols.
     */
    Subsequence(TransformedParameters&& _transform_subseq_parameters, uint16_t _descriptor_subsequence_ID,
                bool _tokentypeFlag, std::vector<TransformedSubSeq>&& transformSubseq_cfgs);

    /**
     * @brief Deserialization constructor. Reads `Subsequence` data from a binary reader.
     * @param tokentype Boolean flag indicating if a token type is used.
     * @param desc Descriptor of the subsequence.
     * @param reader The `BitReader` used for binary deserialization.
     */
    Subsequence(bool tokentype, core::GenDesc desc, util::BitReader& reader);

    /**
     * @brief Virtual destructor.
     */
    virtual ~Subsequence() = default;

    /**
     * @brief Updates the transformation configuration at a specific index.
     * @param index Index of the transformation configuration to update.
     * @param _transformSubseq_cfg The new transformation configuration.
     */
    void setTransformSubseqCfg(size_t index, TransformedSubSeq&& _transformSubseq_cfg);

    /**
     * @brief Serializes the subsequence and its transformations to a binary writer.
     * @param writer The `BitWriter` used for binary serialization.
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     * @brief Retrieves the ID of the subsequence descriptor.
     * @return The subsequence descriptor ID.
     */
    [[nodiscard]] uint16_t getDescriptorSubsequenceID() const;

    /**
     * @brief Retrieves the transformation parameters applied to this subsequence.
     * @return A reference to the `TransformedParameters` object.
     */
    [[nodiscard]] const TransformedParameters& getTransformParameters() const;

    /**
     * @brief Retrieves the transformation configuration at a specific index.
     * @param index Index of the configuration to retrieve.
     * @return A reference to the `TransformedSubSeq` object.
     */
    [[nodiscard]] const TransformedSubSeq& getTransformSubseqCfg(uint8_t index) const;

    /**
     * @brief Retrieves the number of transformation configurations.
     * @return Number of transformation configurations.
     */
    [[nodiscard]] size_t getNumTransformSubseqCfgs() const;

    /**
     * @brief Retrieves all transformation configurations.
     * @return A vector of all `TransformedSubSeq` configurations.
     */
    [[nodiscard]] const std::vector<TransformedSubSeq>& getTransformSubseqCfgs() const;

    /**
     * @brief Checks if the token type flag is set.
     * @return True if the token type flag is set, otherwise false.
     */
    [[nodiscard]] bool getTokentypeFlag() const;

    /**
     * @brief Equality operator for comparing two `Subsequence` objects.
     * @param seq The other `Subsequence` object to compare with.
     * @return True if both objects are equal, otherwise false.
     */
    bool operator==(const Subsequence& seq) const;

    /**
     * @brief Deserialization constructor. Reads `Subsequence` data from a JSON object.
     * @param j The JSON object used for deserialization.
     */
    explicit Subsequence(nlohmann::json j);

    /**
     * @brief Serializes the subsequence and its configurations to a JSON object.
     * @return A JSON representation of the `Subsequence` object.
     */
    [[nodiscard]] nlohmann::json toJson() const;

 private:
    std::optional<uint16_t> descriptor_subsequence_ID;    //!< @brief The ID of the subsequence descriptor.
    TransformedParameters transform_subseq_parameters;    //!< @brief Transformation parameters for the subsequence.
    std::vector<TransformedSubSeq> transformSubseq_cfgs;  //!< @brief List of transformation configurations.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_SUBSEQUENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
