/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/decode_desc_sub_seq.h"

#include <vector>

#include "genie/entropy/gabac/configuration.h"
#include "genie/entropy/gabac/decode_transformed_sub_seq.h"
#include "genie/entropy/gabac/equality_sub_seq_transform.h"
#include "genie/entropy/gabac/match_sub_seq_transform.h"
#include "genie/entropy/gabac/merge_sub_seq_transform.h"
#include "genie/entropy/gabac/rle_sub_seq_transform.h"
#include "genie/entropy/gabac/stream_handler.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------

void DoInverseSubsequenceTransform(
    const paramcabac::Subsequence& sub_sequence_cfg,
    std::vector<util::DataBlock>* const transformed_sub_sequence) {
  switch (sub_sequence_cfg.GetTransformParameters().GetTransformIdSubseq()) {
    case paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM:
      transformed_sub_sequence->resize(1);
      break;
    case paramcabac::TransformedParameters::TransformIdSubseq::EQUALITY_CODING:
      InverseTransformEqualityCoding(transformed_sub_sequence);
      break;
    case paramcabac::TransformedParameters::TransformIdSubseq::MATCH_CODING:
      InverseTransformMatchCoding(transformed_sub_sequence);
      break;
    case paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING:
      InverseTransformRleCoding(sub_sequence_cfg, transformed_sub_sequence);
      break;
    case paramcabac::TransformedParameters::TransformIdSubseq::MERGE_CODING:
      InverseTransformMergeCoding(sub_sequence_cfg, transformed_sub_sequence);
      break;
    default:
      UTILS_DIE("Invalid sub sequence transformation");
  }
}

// -----------------------------------------------------------------------------

uint64_t DecodeDescSubsequence(const IoConfiguration& io_conf,
                               const EncodingConfiguration& en_conf) {
  const paramcabac::Subsequence& sub_sequence_cfg = en_conf.GetSubSeqConfig();
  util::DataBlock dependency(0, 4);

  uint64_t sub_sequence_payload_size_used = 0;
  uint64_t num_desc_sub_sequence_symbols = 0;
  const uint64_t sub_sequence_payload_size =
      StreamHandler::ReadStreamSize(*io_conf.input_stream);

  if (sub_sequence_payload_size <= 0) return 0;

  // read number of symbols in descriptor subsequence
  if (sub_sequence_cfg.GetTokenTypeFlag()) {
    sub_sequence_payload_size_used += StreamHandler::ReadU7(
        *io_conf.input_stream, num_desc_sub_sequence_symbols);
  } else {
    sub_sequence_payload_size_used += StreamHandler::ReadUInt(
        *io_conf.input_stream, num_desc_sub_sequence_symbols, 4);
  }

  if (num_desc_sub_sequence_symbols > 0) {
    if (io_conf.dependency_stream != nullptr) {
      if (num_desc_sub_sequence_symbols !=
          StreamHandler::ReadFull(*io_conf.dependency_stream, &dependency)) {
        UTILS_DIE(
            "Size mismatch between dependency and descriptor "
            "subsequence");
      }
    }

    if (io_conf.input_stream->peek() != EOF) {
      // Set up for the inverse sequence transformation
      const size_t num_transform_configs =
          sub_sequence_cfg.GetNumTransformSubSeqConfigs();

      // Loop through the transformed sequences
      std::vector<util::DataBlock> transformed_sub_sequences(
          num_transform_configs);
      for (size_t i = 0; i < num_transform_configs; i++) {
        const uint8_t word_size =
            i == num_transform_configs - 1 ? io_conf.output_word_size : 1;
        transformed_sub_sequences[i].SetWordSize(word_size);
      }
      for (size_t i = 0; i < num_transform_configs; i++) {
        util::DataBlock decoded_transformed_sub_sequences;
        uint64_t payload_size_remaining = 0;

        if (i < num_transform_configs - 1) {
          sub_sequence_payload_size_used += StreamHandler::ReadUInt(
              *io_conf.input_stream, payload_size_remaining, 4);
        } else {
          payload_size_remaining =
              sub_sequence_payload_size - sub_sequence_payload_size_used;
        }

        if (payload_size_remaining > 0) {
          uint64_t num_transformed_symbols = 0;
          if (num_transform_configs > 1) {
            sub_sequence_payload_size_used += StreamHandler::ReadUInt(
                *io_conf.input_stream, num_transformed_symbols, 4);
            payload_size_remaining -= 4;
          } else {
            num_transformed_symbols = num_desc_sub_sequence_symbols;
          }

          if (num_transformed_symbols <= 0) continue;

          StreamHandler::ReadBytes(*io_conf.input_stream,
                                   payload_size_remaining,
                                   &decoded_transformed_sub_sequences);

          const uint8_t word_size =
              i == num_transform_configs - 1 ? io_conf.output_word_size : 1;
          // Decoding
          sub_sequence_payload_size_used += DecodeTransformSubSeq(
              sub_sequence_cfg.GetTransformSubSeqCfg(static_cast<uint8_t>(i)),
              static_cast<unsigned int>(num_transformed_symbols),
              &decoded_transformed_sub_sequences, word_size,
              !dependency.Empty() ? &dependency : nullptr);
          transformed_sub_sequences[i].Swap(&decoded_transformed_sub_sequences);
        }
      }

      DoInverseSubsequenceTransform(sub_sequence_cfg,
                                    &transformed_sub_sequences);

      StreamHandler::WriteBytes(*io_conf.output_stream,
                                &transformed_sub_sequences[0]);
    }
  }

  return sub_sequence_payload_size_used;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
