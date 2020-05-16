/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "gabac-decompressor.h"
#include <genie/util/exceptions.h>
#include <sstream>
#include "decode-cabac.h"
#include "stream-handler.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

core::AccessUnitRaw::Descriptor decompressTokens(const gabac::EncodingConfiguration& conf0,
                                                 const gabac::EncodingConfiguration&,
                                                 core::AccessUnitPayload::SubsequencePayload&& data) {
    core::AccessUnitPayload::SubsequencePayload in = std::move(data);
    util::DataBlock remainingData = std::move(in.move());
    core::AccessUnitRaw::Descriptor ret(core::GenDesc::RNAME);
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

        uint8_t type_id = reader.read(4);
        uint8_t method = reader.read(4);

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
            ret.add(core::AccessUnitRaw::Subsequence(1, GenSubIndex{GenDesc::RNAME, ret.getSize()}));
        }
        ret.add(core::AccessUnitRaw::Subsequence(std::move(tmp), GenSubIndex{GenDesc::RNAME, mappedTypeId}));
    }
    return ret;
}

core::AccessUnitRaw::Subsequence GabacDecompressor::decompress(const gabac::EncodingConfiguration& conf,
                                                               core::AccessUnitPayload::SubsequencePayload&& data) {
    core::AccessUnitPayload::SubsequencePayload in = std::move(data);
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
    return core::AccessUnitRaw::Subsequence(std::move(tmp), in.getID());
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacDecompressor::flowIn(core::AccessUnitPayload&& t, const util::Section& id) {
    core::AccessUnitPayload payloadSet = std::move(t);
    GabacSeqConfSet configSet;
    auto raw_aus = core::AccessUnitRaw(payloadSet.moveParameters(), payloadSet.getRecordNum());

    configSet.loadParameters(raw_aus.getParameters());

    for (auto& desc : payloadSet) {
        if (desc.isEmpty()) {
            continue;
        }
        if (getDescriptor(desc.getID()).tokentype) {
            auto decomp = decompressTokens(configSet.getConfAsGabac(core::GenSubIndex{desc.getID(), 0}),
                                           configSet.getConfAsGabac(core::GenSubIndex{desc.getID(), 1}),
                                           std::move(*desc.begin()));
            for (auto& subseq : decomp) {
                while (raw_aus.get(desc.getID()).getSize() <= subseq.getID().second) {
                    raw_aus.get(desc.getID())
                        .add(core::AccessUnitRaw::Subsequence(
                            4, core::GenSubIndex{desc.getID(), raw_aus.get(desc.getID()).getSize()}));
                }
                raw_aus.set(subseq.getID(), std::move(subseq));
            }
            continue;
        }
        for (auto& subseq : desc) {
            if (subseq.isEmpty()) {
                continue;
            }
            auto d_id = subseq.getID();
            core::AccessUnitRaw::Subsequence subseqData =
                decompress(configSet.getConfAsGabac(subseq.getID()), std::move(subseq));
            raw_aus.set(d_id, std::move(subseqData));
        }
    }

    raw_aus.setReference(payloadSet.getReference());
    raw_aus.setClassType(payloadSet.getClassType());
    payloadSet.clear();
    flowOut(std::move(raw_aus), id);
}

// ---------------------------------------------------------------------------------------------------------------------

void GabacDecompressor::dryIn() { dryOut(); }
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
