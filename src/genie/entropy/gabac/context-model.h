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

namespace genie::entropy::gabac {

/**
 * @brief
 */
class ContextModel {
 public:
    /**
     * @brief
     * @param initState
     */
    explicit ContextModel(unsigned char initState = 64);

    /**
     * @brief
     */
    ~ContextModel();

    /**
     * @brief
     * @note Inline definition for performance as suggested by NCSA
     */
    [[nodiscard]] unsigned char getState() const;

    /**
     * @brief
     * @note Inline definition for performance as suggested by NCSA
     */
    [[nodiscard]] unsigned char getMps() const;

    /**
     * @brief
     * @note Inline definition for performance as suggested by NCSA
     */
    void updateLps();

    /**
     * @brief
     * @note Inline definition for performance as suggested by NCSA
     */
    void updateMps();

 private:
    unsigned char m_state;  //!<
};

/**
 * @brief
 * @return
 */
inline unsigned char ContextModel::getState() const { return m_state >> 1u; }

/**
 * @brief
 * @return
 */
inline unsigned char ContextModel::getMps() const { return m_state & static_cast<unsigned char>(0x1u); }

/**
 * @brief
 */
inline void ContextModel::updateLps() { m_state = cabactables::nextStateLps[m_state]; }

/**
 * @brief
 */
inline void ContextModel::updateMps() { m_state = cabactables::nextStateMps[m_state]; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_CONTEXT_MODEL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
