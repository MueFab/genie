#include "bitwriter.h"
#include "cabac_binarization.h"
#include "make_unique.h"

Cabac_binarization::Cabac_binarization(
        const Cabac_binarization_parameters::Binarization_ID &_binarization_ID,
        std::unique_ptr<Cabac_binarization_parameters> _cabac_binarization_parameters
) : binarization_ID(_binarization_ID),
    bypass_flag(true),
    cabac_binarization_parameters(std::move(_cabac_binarization_parameters)),
    cabac_context_parameters(nullptr) {
}

Cabac_binarization::Cabac_binarization() : Cabac_binarization(
        Cabac_binarization_parameters::Binarization_ID::Binary_Coding,
        make_unique<Cabac_binarization_parameters>(Cabac_binarization_parameters::Binarization_ID::Binary_Coding, 0)
) {

}

std::unique_ptr<Cabac_binarization> Cabac_binarization::clone() const {
    auto ret = make_unique<Cabac_binarization>();
    ret->binarization_ID = binarization_ID;
    ret->bypass_flag = bypass_flag;
    ret->cabac_binarization_parameters = cabac_binarization_parameters->clone();
    ret->cabac_context_parameters = cabac_context_parameters->clone();
    return ret;
}

void Cabac_binarization::setContextParameters(std::unique_ptr<Cabac_context_parameters> _cabac_context_parameters) {
    bypass_flag = false;
    cabac_context_parameters = std::move(_cabac_context_parameters);
}

void Cabac_binarization::write(BitWriter *writer) const {
    writer->write(uint8_t(binarization_ID), 5);
    writer->write(bypass_flag, 1);
    cabac_binarization_parameters->write(writer);
    if (cabac_context_parameters) {
        cabac_context_parameters->write(writer);
    }
}