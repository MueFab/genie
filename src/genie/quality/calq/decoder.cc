/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/quality/calq/decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace qvcalq {

// ---------------------------------------------------------------------------------------------------------------------

bool Decoder::isAligned(const core::AccessUnit::Descriptor& desc) {
    // TODO: better check
    return desc.getSize() != 3;
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string> Decoder::decodeAligned(const quality::paramqv1::QualityValues1& param,
                                                const std::vector<std::string>& ecigar_vec,
                                                const std::vector<uint64_t>& positions,
                                                core::AccessUnit::Descriptor& desc) {
    // calq variables
    calq::DecodingOptions options;  // default options
    calq::SideInformation sideInformation;
    calq::EncodingBlock output;
    calq::DecodingBlock input;

    sideInformation.posOffset = positions[0];
    sideInformation.qualOffset = 0;
    fillInput(input, desc, param);

    // fill sideInformation
    sideInformation.positions.emplace_back(positions);
    sideInformation.cigars.emplace_back(ecigar_vec);

    calq::decode(options, sideInformation, input, &output);

    return output.qvalues.front();
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string> Decoder::decodeUnaligned(const quality::paramqv1::QualityValues1& param_casted,
                                                  const std::vector<std::string>& ecigar_vec,
                                                  core::AccessUnit::Descriptor& desc) {
    std::vector<std::string> qv;
    auto codebook = param_casted.getCodebook(0).getEntries();

    for (const auto& ecigar : ecigar_vec) {
        qv.emplace_back();
        core::CigarTokenizer::tokenize(
            ecigar, core::getECigarInfo(),
            [&qv, &desc, &param_casted](uint8_t cigar, const util::StringView& bs, const util::StringView&) -> bool {
                (void)cigar;
                for (size_t i = 0; i < bs.length(); ++i) {
                    auto index = (uint8_t)desc.get((uint16_t)(2)).pull();
                    qv.back().push_back(param_casted.getCodebook(0).getEntries()[index]);
                }
                return true;
            });
    }
    return qv;
}

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::fillInput(calq::DecodingBlock& input, core::AccessUnit::Descriptor& desc,
                        const quality::paramqv1::QualityValues1& param) {
    // quantizerIndices + istepIndices
    for (size_t i = 1; i < desc.getSize(); ++i) {
        auto data = static_cast<uint8_t*>(desc.get(i).getData().getData());
        auto size = desc.get(i).getData().size();

        if (i == 1) {
            input.quantizerIndices = std::vector<uint8_t>(data, data + size);
        } else {
            input.stepindices.emplace_back(data, data + size);
        }
    }

    // codebooks
    for (size_t i = 0; i < param.getNumberCodeBooks(); ++i) {
        input.codeBooks.push_back(param.getCodebook(i).getEntries());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<std::vector<std::string>, core::stats::PerfStats> Decoder::process(
    const core::parameter::QualityValues& param, const std::vector<std::string>& ecigar_vec,
    const std::vector<uint64_t>& positions, core::AccessUnit::Descriptor& desc) {
    util::Watch watch;
    const auto& param_casted = dynamic_cast<const quality::paramqv1::QualityValues1&>(param);

    std::vector<std::string> resultQV;

    if (isAligned(desc)) {
        resultQV = decodeAligned(param_casted, ecigar_vec, positions, desc);
    } else {
        resultQV = decodeUnaligned(param_casted, ecigar_vec, desc);
    }

    core::stats::PerfStats stats;
    stats.addDouble("time-qvcalq", watch.check());
    return std::make_tuple(resultQV, stats);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace qvcalq
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
