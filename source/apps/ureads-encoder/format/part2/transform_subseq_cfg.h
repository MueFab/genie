#ifndef GENIE_TRANSFORM_SUBSEQ_CFG_H
#define GENIE_TRANSFORM_SUBSEQ_CFG_H

#include <memory>

#include "support_values.h"
#include "cabac_binarization.h"

/**
 * ISO 23092-2 Section 8.3.1 table lines 9 to 11
 */
class Transform_subseq_cfg {
public:
    Transform_subseq_cfg();

    Transform_subseq_cfg(Support_values::Transform_ID_subsym _transform_ID_subsym,
                         std::unique_ptr<Support_values> _support_values,
                         std::unique_ptr<Cabac_binarization> _cabac_binarization);

    virtual void write(BitWriter *writer) const;

    std::unique_ptr<Transform_subseq_cfg> clone() const;

private:
    Support_values::Transform_ID_subsym transform_ID_subsym; //!< : 3; Line 9
    std::unique_ptr<Support_values> support_values; //!< Line 10
    std::unique_ptr<Cabac_binarization> cabac_binarization; //!< Line 11
};

#endif //GENIE_TRANSFORM_SUBSEQ_CFG_H
