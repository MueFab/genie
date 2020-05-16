/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_CONSTANTS_H_
#define GABAC_CONSTANTS_H_

#include <stdint.h>
#include <functional>
#include <string>
#include <vector>

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie

namespace genie {
namespace entropy {
namespace gabac {

/**
 * @brief Supported transformations
 */
enum class DescSubseqTransformationId {
    no_transform = 0,    /**< @brief Do nothing */
    equality_coding = 1, /**< @brief Find equal values sequentially */
    match_coding = 2,    /**< @brief Find larger sequence matches */
    rle_coding = 3,      /**< @brief Find run lengths */
    merge_coding = 4,    /**< @brief Find run lengths */
};

/**
 * @brief Transformation signature
 */
using DescSubseqTransform =
    std::function<void(const std::vector<uint64_t>& param, std::vector<util::DataBlock>* const)>;

/**
 * @brief Transformation meta data available to applications using gabac
 */
struct TransformationProperties {
    std::string name;                     /**< @brief Name of transformation */
    std::vector<std::string> paramNames;  /**< @brief Name of every parameter */
    std::vector<std::string> streamNames; /**< @brief Name of every stream */
    std::vector<uint8_t> wordsizes;       /**< @brief Wordsizes of every stream. Zero
                                             means word size of input data */
    DescSubseqTransform transform;        /**< @brief Function pointer to transformation */
    DescSubseqTransform inverseTransform; /**< @brief Function pointer to inverse
                                           transformation */
};

/**
 * @brief Get the meta information of one transformation
 * @param id Transformation ID
 * @return Properties object
 */
const TransformationProperties& getTransformation(const gabac::DescSubseqTransformationId& id);

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_CONSTANTS_H_
