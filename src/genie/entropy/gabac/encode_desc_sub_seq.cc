/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/encode_desc_sub_seq.h"

#include <vector>

#include "genie/entropy/gabac/configuration.h"
#include "genie/entropy/gabac/encode_transformed_sub_seq.h"
#include "genie/entropy/gabac/equality_sub_seq_transform.h"
#include "genie/entropy/gabac/match_sub_seq_transform.h"
#include "genie/entropy/gabac/merge_sub_seq_transform.h"
#include "genie/entropy/gabac/rle_sub_seq_transform.h"
#include "genie/entropy/gabac/stream_handler.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------

void DoSubsequenceTransform(
    const paramcabac::Subsequence& sub_seq_cfg,
    std::vector<util::DataBlock>* transformed_sub_seqs) {
  switch (sub_seq_cfg.GetTransformParameters().GetTransformIdSubseq()) {
    case paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM:
      transformed_sub_seqs->resize(1);
      break;
    case paramcabac::TransformedParameters::TransformIdSubseq::EQUALITY_CODING:
      TransformEqualityCoding(transformed_sub_seqs);
      break;
    case paramcabac::TransformedParameters::TransformIdSubseq::MATCH_CODING:
      TransformMatchCoding(sub_seq_cfg, transformed_sub_seqs);
      break;
    case paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING:
      TransformRleCoding(sub_seq_cfg, transformed_sub_seqs);
      break;
    case paramcabac::TransformedParameters::TransformIdSubseq::MERGE_CODING:
      TransformMergeCoding(sub_seq_cfg, transformed_sub_seqs);
      break;
    default:
      UTILS_DIE("Invalid sub seq transformation");
  }
}

// -----------------------------------------------------------------------------

uint64_t EncodeDescSubsequence(const IoConfiguration& conf,
                               const EncodingConfiguration& en_conf) {
  conf.Validate();
  const paramcabac::Subsequence& sub_seq_cfg = en_conf.GetSubSeqConfig();
  util::DataBlock subsequence(0, conf.input_word_size);
  util::DataBlock dependency(0, conf.input_word_size);

  size_t num_desc_sub_seq_symbols = 0;
  size_t sub_seq_payload_size = 0;

  num_desc_sub_seq_symbols =
      StreamHandler::ReadFull(*conf.input_stream, &subsequence);
  if (conf.dependency_stream != nullptr) {
    if (num_desc_sub_seq_symbols !=
        StreamHandler::ReadFull(*conf.dependency_stream, &dependency)) {
      UTILS_DIE("Size mismatch between dependency and descriptor subsequence");
    }
  }

  if (num_desc_sub_seq_symbols > 0) {
    // write number of symbols in descriptor subsequence
    if (sub_seq_cfg.GetTokenTypeFlag()) {
      sub_seq_payload_size +=
          StreamHandler::WriteU7(*conf.output_stream, num_desc_sub_seq_symbols);
    } else {
      sub_seq_payload_size += StreamHandler::WriteUInt(
          *conf.output_stream, num_desc_sub_seq_symbols, 4);
    }

    // Insert subsequence into vector
    std::vector<util::DataBlock> transformed_sub_seqs;
    transformed_sub_seqs.resize(1);
    transformed_sub_seqs[0].Swap(&subsequence);

    // Put descriptor subsequence, get transformed subsequences out
    DoSubsequenceTransform(sub_seq_cfg, &transformed_sub_seqs);
    const size_t num_transformed_sub_seqs = transformed_sub_seqs.size();

    // Loop through the transformed sequences
    for (size_t i = 0; i < num_transformed_sub_seqs; i++) {
      const uint64_t num_transformed_symbols = transformed_sub_seqs[i].Size();
      uint64_t transformed_sub_seq_payload_size = 0;
      if (num_transformed_symbols > 0) {
        // Encoding
        transformed_sub_seq_payload_size = EncodeTransformSubSeq(
            sub_seq_cfg.GetTransformSubSeqCfg(static_cast<uint8_t>(i)),
            &transformed_sub_seqs[i],
            !dependency.Empty() ? &dependency : nullptr);
      }

      if (i < num_transformed_sub_seqs - 1) {
        sub_seq_payload_size += StreamHandler::WriteUInt(
            *conf.output_stream, transformed_sub_seq_payload_size + 4, 4);
      }

      if (num_transformed_sub_seqs > 1) {
        sub_seq_payload_size += StreamHandler::WriteUInt(
            *conf.output_stream, num_transformed_symbols, 4);
      }

      if (transformed_sub_seq_payload_size > 0) {
        sub_seq_payload_size += StreamHandler::WriteBytes(
            *conf.output_stream, &transformed_sub_seqs[i]);
      }
    }
  }

  return sub_seq_payload_size;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
