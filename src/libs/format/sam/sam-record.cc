#include "sam-record.h"

namespace format {
namespace sam {

SamRecord::SamRecord(std::string _qname, uint16_t _flag, std::string _rname, uint32_t _pos, uint8_t _mapq,
                     std::string _cigar, std::string _rnext, uint32_t _pnext, int32_t _tlen, std::string _seq,
                     std::string _qual)
    : qname(std::move(_qname)),
      flag(_flag),
      rname(std::move(_rname)),
      pos(_pos),
      mapq(_mapq),
      cigar(std::move(_cigar)),
      rnext(std::move(_rnext)),
      pnext(_pnext),
      tlen(_tlen),
      seq(std::move(_seq)),
      qual(std::move(_qual)) {}

SamRecord::SamRecord(const std::string& string) {
    auto tokens = tokenize(string, '\t');
    if (tokens.size() < 11) {
        UTILS_DIE("Invalid SAM record");
    }
    qname = tokens[0];
    flag = std::stoi(tokens[1]);
    rname = tokens[2];
    pos = std::stoi(tokens[3]);
    mapq = std::stoi(tokens[4]);
    cigar = tokens[5];
    rnext = tokens[6];
    pnext = std::stoi(tokens[7]);
    tlen = std::stoi(tokens[8]);
    seq = tokens[9];
    qual = tokens[10];
    //    check();
}

void SamRecord::check() const {
    static const std::regex REGEX_QNAME("[!-?A-~]{1,254}");
    static const std::regex REGEX_RNAME("\\*|" + getSAMRegex());
    static const std::regex REGEX_CIGAR("\\*|([0-9]+[MIDNSHPX=])+");
    static const std::regex REGEX_RNEXT("\\*|=|" + getSAMRegex());
    static const std::regex REGEX_SEQ("\\*|[A-Za-z=.]+");
    static const std::regex REGEX_QUAL("[!-~]+");
    if (!std::regex_match(qname, REGEX_QNAME) || !std::regex_match(rname, REGEX_RNAME) ||
        !std::regex_match(cigar, REGEX_CIGAR) || !std::regex_match(rnext, REGEX_RNEXT) ||
        !std::regex_match(seq, REGEX_SEQ) || !std::regex_match(qual, REGEX_QUAL)) {
        UTILS_DIE("Invalid SAM record");
    }
}

const std::string& SamRecord::getQname() const { return qname; }
uint16_t SamRecord::getFlags() const { return flag; }
bool SamRecord::checkFlag(FlagPos f) const { return getFlags() & (1u << uint8_t(f)); }
const std::string& SamRecord::getRname() const { return rname; }
uint32_t SamRecord::getPos() const { return pos; }
uint8_t SamRecord::getMapQ() const { return mapq; }

const std::string& SamRecord::getCigar() const { return cigar; }
const std::string& SamRecord::getRnext() const { return rnext; }
uint32_t SamRecord::getPnext() const { return pnext; }
int32_t SamRecord::getTlen() const { return tlen; }
const std::string& SamRecord::getSeq() const { return seq; }
const std::string& SamRecord::getQual() const { return qual; }

std::string SamRecord::toString() const {
    std::string ret;
    ret += qname + '\t' + std::to_string(flag) + '\t' + rname + '\t' + std::to_string(pos) + '\t';
    ret += std::to_string(mapq) + '\t' + cigar + '\t' + rnext + '\t' + std::to_string(pnext) + '\t';
    ret += std::to_string(tlen) + '\t' + seq + '\t' + qual;
    return ret;
}
}  // namespace sam
}  // namespace format