/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

// -----------------------------------------------------------------------------

#include "genie/entropy/gabac/mismatch_decoder.h"

#include <memory>
#include <utility>
#include <vector>

#include "genie/entropy/gabac/stream_handler.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------
MismatchDecoder::MismatchDecoder(util::DataBlock&& d,
                                 const EncodingConfiguration& c)
    : num_subseq_symbols_total_(0),
      num_subseq_symbols_decoded_(0),
      num_trnsf_subseqs_(0) {
  const paramcabac::Subsequence& subseq_cfg = c.GetSubSeqConfig();
  uint64_t subseq_payload_size_used = 0;

  util::DataBlock data = std::move(d);
  IBufferStream input_stream(&data, 0);
  const uint64_t subseq_payload_size =
      StreamHandler::ReadStreamSize(input_stream);
  if (subseq_payload_size <= 0)
    return;  // Simple return as kSubseqPayloadSize can be zero.

  // Read number of symbols in descriptor subsequence
  if (subseq_cfg.GetTokenTypeFlag()) {
    subseq_payload_size_used +=
        StreamHandler::ReadU7(input_stream, num_subseq_symbols_total_);
  } else {
    subseq_payload_size_used +=
        StreamHandler::ReadUInt(input_stream, num_subseq_symbols_total_, 4);
  }

  if (num_subseq_symbols_total_ > 0) {
    if (input_stream.peek() != EOF) {
      // Set up for the inverse sequence transformation
      num_trnsf_subseqs_ = subseq_cfg.GetNumTransformSubSeqConfigs();
      if (num_trnsf_subseqs_ > 1)
        return;  // Note: Mismatch decoder is only allowed with 1
                 // transformed subseq.

      // Loop through the transformed sequences
      trnsf_subseq_data_.resize(num_trnsf_subseqs_);
      for (size_t i = 0; i < num_trnsf_subseqs_; i++) {
        uint64_t trnsf_subseq_payload_size_remain = 0;

        if (i < num_trnsf_subseqs_ - 1) {
          subseq_payload_size_used += StreamHandler::ReadUInt(
              input_stream, trnsf_subseq_payload_size_remain, 4);
        } else {
          trnsf_subseq_payload_size_remain =
              subseq_payload_size - subseq_payload_size_used;
        }

        if (trnsf_subseq_payload_size_remain > 0) {
          uint64_t curr_numtrnsf_symbols = 0;
          if (num_trnsf_subseqs_ > 1) {
            subseq_payload_size_used +=
                StreamHandler::ReadUInt(input_stream, curr_numtrnsf_symbols, 4);
            trnsf_subseq_payload_size_remain -= 4;
          } else {
            curr_numtrnsf_symbols = num_subseq_symbols_total_;
          }

          if (curr_numtrnsf_symbols > 0) {
            subseq_payload_size_used += StreamHandler::ReadBytes(
                input_stream, trnsf_subseq_payload_size_remain,
                &trnsf_subseq_data_[i]);
          }

          trnsf_symbols_decoder_.emplace_back(
              &trnsf_subseq_data_[i],
              subseq_cfg.GetTransformSubSeqCfg(static_cast<uint8_t>(i)),
              static_cast<unsigned int>(curr_numtrnsf_symbols));
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
uint64_t MismatchDecoder::DecodeMismatch(const uint64_t ref) {
  std::vector<uint64_t> decoded_trnsf_symbols(num_trnsf_subseqs_, 0);

  for (size_t i = 0; i < num_trnsf_subseqs_; i++) {
    decoded_trnsf_symbols[i] =
        trnsf_symbols_decoder_[i].SymbolsAvail() > 0
            ? trnsf_symbols_decoder_[i].DecodeNextSymbol(&ref)
            : 0;
  }

  num_subseq_symbols_decoded_++;
  // Note: Mismatch decoder is only allowed with 1 transform subseq.
  return decoded_trnsf_symbols[0];
}

// -----------------------------------------------------------------------------
bool MismatchDecoder::DataLeft() const {
  return num_subseq_symbols_decoded_ < num_subseq_symbols_total_;
}

// -----------------------------------------------------------------------------
[[maybe_unused]] uint64_t MismatchDecoder::GetSubseqSymbolsTotal() const {
  return num_subseq_symbols_total_;
}

// -----------------------------------------------------------------------------
std::unique_ptr<core::MismatchDecoder> MismatchDecoder::Copy() const {
  return std::make_unique<MismatchDecoder>(*this);
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
