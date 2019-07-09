/**
 * @file
 * @brief Defining all basic constants describing the gabac functionality
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_CONSTANTS_H_
#define GABAC_CONSTANTS_H_

#include <functional>
#include <string>
#include <vector>

namespace gabac {

class DataBlock;

/**
 * @brief Supported transformations
 */
enum class SequenceTransformationId
{
    no_transform = 0,  /**< @brief Do nothing */
    equality_coding = 1,  /**< @brief Find equal values sequentially */
    match_coding = 2,  /**< @brief Find larger sequence matches */
    rle_coding = 3,  /**< @brief Find run lengths */
    lut_transform = 4,  /**< @brief Remap symbols based on probability */
    diff_coding = 5,  /**< @brief Use differences between symbol values instead of symbols */
    cabac_coding = 6  /**< @brief Entropy coding based on cabac */
};

/**
 * @brief Supported binarization
 */
enum class BinarizationId
{
    BI = 0,  /**< @brief Binary */
    TU = 1,  /**< @brief Truncated Unary */
    EG = 2,  /**< @brief Exponential Golomb */
    SEG = 3,  /**< @brief Signed Exponential Golomb */
    TEG = 4,  /**< @brief Truncated Exponential Golomb */
    STEG = 5  /**< @brief Signed Truncated Exponential Golomb */
};

/**
 * @brief Supported cabac contexts
 */
enum class ContextSelectionId
{
    bypass = 0,  /**< @brief Do not use arithmetic coding */
    adaptive_coding_order_0 = 1,  /**< @brief Current symbol only */
    adaptive_coding_order_1 = 2,  /**< @brief Use current + previous symbol */
    adaptive_coding_order_2 = 3  /**< @brief Use current + previous + before previous symbol */
};

/**
 * @brief Transformation signature
 */
using SequenceTransform = std::function<void(const std::vector<uint64_t>& param,
                                             std::vector<gabac::DataBlock> *const
)>;

/**
 * @brief Get property based on binarization parameter
 */
using SignedBinarizationBorder = std::function<uint64_t(uint64_t parameter
)>;

/**
 * @brief Transformation meta data available to applications using gabac
 */
struct TransformationProperties
{
    std::string name;  /**< @brief Name of transformation */
    std::vector<std::string> paramNames;  /**< @brief Name of every parameter */
    std::vector<std::string> streamNames;  /**< @brief Name of every stream */
    std::vector<uint8_t> wordsizes;  /**< @brief Wordsizes of every stream. Zero means word size of input data */
    SequenceTransform transform;  /**< @brief Function pointer to transformation */
    SequenceTransform inverseTransform;  /**< @brief Function pointer to inverse transformation */
};

/**
 * @brief Meta information about all binarizations.
 */
struct BinarizationProperties
{
    std::string name;  /**< @brief Name of binarization */
    int64_t paramMin;  /**< @brief Minimum value for parameter */
    int64_t paramMax;  /**< @brief Maximum value for parameter */
    bool isSigned;  /**< @brief If this supports signed symbols */
    SignedBinarizationBorder min;  /**< @brief Minimum supported symbol with a particular parameter */
    SignedBinarizationBorder max;  /**< @brief Maximum supported symbol with a particular parameter */

    /**
     * @brief Check if a stream is supported by this binarization with some parameter
     * @param minv Minimum symbol of stream
     * @param maxv Maximum symbol of stream
     * @param parameter Binarization parameter
     * @return True if the stream is supported
     * @note For signed binarization you can convert negative values to uint64_t. It will be
     * converted back internally
     */
    bool sbCheck(uint64_t minv, uint64_t maxv, uint64_t parameter) const;
};

/**
 * @brief Get the meta information of one transformation
 * @param id Transformation ID
 * @return Properties object
 */
const TransformationProperties& getTransformation(const gabac::SequenceTransformationId& id);

/**
 * @brief Get the meta information of one binarization
 * @param id Binarization ID
 * @return Properties object
 */
const BinarizationProperties& getBinarization(const gabac::BinarizationId& id);


}  // namespace gabac


#endif  // GABAC_CONSTANTS_H_
