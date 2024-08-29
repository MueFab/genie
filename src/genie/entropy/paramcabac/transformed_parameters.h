/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_TRANSFORMED_PARAMETERS_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_TRANSFORMED_PARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <optional>
#include <vector>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

/**
 * @brief
 */
class TransformedParameters {
 public:
    /**
     * @brief
     */
    enum class TransformIdSubseq : uint8_t {
        NO_TRANSFORM = 0,
        EQUALITY_CODING = 1,
        MATCH_CODING = 2,
        RLE_CODING = 3,
        MERGE_CODING = 4
    };

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter &writer) const;

    /**
     * @brief
     */
    TransformedParameters();

    /**
     * @brief
     * @param reader
     */
    explicit TransformedParameters(util::BitReader &reader);

    /**
     * @brief
     * @param _transform_ID_subseq
     * @param param
     */
    TransformedParameters(const TransformIdSubseq &_transform_ID_subseq, uint16_t param);

    /**
     * @brief
     */
    virtual ~TransformedParameters() = default;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] size_t getNumStreams() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] TransformIdSubseq getTransformIdSubseq() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint16_t getParam() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<uint8_t> getMergeCodingShiftSizes() const;

    /**
     * @brief
     * @param mergeCodingshiftSizes
     */
    void setMergeCodingShiftSizes(std::vector<uint8_t> mergeCodingshiftSizes);

    /**
     * @brief
     * @param val
     * @return
     */
    bool operator==(const TransformedParameters &val) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] nlohmann::json toJson() const;

    /**
     * @brief
     * @param _json
     */
    explicit TransformedParameters(const nlohmann::json &_json);

 private:
    TransformIdSubseq transform_ID_subseq;             //!< @brief
    std::optional<uint16_t> match_coding_buffer_size;  //!< @brief
    std::optional<uint8_t> rle_coding_guard;           //!< @brief
    std::optional<uint8_t> merge_coding_subseq_count;  //!< @brief
    std::vector<uint8_t> merge_coding_shift_size;      //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_TRANSFORMED_PARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
