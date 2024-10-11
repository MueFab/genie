/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/bsc/encoder.h"

#include <libbsc.h>
#include <atomic>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include "genie/core/parameter/descriptor_present/decoder-regular.h"
#include "genie/core/parameter/descriptor_present/descriptor_present.h"
#include "genie/entropy/bsc/param_decoder.h"
#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::bsc {

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void fillDecoder(const core::GenomicDescriptorProperties &desc, T &decoder_config) {
    for (const auto &subdesc : desc.subseqs) {
        const auto bits_p2 = core::range2bytes(subdesc.range) * 8;
        auto subseqCfg = Subsequence(bits_p2);
        decoder_config.setSubsequenceCfg(static_cast<uint8_t>(subdesc.id.second), std::move(subseqCfg));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void storeParameters(core::GenDesc desc, core::parameter::DescriptorSubseqCfg &parameterSet) {
    auto descriptor_configuration = std::make_unique<core::parameter::desc_pres::DescriptorPresent>();

    auto decoder_config = std::make_unique<DecoderRegular>(desc);
    fillDecoder(getDescriptor(desc), *decoder_config);
    descriptor_configuration->setDecoder(std::move(decoder_config));

    parameterSet = core::parameter::DescriptorSubseqCfg();
    parameterSet.set(std::move(descriptor_configuration));
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit::Subsequence compress(core::AccessUnit::Subsequence &&in) {
    const size_t num_symbols = in.getNumSymbols();
    util::DataBlock input_buffer = in.move();
    util::DataBlock output_buffer(input_buffer.getRawSize() + LIBBSC_HEADER_SIZE, 1);

    UTILS_DIE_IF(bsc_init(0) != LIBBSC_NO_ERROR, "bsc initialization failed");
    const int compressedSize = bsc_compress(static_cast<const unsigned char *>(input_buffer.getData()),
                                      static_cast<unsigned char *>(output_buffer.getData()), input_buffer.getRawSize(),
                                      LIBBSC_DEFAULT_LZPHASHSIZE, LIBBSC_DEFAULT_LZPMINLEN, LIBBSC_DEFAULT_BLOCKSORTER,
                                      LIBBSC_DEFAULT_CODER, 0);
    UTILS_DIE_IF(compressedSize < 0, "bsc compression failed: " + std::to_string(compressedSize));
    output_buffer.resize(compressedSize);

    core::AccessUnit::Subsequence out(in.getID());
    out.annotateNumSymbols(num_symbols);
    out.set(std::move(output_buffer));
    return out;
}

// ---------------------------------------------------------------------------------------------------------------------

core::EntropyEncoder::EntropyCoded Encoder::process(core::AccessUnit::Descriptor &desc) {
    EntropyCoded ret;
    const util::Watch watch;
    std::get<1>(ret) = std::move(desc);
    for (auto &subdesc : std::get<1>(ret)) {
        if (!subdesc.isEmpty()) {
            // add compressed payload
            const auto id = subdesc.getID();

            std::get<2>(ret).addInteger("size-bsc-total-raw", subdesc.getRawSize());
            auto subseq_name = std::string();
            if (getDescriptor(std::get<1>(ret).getID()).tokentype) {
                subseq_name = getDescriptor(std::get<1>(ret).getID()).name;
            } else {
                subseq_name = getDescriptor(std::get<1>(ret).getID()).name + "-" +
                              getDescriptor(std::get<1>(ret).getID()).subseqs[id.second].name;
            }
            std::get<2>(ret).addInteger("size-bsc-" + subseq_name + "-raw", subdesc.getRawSize());

            std::get<1>(ret).set(id.second, compress(std::move(subdesc)));

            if (!std::get<1>(ret).get(id.second).isEmpty()) {
                std::get<2>(ret).addInteger("size-bsc-total-comp", std::get<1>(ret).get(id.second).getRawSize());
                std::get<2>(ret).addInteger("size-bsc-" + subseq_name + "-comp",
                                            std::get<1>(ret).get(id.second).getRawSize());
            }
        } else {
            // add empty payload
            std::get<1>(ret).set(subdesc.getID().second,
                                 core::AccessUnit::Subsequence(subdesc.getID(), util::DataBlock(0, 1)));
        }
    }
    storeParameters(std::get<1>(ret).getID(), std::get<0>(ret));
    std::get<2>(ret).addDouble("time-bsc", watch.check());
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

Encoder::Encoder(const bool _writeOutStreams) : writeOutStreams(_writeOutStreams) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
