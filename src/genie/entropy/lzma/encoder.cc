/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/lzma/encoder.h"

#include <lzma.h>
#include <atomic>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include "genie/core/parameter/descriptor_present/decoder-regular.h"
#include "genie/core/parameter/descriptor_present/descriptor_present.h"
#include "genie/entropy/lzma/param_decoder.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::lzma {

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void fillDecoder(const core::GenomicDescriptorProperties &desc, T &decoder_config) {
    for (const auto &subdesc : desc.subseqs) {
        auto bits_p2 = genie::core::range2bytes(subdesc.range) * 8;
        auto subseqCfg = Subsequence(bits_p2);
        decoder_config.setSubsequenceCfg(uint8_t(subdesc.id.second), std::move(subseqCfg));
    }
}

void storeParameters(core::GenDesc desc, core::parameter::DescriptorSubseqCfg &parameterSet) {
    auto descriptor_configuration = std::make_unique<core::parameter::desc_pres::DescriptorPresent>();

    auto decoder_config = std::make_unique<DecoderRegular>(desc);
    fillDecoder(core::getDescriptor(desc), *decoder_config);
    descriptor_configuration->setDecoder(std::move(decoder_config));

    parameterSet = core::parameter::DescriptorSubseqCfg();
    parameterSet.set(std::move(descriptor_configuration));
}

core::AccessUnit::Subsequence compress(core::AccessUnit::Subsequence &&in) {
    size_t num_symbols = in.getNumSymbols();
    util::DataBlock input_buffer = in.move();

    lzma_stream strm = LZMA_STREAM_INIT;  // Initialize the lzma_stream structure

    UTILS_DIE_IF(lzma_easy_encoder(&strm, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64) != LZMA_OK,
                 "lzma initialization failed");

    strm.next_in = static_cast<const unsigned char *>(input_buffer.getData());
    strm.avail_in = input_buffer.getRawSize();

    size_t outSize = lzma_stream_buffer_bound(input_buffer.getRawSize());
    util::DataBlock output_buffer(outSize, 1);

    strm.next_out = static_cast<unsigned char *>(output_buffer.getData());
    strm.avail_out = output_buffer.getRawSize();
    lzma_ret ret = LZMA_OK;
    while (ret == LZMA_OK) {
        ret = lzma_code(&strm, LZMA_FINISH);
        UTILS_DIE_IF(ret != LZMA_STREAM_END && ret != LZMA_OK, "lzma compression failed: " + std::to_string(ret));
    }

    lzma_end(&strm);

    output_buffer.resize(strm.total_out);

    core::AccessUnit::Subsequence out(in.getID());
    out.annotateNumSymbols(num_symbols);
    out.set(std::move(output_buffer));
    return out;
}

core::EntropyEncoder::EntropyCoded Encoder::process(core::AccessUnit::Descriptor &desc) {
    EntropyCoded ret;
    util::Watch watch;
    std::get<1>(ret) = std::move(desc);
    for (auto &subdesc : std::get<1>(ret)) {
        if (!subdesc.isEmpty()) {
            // add compressed payload
            auto id = subdesc.getID();

            std::get<2>(ret).addInteger("size-lzma-total-raw", subdesc.getRawSize());
            auto subseq_name = std::string();
            if (core::getDescriptor(std::get<1>(ret).getID()).tokentype) {
                subseq_name = core::getDescriptor(std::get<1>(ret).getID()).name;
            } else {
                subseq_name = core::getDescriptor(std::get<1>(ret).getID()).name + "-" +
                              core::getDescriptor(std::get<1>(ret).getID()).subseqs[id.second].name;
            }
            std::get<2>(ret).addInteger("size-lzma-" + subseq_name + "-raw", subdesc.getRawSize());

            std::get<1>(ret).set(id.second, compress(std::move(subdesc)));

            if (!std::get<1>(ret).get(id.second).isEmpty()) {
                std::get<2>(ret).addInteger("size-lzma-total-comp", std::get<1>(ret).get(id.second).getRawSize());
                std::get<2>(ret).addInteger("size-lzma-" + subseq_name + "-comp",
                                            std::get<1>(ret).get(id.second).getRawSize());
            }
        } else {
            // add empty payload
            std::get<1>(ret).set(subdesc.getID().second,
                                 core::AccessUnit::Subsequence(subdesc.getID(), util::DataBlock(0, 1)));
        }
    }
    storeParameters(std::get<1>(ret).getID(), std::get<0>(ret));
    std::get<2>(ret).addDouble("time-lzma", watch.check());
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

Encoder::Encoder(bool _writeOutStreams) : writeOutStreams(_writeOutStreams) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::lzma

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
