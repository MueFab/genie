#include "access_unit.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

AccessUnit::AccessUnit(uint32_t _access_unit_ID, uint8_t _parameter_set_ID, core::record::ClassType _au_type,
                                 uint32_t _reads_count, core::parameter::DataUnit::DatasetType dataset_type,
                                 uint8_t posSize, uint8_t signatureSize, uint32_t multiple_signature_base)
    : DataUnit(DataUnitType::ACCESS_UNIT),
      access_unit_ID(_access_unit_ID),
      num_blocks(0),
      parameter_set_ID(_parameter_set_ID),
      au_type(_au_type),
      reads_count(_reads_count) {

    if (au_type == core::record::ClassType::CLASS_N || au_type == core::record::ClassType::CLASS_M) {
        mm_cfg = util::make_unique<mgb::MmCfg>();
    }
    if (dataset_type == DatasetType::REFERENCE) {
        ref_cfg = util::make_unique<mgb::RefCfg>(posSize);
    }
    if (au_type != core::record::ClassType::CLASS_U) {
        au_Type_U_Cfg = util::make_unique<mgb::AuTypeCfg>(posSize);
    } else {
        if (multiple_signature_base != 0) {
            signature_config = util::make_unique<mgb::SignatureCfg>(0, signatureSize);
        }
    }
}

AccessUnit::AccessUnit(mgb::AccessUnit&& au)
    : DataUnit(DataUnitType::ACCESS_UNIT),
      access_unit_ID(au.getID()),
      num_blocks(0),
      parameter_set_ID(au.getParameterID()),
      au_type(au.getClass()),
      reads_count(au.getReadCount()) {


}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie