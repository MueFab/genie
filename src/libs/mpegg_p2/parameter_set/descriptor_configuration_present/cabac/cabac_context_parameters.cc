#include "cabac_context_parameters.h"
#include "util/bitwriter.h"
#include "util/make_unique.h"

// -----------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_p2 {
namespace desc_conf_pres {
namespace cabac {
CabacContextParameters::CabacContextParameters(bool _adaptive_mode_flag, uint8_t _coding_subsym_size,
                                               uint8_t _output_symbol_size, bool _share_subsym_ctx_flag)
    : adaptive_mode_flag(_adaptive_mode_flag),
      num_contexts(0),
      context_initialization_value(0),
      share_subsym_ctx_flag(nullptr) {
    if (_coding_subsym_size < _output_symbol_size) {
        share_subsym_ctx_flag = util::make_unique<bool>(_share_subsym_ctx_flag);
    }
}

// -----------------------------------------------------------------------------------------------------------------

CabacContextParameters::CabacContextParameters() : CabacContextParameters(false, 8, 8, false) {}

CabacContextParameters::CabacContextParameters(uint8_t coding_subsym_size, uint8_t output_symbol_size,
                                               util::BitReader& reader) {
    adaptive_mode_flag = reader.read(1);
    num_contexts = reader.read(16);
    for (size_t i = 0; i < num_contexts; ++i) {
        context_initialization_value.emplace_back(reader.read(7));
    }
    if (coding_subsym_size < output_symbol_size) {
        share_subsym_ctx_flag = util::make_unique<bool>(reader.read(1));
    }
}

// -----------------------------------------------------------------------------------------------------------------

void CabacContextParameters::addContextInitializationValue(uint8_t _context_initialization_value) {
    ++num_contexts;
    this->context_initialization_value.push_back(_context_initialization_value);
}

// -----------------------------------------------------------------------------------------------------------------

void CabacContextParameters::write(util::BitWriter& writer) const {
    writer.write(adaptive_mode_flag, 1);
    writer.write(num_contexts, 16);
    for (auto& i : context_initialization_value) {
        writer.write(i, 7);
    }
    if (share_subsym_ctx_flag) {
        writer.write(*share_subsym_ctx_flag, 1);
    }
}

// -----------------------------------------------------------------------------------------------------------------

std::unique_ptr<CabacContextParameters> CabacContextParameters::clone() const {
    auto ret = util::make_unique<CabacContextParameters>();
    ret->adaptive_mode_flag = adaptive_mode_flag;
    ret->num_contexts = num_contexts;
    ret->context_initialization_value = context_initialization_value;
    ret->share_subsym_ctx_flag = util::make_unique<bool>(*share_subsym_ctx_flag);
    return ret;
}

uint8_t CabacContextParameters::getAdaptiveModeFlag() const { return adaptive_mode_flag; }

uint16_t CabacContextParameters::getNumContexts() const { return num_contexts; }

bool CabacContextParameters::getShareSubsymCtxFlag() const { return *share_subsym_ctx_flag; }

const std::vector<uint8_t>* CabacContextParameters::getContextInitializationValue() const {
    return &context_initialization_value;
}

}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace genie