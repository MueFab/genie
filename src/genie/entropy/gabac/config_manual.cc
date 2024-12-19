/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/config_manual.h"

#include <utility>
#include <vector>

#include "genie/entropy/paramcabac/subsequence.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

#define MAX_NUM_TRANSPORT_SEQS 4  // put max 4 for now.

// -----------------------------------------------------------------------------
uint8_t GetCmaxTu(const uint64_t num_alpha_special,
                  const uint8_t coding_subsym_size) {
  if (num_alpha_special > 0) { /* special case */
    return static_cast<uint8_t>(num_alpha_special - 1);
  }
  return static_cast<uint8_t>(
      paramcabac::StateVars::Get2PowN(coding_subsym_size) - 1);
}

// -----------------------------------------------------------------------------
paramcabac::Subsequence GetEncoderConfigManual(const core::GenSubIndex& sub) {
  const auto genie_desc_id = static_cast<core::GenDesc>(sub.first);
  const core::GenSubIndex genie_subseq_id = sub;
  const auto subseq_id = static_cast<uint8_t>(sub.second);

  // default values
  constexpr auto alpha_id = core::AlphabetId::kAcgtn;
  const uint64_t num_alpha_special =
      paramcabac::StateVars::GetNumAlphaSpecial(genie_subseq_id, alpha_id);
  // const uint8_t symSize = (alphaID == core::AlphabetID::ACGTN) ? 3 : 5;

  auto trnsf_subseq_id =
      paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM;
  uint16_t trnsf_subseq_param = 255;
  bool token_type_flag = false;

  uint8_t coding_sub_sym_size[MAX_NUM_TRANSPORT_SEQS] = {8};
  uint8_t output_symbol_size[MAX_NUM_TRANSPORT_SEQS] = {8};
  uint8_t coding_order[MAX_NUM_TRANSPORT_SEQS] = {};
  paramcabac::SupportValues::TransformIdSubsym
      trnsf_subsym_id[MAX_NUM_TRANSPORT_SEQS] = {
          paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM};
  paramcabac::BinarizationParameters::BinarizationId
      bin_id[MAX_NUM_TRANSPORT_SEQS] = {
          paramcabac::BinarizationParameters::BinarizationId::BI};
  std::vector bin_params(MAX_NUM_TRANSPORT_SEQS, std::vector<uint8_t>(2, 0));
  bool bypass_flag[MAX_NUM_TRANSPORT_SEQS] = {false};
  bool adaptive_mode_flag[MAX_NUM_TRANSPORT_SEQS] = {true};
  bool share_subsym_prv_flag[MAX_NUM_TRANSPORT_SEQS] = {true};
  bool share_subsym_lut_flag[MAX_NUM_TRANSPORT_SEQS] = {true};
  bool share_subsym_ctx_flag[MAX_NUM_TRANSPORT_SEQS] = {false};

  bool found = true;

  // populate config below
  switch (genie_desc_id) {
    case core::GenDesc::kPosition:
      switch (subseq_id) {
        case 0:  // POS for first alignment
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::EQUALITY_CODING;
          coding_order[0] = 2;
          output_symbol_size[0] = 1;
          coding_sub_sym_size[0] = 1;
          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::BI;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

          coding_order[1] = 0;
          output_symbol_size[1] = 32;
          coding_sub_sym_size[1] = 32;
          bin_id[1] = paramcabac::BinarizationParameters::BinarizationId::EG;
          trnsf_subsym_id[1] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;
          break;
        case 1:  // POS for other alignments
          break;
        default:
          found = false;
      }
      break;
    case core::GenDesc::kReverseComplement:
      if (subseq_id == 0) {
        trnsf_subseq_id = paramcabac::TransformedParameters::TransformIdSubseq::
            EQUALITY_CODING;
        coding_order[0] = 2;
        output_symbol_size[0] = 1;
        coding_sub_sym_size[0] = 1;
        bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::BI;
        trnsf_subsym_id[0] =
            paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

        coding_order[1] = 1;
        output_symbol_size[1] = 1;
        coding_sub_sym_size[1] = 1;
        trnsf_subsym_id[1] =
            paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
        bin_id[1] = paramcabac::BinarizationParameters::BinarizationId::TU;
        bin_params[1][0] = 1;
      } else {
        found = false;
      }
      break;
    case core::GenDesc::kFlags:
      switch (subseq_id) {
        case 0:
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::NO_TRANSFORM;
          output_symbol_size[0] = 1;
          coding_sub_sym_size[0] = 1;
          coding_order[0] = 0;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::BI;
          break;
        case 1:
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::NO_TRANSFORM;
          output_symbol_size[0] = 1;
          coding_sub_sym_size[0] = 1;
          coding_order[0] = 1;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;

          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
          bin_params[0][0] = 1;
          break;
        case 2:
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::NO_TRANSFORM;
          output_symbol_size[0] = 1;
          coding_sub_sym_size[0] = 1;
          coding_order[0] = 0;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::BI;
          break;
        default:
          found = false;
      }
      break;
    case core::GenDesc::kMismatchPosition:
      switch (subseq_id) {
        case 0:  // mmposFlag
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::NO_TRANSFORM;
          output_symbol_size[0] = 1;
          coding_sub_sym_size[0] = 1;
          coding_order[0] = 2;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::BI;
          break;
        case 1:  // mmposPos
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::MATCH_CODING;
          output_symbol_size[0] = 8;
          coding_sub_sym_size[0] = 8;
          coding_order[0] = 0;
          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::EG;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

          output_symbol_size[1] = 8;
          coding_sub_sym_size[1] = 8;
          coding_order[1] = 1;
          bin_id[1] = paramcabac::BinarizationParameters::BinarizationId::TU;
          bin_params[1][0] = 255;  // cmax;
          trnsf_subsym_id[1] =
              paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;

          output_symbol_size[2] = 16;
          coding_sub_sym_size[2] = 16;
          coding_order[2] = 0;
          bin_id[2] = paramcabac::BinarizationParameters::BinarizationId::EG;
          break;
        default:
          found = false;
      }
      break;
    case core::GenDesc::kMismatchType:
      switch (subseq_id) {
        case 0:  // mmtype
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::NO_TRANSFORM;
          output_symbol_size[0] = 2;
          coding_sub_sym_size[0] = 2;
          coding_order[0] = 1;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
          bin_params[0][0] = 2;  // cmax;

          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
          break;
        case 1:  // mmtypeSbs
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::NO_TRANSFORM;
          output_symbol_size[0] = 3;
          coding_sub_sym_size[0] = 3;
          coding_order[0] = 1;

          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
          bin_params[0][0] = 4;  // cmax;

          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
          break;
        case 2:  // mmtypeIndels
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::NO_TRANSFORM;
          output_symbol_size[0] = 3;
          coding_sub_sym_size[0] = 3;
          coding_order[0] = 2;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
          bin_params[0][0] = 4;  // cmax;

          break;
        default:
          found = false;
      }
      break;
    case core::GenDesc::kClips:
      switch (subseq_id) {
        case 0:  // clipsRecordID
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::NO_TRANSFORM;
          output_symbol_size[0] = 32;
          coding_sub_sym_size[0] = 32;
          coding_order[0] = 0;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::BI;
          break;
        case 1:  // clipsType
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::NO_TRANSFORM;
          output_symbol_size[0] = 4;
          coding_sub_sym_size[0] = 4;
          coding_order[0] = 2;

          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
          bin_params[0][0] = GetCmaxTu(num_alpha_special,
                                       coding_sub_sym_size[0]);  // cmax;

          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
          break;
        case 2:  // clipsSoftClips
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::NO_TRANSFORM;
          output_symbol_size[0] = 3;
          coding_sub_sym_size[0] = 3;
          coding_order[0] = 2;

          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
          bin_params[0][0] = GetCmaxTu(num_alpha_special,
                                       coding_sub_sym_size[0]);  // cmax;

          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
          break;
        case 3:  // clipsHardClipsLength
          break;
        default:
          found = false;
      }
      break;
    case core::GenDesc::kUnalignedReads:
      if (subseq_id == 0) {
        trnsf_subseq_id =
            paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM;
        output_symbol_size[0] = 3;
        coding_sub_sym_size[0] = 3;
        coding_order[0] = 2;
        bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
        bin_params[0][0] = 4;  // splitUnitSize;
        trnsf_subsym_id[0] =
            paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
      } else {
        found = false;
      }
      break;
    case core::GenDesc::kReadLength:
      if (subseq_id == 0) {
        trnsf_subseq_id = paramcabac::TransformedParameters::TransformIdSubseq::
            EQUALITY_CODING;
        coding_order[0] = 1;
        output_symbol_size[0] = 1;
        coding_sub_sym_size[0] = 1;
        trnsf_subsym_id[0] =
            paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
        bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
        bin_params[0][0] = 1;  // splitUnitSize;

        coding_order[1] = 0;
        output_symbol_size[1] = 32;
        coding_sub_sym_size[1] = 32;
        bin_id[1] = paramcabac::BinarizationParameters::BinarizationId::EG;
        trnsf_subsym_id[1] =
            paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;
      } else {
        found = false;
      }
      break;
    case core::GenDesc::kPair:
      switch (subseq_id) {
        case 0:  // pairType
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::EQUALITY_CODING;
          coding_order[0] = 2;
          output_symbol_size[0] = 1;
          coding_sub_sym_size[0] = 1;
          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::BI;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

          coding_order[1] = 2;
          output_symbol_size[1] = 3;
          coding_sub_sym_size[1] = 3;
          bin_id[1] = paramcabac::BinarizationParameters::BinarizationId::TU;
          bin_params[1][0] = 6;  // splitUnitSize;
          trnsf_subsym_id[1] =
              paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
          break;
        case 1:  // pairReadDistance
          trnsf_subseq_id =
              paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING;
          output_symbol_size[0] = 8;
          coding_sub_sym_size[0] = 8;
          coding_order[0] = 0;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
          bin_params[0][0] = 255;  // cmax;

          output_symbol_size[1] = 16;
          coding_sub_sym_size[1] = 16;
          coding_order[1] = 0;
          trnsf_subsym_id[1] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

          bin_id[1] = paramcabac::BinarizationParameters::BinarizationId::EG;

          break;
        case 2:
          trnsf_subseq_id =
              paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING;
          output_symbol_size[0] = 8;
          coding_sub_sym_size[0] = 2;
          coding_order[0] = 1;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;

          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
          bin_params[0][0] = 255;  // cmax;

          output_symbol_size[1] = 32;
          coding_sub_sym_size[1] = 32;
          coding_order[1] = 0;
          trnsf_subsym_id[1] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

          bin_id[1] = paramcabac::BinarizationParameters::BinarizationId::BI;

          break;
        case 3:
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::EQUALITY_CODING;
          coding_order[0] = 1;
          output_symbol_size[0] = 1;
          coding_sub_sym_size[0] = 1;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
          bin_params[0][0] = 1;

          coding_order[1] = 0;
          output_symbol_size[1] = 32;
          coding_sub_sym_size[1] = 32;
          bin_id[1] = paramcabac::BinarizationParameters::BinarizationId::BI;
          trnsf_subsym_id[1] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;
          break;
        case 4:
        case 5:  // pairSeqID
          trnsf_subseq_id =
              paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING;
          output_symbol_size[0] = 8;
          coding_sub_sym_size[0] = 8;
          coding_order[0] = 0;
          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::EG;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

          output_symbol_size[1] = 16;
          coding_sub_sym_size[1] = 16;
          coding_order[1] = 0;
          bin_id[1] = paramcabac::BinarizationParameters::BinarizationId::EG;
          trnsf_subsym_id[1] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;
          break;
        case 6:
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::EQUALITY_CODING;
          coding_order[0] = 0;
          output_symbol_size[0] = 1;
          coding_sub_sym_size[0] = 1;
          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::BI;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

          coding_order[1] = 0;
          output_symbol_size[1] = 32;
          coding_sub_sym_size[1] = 32;
          bin_id[1] = paramcabac::BinarizationParameters::BinarizationId::BI;
          trnsf_subsym_id[1] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;
          break;
        case 7:  // pairAbsPos
          trnsf_subseq_id =
              paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING;
          output_symbol_size[0] = 8;
          coding_sub_sym_size[0] = 8;
          coding_order[0] = 0;
          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
          bin_params[0][0] = 255;
          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;

          output_symbol_size[1] = 32;
          coding_sub_sym_size[1] = 32;
          coding_order[1] = 0;
          bin_id[1] = paramcabac::BinarizationParameters::BinarizationId::BI;
          trnsf_subsym_id[1] =
              paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;
          break;
        default:
          found = false;
      }
      break;
    case core::GenDesc::kMappingScore:
      if (subseq_id == 0) {
        trnsf_subseq_id =
            paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING;
        output_symbol_size[0] = 8;
        coding_sub_sym_size[0] = 8;
        coding_order[0] = 1;
        bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::EG;
        trnsf_subsym_id[0] =
            paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;

        output_symbol_size[1] = 8;
        coding_sub_sym_size[1] = 8;
        trnsf_subsym_id[1] =
            paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
        coding_order[1] = 1;
        bin_id[1] = paramcabac::BinarizationParameters::BinarizationId::TU;
        bin_params[1][0] = 255;
      } else {
        found = false;
      }
      break;
    case core::GenDesc::kMultiMap:
      break;
    case core::GenDesc::kMultiSegmentAlignment:
    case core::GenDesc::kReadName:
      token_type_flag = true;
      trnsf_subseq_id =
          paramcabac::TransformedParameters::TransformIdSubseq::MATCH_CODING;
      output_symbol_size[0] = 8;
      coding_sub_sym_size[0] = 8;
      coding_order[0] = 0;
      trnsf_subsym_id[0] =
          paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;
      bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::BI;

      output_symbol_size[1] = 8;
      coding_sub_sym_size[1] = 8;
      coding_order[1] = 0;
      trnsf_subsym_id[1] =
          paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;
      bin_id[1] = paramcabac::BinarizationParameters::BinarizationId::TU;
      bin_params[1][0] = 255;  // splitUnitSize;

      output_symbol_size[2] = 32;
      coding_sub_sym_size[2] = 32;
      coding_order[2] = 0;
      trnsf_subsym_id[2] =
          paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM;
      bin_id[2] = paramcabac::BinarizationParameters::BinarizationId::EG;

      break;
    case core::GenDesc::kRtype:
      if (subseq_id == 0) {
        trnsf_subseq_id =
            paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM;
        output_symbol_size[0] = 3;
        coding_sub_sym_size[0] = 3;
        coding_order[0] = 1;

        bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
        bin_params[0][0] = 6;  // cmax;

        trnsf_subsym_id[0] =
            paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
      } else {
        found = false;
      }
      break;
    case core::GenDesc::kReadGroup:
      break;
    case core::GenDesc::kQv:
      switch (subseq_id) {
        case 0:  // qvPresenceFlag
          break;
        case 2:  // subSequenceID >= 2
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::NO_TRANSFORM;
          output_symbol_size[0] = 8;
          coding_sub_sym_size[0] = 8;
          coding_order[0] = 2;

          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
          bin_params[0][0] = 255;  // cmax;

          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
          break;
        default:  // subSequenceID >= 2
          trnsf_subseq_id = paramcabac::TransformedParameters::
              TransformIdSubseq::NO_TRANSFORM;
          output_symbol_size[0] = 8;
          coding_sub_sym_size[0] = 8;
          coding_order[0] = 1;

          bin_id[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
          bin_params[0][0] = 255;  // cmax;

          trnsf_subsym_id[0] =
              paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
          break;
      }
      break;
    case core::GenDesc::kRefTransPosition:
    case core::GenDesc::kRefTransTransform:
      break;
    default:
      UTILS_DIE("Invalid genomic descriptor");
  }

  if (!found) {
    // UTILS_DIE_("Unsupported kSubseqId "+std::to_string(kSubseqId)+" for
    // descID value
    // "+std::to_string(descID));
  }

  paramcabac::TransformedParameters trnsf_subseq_params(trnsf_subseq_id,
                                                        trnsf_subseq_param);
  size_t num_trnsf_subseqs = trnsf_subseq_params.GetNumStreams();
  std::vector<paramcabac::TransformedSubSeq> trnsf_subseq_cfgs;

  trnsf_subseq_cfgs.reserve(num_trnsf_subseqs);
  for (size_t t = 0; t < num_trnsf_subseqs; t++) {
    trnsf_subseq_cfgs.emplace_back(
        trnsf_subsym_id[t],
        paramcabac::SupportValues(output_symbol_size[t], coding_sub_sym_size[t],
                                  coding_order[t], share_subsym_lut_flag[t],
                                  share_subsym_prv_flag[t]),
        paramcabac::Binarization(
            bin_id[t], bypass_flag[t],
            paramcabac::BinarizationParameters(bin_id[t], bin_params[t]),
            paramcabac::Context(adaptive_mode_flag[t], output_symbol_size[t],
                                coding_sub_sym_size[t],
                                share_subsym_ctx_flag[t])),
        genie_subseq_id, t == num_trnsf_subseqs - 1, alpha_id);
  }

  paramcabac::Subsequence subseq_cfg(std::move(trnsf_subseq_params), subseq_id,
                                     token_type_flag,
                                     std::move(trnsf_subseq_cfgs));

  return subseq_cfg;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
