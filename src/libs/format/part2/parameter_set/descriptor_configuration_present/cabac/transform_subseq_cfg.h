#ifndef GENIE_TRANSFORM_SUBSEQ_CFG_H
#define GENIE_TRANSFORM_SUBSEQ_CFG_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>

// -----------------------------------------------------------------------------------------------------------------

#include "cabac_binarization.h"
#include "support_values.h"
#include "util/bitwriter.h"


// -----------------------------------------------------------------------------------------------------------------

namespace format {
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

    virtual ~TransformSubseqCfg() = default;

    virtual void write(util::BitWriter *writer) const;

    std::unique_ptr<TransformSubseqCfg> clone() const;

   private:
    SupportValues::TransformIdSubsym transform_ID_subsym;   //!< : 3; Line 9
    std::unique_ptr<SupportValues> support_values;          //!< Line 10
    std::unique_ptr<CabacBinarization> cabac_binarization;  //!< Line 11
};

}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_TRANSFORM_SUBSEQ_CFG_H
