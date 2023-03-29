/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_VARIANT_GENOTYPE_H_
#define SRC_GENIE_CORE_RECORD_VARIANT_GENOTYPE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace variant_genotype {

/**
 *  @brief
 */
class info_field {
 private:
    uint8_t info_tag_len;              //!< @brief
    std::string info_tag;              //!< @brief
    uint8_t info_type;                 //!< @brief
    uint8_t info_array_len;            //!< @brief
    std::vector<uint8_t> info_value;   //!< @brief type??
};

/**
 *  @brief
 */

/**
 *  @brief
 */
class linked_record {
 public:
    uint8_t link_name_len;     //!< @brief
    std::string link_name;     //!< @brief
    uint8_t reference_box_ID;  //!< @brief
};

class FormatField {
 public:
    uint8_t len;
    std::string format;
    uint8_t type;
    uint8_t array_len;
    std::vector<std::vector<std::vector<uint8_t>>> value;
};

/**
 *  @brief
 */
class Record {
 private:
    util::BitReader& reader;

    uint64_t variant_index;             //!< @brief
    uint32_t sample_index_from;         //!< @brief
    uint32_t sample_count;              //!< @brief

    uint8_t format_count;               //!< @brief
    std::vector<FormatField> format;    //!< @brief

    uint8_t genotype_present;
    uint8_t likelihood_present;

    uint8_t n_alleles_per_sample;
    std::vector<std::vector<uint8_t>> alleles;
    std::vector<std::vector<uint8_t>> phasing;

    uint8_t n_likelihoods;
    std::vector<std::vector<float>> likelihoods; // type?

    uint8_t linked_record;
    uint8_t link_name_len;
    std::string link_name;
    uint8_t reference_box_ID;

    uint8_t determineSize(uint8_t selectType) const;
    
    std::vector<uint8_t> convertTypeToArray(uint8_t type);
    std::string convertArrayToString(uint8_t type, std::vector<uint8_t> value) const;



 public:
    /**
     * @brief
     */
    Record(util::BitReader& reader) ;
    void write(std::ostream& outputfile) const;

};


// ---------------------------------------------------------------------------------------------------------------------

}  // namespace variant_genotype
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_VARIANT_GENOTYPE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
