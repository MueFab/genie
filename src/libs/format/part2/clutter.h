#ifndef GENIE_CLUTTER_H
#define GENIE_CLUTTER_H

#include <cstdint>
#include "access_unit.h"
#include "parameter_set.h"

namespace format {
enum class GenomicDescriptor : uint8_t {
    pos = 0,
    rcomp = 1,
    flags = 2,
    mmpos = 3,
    mmtype = 4,
    clips = 5,
    ureads = 6,
    rlen = 7,
    pair = 8,
    mscore = 9,
    mmap = 10,
    msar = 11,
    rtype = 12,
    rgroup = 13,
    qv = 14,
    rname = 15,
    rftp = 16,
    rftt = 17
};

struct GenomicDescriptorProperties {
    std::string name;
    uint8_t number_subsequences;
};
constexpr size_t NUM_DESCRIPTORS = 18;

struct Alphabet {
    std::vector<char> lut;
    std::vector<char> inverseLut;
};

const Alphabet &getAlphabetProperties(ParameterSet::AlphabetID id);

const std::vector<GenomicDescriptorProperties> &getDescriptorProperties();

/* ----------------------------------------------------------------------------------------------------------- */

ParameterSet createQuickParameterSet(uint8_t _parameter_set_id, uint8_t _read_length, bool paired_end,
                                         bool qv_values_present, DataUnit::AuType type,
                                         const std::vector<std::vector<gabac::EncodingConfiguration>> &parameters,
                                         bool reverse_flag);

AccessUnit createQuickAccessUnit(uint32_t access_unit_id, uint8_t parameter_set_id, uint32_t reads_count, DataUnit::AuType autype, DataUnit::DatasetType datatype,
                                 std::vector<std::vector<std::vector<gabac::DataBlock>>> *data, uint32_t records_count=0);

}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CLUTTER_H
