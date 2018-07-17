// Copyright 2018 The genie authors


/**
 *  @file SamRecord.h
 *  @brief SAM record implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */

#include "SamRecord.h"


namespace dsg {
namespace input {
namespace sam {


SamRecord::SamRecord(
    const std::vector<std::string>& fields)
    : qname(fields[0]),
      flag(static_cast<uint16_t>(std::stoi(fields[1]))),
      rname(fields[2]),
      pos((uint32_t)std::stoi(fields[3])),
      mapq((uint8_t)std::stoi(fields[4])),
      cigar(fields[5]),
      rnext(fields[6]),
      pnext((uint32_t)std::stoi(fields[7])),
      tlen((int64_t)std::stoi(fields[8])),
      seq(fields[9]),
      qual(fields[10]),
      opt(fields[11])
{
    // Nothing to do here.
}


SamRecord::~SamRecord(void)
{
    // Nothing to do here.
}


}  // namespace sam
}  // namespace input
}  // namespace dsg

