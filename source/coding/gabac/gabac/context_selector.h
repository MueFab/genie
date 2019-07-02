/**
 * @file
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_CONTEXT_SELECTOR_H_
#define GABAC_CONTEXT_SELECTOR_H_

#include "gabac/context_tables.h"

#include <cassert>


namespace gabac {


class ContextSelector
{
 public:
    ContextSelector() = default;

    ~ContextSelector() = default;

    /**
     * @note Inline definition for performance as suggested by NCSA
     */
    static unsigned int getContextForBi(
            unsigned int contextSetIdx,
            unsigned int binIdx
    );

    /**
     * @note Inline definition for performance as suggested by NCSA
     */
    static unsigned int getContextForTu(
            unsigned int contextSetIdx,
            unsigned int binIdx
    );

    /**
     * @note Inline definition for performance as suggested by NCSA
     */
    static unsigned int getContextForEg(
            unsigned int contextSetIdx,
            unsigned int binIdx
    );
};

inline unsigned int ContextSelector::getContextForBi(
        unsigned int contextSetIdx,
        unsigned int binIdx
){
    assert(binIdx < contexttables::CONTEXT_SET_LENGTH);
    // TODO(Tom): add explanation for this assertion
    assert(contextSetIdx < 16);

    return (contexttables::OFFSET_BINARY_0
            + (contextSetIdx * contexttables::CONTEXT_SET_LENGTH)
            + binIdx
    );
}

inline unsigned int ContextSelector::getContextForTu(
        unsigned int contextSetIdx,
        unsigned int binIdx
){
    assert(binIdx < contexttables::CONTEXT_SET_LENGTH);
    // TODO(Tom): add explanation for this assertion
    assert(contextSetIdx < 16);

    return (contexttables::OFFSET_TRUNCATED_UNARY_0
            + (contextSetIdx * contexttables::CONTEXT_SET_LENGTH)
            + binIdx
    );
}

inline unsigned int ContextSelector::getContextForEg(
        unsigned int contextSetIdx,
        unsigned int binIdx
){
    assert(binIdx < contexttables::CONTEXT_SET_LENGTH);
    // TODO(Tom): add explanation for this assertion
    assert(contextSetIdx < 16);

    return (contexttables::OFFSET_EXPONENTIAL_GOLOMB_0
            + (contextSetIdx * contexttables::CONTEXT_SET_LENGTH)
            + binIdx
    );
}


}  // namespace gabac


#endif  // GABAC_CONTEXT_SELECTOR_H_
