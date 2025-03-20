/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/luts_sub_symbol_transform.h"

#include <algorithm>
#include <functional>
#include <vector>

#include "genie/util/block_stepper.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------

bool LutEntry::operator>(const LutEntry& entry) const {
  return freq > entry.freq;
}

// -----------------------------------------------------------------------------

LutOrder1 LuTsSubSymbolTransform::GetInitLutsOrder1(
    const uint64_t num_alpha_subsym) {
  return std::vector<LutRow>(
      num_alpha_subsym,
      {std::vector<LutEntry>(num_alpha_subsym, {0, 0}),  // value, freq
       0});
}

// -----------------------------------------------------------------------------

LuTsSubSymbolTransform::LuTsSubSymbolTransform(
    const paramcabac::SupportValues& support_vals,
    const paramcabac::StateVars& state_vars, const uint8_t num_luts,
    const uint8_t num_prvs, const bool mode_flag)
    : support_vals_(support_vals),
      state_vars_(state_vars),
      num_luts_(num_luts),
      num_prvs_(num_prvs),
      encoding_mode_flag_(mode_flag) {}

// -----------------------------------------------------------------------------

LuTsSubSymbolTransform::LuTsSubSymbolTransform(
    const LuTsSubSymbolTransform& src) = default;

// -----------------------------------------------------------------------------

void LuTsSubSymbolTransform::SetupLutsOrder1(const uint8_t num_subsyms,
                                             const uint64_t num_alpha_subsym) {
  if (num_alpha_subsym > paramcabac::kMaxLutSize) return;

  luts_o_1_.clear();
  luts_o_1_ = std::vector(num_subsyms, GetInitLutsOrder1(num_alpha_subsym));
}

// -----------------------------------------------------------------------------

void LuTsSubSymbolTransform::SetupLutsOrder2(const uint8_t num_subsyms,
                                             const uint64_t num_alpha_subsym) {
  if (num_alpha_subsym > paramcabac::kMaxLutSize) return;

  luts_o_2_.clear();
  luts_o_2_ = std::vector(
      num_subsyms,
      std::vector(num_alpha_subsym, GetInitLutsOrder1(num_alpha_subsym)));
}

// -----------------------------------------------------------------------------

void LuTsSubSymbolTransform::BuildLuts(const util::DataBlock* symbols,
                                       const util::DataBlock* dep_symbols) {
  if (num_luts_ == 0 || !encoding_mode_flag_) return;  // NOLINT

  assert(symbols != nullptr);

  if (const size_t num_symbols = symbols->Size(); num_symbols <= 0) {
    return;
  }

  const uint8_t output_symbol_size = support_vals_.GetOutputSymbolSize();
  const uint8_t coding_subsym_size = support_vals_.GetCodingSubsymSize();
  const uint8_t coding_order = support_vals_.GetCodingOrder();
  const auto num_subsymbols =
      static_cast<uint8_t>(state_vars_.GetNumSubsymbols());
  const uint64_t num_alpha_subsym = state_vars_.GetNumAlphaSubsymbol();
  const uint64_t subsym_mask =
      paramcabac::StateVars::Get2PowN(coding_subsym_size) - 1;

  util::BlockStepper r = symbols->GetReader();
  std::vector<Subsymbol> subsymbols(state_vars_.GetNumSubsymbols());

  util::BlockStepper d;
  if (dep_symbols) {
    d = dep_symbols->GetReader();
  }

  if (coding_order == 2)
    SetupLutsOrder2(num_subsymbols, num_alpha_subsym);
  else
    SetupLutsOrder1(num_subsymbols, num_alpha_subsym);

  while (r.IsValid()) {
    // Split symbol into subsymbols and then build subsymbols
    const uint64_t symbol_value = r.Get();
    uint64_t subsym_value = 0;
    uint32_t oss = output_symbol_size;

    uint64_t dep_symbol_value = 0, dep_subsym_value = 0;
    if (d.IsValid()) {
      dep_symbol_value = d.Get();
      d.Inc();
    }

    const uint64_t sym_value = abs(static_cast<int>(symbol_value));
    for (uint8_t s = 0; s < num_subsymbols; s++) {
      const uint8_t lut_idx =
          num_luts_ > 1 ? s : 0;  // either private or shared LUT
      const uint8_t prv_idx =
          num_prvs_ > 1 ? s : 0;  // either private or shared PRV

      if (dep_symbols) {
        dep_subsym_value =
            (dep_symbol_value >> (oss - coding_subsym_size)) &  // NOLINT
            subsym_mask;                                        // NOLINT
        subsymbols[prv_idx].prv_values[0] = dep_subsym_value;
      }

      subsym_value = sym_value >> (oss -= coding_subsym_size) & subsym_mask;

      if (coding_order == 2) {
        LutOrder2& lut = luts_o_2_[lut_idx];
        lut[subsymbols[prv_idx].prv_values[1]]
           [subsymbols[prv_idx].prv_values[0]]
               .entries[subsym_value]
               .freq++;
        lut[subsymbols[prv_idx].prv_values[1]]
           [subsymbols[prv_idx].prv_values[0]]
               .entries[subsym_value]
               .value = subsym_value;

        subsymbols[prv_idx].prv_values[1] = subsymbols[prv_idx].prv_values[0];
        subsymbols[prv_idx].prv_values[0] = subsym_value;

      } else if (coding_order == 1) {
        LutOrder1& lut = luts_o_1_[lut_idx];
        lut[subsymbols[prv_idx].prv_values[0]].entries[subsym_value].freq++;
        lut[subsymbols[prv_idx].prv_values[0]].entries[subsym_value].value =
            subsym_value;

        subsymbols[prv_idx].prv_values[0] = subsym_value;
      }
    }

    r.Inc();
  }
}

// -----------------------------------------------------------------------------

void LuTsSubSymbolTransform::DecodeLutOrder1(Reader& reader,
                                             const uint64_t num_alpha_subsym,
                                             const uint8_t coding_subsym_size,
                                             LutOrder1& lut) {
  for (uint32_t i = 0; i < num_alpha_subsym; i++) {
    lut[i].num_max_elems = reader.ReadLutSymbol(coding_subsym_size);
    for (uint32_t j = 0; j <= lut[i].num_max_elems; j++) {
      lut[i].entries[j].value = reader.ReadLutSymbol(coding_subsym_size);
    }
  }
}

// -----------------------------------------------------------------------------

void LuTsSubSymbolTransform::DecodeLuTs(Reader& reader) {
  if (num_luts_ == 0 || encoding_mode_flag_) return;

  const uint8_t coding_subsym_size = support_vals_.GetCodingSubsymSize();
  const uint8_t coding_order = support_vals_.GetCodingOrder();
  const uint64_t num_alpha_subsym = state_vars_.GetNumAlphaSubsymbol();

  if (coding_order == 2) {
    SetupLutsOrder2(static_cast<uint8_t>(state_vars_.GetNumSubsymbols()),
                    num_alpha_subsym);
    for (uint32_t s = 0; s < num_luts_; s++) {
      for (uint32_t k = 0; k < num_alpha_subsym; k++) {
        DecodeLutOrder1(reader, num_alpha_subsym, coding_subsym_size,
                        luts_o_2_[s][k]);
      }
    }
  } else if (coding_order == 1) {
    SetupLutsOrder1(static_cast<uint8_t>(state_vars_.GetNumSubsymbols()),
                    num_alpha_subsym);
    for (uint32_t s = 0; s < num_luts_; s++) {
      DecodeLutOrder1(reader, num_alpha_subsym, coding_subsym_size,
                      luts_o_1_[s]);
    }
  }
}

// -----------------------------------------------------------------------------

void LuTsSubSymbolTransform::SortLutRow(LutRow& lut_row) {
  // sort entries in descending order and populate numMaxElems;
  sort(lut_row.entries.begin(), lut_row.entries.end(), std::greater<>());
  lut_row.num_max_elems =
      std::count_if(lut_row.entries.begin(), lut_row.entries.end(),
                    [](const LutEntry e) { return e.freq != 0; });
  if (lut_row.num_max_elems > 0) lut_row.num_max_elems--;
}

// -----------------------------------------------------------------------------

void LuTsSubSymbolTransform::EncodeLutOrder1(Writer& writer,
                                             const uint64_t num_alpha_subsym,
                                             const uint8_t coding_subsym_size,
                                             LutOrder1& lut) {
  for (uint32_t i = 0; i < num_alpha_subsym; i++) {
    SortLutRow(lut[i]);
    writer.WriteLutSymbol(lut[i].num_max_elems, coding_subsym_size);
    for (uint32_t j = 0; j <= lut[i].num_max_elems; j++) {
      writer.WriteLutSymbol(lut[i].entries[j].value, coding_subsym_size);
    }
  }
}

// -----------------------------------------------------------------------------

void LuTsSubSymbolTransform::EncodeLuTs(Writer& writer,
                                        const util::DataBlock* symbols,
                                        const util::DataBlock* dep_symbols) {
  if (num_luts_ == 0 || !encoding_mode_flag_) return;

  const uint8_t coding_subsym_size = support_vals_.GetCodingSubsymSize();
  const uint8_t coding_order = support_vals_.GetCodingOrder();
  const uint64_t num_alpha_subsym = state_vars_.GetNumAlphaSubsymbol();

  // build LUTs from symbols
  BuildLuts(symbols, dep_symbols);

  // encode LUTs
  if (coding_order == 2) {
    for (uint32_t s = 0; s < num_luts_; s++) {
      for (uint32_t k = 0; k < num_alpha_subsym; k++) {
        EncodeLutOrder1(writer, num_alpha_subsym, coding_subsym_size,
                        luts_o_2_[s][k]);
      }
    }
  } else if (coding_order == 1) {
    for (uint32_t s = 0; s < num_luts_; s++) {
      EncodeLutOrder1(writer, num_alpha_subsym, coding_subsym_size,
                      luts_o_1_[s]);
    }
  }
}

// -----------------------------------------------------------------------------

void LuTsSubSymbolTransform::TransformOrder2(std::vector<Subsymbol>& subsymbols,
                                             const uint8_t subsym_idx,
                                             const uint8_t lut_idx,
                                             const uint8_t prv_idx) const {
  Subsymbol& subsymbol = subsymbols[subsym_idx];
  const uint64_t subsym_val = subsymbol.subsym_value;
  const auto [kEntries, kNumMaxElems] =
      luts_o_2_[lut_idx][subsymbols[prv_idx].prv_values[1]]
               [subsymbols[prv_idx].prv_values[0]];
  const auto entry = std::find_if(kEntries.begin(), kEntries.end(),
                                  [subsym_val](const LutEntry e) {
                                    return e.value == subsym_val && e.freq > 0;
                                  });
  if (entry != kEntries.end()) {
    subsymbol.lut_num_max_elems = kNumMaxElems;
    subsymbol.lut_entry_idx = distance(kEntries.begin(), entry);
  }
}

// -----------------------------------------------------------------------------

void LuTsSubSymbolTransform::TransformOrder1(std::vector<Subsymbol>& subsymbols,
                                             const uint8_t subsym_idx,
                                             const uint8_t lut_idx,
                                             const uint8_t prv_idx) const {
  Subsymbol& subsymbol = subsymbols[subsym_idx];
  const uint64_t subsym_val = subsymbol.subsym_value;
  const auto [kEntries, kNumMaxElems] =
      luts_o_1_[lut_idx][subsymbols[prv_idx].prv_values[0]];

  const auto entry = std::find_if(kEntries.begin(), kEntries.end(),
                                  [subsym_val](const LutEntry e) {
                                    return e.value == subsym_val && e.freq > 0;
                                  });
  if (entry != kEntries.end()) {
    subsymbol.lut_num_max_elems = kNumMaxElems;
    subsymbol.lut_entry_idx = distance(kEntries.begin(), entry);
  }
}

// -----------------------------------------------------------------------------

uint64_t LuTsSubSymbolTransform::GetNumMaxElemsOrder2(
    const std::vector<Subsymbol>& subsymbols, const uint8_t lut_idx,
    const uint8_t prv_idx) const {
  return luts_o_2_[lut_idx][subsymbols[prv_idx].prv_values[1]]
                  [subsymbols[prv_idx].prv_values[0]]
                      .num_max_elems;
}

// -----------------------------------------------------------------------------

uint64_t LuTsSubSymbolTransform::GetNumMaxElemsOrder1(
    const std::vector<Subsymbol>& subsymbols, const uint8_t lut_idx,
    const uint8_t prv_idx) const {
  return luts_o_1_[lut_idx][subsymbols[prv_idx].prv_values[0]].num_max_elems;
}

// -----------------------------------------------------------------------------

void LuTsSubSymbolTransform::InvTransformOrder2(
    std::vector<Subsymbol>& subsymbols, const uint8_t subsym_idx,
    const uint8_t lut_idx, const uint8_t prv_idx) const {
  subsymbols[subsym_idx].subsym_value =
      luts_o_2_[lut_idx][subsymbols[prv_idx].prv_values[1]]
               [subsymbols[prv_idx].prv_values[0]]
                   .entries[subsymbols[subsym_idx].lut_entry_idx]
                   .value;
}

// -----------------------------------------------------------------------------

void LuTsSubSymbolTransform::InvTransformOrder1(
    std::vector<Subsymbol>& subsymbols, const uint8_t subsym_idx,
    const uint8_t lut_idx, const uint8_t prv_idx) const {
  subsymbols[subsym_idx].subsym_value =
      luts_o_1_[lut_idx][subsymbols[prv_idx].prv_values[0]]
          .entries[subsymbols[subsym_idx].lut_entry_idx]
          .value;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------