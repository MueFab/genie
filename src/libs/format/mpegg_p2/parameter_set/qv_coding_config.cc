#include "qv_coding_config.h"
#include "qv_coding_config_1/qv_coding_config_1.h"
// -----------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_p2 {
QvCodingConfig::QvCodingConfig(QvCodingMode _qv_coding_mode, bool _qv_reverse_flag)
    : qv_coding_mode(_qv_coding_mode), qv_reverse_flag(_qv_reverse_flag) {}
std::unique_ptr<QvCodingConfig> QvCodingConfig::factory(util::BitReader &reader) {
    auto qv_coding_mode = QvCodingMode(reader.read(4));
    switch (qv_coding_mode) {
        case QvCodingMode::ONE:
            return util::make_unique<qv_coding1::QvCodingConfig1>(reader);
        default:
            UTILS_DIE("Unsupported coding mode");
    }
}
}  // namespace mpegg_p2
}  // namespace format