/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/read_encoder.h"

#include <atomic>
#include <fstream>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------
void ReadEncoder::SetQvCoder(util::SideSelector<QvEncoder, QvEncoder::qv_coded,
                                                const record::Chunk&>* coder) {
  qvcoder_ = coder;
}

// -----------------------------------------------------------------------------
void ReadEncoder::SetNameCoder(name_selector* coder) { namecoder_ = coder; }

// -----------------------------------------------------------------------------
void ReadEncoder::SetEntropyCoder(entropy_selector* coder) {
  entropycoder_ = coder;
}

// -----------------------------------------------------------------------------
AccessUnit ReadEncoder::EntropyCodeAu(entropy_selector* entropycoder,
                                      AccessUnit&& a, bool write_raw) {
  AccessUnit au = std::move(a);
  if (write_raw) {
    static std::atomic<uint64_t> id(0);
    auto this_id = id++;
    for (const auto& d : GetDescriptors()) {
      for (auto& s : au.Get(d.id)) {
        if (s.IsEmpty()) {
          continue;
        }
        std::ofstream out_file_stream(
            "rawstream_" + std::to_string(this_id) + "_" +
            std::to_string(static_cast<uint8_t>(d.id)) + "_" +
            std::to_string(static_cast<uint8_t>(s.GetId().second)));
        out_file_stream.write(
            static_cast<char*>(s.GetData().GetData()),
            static_cast<std::streamsize>(s.GetData().GetRawSize()));
      }
    }
  }
  for (auto& d : au) {
    auto encoded = entropycoder->Process(d);
    au.GetParameters().SetDescriptor(d.GetId(),
                                     std::move(std::get<0>(encoded)));
    au.Set(d.GetId(), std::move(std::get<1>(encoded)));
    au.GetStats().Add(std::get<2>(encoded));
  }
  return au;
}

// -----------------------------------------------------------------------------
AccessUnit ReadEncoder::EntropyCodeAu(AccessUnit&& a) const {
  return EntropyCodeAu(entropycoder_, std::move(a), write_out_streams_);
}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
