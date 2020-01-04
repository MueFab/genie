#ifndef GENIE_PARAMETER_SET_QVPS_H
#define GENIE_PARAMETER_SET_QVPS_H

// -----------------------------------------------------------------------------------------------------------------

#include <util/make_unique.h>
#include <cstdint>
#include <memory>
#include <vector>

#include "qv_codebook.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_p2 {
namespace qv_coding1 {

/**
 * ISO 23092-2 Section 3.3.2.2 table 9
 */
class ParameterSetQvps {
   private:
    // uint8_t qv_num_codebooks_total : 4;    //!< Line 2
    std::vector<QvCodebook> qv_codebooks;  //!< Lines 3 to 8

   public:
    ParameterSetQvps();

    virtual ~ParameterSetQvps() = default;

    void addCodeBook(const QvCodebook &book);

    virtual void write(util::BitWriter &writer) const;

    std::unique_ptr<ParameterSetQvps> clone() const {
        auto ret = util::make_unique<ParameterSetQvps>();
        ret->qv_codebooks = qv_codebooks;
        return ret;
    }
};
}  // namespace qv_coding1
}  // namespace mpegg_p2
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_PARAMETER_SET_QVPS_H
