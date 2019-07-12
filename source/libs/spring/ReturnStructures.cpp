#include "ReturnStructures.h"

generated_aus_ref::generated_aus_ref(const std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> &refAusLoc,
                                     const std::vector<uint64_t> &refStartLoc, const std::vector<uint64_t> &refEndLoc)
        : refAus(refAusLoc), refStart(refStartLoc), refEnd(refEndLoc) {}

const std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> &generated_aus_ref::getRefAus() const {
    return refAus;
}

const std::vector<uint64_t> &generated_aus_ref::getRefStart() const {
    return refStart;
}

const std::vector<uint64_t> &generated_aus_ref::getRefEnd() const {
    return refEnd;
}

generated_aus::generated_aus(const std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> &encodedFastqAusLoc)
        : encodedFastqAus(encodedFastqAusLoc) {}

const std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> &generated_aus::getEncodedFastqAus() const {
    return encodedFastqAus;
}
