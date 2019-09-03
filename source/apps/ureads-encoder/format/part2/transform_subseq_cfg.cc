#include "transform_subseq_cfg.h"
#include "make_unique.h"
#include "bitwriter.h"

Transform_subseq_cfg::Transform_subseq_cfg() : Transform_subseq_cfg(Support_values::Transform_ID_subsym::no_transform,
                                                                    make_unique<Support_values>(),
                                                                    make_unique<Cabac_binarization>()) {

}

Transform_subseq_cfg::Transform_subseq_cfg(Support_values::Transform_ID_subsym _transform_ID_subsym,
                                           std::unique_ptr<Support_values> _support_values,
                                           std::unique_ptr<Cabac_binarization> _cabac_binarization)
        : transform_ID_subsym(_transform_ID_subsym),
          support_values(std::move(_support_values)),
          cabac_binarization(std::move(_cabac_binarization)) {
}

std::unique_ptr<Transform_subseq_cfg> Transform_subseq_cfg::clone() const {
    auto ret = make_unique<Transform_subseq_cfg>();
    ret->transform_ID_subsym = transform_ID_subsym;
    ret->support_values = support_values->clone();
    ret->cabac_binarization = cabac_binarization->clone();
    return ret;
}

void Transform_subseq_cfg::write(BitWriter *writer) const {
    writer->write(uint8_t(transform_ID_subsym), 3);
    support_values->write(writer);
    cabac_binarization->write(writer);
}