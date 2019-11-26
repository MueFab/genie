#include "cabac_context_parameters.h"
#include "../../../make_unique.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace desc_conf_pres {
namespace cabac {
CabacContextParameters::CabacContextParameters(bool _adaptive_mode_flag, uint8_t _coding_subsym_size,
                                               uint8_t _output_symbol_size, bool _share_subsym_ctx_flag)
    : adaptive_mode_flag(_adaptive_mode_flag),
      num_contexts(0),
      context_initialization_value(0),
      share_subsym_ctx_flag(nullptr) {
    if (_coding_subsym_size < _output_symbol_size) {
        share_subsym_ctx_flag = make_unique<bool>(_share_subsym_ctx_flag);
    }
}

// -----------------------------------------------------------------------------------------------------------------

CabacContextParameters::CabacContextParameters() : CabacContextParameters(false, 8, 8, false) {}

// -----------------------------------------------------------------------------------------------------------------

void CabacContextParameters::addContextInitializationValue(uint8_t _context_initialization_value) {
    ++num_contexts;
    this->context_initialization_value.push_back(_context_initialization_value);
}

// -----------------------------------------------------------------------------------------------------------------

void CabacContextParameters::write(util::BitWriter *writer) const {
    writer->write(adaptive_mode_flag, 1);
    writer->write(num_contexts, 16);
    for (auto &i : context_initialization_value) {
        writer->write(i, 7);
    }
    if (share_subsym_ctx_flag) {
        writer->write(*share_subsym_ctx_flag, 1);
    }
}

// -----------------------------------------------------------------------------------------------------------------

std::unique_ptr<CabacContextParameters> CabacContextParameters::clone() const {
    auto ret = make_unique<CabacContextParameters>();
    ret->adaptive_mode_flag = adaptive_mode_flag;
    ret->num_contexts = num_contexts;
    ret->context_initialization_value = context_initialization_value;
    ret->share_subsym_ctx_flag = make_unique<bool>(*share_subsym_ctx_flag);
    return ret;
}
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace format
