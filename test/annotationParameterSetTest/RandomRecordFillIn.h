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
#include "genie/core/record/annotation_parameter_set/record.h"

// ---------------------------------------------------------------------------------------------------------------------

class RandomAnnotationEncodingParameters {
 public:
    genie::core::record::annotation_parameter_set::LikelihoodParameters randomLikelihood();

    genie::core::record::annotation_parameter_set::GenotypeParameters randomGenotypeParameters();

    genie::core::record::annotation_parameter_set::AttributeParameterSet randomAttributeParameterSet();

    genie::core::record::annotation_parameter_set::AlgorithmParameters randomAlgorithmParameters();
    genie::core::record::annotation_parameter_set::AlgorithmParameters randomAlgorithmParameters(
        uint8_t n_pars, std::vector<uint8_t> parNumArrayDims);

    genie::core::record::annotation_parameter_set::TileStructure randomTileStructure();

    genie::core::record::annotation_parameter_set::TileStructure randomTileStructure(uint8_t ATCoordSize,
                                                                                     bool two_dimensional);

    genie::core::record::annotation_parameter_set::TileStructure simpleTileStructure(uint8_t ATCoordSize,
                                                                                     bool two_dimensional);

    genie::core::record::annotation_parameter_set::TileConfiguration randomTileConfiguration();
    genie::core::record::annotation_parameter_set::TileConfiguration randomTileConfiguration(uint8_t ATCoordSize);
    genie::core::record::annotation_parameter_set::ContactMatrixParameters randomContactMatrixParameters();
    genie::core::record::annotation_parameter_set::ContactMatrixParameters simpleContactMatrixParameters();
    genie::core::record::annotation_parameter_set::DescriptorConfiguration randomDescriptorConfiguration();
    genie::core::record::annotation_parameter_set::CompressorParameterSet randomCompressorParameterSet();
    genie::core::record::annotation_parameter_set::AnnotationEncodingParameters randomAnnotationEncodingParameters();
    genie::core::record::annotation_parameter_set::Record randomAnnotationParameterSet();

 private:
    uint64_t randomU64() {
        uint64_t randomnumber = static_cast<uint64_t>(randomU32()) * static_cast<uint64_t>(randomU32());
        return randomnumber;
    }
    uint32_t randomU32() {
        uint32_t randomnumber = randomU16() * randomU16();
        return randomnumber;
    }
    uint16_t randomU16() {
        uint16_t randomnumber = static_cast<uint16_t>(rand() + rand());
        return randomnumber;
    }
    uint16_t randomU10() { return randomU16() % 1024; }

    uint8_t randomU8() { return static_cast<uint8_t>(rand() % 256); }
    uint8_t randomU7() { return static_cast<uint8_t>(rand() % 128); }
    uint8_t randomU6() { return static_cast<uint8_t>(rand() % 64); }

    uint8_t randomU4() { return static_cast<uint8_t>((rand() % 16)); }
    uint8_t randomU3() { return static_cast<uint8_t>(rand() % 8); }
    uint8_t randomU2() { return static_cast<uint8_t>(rand() % 4); }
    bool randomBool() { return static_cast<bool>(rand() % 2); }

    uint8_t randomType() {
        uint8_t Type = static_cast<uint8_t>(rand() % 13);
        // replace all signed types
        if (Type == 3) Type = 4;
        if (Type == 5) Type = 6;
        if (Type == 7) Type = 8;
        if (Type == 9) Type = 10;
        return Type;
    }

    std::string randomString(uint16_t len) {
        std::string random = "";
        for (auto i = 0; i < len; ++i) {
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
        if (size == 1)
            randomVal.push_back(static_cast<uint8_t>(randomBool()));
        else if (size == 0) {
            uint8_t randomstrlen = randomU4() + 3;
            randomVal.resize(randomstrlen, 0);
            std::string randomval = randomString(randomstrlen - 1);
            for (auto i = 0; i < randomstrlen - 1; ++i) randomVal[i] = randomval[i];
        } else {
            randomVal.resize(size / 8);
            for (auto& byte : randomVal) byte = randomU8();
        }
        return randomVal;
    }

    uint64_t randomAtCoordSize(uint8_t ATCoordSize) {
        if (ATCoordSize == 0) return randomU8();
        if (ATCoordSize == 1) return randomU16();
        if (ATCoordSize == 2) return randomU32();
        if (ATCoordSize == 3) return randomU64();
        return 0;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_TEST_RANDOMRECORDFILLIN_H_

// ---------------------------------------------------------------------------------------------------------------------
