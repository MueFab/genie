/**
 * Copyright 2018-2024 The Genie Authors.
 * @file default_setup.cc
 *
 * @brief Implements default setups for encoding, decoding, and converting
 * genomic data.
 *
 * This file is part of the Genie project, which provides advanced tools for
 * genomic data compression, encoding, and decoding. The `default_setup.cpp`
 * file configures default pipelines for processing genomic data, including
 * encoders, decoders, and converters for various data components such as reads,
 * quality values, names, and entropy.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/module/default_setup.h"

#include <name/write_out/decoder.h>
#include <name/write_out/encoder.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/classifier_bypass.h"
#include "genie/core/classifier_regroup.h"
#include "genie/core/flow_graph_convert.h"
#include "genie/entropy/bsc/decoder.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/gabac/decoder.h"
#include "genie/entropy/gabac/encoder.h"
#include "genie/entropy/lzma/decoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/decoder.h"
#include "genie/entropy/zstd/encoder.h"
#include "genie/name/tokenizer/decoder.h"
#include "genie/name/tokenizer/encoder.h"
#include "genie/quality/calq//decoder.h"
#include "genie/quality/qvwriteout/encoder.h"
#include "genie/read/localassembly/decoder.h"
#include "genie/read/localassembly/encoder.h"
#include "genie/read/lowlatency/decoder.h"
#include "genie/read/lowlatency/encoder.h"
#include "genie/read/refcoder/decoder.h"
#include "genie/read/refcoder/encoder.h"
#include "genie/read/spring/decoder.h"
#include "genie/read/spring/encoder.h"

#define USE_ZSTD 1

// -----------------------------------------------------------------------------

namespace genie::module {

// -----------------------------------------------------------------------------

std::unique_ptr<core::FlowGraphEncode> BuildDefaultEncoder(
    size_t threads, const std::string& working_dir, size_t block_size,
    core::ClassifierRegroup::RefMode external_ref, bool raw_ref,
    bool write_raw_streams) {
  auto ret = std::make_unique<core::FlowGraphEncode>(threads);

  ret->SetClassifier(std::make_unique<core::ClassifierRegroup>(
      block_size, &ret->GetRefMgr(), external_ref, raw_ref));

  ret->AddReadCoder(
      std::make_unique<read::refcoder::Encoder>(write_raw_streams));
  ret->AddReadCoder(
      std::make_unique<read::localassembly::Encoder>(false, write_raw_streams));
  ret->AddReadCoder(
      std::make_unique<read::lowlatency::Encoder>(write_raw_streams));
  ret->AddReadCoder(std::make_unique<read::spring::Encoder>(
      working_dir, threads, false, write_raw_streams));
  ret->AddReadCoder(std::make_unique<read::spring::Encoder>(
      working_dir, threads, true, write_raw_streams));
  ret->SetReadCoderSelector([](const core::record::Chunk& chunk) -> size_t {
    if (chunk.GetData().empty()) {
      return 2;
    }
    if (chunk.GetData().front().GetClassId() ==
        core::record::ClassType::kClassU) {
      if (chunk.IsReferenceOnly() ||
          chunk.GetData().front().GetNumberOfTemplateSegments() !=
              chunk.GetData().front().GetSegments().size()) {
        return 2;
      }
      if (chunk.GetData().front().GetNumberOfTemplateSegments() > 1) {
        return 4;
      }
      return 3;
    }
    if (chunk.GetRef().IsEmpty()) {
      return 1;
    }
    return 0;
  });

  ret->AddQvCoder(std::make_unique<quality::qvwriteout::Encoder>());
  ret->SetQvSelector([](const core::record::Chunk&) -> size_t { return 0; });

  ret->AddNameCoder(std::make_unique<name::tokenizer::Encoder>());
  ret->AddNameCoder(std::make_unique<name::write_out::Encoder>());
  ret->SetNameSelector([](const core::record::Chunk&) -> size_t {
#if USE_ZSTD
    return 1;
#else
    return 0;
#endif
  });

  ret->AddEntropyCoder(
      std::make_unique<entropy::gabac::Encoder>(write_raw_streams));
  ret->AddEntropyCoder(
      std::make_unique<entropy::lzma::Encoder>(write_raw_streams));
  ret->AddEntropyCoder(
      std::make_unique<entropy::zstd::Encoder>(write_raw_streams));
  ret->AddEntropyCoder(
      std::make_unique<entropy::bsc::Encoder>(write_raw_streams));
  ret->SetEntropyCoderSelector(
      [](const core::AccessUnit::Descriptor&) -> size_t {
#if USE_ZSTD
        return 2;
#else
        return 0;
#endif
      });

  ret->SetExporterSelector([](const core::AccessUnit&) -> size_t { return 0; });

  return ret;
}

// -----------------------------------------------------------------------------

std::unique_ptr<core::FlowGraphDecode> build_default_decoder(
    size_t threads, const std::string& working_dir, bool combine_pairs_flag) {
  auto ret = std::make_unique<core::FlowGraphDecode>(threads);

  ret->AddReadCoder(std::make_unique<read::refcoder::Decoder>());
  ret->AddReadCoder(std::make_unique<read::localassembly::Decoder>());
  auto lld = std::make_unique<read::lowlatency::Decoder>();
  ret->SetRefDecoder(lld.get());
  ret->AddReadCoder(std::move(lld));
  ret->AddReadCoder(std::make_unique<read::spring::Decoder>(
      working_dir, combine_pairs_flag, false));
  ret->AddReadCoder(std::make_unique<read::spring::Decoder>(
      working_dir, combine_pairs_flag, true));
  ret->SetReadCoderSelector([](const core::AccessUnit& au) -> size_t {
    if (au.GetParameters().IsComputedReference()) {
      switch (au.GetParameters().GetComputedRef().GetAlgorithm()) {
        case core::parameter::ComputedRef::Algorithm::kGlobalAssembly: {
          if (au.GetParameters().GetNumberTemplateSegments() >= 2) {
            return 4;
          }
          return 3;
        }
        case core::parameter::ComputedRef::Algorithm::kRefTransform:
          UTILS_DIE("Ref Transform decoding not supported");
        case core::parameter::ComputedRef::Algorithm::kLocalAssembly:
          return 1;
        case core::parameter::ComputedRef::Algorithm::kPushIn:
          UTILS_DIE("Push in decoding not supported");
        default:
          UTILS_DIE("Unknown decoding not supported");
      }
    }
    if (au.GetClassType() == core::record::ClassType::kClassU) {
      return 2;
    }
    return 0;
  });

  ret->AddQvCoder(std::make_unique<quality::calq::Decoder>());
  ret->SetQvSelector([](const core::parameter::QualityValues& param,
                        const std::vector<std::string>&,
                        const std::vector<uint64_t>&,
                        core::AccessUnit::Descriptor&) -> size_t {
    UTILS_DIE_IF(param.GetMode() != 1, "Unsupported QV decoding mode");
    return 0;
  });

  ret->AddNameCoder(std::make_unique<name::tokenizer::Decoder>());
  ret->AddNameCoder(std::make_unique<name::write_out::Decoder>());
  ret->SetNameSelector([](const core::AccessUnit::Descriptor& d) -> size_t {
    return d.GetSize() <= 1 ? 1 : 0;
    // TODO(Fabian): Check if there is a better way to determine the name
    // decoder
  });

  ret->AddEntropyCoder(std::make_unique<entropy::gabac::Decoder>());
  ret->AddEntropyCoder(std::make_unique<entropy::lzma::Decoder>());
  ret->AddEntropyCoder(std::make_unique<entropy::zstd::Decoder>());
  ret->AddEntropyCoder(std::make_unique<entropy::bsc::Decoder>());
  ret->SetEntropyCoderSelector(
      [](const core::parameter::DescriptorSubSequenceCfg& cfg,
         core::AccessUnit::Descriptor&, bool) -> size_t {
        UTILS_DIE_IF(cfg.IsClassSpecific(),
                     "Class-specific entropy decoding not supported");
        auto* desc =
            dynamic_cast<const core::parameter::desc_pres::DescriptorPresent*>(
                &cfg.Get());
        UTILS_DIE_IF(desc == nullptr, "Decoder configuration not present");
        if (dynamic_cast<const core::parameter::desc_pres::DecoderTokenType*>(
                &desc->GetDecoder()) != nullptr) {
          return 0;
        }
        auto* decoder =
            dynamic_cast<const core::parameter::desc_pres::DecoderRegular*>(
                &desc->GetDecoder());
        UTILS_DIE_IF(decoder == nullptr, "Unknown decoder configuration");
        UTILS_DIE_IF(
            decoder->GetMode() > 3,
            "Unknown entropy decoder: " + std::to_string(decoder->GetMode()));
        return decoder->GetMode();
      });

  ret->SetExporterSelector(
      [](const core::record::Chunk&) -> size_t { return 0; });

  return ret;
}

// -----------------------------------------------------------------------------

std::unique_ptr<core::FlowGraphConvert> build_default_converter(
    size_t threads) {
  auto ret = std::make_unique<core::FlowGraphConvert>(threads);

  ret->SetExporterSelector(
      [](const core::record::Chunk&) -> size_t { return 0; });

  ret->SetClassifier(std::make_unique<core::ClassifierBypass>());

  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::module

// -----------------------------------------------------------------------------
