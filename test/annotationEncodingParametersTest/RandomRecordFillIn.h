/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_TEST_RANDOMRECORDFILLIN_H_
#define SRC_TEST_RANDOMRECORDFILLIN_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/record/annotation_encoding_parameters/record.h"

// ---------------------------------------------------------------------------------------------------------------------

class RandomAnnotationEncodingParameters {
 public:
    genie::core::record::annotation_encoding_parameters::LikelihoodParameters randomLikelihood();

    genie::core::record::annotation_encoding_parameters::GenotypeParameters randomGenotypeParameters();

    genie::core::record::annotation_encoding_parameters::AttributeParameterSet randomAttributeParameterSet();

    genie::core::record::annotation_encoding_parameters::AlgorithmParameters randomAlgorithmParameters();

    uint8_t randomU4() { return static_cast<uint8_t>((rand() % 15)); }
 private:
    uint16_t randomU16() { return static_cast<uint16_t>(((rand() + rand()) % (65536 - 2)) + 1); }
    uint8_t randomU8() { return static_cast<uint8_t>((rand() % (254) + 1)); }
    uint8_t randomU2() { return static_cast<uint8_t>(rand() % 4); }
    uint8_t randomType() { return static_cast<uint8_t>(rand() % 13); }
    bool randomBool() { return static_cast<bool>(rand() % 2); }

    std::string randomString(uint8_t len) {
        std::string random = "rs";
        for (auto i = 2; i < len; ++i) {
            uint8_t byteSeed = rand() % (26);
            random += static_cast<char>(byteSeed + 'a');
        }
        return random;
    }

    uint8_t typeSize(uint8_t selectType) const {
        switch (selectType) {
            case 0:
                return 0;
            case 1:
            case 3:
            case 4:
                return 8;
            case 5:
            case 6:
                return 16;
            case 2:
                return 1;
            case 7:
            case 8:
            case 11:
                return 32;
            case 9:
            case 10:
            case 12:
                return 64;
            default:
                return 0;
        }
    }

    std::vector<uint8_t> randomValForType(uint8_t selectType) {
        uint8_t size = typeSize(selectType);
        std::vector<uint8_t> randomVal{};
        if (size == 2)
            randomVal.push_back(static_cast<uint8_t>(randomBool()));
        else if (size == 0) {
            uint8_t randomstrlen = randomU4() + 2;
            randomVal.resize(static_cast<uint8_t>(randomstrlen + 1), 0);
            std::string randomval = randomString(randomstrlen);
            for (auto i = 0; i < randomstrlen; ++i) randomVal[i] = randomval[i];
        } else {
            randomVal.resize(size / 8);
            for (auto& byte : randomVal) byte = randomU8();
        }
        return randomVal;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_TEST_RANDOMRECORDFILLIN_H_

// ---------------------------------------------------------------------------------------------------------------------
