/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CONTEXT_H
#define GENIE_CONTEXT_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>
#include <boost/optional/optional.hpp>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

/**
 * ISO 23092-2 Section 8.3.3.2 table 99
 */
class Context {
   private:
    uint8_t adaptive_mode_flag : 1;                     //!< Line 2
    uint16_t num_contexts : 16;                         //!< Line 3
    std::vector<uint8_t> context_initialization_value;  //!< : 7; Lines 4-6
    boost::optional<bool> share_subsym_ctx_flag;        //!< : 1; Line 8
   public:
    Context(bool adaptive_mode_flag, uint8_t coding_subsym_size, uint8_t output_symbol_size,
            bool _share_subsym_ctx_flag);

    Context();

    Context(uint8_t coding_subsym_size, uint8_t output_symbol_size, util::BitReader& reader);
    virtual ~Context() = default;

    void addContextInitializationValue(uint8_t _context_initialization_value);

    virtual void write(util::BitWriter& writer) const;

    uint8_t getAdaptiveModeFlag() const;

    uint16_t getNumContexts() const;

    bool getShareSubsymCtxFlag() const;

    const std::vector<uint8_t>& getContextInitializationValue() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTEXT_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------