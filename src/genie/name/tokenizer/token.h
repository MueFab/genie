/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_NAME_TOKENIZER_TOKEN_H_
#define SRC_GENIE_NAME_TOKENIZER_TOKEN_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace name {
namespace tokenizer {

// ---------------------------------------------------------------------------------------------------------------------

constexpr uint8_t TYPE_SEQ = 0;  //!< @brief

/**
 * @brief
 */
enum class Tokens : uint8_t {
    DUP = 0,
    DIFF = 1,
    STRING = 2,
    CHAR = 3,
    DIGITS = 4,
    DELTA = 5,
    DIGITS0 = 6,
    DELTA0 = 7,
    MATCH = 8,
    DZLEN = 9,
    END = 10,
    NONE = 11
};

/**
 * @brief
 */
struct TokenInfo {
    std::string name;  //!< @brief
    int8_t paramSeq;   //!< @brief
};

/**
 * @brief
 */
struct SingleToken {
    Tokens token;             //!< @brief
    uint32_t param;           //!< @brief
    std::string paramString;  //!< @brief

    /**
     * @brief
     * @param t
     * @param p
     * @param ps
     */
    SingleToken(Tokens t, uint32_t p, std::string ps);

    /**
     * @brief
     * @param t
     * @return
     */
    bool operator==(const SingleToken& t) const;

    /**
     * @brief
     * @param t
     * @return
     */
    bool operator!=(const SingleToken& t) const;
};

/**
 * @brief
 * @param t
 * @return
 */
const TokenInfo& getTokenInfo(Tokens t);

/**
 * @brief
 * @param oldString
 * @param newString
 * @return
 */
std::vector<SingleToken> patch(const std::vector<SingleToken>& oldString, const std::vector<SingleToken>& newString);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace tokenizer
}  // namespace name
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_NAME_TOKENIZER_TOKEN_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
