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
class alt_allele {
    uint32_t alt_len;   //!< @brief
    std::string alt;    //!< @brief
};

/**
 *  @brief
 */
class linked_record {
    uint8_t link_name_len;     //!< @brief
    std::string link_name;     //!< @brief
    uint8_t reference_box_ID;  //!< @brief
};

/**
 *  @brief
 */
class Record {
 private:
    // TODO: To be filled
    uint64_t variant_index;             //!< @brief
    uint32_t sample_index_from;         //!< @brief
    uint32_t sample_cout;               //!< @brief

    uint8_t format_count;               //!< @brief
    std::vector<uint8_t> format_len{};  //!< @brief

    uint8_t ID_len;                     //!< @brief
    std::string ID;                     //!< @brief
    uint8_t description_len;            //!< @brief
    std::string description;            //!< @brief
    uint32_t ref_len;                   //!< @brief
    std::string ref;                    //!< @brief

    uint8_t alt_count;                  //!< @brief
    std::vector<alt_allele> alt;        //!< @brief

    float depth;                        //!< @brief
    float seq_qual;                     //!< @brief
    float map_qual;                     //!< @brief
    float map_num_qual_0;               //!< @brief

    uint8_t filters_len;                //!< @brief
    std::string filters;                //!< @brief

    uint8_t info_count;                 //!< @brief
    std::vector<info_field> infoFields; //!< @brief

    uint8_t linked_record_en;           //!< @brief
    linked_record linked_record;        //!< @brief


 public:
    /**
     * @brief
     */
    Record();


};

#include "../../../util/bitreader.h"
#include <istream>

class ParseMrecs {
 public:
    void parse_mpegg_genotype(const char* inputfile, Record& mpegg_genotype);   

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
