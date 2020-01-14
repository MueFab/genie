#include "parameter_set_qvps.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_p2 {
namespace qv_coding1 {
ParameterSetQvps::ParameterSetQvps() {
    // TODO
}

// -----------------------------------------------------------------------------------------------------------------

void ParameterSetQvps::addCodeBook(const QvCodebook &book) {
    // TODO
    (void)book;  // Silence warning about unused variable
}

// -----------------------------------------------------------------------------------------------------------------

void ParameterSetQvps::write(util::BitWriter &writer) const {
    // TODO
    (void)writer;  // Silence warning about unused variable
}

std::unique_ptr<ParameterSetQvps> ParameterSetQvps::clone() const {
    auto ret = util::make_unique<ParameterSetQvps>();
    ret->qv_codebooks = qv_codebooks;
    return ret;
}

}  // namespace qv_coding1
}  // namespace mpegg_p2
}  // namespace genie
