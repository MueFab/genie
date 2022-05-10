/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_CONTEXT_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_CONTEXT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "boost/optional/optional.hpp"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"

#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

/**
 * @brief
 */
class Context {
 public:
    /**
     * @brief
     */
    Context();

    /**
     * @brief
     * @param adaptive_mode_flag
     * @param output_symbol_size
     * @param coding_subsym_size
     * @param _share_subsym_ctx_flag
     */
    Context(bool adaptive_mode_flag, uint8_t output_symbol_size, uint8_t coding_subsym_size,
            bool _share_subsym_ctx_flag);

    /**
     * @brief
     * @param output_symbol_size
     * @param coding_subsym_size
     * @param reader
     */
    Context(uint8_t output_symbol_size, uint8_t coding_subsym_size, util::BitReader& reader);

    /**
     * @brief
     */
    virtual ~Context() = default;

    /**
     * @brief
     * @param _context_initialization_value
     */
    void addContextInitializationValue(uint8_t _context_initialization_value);

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     * @brief
     * @return
     */
    bool getAdaptiveModeFlag() const;

    /**
     * @brief
     * @return
     */
    uint16_t getNumContexts() const;

    /**
     * @brief
     * @return
     */
    bool getShareSubsymCtxFlag() const;

    /**
     * @brief
     * @return
     */
    const std::vector<uint8_t>& getContextInitializationValue() const;

    /**
     * @brief
     * @param ctx
     * @return
     */
    bool operator==(const Context& ctx) const;

    /**
     * @brief
     * @param j
     */
    explicit Context(nlohmann::json j);

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const;

 private:
    bool adaptive_mode_flag{true};                      //!< @brief
    uint16_t num_contexts{16};                          //!< @brief
    std::vector<uint8_t> context_initialization_value;  //!< @brief
    boost::optional<bool> share_subsym_ctx_flag;        //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_CONTEXT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
