#include "transform_subseq_cfg.h"
#include "util/bitwriter.h"
#include "util/make_unique.h"

// -----------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_p2 {
namespace desc_conf_pres {
namespace cabac {
TransformSubseqCfg::TransformSubseqCfg()
    : TransformSubseqCfg(SupportValues::TransformIdSubsym::NO_TRANSFORM, util::make_unique<SupportValues>(),
                         util::make_unique<CabacBinarization>()) {}

// -----------------------------------------------------------------------------------------------------------------

TransformSubseqCfg::TransformSubseqCfg(SupportValues::TransformIdSubsym _transform_ID_subsym,
                                       std::unique_ptr<SupportValues> _support_values,
                                       std::unique_ptr<CabacBinarization> _cabac_binarization)
    : transform_ID_subsym(_transform_ID_subsym),
      support_values(std::move(_support_values)),
      cabac_binarization(std::move(_cabac_binarization)) {}

TransformSubseqCfg::TransformSubseqCfg(util::BitReader& reader) {
    transform_ID_subsym = SupportValues::TransformIdSubsym(reader.read(3));
    support_values = util::make_unique<SupportValues>(transform_ID_subsym, reader);
    cabac_binarization = util::make_unique<CabacBinarization>(support_values->getCodingSubsymSize(),
                                                              support_values->getOutputSymbolSize(), reader);
}

// -----------------------------------------------------------------------------------------------------------------

std::unique_ptr<TransformSubseqCfg> TransformSubseqCfg::clone() const {
    auto ret = util::make_unique<TransformSubseqCfg>();
    ret->transform_ID_subsym = transform_ID_subsym;
    ret->support_values = support_values->clone();
    ret->cabac_binarization = cabac_binarization->clone();
    return ret;
}

SupportValues::TransformIdSubsym TransformSubseqCfg::getTransformID() const { return transform_ID_subsym; }

const SupportValues* TransformSubseqCfg::getSupportValues() const { return support_values.get(); }

const CabacBinarization* TransformSubseqCfg::getBinarization() const { return cabac_binarization.get(); }

// -----------------------------------------------------------------------------------------------------------------

void TransformSubseqCfg::write(util::BitWriter& writer) const {
    writer.write(uint8_t(transform_ID_subsym), 3);
    support_values->write(writer);
    cabac_binarization->write(writer);
}
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace genie