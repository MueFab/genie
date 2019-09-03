#include "descriptor_subsequence_cfg.h"
#include "make_unique.h"
#include "bitwriter.h"

Descriptor_subsequence_cfg::Descriptor_subsequence_cfg() {

}

std::unique_ptr<Descriptor_subsequence_cfg> Descriptor_subsequence_cfg::clone() const {
    auto ret = make_unique<Descriptor_subsequence_cfg>();
    ret->descriptor_subsequence_ID = descriptor_subsequence_ID;
    ret->transform_subseq_parameters = transform_subseq_parameters->clone();
    for(const auto& c : transformSubseq_cfgs) {
        ret->transformSubseq_cfgs.push_back(c->clone());
    }
    return ret;
}

Descriptor_subsequence_cfg::Descriptor_subsequence_cfg(
        std::unique_ptr<Transform_subseq_parameters> _transform_subseq_parameters,
        uint16_t _descriptor_subsequence_ID
) : descriptor_subsequence_ID(_descriptor_subsequence_ID),
    transform_subseq_parameters(std::move(_transform_subseq_parameters)),
    transformSubseq_cfgs(0) {

}

void Descriptor_subsequence_cfg::addTransformSubseqCfg(std::unique_ptr<Transform_subseq_cfg> _transformSubseq_cfg) {
    transformSubseq_cfgs.push_back(std::move(_transformSubseq_cfg));
}

void Descriptor_subsequence_cfg::write(BitWriter *writer) const {
    writer->write(descriptor_subsequence_ID, 10);
    transform_subseq_parameters->write(writer);
    for (auto &i : transformSubseq_cfgs) {
        i->write(writer);
    }
}