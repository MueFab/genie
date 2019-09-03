#include "bitwriter.h"
#include "make_unique.h"
#include "cabac_binarization_parameters.h"

void Cabac_binarization_parameters::write(BitWriter *writer) const {
    if (cmax) {
        writer->write(*cmax, 8);
    }
    if (cmax_teg) {
        writer->write(*cmax_teg, 8);
    }
    if (cmax_dtu) {
        writer->write(*cmax_dtu, 8);
    }
    if (split_unit_size) {
        writer->write(*split_unit_size, 4);
    }
}

Cabac_binarization_parameters::Cabac_binarization_parameters() : Cabac_binarization_parameters(
        Binarization_ID::Binary_Coding, 0) {

}

Cabac_binarization_parameters::Cabac_binarization_parameters(
        const Binarization_ID &_binarization_id,
        uint8_t param
) : cmax(nullptr),
    cmax_teg(nullptr),
    cmax_dtu(nullptr),
    split_unit_size(nullptr) {

    switch (_binarization_id) {
        case Binarization_ID::Truncated_Unary:
            cmax = make_unique<uint8_t>(param);
            break;
        case Binarization_ID::Truncated_Exponential_Golomb:
        case Binarization_ID::Signed_Truncated_Exponential_Golomb:
            cmax_teg = make_unique<uint8_t>(param);
            break;
        case Binarization_ID::Binary_Coding:
        case Binarization_ID::Exponential_Golomb:
        case Binarization_ID::Signed_Exponential_Gomb:
            break;
        default:
            GENIE_THROW_RUNTIME_EXCEPTION("Binarization not supported");
    }

}
std::unique_ptr<Cabac_binarization_parameters> Cabac_binarization_parameters::clone() const{
    auto r = make_unique<Cabac_binarization_parameters>();
    r->cmax = make_unique<uint8_t>(*cmax);
    r->cmax_teg = make_unique<uint8_t>(*cmax_teg);
    r->cmax_dtu = make_unique<uint8_t>(*cmax_dtu);
    r->split_unit_size = make_unique<uint8_t>(*split_unit_size);
    return r;
}