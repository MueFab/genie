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
namespace genie {
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

    TransformSubseqCfg(util::BitReader& reader);

    virtual ~TransformSubseqCfg() = default;

    virtual void write(util::BitWriter& writer) const;

    std::unique_ptr<TransformSubseqCfg> clone() const;

    SupportValues::TransformIdSubsym getTransformID() const;

    const SupportValues* getSupportValues() const;

    const CabacBinarization* getBinarization() const;

   private:
    SupportValues::TransformIdSubsym transform_ID_subsym;   //!< : 3; Line 9
    std::unique_ptr<SupportValues> support_values;          //!< Line 10
    std::unique_ptr<CabacBinarization> cabac_binarization;  //!< Line 11
};
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace mpegg_p2
}  // namespace genie
// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_TRANSFORM_SUBSEQ_CFG_H
