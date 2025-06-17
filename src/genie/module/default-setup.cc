/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/module/default-setup.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/classifier_bypass.h"
#include "genie/core/classifier_regroup.h"
#include "genie/core/flow_graph_convert.h"
#include "genie/entropy/gabac/decoder.h"
#include "genie/entropy/gabac/encoder.h"
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

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace module {

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<core::FlowGraphEncode> BuildDefaultEncoder(size_t threads, const std::string& working_dir,
                                                           size_t blocksize,
                                                           core::ClassifierRegroup::RefMode externalref, bool rawref,
                                                           bool writeRawStreams) {
    std::unique_ptr<core::FlowGraphEncode> ret = genie::util::make_unique<core::FlowGraphEncode>(threads);

    ret->SetClassifier(
        genie::util::make_unique<genie::core::ClassifierRegroup>(blocksize, &ret->GetRefMgr(), externalref, rawref));

    ret->AddReadCoder(genie::util::make_unique<genie::read::refcoder::Encoder>(writeRawStreams));
    ret->AddReadCoder(genie::util::make_unique<genie::read::localassembly::Encoder>(false, writeRawStreams));
    ret->AddReadCoder(genie::util::make_unique<genie::read::lowlatency::Encoder>(writeRawStreams));
    ret->AddReadCoder(
        genie::util::make_unique<genie::read::spring::Encoder>(working_dir, threads, false, writeRawStreams));
    ret->AddReadCoder(
        genie::util::make_unique<genie::read::spring::Encoder>(working_dir, threads, true, writeRawStreams));
    ret->SetReadCoderSelector([](const genie::core::record::Chunk& chunk) -> size_t {
        if (chunk.GetData().empty()) {
            return 2;
        }
        if (chunk.GetData().front().GetClassId() == genie::core::record::ClassType::kClassU) {
            if (chunk.IsReferenceOnly() ||
                chunk.GetData().front().GetNumberOfTemplateSegments() != chunk.GetData().front().GetSegments().size()) {
                return 2;
            }
            if (chunk.GetData().front().GetNumberOfTemplateSegments() > 1) {
                return 4;
            } else {
                return 3;
            }
        } else {
            if (chunk.GetRef().IsEmpty()) {
                return 1;
            } else {
                return 0;
            }
        }
    });

    ret->AddQvCoder(genie::util::make_unique<genie::quality::qvwriteout::Encoder>());
    ret->SetQvSelector([](const genie::core::record::Chunk&) -> size_t { return 0; });

    ret->AddNameCoder(genie::util::make_unique<genie::name::tokenizer::Encoder>());
    ret->SetNameSelector([](const genie::core::record::Chunk&) -> size_t { return 0; });

    ret->AddEntropyCoder(genie::util::make_unique<genie::entropy::gabac::Encoder>(writeRawStreams));
    ret->SetEntropyCoderSelector([](const genie::core::AccessUnit::Descriptor&) -> size_t { return 0; });

    ret->SetExporterSelector([](const genie::core::AccessUnit&) -> size_t { return 0; });

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<core::FlowGraphDecode> build_default_decoder(size_t threads, const std::string& working_dir,
                                                           bool combinePairsFlag, size_t) {
    std::unique_ptr<core::FlowGraphDecode> ret = genie::util::make_unique<core::FlowGraphDecode>(threads);

    ret->AddReadCoder(genie::util::make_unique<genie::read::refcoder::Decoder>());
    ret->AddReadCoder(genie::util::make_unique<genie::read::localassembly::Decoder>());
    auto lld = genie::util::make_unique<genie::read::lowlatency::Decoder>();
    ret->SetRefDecoder(lld.get());
    ret->AddReadCoder(std::move(lld));
    ret->AddReadCoder(genie::util::make_unique<genie::read::spring::Decoder>(working_dir, combinePairsFlag, false));
    ret->AddReadCoder(genie::util::make_unique<genie::read::spring::Decoder>(working_dir, combinePairsFlag, true));
    ret->SetReadCoderSelector([](const genie::core::AccessUnit& au) -> size_t {
        if (au.GetParameters().IsComputedReference()) {
            switch (au.GetParameters().GetComputedRef().GetAlgorithm()) {
                case core::parameter::ComputedRef::Algorithm::kGlobalAssembly:
                    if (au.GetParameters().GetNumberTemplateSegments() >= 2) {
                        return 4;
                    } else {
                        return 3;
                    }
                    break;
                case core::parameter::ComputedRef::Algorithm::kRefTransform:
                    UTILS_DIE("Ref Transform decoding not supported");
                    break;
                case core::parameter::ComputedRef::Algorithm::kLocalAssembly:
                    return 1;
                case core::parameter::ComputedRef::Algorithm::kPushIn:
                    UTILS_DIE("Push in decoding not supported");
                    break;
                default:
                    UTILS_DIE("Unknown decoding not supported");
                    break;
            }
        } else {
            if (au.GetClassType() == core::record::ClassType::kClassU) {
                return 2;
            } else {
                return 0;
            }
        }
    });

    ret->AddQvCoder(genie::util::make_unique<genie::quality::calq::Decoder>());
    ret->SetQvSelector([](const genie::core::parameter::QualityValues& param, const std::vector<std::string>&,
                          const std::vector<uint64_t>&, genie::core::AccessUnit::Descriptor&) -> size_t {
        UTILS_DIE_IF(param.GetMode() != 1, "Unsupported QV decoding mode");
        return 0;
    });

    ret->AddNameCoder(genie::util::make_unique<genie::name::tokenizer::Decoder>());
    ret->SetNameSelector([](const genie::core::AccessUnit::Descriptor&) -> size_t { return 0; });

    ret->AddEntropyCoder(genie::util::make_unique<genie::entropy::gabac::Decoder>());
    ret->SetEntropyCoderSelector([](const genie::core::parameter::DescriptorSubSequenceCfg&,
                                    genie::core::AccessUnit::Descriptor&, bool) -> size_t { return 0; });

    ret->SetExporterSelector([](const genie::core::record::Chunk&) -> size_t { return 0; });

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<core::FlowGraphConvert> build_default_converter(size_t threads) {
    std::unique_ptr<core::FlowGraphConvert> ret = genie::util::make_unique<core::FlowGraphConvert>(threads);

    ret->SetExporterSelector([](const genie::core::record::Chunk&) -> size_t { return 0; });

    ret->SetClassifier(genie::util::make_unique<genie::core::ClassifierBypass>());

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace module
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
