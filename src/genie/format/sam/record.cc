/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "record.h"
#include <constants.h>
#include <genie/util/exceptions.h>
#include <genie/util/tokenize.h>
#include <regex>
#include "header/tag.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

const core::CigarFormatInfo& getSAMCigarInfo() {
    const static auto formatInfo = []() -> core::CigarFormatInfo {
        const auto ref_step2 = []() -> std::vector<uint8_t> {
            std::vector<uint8_t> lut(128, 0);
            lut['M'] = 1;
            lut['='] = 1;
            lut['X'] = 1;
            lut['I'] = 0;
            lut['D'] = 1;
            lut['N'] = 1;
            lut['S'] = 0;
            lut['H'] = 0;
            lut['P'] = 0;
            return lut;
        }();
        const auto seq_step2 = []() -> std::vector<uint8_t> {
            std::vector<uint8_t> lut(128, 0);
            lut['M'] = 1;
            lut['='] = 1;
            lut['X'] = 1;
            lut['I'] = 1;
            lut['D'] = 0;
            lut['N'] = 0;
            lut['S'] = 1;
            lut['H'] = 0;
            lut['P'] = 0;
            return lut;
        }();
        return {ref_step2, seq_step2, {}, false};
    }();
    return formatInfo;
}

// ---------------------------------------------------------------------------------------------------------------------

Record::Record(std::string _qname, uint16_t _flag, std::string _rname, uint32_t _pos, uint8_t _mapq, std::string _cigar,
               std::string _rnext, uint32_t _pnext, int32_t _tlen, std::string _seq, std::string _qual)
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

// ---------------------------------------------------------------------------------------------------------------------

Record::Record() : Record("", 0, "", 0, 0, "", "", 0, 0, "", "") {}

// ---------------------------------------------------------------------------------------------------------------------

Record::Record(const std::string& string) {
    auto tokens = util::tokenize(string, '\t');
    UTILS_DIE_IF(tokens.size() < 11, "Invalid SAM record");
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

// ---------------------------------------------------------------------------------------------------------------------

void Record::check() const {
    static const std::regex REGEX_QNAME("[!-?A-~]{1,254}");
    static const std::regex REGEX_RNAME("\\*|" + header::getSAMRegex());
    static const std::regex REGEX_CIGAR("\\*|([0-9]+[MIDNSHPX=])+");
    static const std::regex REGEX_RNEXT("\\*|=|" + header::getSAMRegex());
    static const std::regex REGEX_SEQ("\\*|[A-Za-z=.]+");
    static const std::regex REGEX_QUAL("[!-~]+");
    UTILS_DIE_IF(!std::regex_match(qname, REGEX_QNAME) || !std::regex_match(rname, REGEX_RNAME) ||
                     !std::regex_match(cigar, REGEX_CIGAR) || !std::regex_match(rnext, REGEX_RNEXT) ||
                     !std::regex_match(seq, REGEX_SEQ) || !std::regex_match(qual, REGEX_QUAL),
                 "Invalid SAM record");
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Record::getQname() const { return qname; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Record::getFlags() const { return flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool Record::checkFlag(FlagPos f) const { return getFlags() & (1u << uint8_t(f)); }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Record::getRname() const { return rname; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Record::getPos() const { return pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Record::getMapQ() const { return mapq; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Record::getCigar() const { return cigar; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Record::getRnext() const { return rnext; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Record::getPnext() const { return pnext; }

// ---------------------------------------------------------------------------------------------------------------------

int32_t Record::getTlen() const { return tlen; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Record::getSeq() const { return seq; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Record::getQual() const { return qual; }

// ---------------------------------------------------------------------------------------------------------------------

std::string&& Record::moveQname() { return std::move(qname); }

// ---------------------------------------------------------------------------------------------------------------------

std::string&& Record::moveRname() { return std::move(rname); }

// ---------------------------------------------------------------------------------------------------------------------

std::string&& Record::moveCigar() { return std::move(cigar); }

// ---------------------------------------------------------------------------------------------------------------------

std::string&& Record::moveRnext() { return std::move(rnext); }

// ---------------------------------------------------------------------------------------------------------------------

std::string&& Record::moveSeq() { return std::move(seq); }

// ---------------------------------------------------------------------------------------------------------------------

std::string&& Record::moveQual() { return std::move(qual); }

// ---------------------------------------------------------------------------------------------------------------------

std::string Record::toString() const {
    std::string ret;
    ret += qname + '\t' + std::to_string(flag) + '\t' + rname + '\t' + std::to_string(pos) + '\t';
    ret += std::to_string(mapq) + '\t' + cigar + '\t' + rnext + '\t' + std::to_string(pnext) + '\t';
    ret += std::to_string(tlen) + '\t' + seq + '\t' + qual;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------