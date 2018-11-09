#include "ReturnStructures.h"

generated_aus_ref::generated_aus_ref(const std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> &refAus,
const std::vector<uint64_t> &refStart, const std::vector<uint64_t> &refEnd)
: refAus(refAus), refStart(refStart), refEnd(refEnd) {}

const std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> &generated_aus_ref::getRefAus() const {
    return refAus;
}

const std::vector<uint64_t> &generated_aus_ref::getRefStart() const {
    return refStart;
}

const std::vector<uint64_t> &generated_aus_ref::getRefEnd() const {
    return refEnd;
}

generated_aus::generated_aus(const generated_aus_ref &generatedAusRef,
                             const std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> &encodedFastqAus)
: generatedAusRef(generatedAusRef), encodedFastqAus(encodedFastqAus) {}

const generated_aus_ref &generated_aus::getGeneratedAusRef() const {
    return generatedAusRef;
}

const std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> &generated_aus::getEncodedFastqAus() const {
    return encodedFastqAus;
}

