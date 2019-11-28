#include "qv_coding_config.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
QvCodingConfig::QvCodingConfig(QvCodingMode _qv_coding_mode, bool _qv_reverse_flag)
    : qv_coding_mode(_qv_coding_mode), qv_reverse_flag(_qv_reverse_flag) {}
}  // namespace format