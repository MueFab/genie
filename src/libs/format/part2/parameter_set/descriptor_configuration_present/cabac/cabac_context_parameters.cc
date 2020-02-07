#include "cabac_context_parameters.h"
#include "../../../make_unique.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace desc_conf_pres {
namespace cabac {
CabacContextParameters::CabacContextParameters(bool _adaptive_mode_flag, uint16_t _num_contexts,
                                               uint8_t _coding_subsym_size, uint8_t _output_symbol_size,
                                               bool _share_subsym_ctx_flag)
    : adaptive_mode_flag(_adaptive_mode_flag),
      num_contexts(_num_contexts),
      //context_initialization_value(0),
      coding_subsym_size(_coding_subsym_size),
      output_symbol_size(_output_symbol_size),
      share_subsym_ctx_flag(_share_subsym_ctx_flag) {
}

// -----------------------------------------------------------------------------------------------------------------

CabacContextParameters::CabacContextParameters() : CabacContextParameters(false, 0, 8, 8, false) {}

// -----------------------------------------------------------------------------------------------------------------

#if 0
void CabacContextParameters::addContextInitializationValue(uint8_t _context_initialization_value) {
    ++num_contexts;
    this->context_initialization_value.push_back(_context_initialization_value);
}
#endif

// -----------------------------------------------------------------------------------------------------------------

void CabacContextParameters::write(util::BitWriter *writer) const {
    writer->write(adaptive_mode_flag, 1);
    writer->write(num_contexts, 16);
    for (uint16_t i = 0; i < num_contexts; i++) {
        // Out version of gabac always uses initial context values of 0.
        // This is hard-coded in gabac/encode-cabac.cc
        writer->write(0, 7);
    }
    if (coding_subsym_size < output_symbol_size) {
        writer->write(share_subsym_ctx_flag, 1);
    }
}

// -----------------------------------------------------------------------------------------------------------------

std::unique_ptr<CabacContextParameters> CabacContextParameters::clone() const {
    auto ret = make_unique<CabacContextParameters>();
    ret->adaptive_mode_flag = adaptive_mode_flag;
    ret->num_contexts = num_contexts;
    //ret->context_initialization_value = context_initialization_value;
    ret->coding_subsym_size = coding_subsym_size;
    ret->output_symbol_size = output_symbol_size;
    ret->share_subsym_ctx_flag = share_subsym_ctx_flag;
    return ret;
}
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace format
