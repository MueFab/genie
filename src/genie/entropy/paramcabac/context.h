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
 *
 */
class Context {
   public:
    /**
     *
     */
    Context();

    /**
     *
     * @param adaptive_mode_flag
     * @param output_symbol_size
     * @param coding_subsym_size
     * @param _share_subsym_ctx_flag
     */
    Context(bool adaptive_mode_flag, uint8_t output_symbol_size, uint8_t coding_subsym_size,
            bool _share_subsym_ctx_flag);

    /**
     *
     * @param output_symbol_size
     * @param coding_subsym_size
     * @param reader
     */
    Context(uint8_t output_symbol_size, uint8_t coding_subsym_size, util::BitReader& reader);

    /**
     *
     */
    virtual ~Context() = default;

    /**
     *
     * @param _context_initialization_value
     */
    void addContextInitializationValue(uint8_t _context_initialization_value);

    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;

    /**
     *
     * @return
     */
    bool getAdaptiveModeFlag() const;

    /**
     *
     * @return
     */
    uint16_t getNumContexts() const;

    /**
     *
     * @return
     */
    bool getShareSubsymCtxFlag() const;

    /**
     *
     * @return
     */
    const std::vector<uint8_t>& getContextInitializationValue() const;

   private:
    bool adaptive_mode_flag : true;                     //!<
    uint16_t num_contexts : 16;                         //!<
    std::vector<uint8_t> context_initialization_value;  //!<
    boost::optional<bool> share_subsym_ctx_flag;        //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTEXT_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
