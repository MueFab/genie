/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the `ContextModel` class for binary arithmetic coding.
 *
 * This file contains the declaration of the `ContextModel` class, which
 * represents a single context model used in CABAC (Context-based Adaptive
 * Binary Arithmetic Coding). The class manages the probability state and the
 * most probable symbol (MPS) for binary arithmetic encoding and decoding. It
 * provides methods to update the state based on whether the Least Probable
 * Symbol (LPS) or MPS was encountered.
 *
 * @details The `ContextModel` class plays a crucial role in managing the state
 * transitions of binary symbols during entropy coding. It uses predefined state
 * transition tables to efficiently encode and Decode binary symbols. The
 * context models are typically used in conjunction with a CABAC encoder or
 * decoder to represent probability distributions of symbols in various
 * contexts.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_CONTEXT_MODEL_H_
#define SRC_GENIE_ENTROPY_GABAC_CONTEXT_MODEL_H_

// -----------------------------------------------------------------------------

#include "genie/entropy/gabac/cabac_tables.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Represents a single context model for binary arithmetic coding.
 *
 * The `ContextModel` class encapsulates the state and the most probable symbol
 * (MPS) for a given context in the CABAC framework. It provides methods to
 * retrieve the current state and MPS, as well as update the state based on the
 * outcome of binary decisions (MPS or LPS). Each context model can transition
 * between multiple states to adjust the probability estimates as more symbols
 * are encoded or decoded.
 */
class ContextModel {
 public:
  /**
   * @brief Constructs a `ContextModel` object with an optional initial state.
   *
   * The initial state is used to define the starting probability estimate for
   * the context. The default value is 64, which represents an equal
   * probability for both MPS and LPS.
   *
   * @param init_state The initial state for the context model. Valid values
   * range from 0 to 127, where the higher the value, the stronger the bias
   *                  towards the MPS.
   */
  explicit ContextModel(unsigned char init_state = 64);

  /**
   * @brief Retrieves the current state of the context model.
   *
   * The state is represented as an unsigned char value where the upper 7 bits
   * indicate the state index, and the least significant bit indicates the
   * most probable symbol (MPS).
   *
   * @return The current state of the context model.
   */
  [[nodiscard]] unsigned char GetState() const;

  /**
   * @brief Retrieves the most probable symbol (MPS).
   *
   * The MPS is represented by the least significant bit of the state
   * variable. This value determines whether the most probable symbol is 0
   * or 1.
   *
   * @return The current most probable symbol (MPS), either 0 or 1.
   */
  [[nodiscard]] unsigned char GetMps() const;

  /**
   * @brief Updates the context state when the least probable symbol (LPS) is
   * encountered.
   *
   * When the LPS is encountered during binary arithmetic coding, the context
   * state transitions to a new state defined by the `nextStateLps` table.
   * This update reflects a change in the probability estimate for the
   * symbols.
   */
  void UpdateLps();

  /**
   * @brief Updates the context state when the most probable symbol (MPS) is
   * encountered.
   *
   * When the MPS is encountered, the context state transitions to a new state
   * defined by the `nextStateMps` table. This update reflects a reinforcement
   * of the probability estimate that the MPS is more likely to occur than the
   * LPS.
   */
  void UpdateMps();

 private:
  unsigned char state_;  //!< Represents the current state of the context
                         //!< model. The upper 7 bits are the state index, and
                         //!< the least significant bit is the MPS (0 or 1).
};

/**
 * @brief Retrieves the current state of the context model.
 * @return The current state index (upper 7 bits of the state variable).
 */
inline unsigned char ContextModel::GetState() const { return state_ >> 1u; }

/**
 * @brief Retrieves the most probable symbol (MPS).
 * @return The MPS, which is stored as the least significant bit of the state
 * variable.
 */
inline unsigned char ContextModel::GetMps() const {
  return state_ & static_cast<unsigned char>(0x1u);
}

/**
 * @brief Updates the context model when the least probable symbol (LPS) is
 * encountered.
 *
 * This function transitions the context state using the `nextStateLps` table,
 * which defines the new state based on the current state.
 */
inline void ContextModel::UpdateLps() {
  state_ = cabactables::kNextStateLps[state_];
}

/**
 * @brief Updates the context model when the most probable symbol (MPS) is
 * encountered.
 *
 * This function transitions the context state using the `nextStateMps` table,
 * which defines the new state based on the current state.
 */
inline void ContextModel::UpdateMps() {
  state_ = cabactables::kNextStateMps[state_];
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_CONTEXT_MODEL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
