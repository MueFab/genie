#include "probability_distribution.h"

// -----------------------------------------------------------------------------

#include "error_exception_reporter.h"

// -----------------------------------------------------------------------------

namespace calq {

// -----------------------------------------------------------------------------

ProbabilityDistribution::ProbabilityDistribution(size_t rangeMin,
                                                 size_t rangeMax
){
    this->rangeMin = rangeMin;
    this->pdf.resize(rangeMax - rangeMin + 1);
}

// -----------------------------------------------------------------------------

void ProbabilityDistribution::addToPdf(size_t qualScore,
                                       size_t number
){
    if (qualScore < rangeMin || qualScore > rangeMin + pdf.size() - 1)
        throwErrorException("PDF: Score not in range");
    pdf[qualScore - rangeMin] += number;
}

// -----------------------------------------------------------------------------

void ProbabilityDistribution::resetPdf(){
    std::fill(pdf.begin(), pdf.end(), 0);
}

// -----------------------------------------------------------------------------

size_t ProbabilityDistribution::getCount(size_t value) const{
    if (value < rangeMin || value > rangeMin + pdf.size() - 1)
        throwErrorException("PDF: Value not in range");
    return value;
}

// -----------------------------------------------------------------------------

size_t ProbabilityDistribution::operator[](size_t index) const{
    if (index >= pdf.size())
        throwErrorException("PDF: Index not in range");
    return pdf[index];
}

// -----------------------------------------------------------------------------

size_t ProbabilityDistribution::size() const{
    return pdf.size();
}

// -----------------------------------------------------------------------------

size_t ProbabilityDistribution::getRangeMin() const{
    return rangeMin;
}

// -----------------------------------------------------------------------------

size_t ProbabilityDistribution::getRangeMax() const{
    return rangeMin + pdf.size() - 1;
}

// -----------------------------------------------------------------------------

}  // namespace calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
