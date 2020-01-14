#include "parameter_set_crps.h"
#include "util/bitwriter.h"
#include "util/exceptions.h"
#include "util/make_unique.h"

// -----------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_p2 {
ParameterSetCrps::ParameterSetCrps(CrAlgId _cr_alg_ID) : cr_alg_ID(_cr_alg_ID) {
    if (cr_alg_ID == CrAlgId::PUSH_IN || cr_alg_ID == CrAlgId::LOCAL_ASSEMBLY) {
        crps_info = util::make_unique<CrpsInfo>();
    }
}

ParameterSetCrps::ParameterSetCrps(util::BitReader &reader) {
cr_alg_ID = CrAlgId(reader.read(8));
if (cr_alg_ID == CrAlgId::PUSH_IN || cr_alg_ID == CrAlgId::LOCAL_ASSEMBLY) {
auto pad = reader.read(8);
auto buffer = reader.read(24);
crps_info = util::make_unique<CrpsInfo>(pad, buffer);
}
}

// -----------------------------------------------------------------------------------------------------------------

void ParameterSetCrps::setCrpsInfo(std::unique_ptr<CrpsInfo> _crps_info) {
    if (!crps_info) {
        UTILS_THROW_RUNTIME_EXCEPTION("Invalid crps mode for crps info");
    }
    crps_info = std::move(_crps_info);
}
// -----------------------------------------------------------------------------------------------------------------

const CrpsInfo &ParameterSetCrps::getCrpsInfo() const { return *crps_info; }

void ParameterSetCrps::write(util::BitWriter &writer) const {
    writer.write(uint8_t(cr_alg_ID), 8);
    if (crps_info) {
        crps_info->write(writer);
    }
}

std::unique_ptr<ParameterSetCrps> ParameterSetCrps::clone() const {
    auto ret = util::make_unique<ParameterSetCrps>(cr_alg_ID);
    ret->setCrpsInfo(crps_info->clone());
    return ret;
}

}  // namespace mpegg_p2
}  // namespace genie
