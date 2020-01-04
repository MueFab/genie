#ifndef GENIE_TRANSFORM_SUBSEQ_CFG_H
#define GENIE_TRANSFORM_SUBSEQ_CFG_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>

// -----------------------------------------------------------------------------------------------------------------

#include "cabac_binarization.h"
#include "support_values.h"
#include "util/bitwriter.h"
#include "util/make_unique.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_p2 {
namespace desc_conf_pres {
namespace cabac {
/**
 * ISO 23092-2 Section 8.3.1 table lines 9 to 11
 */
class TransformSubseqCfg {
   public:
    TransformSubseqCfg();

    TransformSubseqCfg(SupportValues::TransformIdSubsym _transform_ID_subsym,
                       std::unique_ptr<SupportValues> _support_values,
                       std::unique_ptr<CabacBinarization> _cabac_binarization);

    TransformSubseqCfg(util::BitReader& reader) {
        transform_ID_subsym = SupportValues::TransformIdSubsym(reader.read(3));
        support_values = util::make_unique<SupportValues>(transform_ID_subsym, reader);
        cabac_binarization = util::make_unique<CabacBinarization>(support_values->getCodingSubsymSize(),
                                                                  support_values->getOutputSymbolSize(), reader);
    }

    virtual ~TransformSubseqCfg() = default;

    virtual void write(util::BitWriter& writer) const;

    std::unique_ptr<TransformSubseqCfg> clone() const;

    SupportValues::TransformIdSubsym getTransformID() const { return transform_ID_subsym; }

    const SupportValues* getSupportValues() const { return support_values.get(); }

    const CabacBinarization* getBinarization() const { return cabac_binarization.get(); }

   private:
    SupportValues::TransformIdSubsym transform_ID_subsym;   //!< : 3; Line 9
    std::unique_ptr<SupportValues> support_values;          //!< Line 10
    std::unique_ptr<CabacBinarization> cabac_binarization;  //!< Line 11
};
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_TRANSFORM_SUBSEQ_CFG_H
