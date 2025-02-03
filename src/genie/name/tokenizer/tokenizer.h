/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_NAME_TOKENIZER_TOKENIZER_H_
#define SRC_GENIE_NAME_TOKENIZER_TOKENIZER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <limits>
#include <string>
#include <vector>
#include "genie/core/access-unit.h"
#include "genie/name/tokenizer/token.h"
#include "genie/util/data_block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace name {
namespace tokenizer {

/**
 * @brief
 */
struct TokenState {
 private:
    uint32_t token_pos;                      //!< @brief
    const std::vector<SingleToken>& oldRec;  //!< @brief
    std::vector<SingleToken> curRec;         //!< @brief

    std::string::const_iterator cur_it;  //!< @brief
    std::string::const_iterator end_it;  //!< @brief

 protected:
    /**
     * @brief
     * @return
     */
    bool more() const;

    /**
     * @brief
     */
    void step();

    /**
     * @brief
     * @return
     */
    char get();

    /**
     * @brief
     * @param t
     * @param param
     */
    void pushToken(Tokens t, uint32_t param = 0);

    /**
     * @brief
     * @param t
     */
    void pushToken(const SingleToken& t);

    /**
     * @brief
     * @param param
     */
    void pushTokenString(const std::string& param);

    /**
     * @brief
     * @return
     */
    const SingleToken& getOldToken();

    /**
     * @brief
     */
    void alphabetic();

    /**
     * @brief
     */
    void zeros();

    /**
     * @brief
     */
    void number();

    /**
     * @brief
     */
    void character();

 public:
    /**
     * @brief
     * @param old
     * @param input
     */
    TokenState(const std::vector<SingleToken>& old, const std::string& input);

    /**
     * @brief
     * @return
     */
    std::vector<SingleToken>&& run();

    /**
     * @brief
     * @param tokens
     * @param streams
     */
    static void encode(const std::vector<SingleToken>& tokens, core::AccessUnit::Descriptor& streams);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace tokenizer
}  // namespace name
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_NAME_TOKENIZER_TOKENIZER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
