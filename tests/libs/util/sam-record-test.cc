#include "calq/sam-record.h"
#include <gtest/gtest.h>

TEST(SamRecord, String) {  // NOLINT(cert-err58-cpp)
    std::vector<std::string> fields;

    fields.emplace_back("QNAME");
    fields.emplace_back("0");
    fields.emplace_back("RNAME");
    fields.emplace_back("0");
    fields.emplace_back("0");
    fields.emplace_back("CIGAR");
    fields.emplace_back("RNEXT");
    fields.emplace_back("0");
    fields.emplace_back("0");
    fields.emplace_back("SEQ");
    fields.emplace_back("QUAL");
    fields.emplace_back("OPT");

    calq::SamRecord record(fields);
}
