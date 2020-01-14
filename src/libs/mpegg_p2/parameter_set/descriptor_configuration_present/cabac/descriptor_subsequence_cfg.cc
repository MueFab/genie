#include "descriptor_subsequence_cfg.h"
#include "util/bitwriter.h"
#include "util/make_unique.h"

// -----------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_p2 {
namespace desc_conf_pres {
namespace cabac {
std::unique_ptr<DescriptorSubsequenceCfg> DescriptorSubsequenceCfg::clone() const {
    auto ret = util::make_unique<DescriptorSubsequenceCfg>(transform_subseq_parameters->clone(), 0, false);
    if (descriptor_subsequence_ID) {
        ret->descriptor_subsequence_ID = util::make_unique<uint16_t>(*descriptor_subsequence_ID);
    } else {
        ret->descriptor_subsequence_ID = nullptr;
    }
    ret->transform_subseq_parameters = transform_subseq_parameters->clone();
    ret->transformSubseq_cfgs.clear();
    for (const auto &c : transformSubseq_cfgs) {
        ret->transformSubseq_cfgs.push_back(c->clone());
    }
    return ret;
}

// -----------------------------------------------------------------------------------------------------------------

DescriptorSubsequenceCfg::DescriptorSubsequenceCfg(
    std::unique_ptr<TransformSubseqParameters> _transform_subseq_parameters, uint16_t _descriptor_subsequence_ID,
    bool tokentype)
    : descriptor_subsequence_ID(nullptr),
      transform_subseq_parameters(std::move(_transform_subseq_parameters)),
      transformSubseq_cfgs(transform_subseq_parameters->getNumStreams()) {
    if (!tokentype) {
        descriptor_subsequence_ID = util::make_unique<uint16_t>(_descriptor_subsequence_ID);
    }
}

DescriptorSubsequenceCfg::DescriptorSubsequenceCfg(bool tokentype, util::BitReader& reader) {
    if (!tokentype) {
        descriptor_subsequence_ID = util::make_unique<uint16_t>(reader.read(10));
    }
    transform_subseq_parameters = util::make_unique<TransformSubseqParameters>(reader);
    uint8_t numSubseq = 0;
    switch (transform_subseq_parameters->getTransformIdSubseq()) {
        case TransformSubseqParameters::TransformIdSubseq::NO_TRANSFORM:
            numSubseq = 1;
            break;
        case TransformSubseqParameters::TransformIdSubseq::EQUALITY_CODING:
            numSubseq = 2;
            break;
        case TransformSubseqParameters::TransformIdSubseq::RLE_CODING:
            numSubseq = 2;
            break;
        case TransformSubseqParameters::TransformIdSubseq::MATCH_CODING:
            numSubseq = 3;
            break;
        case TransformSubseqParameters::TransformIdSubseq::MERGE_CODING:
            UTILS_DIE("MERGE backbone unsupported");
            break;
    }
    for (size_t i = 0; i < numSubseq; ++i) {
        transformSubseq_cfgs.emplace_back(util::make_unique<TransformSubseqCfg>(reader));
    }
}

// -----------------------------------------------------------------------------------------------------------------

void DescriptorSubsequenceCfg::setTransformSubseqCfg(size_t index,
                                                     std::unique_ptr<TransformSubseqCfg> _transformSubseq_cfg) {
    transformSubseq_cfgs[index] = std::move(_transformSubseq_cfg);
}

// -----------------------------------------------------------------------------------------------------------------

void DescriptorSubsequenceCfg::write(util::BitWriter &writer) const {
    if (descriptor_subsequence_ID) {
        writer.write(*descriptor_subsequence_ID, 10);
    }
    transform_subseq_parameters->write(writer);
    for (auto &i : transformSubseq_cfgs) {
        i->write(writer);
    }
}

uint16_t DescriptorSubsequenceCfg::getDescriptorSubsequenceID() const {
    if (!descriptor_subsequence_ID) {
        UTILS_DIE("descriptor_subsequence_ID not present");
    }
    return *descriptor_subsequence_ID.get();
};

const TransformSubseqParameters* DescriptorSubsequenceCfg::getTransformParameters() const { return transform_subseq_parameters.get(); }

const TransformSubseqCfg* DescriptorSubsequenceCfg::getTransformSubseqCfg(uint8_t index) const { return transformSubseq_cfgs[index].get(); }

size_t DescriptorSubsequenceCfg::getNumTransformSubseqCfgs() const { return transformSubseq_cfgs.size(); }

const std::vector<std::unique_ptr<TransformSubseqCfg>>& DescriptorSubsequenceCfg::getTransformSubseqCfgs() const {
    return transformSubseq_cfgs;
}
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace genie