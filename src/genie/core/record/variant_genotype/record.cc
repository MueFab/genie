/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

#include "record.h"
#include "arrayType.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_genotype {

uint8_t Record::determineSize(uint8_t selectType) const {
    if (selectType == 0) return 0;
    if (selectType < 5) return 1;
    if (selectType < 7) return 2;
    if (selectType < 9 || selectType == 11) return 4;
    return 4;
}

std::vector<uint8_t> Record::convertTypeToArray(uint8_t type) {
    std::vector<uint8_t> byteArray;
    switch (type) {
        case 1:
        case 2:
        case 4:
            byteArray.push_back(reader.readBypassBE<uint8_t>());
            break;
        case 3:
            byteArray.push_back(static_cast<uint8_t>(reader.readBypassBE<int8_t>()));
            break;
        case 5: {
            int16_t temp;
            temp = reader.readBypassBE<int16_t>();
            byteArray.resize(2);
            memcpy(&byteArray[0], &temp, 2);
            break;
        }
        case 6: {
            uint16_t temp;
            temp = reader.readBypassBE<uint16_t>();
            byteArray.resize(2);
            memcpy(&byteArray[0], &temp, 2);
            break;
        }
        case 7: {
            int32_t temp;
            temp = reader.readBypassBE<int32_t>();
            byteArray.resize(4);
            memcpy(&byteArray[0], &temp, 4);
            break;
        }
        case 8: {
            uint32_t temp;
            temp = reader.readBypassBE<uint32_t>();
            byteArray.resize(4);
            memcpy(&byteArray[0], &temp, 4);
            break;
        }
        case 9: {
            int64_t temp;
            temp = reader.readBypassBE<int64_t>();
            byteArray.resize(8);
            memcpy(&byteArray[0], &temp, 8);
            break;
        }
        case 10: {
            uint64_t temp;
            temp = reader.readBypassBE<uint64_t>();
            byteArray.resize(8);
            memcpy(&byteArray[0], &temp, 8);
            break;
        }
        case 11: {
            float temp;
            temp = reader.readBypassBE<float>();
            byteArray.resize(4);
            memcpy(&byteArray[0], &temp, 4);
            break;
        }
        default:  // csae 0
            uint8_t read = 0xF;
            do {
                read = reader.readBypassBE<uint8_t>();
                if (read != 0) byteArray.push_back(read);
            } while (read != 0);
            break;
    }

    return byteArray;
}

std::string Record::convertArrayToString(uint8_t type, std::vector<uint8_t> bytearray) const {
    std::string temp;

    switch (type) {
        case 1:
        case 2:
        case 4:
            temp = std::to_string(bytearray[0]);
            break;
        case 3: {
            int8_t value;
            memcpy(&value, &bytearray[0], 1);
            temp = std::to_string(value);
            break;
        }
        case 5: {
            int16_t value;
            memcpy(&value, &bytearray[0], 2);
            temp = std::to_string(value);
            break;
        }
        case 6: {
            uint16_t value;
            memcpy(&value, &bytearray[0], 2);
            temp = std::to_string(value);
            break;
        }
        case 7: {
            int32_t value;
            memcpy(&value, &bytearray[0], 4);
            temp = std::to_string(value);
            break;
        }
        case 8: {
            uint32_t value;
            memcpy(&value, &bytearray[0], 4);
            temp = std::to_string(value);
            break;
        }
        case 9: {
            int64_t value;
            memcpy(&value, &bytearray[0], 8);
            temp = std::to_string(value);
            break;
        }
        case 10: {
            uint64_t value;
            memcpy(&value, &bytearray[0], 8);
            temp = std::to_string(value);
            break;
        }
        case 11: {
            float value;
            memcpy(&value, &bytearray[0], 4);
            temp = std::to_string(value);
            break;
        }
        default:  // csae 0
            temp.resize(bytearray.size());
            for (auto i = 0; i < bytearray.size(); ++i) temp += bytearray[i];
            break;
    }
    return temp;
}

Record::Record(util::BitReader& reader) : reader(reader) {
    variant_index = (reader.readBypassBE<uint64_t>());
    sample_index_from = (reader.readBypassBE<uint32_t>());
    sample_count = (reader.readBypassBE<uint32_t>());

    arrayType curType;

    format_count = (reader.readBypassBE<uint8_t>());
    if (format_count > 0) {
        format.resize(format_count);
        for (auto& format_item : format) {
            format_item.len = (reader.readBypassBE<uint8_t>());
            format_item.format.resize(format_item.len);
            reader.readBypass(&format_item.format[0], format_item.len);
            format_item.type = (reader.readBypassBE<uint8_t>());
            format_item.array_len = (reader.readBypassBE<uint8_t>());
            format_item.value.resize(sample_count);
            for (auto& values : format_item.value) {
                values.resize(format_item.array_len);
                for (auto& value : values) {
                    value = curType.toArray(format_item.type, reader);
                }
            }
        }
    }
    genotype_present = (reader.readBypassBE<uint8_t>());
    likelihood_present = (reader.readBypassBE<uint8_t>());

    n_alleles_per_sample = (reader.readBypassBE<uint8_t>());
    alleles.resize(sample_count, std::vector<uint8_t>(n_alleles_per_sample));
    phasing.resize(sample_count, std::vector<uint8_t>(n_alleles_per_sample-1));
    for (uint32_t i = 0; i < sample_count; ++i) {
        for (auto& allele : alleles[i]) {
            allele = (reader.readBypassBE<uint8_t>());
        }
        for (auto& phase : phasing[i]) {
            phase = (reader.readBypassBE<uint8_t>());
        }
    }
 
    n_likelihoods = (reader.readBypassBE<uint8_t>());
    likelihoods.resize(sample_count, std::vector<float>(n_likelihoods));
    for (auto& likelihood_sample : likelihoods) {
        for (auto& likelihood : likelihood_sample) {
            likelihood = (reader.readBypassBE<float>());
        }
    }

    linked_record = (reader.readBypassBE<uint8_t>());
    if (linked_record) {
        link_name_len = (reader.readBypassBE<uint8_t>());
        reader.readBypass(&link_name[0], link_name_len);
        reference_box_ID = (reader.readBypassBE<uint8_t>());
    }
}

void Record::write(std::ostream& outputfile) const {
    outputfile << std::to_string(variant_index) << ",";
    outputfile << std::to_string(sample_index_from) << ",";
    outputfile << std::to_string(sample_count) << ",";

    outputfile << std::to_string(format_count) << ",";

    arrayType curArray;

    for (auto format_item : format) {
        outputfile << std::to_string(format_item.len) << ",";
        outputfile << '"' << format_item.format << '"' << ",";
        outputfile << std::to_string(format_item.type) << ",";
        outputfile << std::to_string(format_item.array_len) << ",";
        for (auto values : format_item.value) {
            for (auto value : values) {
                outputfile << curArray.toString(format_item.type, value) << ",";
            }
        }
    }
    outputfile << std::to_string(genotype_present) << ",";
    outputfile << std::to_string(likelihood_present) << ",";

    outputfile << std::to_string(n_alleles_per_sample) << ",";

    for (auto allele_sample : alleles) {
        for (auto& allele : allele_sample) {
            outputfile << std::to_string(allele) << ",";
        }
    }

    for (auto& phasing_sample : phasing) {
        for (auto& phase : phasing_sample) {
            outputfile << std::to_string(phase) << ",";
        }
    }

    outputfile << std::to_string(n_likelihoods) << ",";

    for (auto likelihood_sample : likelihoods) {
        for (auto likelihood : likelihood_sample) {
            outputfile << std::to_string(likelihood) << ",";
        }
    }

    outputfile << std::to_string(linked_record) << ",";
    if (linked_record) {
        outputfile << std::to_string(link_name_len) << ",";
        outputfile << '"' << link_name << '"' << ",";
        outputfile << std::to_string(reference_box_ID) << ",";
    }
}

}  // namespace variant_genotype
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
