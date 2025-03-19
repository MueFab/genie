/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/parameter/descriptor_present/descriptor_present.h"

#include <memory>
#include <utility>

#include "genie/core/global_cfg.h"
#include "genie/core/parameter/descriptor_present/decoder_regular.h"
#include "genie/core/parameter/descriptor_present/decoder_token_type.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter::desc_pres {

// -----------------------------------------------------------------------------

DescriptorPresent::DescriptorPresent()
    : Descriptor(kPresent), decoder_configuration_(nullptr) {
  decoder_configuration_ = nullptr;
}

// -----------------------------------------------------------------------------

DescriptorPresent::DescriptorPresent(const GenDesc desc,
                                     util::BitReader& reader)
    : Descriptor(kPresent) {
  if (const auto mode = reader.Read<uint8_t>();
      (desc == GenDesc::kMultiSegmentAlignment || desc == GenDesc::kReadName) &&
      mode == 0) {
    decoder_configuration_ =
        GlobalCfg::GetSingleton()
            .GetIndustrialPark()
            .Construct<DecoderTokenType>(mode, desc, reader);
  } else {
    decoder_configuration_ =
        GlobalCfg::GetSingleton().GetIndustrialPark().Construct<DecoderRegular>(
            mode, desc, reader);
  }
}

// -----------------------------------------------------------------------------

bool DescriptorPresent::Equals(const Descriptor* desc) const {
  return Descriptor::Equals(desc) &&
         decoder_configuration_->Equals(
             dynamic_cast<const DescriptorPresent*>(desc)
                 ->decoder_configuration_.get());
}

// -----------------------------------------------------------------------------

std::unique_ptr<Descriptor> DescriptorPresent::Clone() const {
  auto ret = std::make_unique<DescriptorPresent>();
  ret->dec_cfg_preset_ = dec_cfg_preset_;
  ret->decoder_configuration_ = decoder_configuration_->Clone();
  return ret;
}

// -----------------------------------------------------------------------------

void DescriptorPresent::Write(util::BitWriter& writer) const {
  Descriptor::Write(writer);
  if (dec_cfg_preset_ != kPresent) {
    return;
  }
  decoder_configuration_->Write(writer);
}

// -----------------------------------------------------------------------------

void DescriptorPresent::SetDecoder(std::unique_ptr<Decoder> conf) {
  decoder_configuration_ = std::move(conf);
}

// -----------------------------------------------------------------------------

const Decoder& DescriptorPresent::GetDecoder() const {
  return *decoder_configuration_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter::desc_pres

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------