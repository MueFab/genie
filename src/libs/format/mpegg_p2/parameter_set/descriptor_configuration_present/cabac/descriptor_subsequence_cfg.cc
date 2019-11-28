#include "descriptor_subsequence_cfg.h"
#include "../../../make_unique.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace desc_conf_pres {
namespace cabac {
std::unique_ptr<DescriptorSubsequenceCfg> DescriptorSubsequenceCfg::clone() const {
    auto ret = make_unique<DescriptorSubsequenceCfg>(transform_subseq_parameters->clone(), 0, false);
    if (descriptor_subsequence_ID) {
        ret->descriptor_subsequence_ID = make_unique<uint16_t>(*descriptor_subsequence_ID);
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
        descriptor_subsequence_ID = make_unique<uint16_t>(_descriptor_subsequence_ID);
    }
}

// -----------------------------------------------------------------------------------------------------------------

void DescriptorSubsequenceCfg::setTransformSubseqCfg(size_t index,
                                                     std::unique_ptr<TransformSubseqCfg> _transformSubseq_cfg) {
    transformSubseq_cfgs[index] = std::move(_transformSubseq_cfg);
}

// -----------------------------------------------------------------------------------------------------------------

TransformSubseqCfg *DescriptorSubsequenceCfg::getTransformSubseqCfg(size_t index) const {
    return transformSubseq_cfgs[index].get();
}

// -----------------------------------------------------------------------------------------------------------------

void DescriptorSubsequenceCfg::write(util::BitWriter *writer) const {
    if (descriptor_subsequence_ID) {
        writer->write(*descriptor_subsequence_ID, 10);
    }
    transform_subseq_parameters->write(writer);
    for (auto &i : transformSubseq_cfgs) {
        i->write(writer);
    }
}
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace format
