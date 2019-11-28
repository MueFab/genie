#include "alignment.h"

namespace format {
    namespace mpegg_rec {
        Alignment::Alignment(
                std::unique_ptr<std::string> _ecigar_string,
                uint8_t _reverse_comp
        ) : ecigar_len(_ecigar_string->length()),
            ecigar_string(std::move(_ecigar_string)),
            reverse_comp(_reverse_comp),
            mapping_score() {

        }

        size_t Alignment::getAsDepth() const {
            return mapping_score.size();
        }

        void Alignment::addMappingScore(int32_t score) {
            mapping_score.push_back(score);
        }
    }
}