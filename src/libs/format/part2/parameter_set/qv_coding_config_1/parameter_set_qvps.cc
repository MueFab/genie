#include "parameter_set_qvps.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
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

void ParameterSetQvps::write(util::BitWriter *writer) const {
    // TODO
    (void)writer;  // Silence warning about unused variable
}

}  // namespace qv_coding1
}  // namespace format
