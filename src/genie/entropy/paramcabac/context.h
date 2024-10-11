/**
 * @file
 * @brief Defines the `Context` class for handling CABAC context parameters in GENIE.
 * @details The `Context` class encapsulates the parameters for CABAC (Context-based Adaptive Binary Arithmetic Coding)
 *          context models used in entropy coding. This includes flags for adaptive mode, context initialization values,
 *          and configuration flags for subsymbol sharing. The class provides methods for serialization and
 *          deserialization of these parameters from and to bitstreams, as well as conversion to and from JSON format.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_CONTEXT_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_CONTEXT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <optional>
#include <vector>
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"
#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

/**
 * @brief Defines CABAC context parameters used for entropy coding.
 * @details The `Context` class represents the configuration of context models in CABAC entropy coding. It manages
 *          parameters like adaptive mode, number of contexts, initialization values, and context sharing flags. The
 *          class also provides methods for reading and writing these parameters from and to bitstreams, as well as
 *          converting them to JSON format.
 */
class Context {
 public:
    /**
     * @brief Default constructor initializing the context with default values.
     */
    Context();

    /**
     * @brief Parameterized constructor for initializing a CABAC context.
     * @param adaptive_mode_flag Flag indicating if the context is in adaptive mode.
     * @param output_symbol_size The size of the output symbols.
     * @param coding_subsym_size The size of the coding subsymbols.
     * @param _share_subsym_ctx_flag Flag indicating if context sharing between subsymbols is enabled.
     */
    Context(bool adaptive_mode_flag, uint8_t output_symbol_size, uint8_t coding_subsym_size,
            bool _share_subsym_ctx_flag);

    /**
     * @brief Constructs a `Context` from a bitstream reader.
     * @param output_symbol_size The size of the output symbols.
     * @param coding_subsym_size The size of the coding subsymbols.
     * @param reader Reference to a `BitReader` object for reading from a bitstream.
     */
    Context(uint8_t output_symbol_size, uint8_t coding_subsym_size, util::BitReader& reader);

    /**
     * @brief Default virtual destructor.
     */
    virtual ~Context() = default;

    /**
     * @brief Adds a context initialization value to the context model.
     * @param _context_initialization_value The initialization value to add.
     */
    void addContextInitializationValue(uint8_t _context_initialization_value);

    /**
     * @brief Serializes the context parameters to a bitstream.
     * @param writer Reference to a `BitWriter` object for writing to a bitstream.
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     * @brief Retrieves the adaptive mode flag.
     * @return True if the context is in adaptive mode, false otherwise.
     */
    [[nodiscard]] bool getAdaptiveModeFlag() const;

    /**
     * @brief Retrieves the number of contexts.
     * @return The number of contexts in the model.
     */
    [[nodiscard]] uint16_t getNumContexts() const;

    /**
     * @brief Retrieves the context sharing flag for subsymbols.
     * @return True if context sharing between subsymbols is enabled, false otherwise.
     */
    [[nodiscard]] bool getShareSubsymCtxFlag() const;

    /**
     * @brief Retrieves the initialization values for the contexts.
     * @return A vector containing the initialization values for the contexts.
     */
    [[nodiscard]] const std::vector<uint8_t>& getContextInitializationValue() const;

    /**
     * @brief Equality comparison operator for `Context` objects.
     * @param ctx Reference to another `Context` object.
     * @return True if both contexts are equal, false otherwise.
     */
    bool operator==(const Context& ctx) const;

    /**
     * @brief Constructs a `Context` from a JSON object.
     * @param j JSON object containing the context parameters.
     */
    explicit Context(nlohmann::json j);

    /**
     * @brief Converts the `Context` object to a JSON representation.
     * @return A JSON object representing the context parameters.
     */
    [[nodiscard]] nlohmann::json toJson() const;

 private:
    bool adaptive_mode_flag{true};                      //!< @brief Flag indicating adaptive mode for the context model.
    uint16_t num_contexts{16};                          //!< @brief Number of contexts in the model.
    std::vector<uint8_t> context_initialization_value;  //!< @brief Initialization values for the contexts.
    std::optional<bool> share_subsym_ctx_flag;  //!< @brief Optional flag for context sharing between subsymbols.
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_CONTEXT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
