#include "reference.h"
#include "importer.h"
namespace genie {
namespace format {
namespace mgb {

std::string Reference::getSequence(uint64_t _start, uint64_t _end) {
    return mgr->getRef(raw, f_pos, _start, _end);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace format
}  // namespace genie
}  // namespace genie