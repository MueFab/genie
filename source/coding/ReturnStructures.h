#ifndef GENIE_RETURNSTRUCTURES_H
#define GENIE_RETURNSTRUCTURES_H

#include <vector>
#include <map>
#include <string>

class generated_aus_ref{
    std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> refAus;
    std::vector<uint64_t> refStart;
    std::vector<uint64_t> refEnd;
public:
    generated_aus_ref(const std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> &refAus,
    const std::vector<uint64_t> &refStart, const std::vector<uint64_t> &refEnd);

    const std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> &getRefAus() const;

    const std::vector<uint64_t> &getRefStart() const;

    const std::vector<uint64_t> &getRefEnd() const;
};

class generated_aus{
    std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> encodedFastqAus;


public:
    generated_aus(const std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> &encodedFastqAus);

    const std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> &getEncodedFastqAus() const;

};

#endif //GENIE_RETURNSTRUCTURES_H
