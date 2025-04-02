/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <atomic>
#include <fstream>
#include <utility>
#include "genie/core/read_encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

void ReadEncoder::setQVCoder(util::SideSelector<QVEncoder, QVEncoder::QVCoded, const record::Chunk&>* coder) {
    qvcoder = coder;
}

// ---------------------------------------------------------------------------------------------------------------------

void ReadEncoder::setNameCoder(NameSelector* coder) { namecoder = coder; }

// ---------------------------------------------------------------------------------------------------------------------

void ReadEncoder::setEntropyCoder(EntropySelector* coder) { entropycoder = coder; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit ReadEncoder::entropyCodeAU(EntropySelector* _entropycoder, AccessUnit&& a, bool write_raw) {
    AccessUnit au = std::move(a);
    if (write_raw) {
        static std::atomic<uint64_t> id(0);
        auto this_id = id++;
        for (const auto& d : genie::core::GetDescriptors()) {
            for (auto& s : au.Get(d.id)) {
                if (s.IsEmpty()) {
                    continue;
                }
                std::ofstream out_file_stream("rawstream_" + std::to_string(this_id) + "_" +
                                              std::to_string(static_cast<uint8_t>(d.id)) + "_" +
                                              std::to_string(static_cast<uint8_t>(s.GetId().second)));
                out_file_stream.write(static_cast<char*>(s.GetData().GetData()), s.GetData().GetRawSize());
            }
        }
    }
    for (auto& d : au) {
        auto encoded = _entropycoder->Process(d);
        au.GetParameters().SetDescriptor(d.GetId(), std::move(std::get<0>(encoded)));
        au.Set(d.GetId(), std::move(std::get<1>(encoded)));
        au.GetStats().Add(std::get<2>(encoded));
    }
    return au;
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit ReadEncoder::entropyCodeAU(AccessUnit&& a) {
    return entropyCodeAU(entropycoder, std::move(a), writeOutStreams);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
