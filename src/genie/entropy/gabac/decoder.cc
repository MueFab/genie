/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "decoder.h"
#include "decode-transformed-subseq.h"
#include "stream-handler.h"

#include <genie/util/runtime-exception.h>
#include <genie/util/watch.h>

#include "mismatch-decoder.h"

#include <iostream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit::Descriptor decompressTokens(const gabac::EncodingConfiguration& conf0,
                                              const gabac::EncodingConfiguration&,
                                              core::AccessUnit::Subsequence&& data) {
    core::AccessUnit::Subsequence in = std::move(data);
    util::DataBlock remainingData = std::move(in.move());
    core::AccessUnit::Descriptor ret(in.getID().first);
    if (remainingData.empty()) {
        return ret;
    }
    size_t offset = 6;
    UTILS_DIE_IF(offset >= remainingData.getRawSize(), "Tokentype stream smaller than expected");
    uint16_t num_tokentype_descriptors = 0;
    {
        const size_t READAHEAD = 6;
        util::DataBlock tmp =
            util::DataBlock(static_cast<uint8_t*>(remainingData.getData()), READAHEAD, remainingData.getWordSize());
        gabac::IBufferStream stream(&tmp);
        util::BitReader reader(stream);

        reader.read<uint32_t>();
        num_tokentype_descriptors = reader.read<uint16_t>();
    }
    int32_t typeNum = -1;
    for (size_t i = 0; i < num_tokentype_descriptors; ++i) {
        UTILS_DIE_IF(offset >= remainingData.getRawSize(), "Tokentype stream smaller than expected");
        const size_t READAHEAD = 11;
        util::DataBlock tmp = util::DataBlock(static_cast<uint8_t*>(remainingData.getData()) + offset, READAHEAD,
                                              remainingData.getWordSize());
        gabac::IBufferStream stream(&tmp);
        util::BitReader reader(stream);

        uint16_t type_id = reader.read<uint16_t>(4);
        uint16_t method = reader.read<uint16_t>(4);

        if (type_id == 0) typeNum++;
        size_t mappedTypeId = (typeNum << 4u) | (type_id & 0xfu);

        UTILS_DIE_IF(method != 3, "Only CABAC0 supported");
        offset++;
        uint64_t numSymbols = 0;
        offset += gabac::StreamHandler::readU7(stream, numSymbols);

        tmp = util::DataBlock(static_cast<uint8_t*>(remainingData.getData()) + offset,
                              remainingData.getRawSize() - offset, remainingData.getWordSize());
        offset += gabac::decodeTransformSubseq(conf0.getSubseqConfig().getTransformSubseqCfg(0), numSymbols, &tmp);
        while (ret.getSize() < mappedTypeId) {
            ret.add(core::AccessUnit::Subsequence(1, GenSubIndex{GenDesc::RNAME, ret.getSize()}));
        }
        ret.add(core::AccessUnit::Subsequence(std::move(tmp), GenSubIndex{GenDesc::RNAME, mappedTypeId}));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit::Subsequence Decoder::decompress(const gabac::EncodingConfiguration& conf,
                                                  core::AccessUnit::Subsequence&& data,
                                                  bool mmCoderEnabled) {
    core::AccessUnit::Subsequence in = std::move(data);

    if(getDescriptor(in.getID().first).getSubSeq(in.getID().second).mismatchDecoding && mmCoderEnabled) {
        in.attachMismatchDecoder(util::make_unique<MismatchDecoder>(in.move()));
        return in;
    }

    // Interface to GABAC library
    util::DataBlock buffer = in.move();
    gabac::IBufferStream in_stream(&buffer, 0);

    util::DataBlock tmp(0, 4);
    gabac::OBufferStream outbuffer(&tmp);

    // Setup
    const size_t GABAC_BLOCK_SIZE = 0;  // 0 means single block (block size is equal to input size)
    std::ostream* const GABC_LOG_OUTPUT_STREAM = &std::cout;
    const gabac::IOConfiguration GABAC_IO_SETUP = {&in_stream,
                                                   nullptr,
                                                   &outbuffer,
                                                   GABAC_BLOCK_SIZE,
                                                   GABC_LOG_OUTPUT_STREAM,
                                                   gabac::IOConfiguration::LogLevel::TRACE};
    const bool GABAC_DECODING_MODE = true;

    // Run
    gabac::run(GABAC_IO_SETUP, conf, GABAC_DECODING_MODE);

    outbuffer.flush(&tmp);
    return core::AccessUnit::Subsequence(std::move(tmp), in.getID());
}

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> Decoder::process(
    const parameter::DescriptorSubseqCfg& param, core::AccessUnit::Descriptor& d, bool mmCoderEnabled) {
    util::Watch watch;
    std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> desc;
    std::get<0>(desc) = std::move(d);
    const auto& param_desc = dynamic_cast<const core::parameter::desc_pres::DescriptorPresent&>(param.get());
    if (getDescriptor(std::get<0>(desc).getID()).tokentype) {
        size_t size = 0;
        for (const auto& s : std::get<0>(desc)) {
            size += s.getNumSymbols() * sizeof(uint32_t);
        }

        if (size) {
            std::get<1>(desc).addInteger("size-gabac-total-comp", size);
            std::get<1>(desc).addInteger("size-gabac-" + core::getDescriptor(std::get<0>(desc).getID()).name + "-comp",
                                         size);
        }

        const auto& token_param = dynamic_cast<const paramcabac::DecoderTokenType&>(param_desc.getDecoder());
        auto conf0 = token_param.getSubsequenceCfg(0);
        auto conf1 = token_param.getSubsequenceCfg(1);
        std::get<0>(desc) =
            decompressTokens(gabac::EncodingConfiguration(std::move(conf0)),
                             gabac::EncodingConfiguration(std::move(conf1)), std::move(*std::get<0>(desc).begin()));

        if (size) {
            std::get<1>(desc).addInteger("size-gabac-total-raw", std::get<0>(desc).begin()->getRawSize());
            std::get<1>(desc).addInteger("size-gabac-" + core::getDescriptor(std::get<0>(desc).getID()).name + "-raw",
                                         std::get<0>(desc).begin()->getRawSize());
        }
    } else {
        for (auto& subseq : std::get<0>(desc)) {
            if (subseq.isEmpty()) {
                continue;
            }
            auto d_id = subseq.getID();

            const auto& token_param = dynamic_cast<const paramcabac::DecoderRegular&>(param_desc.getDecoder());
            auto conf0 = token_param.getSubsequenceCfg(d_id.second);

            if (!subseq.isEmpty()) {
                std::get<1>(desc).addInteger("size-gabac-total-comp", subseq.getRawSize());
                std::get<1>(desc).addInteger(
                    "size-gabac-" + core::getDescriptor(std::get<0>(desc).getID()).name + "-" +
                        core::getDescriptor(std::get<0>(desc).getID()).subseqs[d_id.second].name + "-comp",
                    subseq.getRawSize());
            }

            std::get<0>(desc).set(d_id.second,
                                  decompress(gabac::EncodingConfiguration(std::move(conf0)), std::move(subseq), mmCoderEnabled));

            if (!std::get<0>(desc).get(d_id.second).isEmpty()) {
                std::get<1>(desc).addInteger("size-gabac-total-raw", std::get<0>(desc).get(d_id.second).getRawSize());
                std::get<1>(desc).addInteger(
                    "size-gabac-" + core::getDescriptor(std::get<0>(desc).getID()).name + "-" +
                        core::getDescriptor(std::get<0>(desc).getID()).subseqs[d_id.second].name + "-raw",
                    std::get<0>(desc).get(d_id.second).getRawSize());
            }
        }
    }
    std::get<1>(desc).addDouble("time-gabac", watch.check());
    return desc;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
