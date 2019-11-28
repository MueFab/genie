#include "transform_subseq_cfg.h"
#include "../../../make_unique.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace desc_conf_pres {
namespace cabac {
TransformSubseqCfg::TransformSubseqCfg()
    : TransformSubseqCfg(SupportValues::TransformIdSubsym::NO_TRANSFORM, make_unique<SupportValues>(),
                         make_unique<CabacBinarization>()) {}

// -----------------------------------------------------------------------------------------------------------------

TransformSubseqCfg::TransformSubseqCfg(SupportValues::TransformIdSubsym _transform_ID_subsym,
                                       std::unique_ptr<SupportValues> _support_values,
                                       std::unique_ptr<CabacBinarization> _cabac_binarization)
    : transform_ID_subsym(_transform_ID_subsym),
      support_values(std::move(_support_values)),
      cabac_binarization(std::move(_cabac_binarization)) {}

// -----------------------------------------------------------------------------------------------------------------

std::unique_ptr<TransformSubseqCfg> TransformSubseqCfg::clone() const {
    auto ret = make_unique<TransformSubseqCfg>();
    ret->transform_ID_subsym = transform_ID_subsym;
    ret->support_values = support_values->clone();
    ret->cabac_binarization = cabac_binarization->clone();
    return ret;
}

// -----------------------------------------------------------------------------------------------------------------

void TransformSubseqCfg::write(util::BitWriter *writer) const {
    writer->write(uint8_t(transform_ID_subsym), 3);
    support_values->write(writer);
    cabac_binarization->write(writer);
}
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace format
