#include "sam-record.h"

#include <iostream>
#include <sstream>

namespace fio {

    SamRecord::SamRecord(const std::vector<std::string> &fields)
            : qname(fields[0]),
              flag(static_cast<uint16_t>(std::stoi(fields[1]))),
              rname(fields[2]),
              pos(static_cast<uint32_t>(std::stoi(fields[3]))),
              mapq(static_cast<uint8_t>(std::stoi(fields[4]))),
              cigar(fields[5]),
              rnext(fields[6]),
              pnext(static_cast<uint32_t>(std::stoi(fields[7]))),
              tlen(static_cast<int64_t>(std::stoi(fields[8]))),
              seq(fields[9]),
              qual(fields[10]),
              opt(fields[11]) {}

    SamRecord::~SamRecord() = default;

    std::string SamRecord::str() const {
        std::stringstream ss;

        ss << qname << " ";
        ss << flag << " ";
        ss << rname << " ";
        ss << pos << " ";
        ss << static_cast<int>(mapq) << " ";  // cast needed because std::cout is overloaded for uint8_t
        ss << cigar << " ";
        ss << rnext << " ";
        ss << pnext << " ";
        ss << tlen << " ";
        ss << seq << " ";
        ss << qual << " ";
        ss << opt;

        return ss.str();
    }

}  // namespace fio
