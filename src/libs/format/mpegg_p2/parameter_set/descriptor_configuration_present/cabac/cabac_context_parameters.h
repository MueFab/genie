#ifndef GENIE_CABAC_CONTEXT_PARAMETERS_H
#define GENIE_CABAC_CONTEXT_PARAMETERS_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include "util/bitwriter.h"
#include "util/bitreader.h"
#include "util/make_unique.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_p2 {
namespace desc_conf_pres {
namespace cabac {

/**
 * ISO 23092-2 Section 8.3.3.2 table 99
 */
class CabacContextParameters {
   private:
    uint8_t adaptive_mode_flag : 1;                     //!< Line 2
    uint16_t num_contexts : 16;                         //!< Line 3
    std::vector<uint8_t> context_initialization_value;  //!< : 7; Lines 4-6
    std::unique_ptr<bool> share_subsym_ctx_flag;        //!< : 1; Line 8
   public:
    CabacContextParameters(bool adaptive_mode_flag, uint8_t coding_subsym_size, uint8_t output_symbol_size,
                           bool _share_subsym_ctx_flag);

    CabacContextParameters();

    CabacContextParameters(uint8_t coding_subsym_size, uint8_t output_symbol_size, util::BitReader& reader) {
        adaptive_mode_flag = reader.read(1);
        num_contexts = reader.read(16);
        for (size_t i = 0; i < num_contexts; ++i) {
            context_initialization_value.emplace_back(reader.read(7));
        }
        if (coding_subsym_size < output_symbol_size) {
            share_subsym_ctx_flag = util::make_unique<bool>(reader.read(1));
        }
    }
    virtual ~CabacContextParameters() = default;

    void addContextInitializationValue(uint8_t _context_initialization_value);

    virtual void write(util::BitWriter& writer) const;

    std::unique_ptr<CabacContextParameters> clone() const;

    uint8_t getAdaptiveModeFlag() const { return adaptive_mode_flag; }

    uint16_t getNumContexts() const { return num_contexts; }

    bool getShareSubsymCtxFlag() const { return *share_subsym_ctx_flag; }

    const std::vector<uint8_t>* getContextInitializationValue() const { return &context_initialization_value; }
};
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CABAC_CONTEXT_PARAMETERS_H
