/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_CONTEXT_MODEL_H_
#define SRC_GENIE_ENTROPY_GABAC_CONTEXT_MODEL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/entropy/gabac/cabac-tables.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 *
 */
class ContextModel {
 public:
    /**
     *
     * @param initState
     */
    explicit ContextModel(unsigned char initState = 64);

    /**
     *
     */
    ~ContextModel();

    /**
     * @note Inline definition for performance as suggested by NCSA
     */
    unsigned char getState() const;

    /**
     * @note Inline definition for performance as suggested by NCSA
     */
    unsigned char getMps() const;

    /**
     * @note Inline definition for performance as suggested by NCSA
     */
    void updateLps();

    /**
     * @note Inline definition for performance as suggested by NCSA
     */
    void updateMps();

 private:
    unsigned char m_state;  //!<
};

/**
 *
 * @return
 */
inline unsigned char ContextModel::getState() const { return m_state >> 1u; }

/**
 *
 * @return
 */
inline unsigned char ContextModel::getMps() const { return m_state & static_cast<unsigned char>(0x1u); }

/**
 *
 */
inline void ContextModel::updateLps() { m_state = cabactables::nextStateLps[m_state]; }

/**
 *
 */
inline void ContextModel::updateMps() { m_state = cabactables::nextStateMps[m_state]; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_CONTEXT_MODEL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
