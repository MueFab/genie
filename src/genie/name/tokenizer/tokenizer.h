/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_TOKENIZER_H
#define GENIE_TOKENIZER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <access-unit.h>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

#include "token.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace name {
namespace tokenizer {

/**
 *
 */
struct TokenState {
   private:
    uint32_t token_pos;                      //!<
    const std::vector<SingleToken>& oldRec;  //!<
    std::vector<SingleToken> curRec;         //!<

    std::string::const_iterator cur_it;  //!<
    std::string::const_iterator end_it;  //!<

   protected:
    /**
     *
     * @return
     */
    bool more() const;

    /**
     *
     */
    void step();

    /**
     *
     * @return
     */
    char get();

    /**
     *
     * @param t
     * @param param
     */
    void pushToken(Tokens t, uint32_t param = 0);

    /**
     *
     * @param t
     */
    void pushToken(const SingleToken& t);

    /**
     *
     * @param param
     */
    void pushTokenString(const std::string& param);

    /**
     *
     * @return
     */
    const SingleToken& getOldToken();

    /**
     *
     */
    void alphabetic();

    /**
     *
     */
    void zeros();

    /**
     *
     */
    void number();

    /**
     *
     */
    void character();

   public:
    /**
     *
     * @param old
     * @param input
     */
    TokenState(const std::vector<SingleToken>& old, const std::string& input);

    /**
     *
     * @return
     */
    std::vector<SingleToken>&& run();

    /**
     *
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

#endif  // GENIE_TOKENIZER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------