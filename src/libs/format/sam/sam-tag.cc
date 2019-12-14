#include "sam-tag.h"

namespace format {
    namespace sam {
        SamTag::SamTag(std::string _name) : name(std::move(_name)) {
        }

        std::string SamTag::getName() const {
            return name;
        }


        std::string SamTagTP::topologyString(Topology opt) {
            static const std::vector<std::string> ret = {
                    "linear",
                    "circular"
            };
            return ret[uint8_t(opt)];
        }


        std::string SamTagPL::platformString(Platform opt) {
            static const std::vector<std::string> ret = {
                    "CAPILLARY",
                    "LS454",
                    "ILLUMINA",
                    "SOLID",
                    "HELICOS",
                    "IONTORRENT",
                    "ONT",
                    "PACBIO"
            };
            return ret[uint8_t(opt)];
        }

        SamTagVN::SamTagVN(std::string _content) : SamTag("VN"), content(std::move(_content)) {
            std::regex e("/^[0-9]+\\.[0-9]+$/");
            if (!std::regex_match(content, e)) {
                UTILS_DIE("Invalid sam VN Tag");
            }
        }

        std::string SamTagVN::getContent() const {
            return content;
        }

        SamTagSO::SamTagSO(SortingOrder _content) : SamTag("SO"), content(_content) {
        }

        std::string SamTagSO::getContent() const {
            return sortingOrderString(content);
        }

        std::string SamTagSO::sortingOrderString(SortingOrder opt) {
            static const std::vector<std::string> ret = {
                    "unknown",
                    "unsorted",
                    "queryname",
                    "coordinate"
            };
            return ret[uint8_t(opt)];
        }

        SamTagGO::SamTagGO(Grouping _content) : SamTag("GO"), content(_content) {
        }

        std::string SamTagGO::getContent() const {
            return groupingString(content);
        }

        std::string SamTagGO::groupingString(Grouping opt) {
            static const std::vector<std::string> ret = {
                    "none",
                    "query",
                    "reference"
            };
            return ret[uint8_t(opt)];
        }

        SamTagSS::SamTagSS(SamTagSS* ss, const std::string& _sorting) : SamTag("SS"), content(ss->getContent() +  ":" + _sorting) {
            std::regex rn("(coordinate|queryname|unsorted)(:[A-Za-z0-9_-]+)+");
            if (!std::regex_match(content, rn)) {
                UTILS_DIE("Invalid sam tag content");
            }
        }

        std::string SamTagSS::getContent() const {
            return content;
        }

        SamTagFreeString::SamTagFreeString(std::string _name, std::string _content) : SamTag(std::move(_name)),
                                                                                      content(std::move(_content)) {
            std::regex rn(RNAME);
            if (!std::regex_match(content, rn)) {
                UTILS_DIE("Invalid sam tag content");
            }

            std::regex rt("[a-zA-Z0-9]*");
            if (!std::regex_match(getName(), rt)) {
                UTILS_DIE("Invalid sam tag name");
            }
        }

        std::string SamTagFreeString::getContent() const {
            return content;
        }


        SamTagSN::SamTagSN(std::string _content) : SamTag("SN"), content(std::move(_content)) {
            std::regex rn(SamTag::RNAME);
            if(!std::regex_match(content, rn)) {
                UTILS_DIE("Invalid sam reference name");
            }
        }

        std::string SamTagSN::getContent() const {
            return content;
        }

        SamTagLN::SamTagLN(int32_t _content) : SamTag("SN"), content(_content) {
            if(_content <= 0) {
                UTILS_DIE("Invalid sam LN length");
            }
        }

        std::string SamTagLN::getContent() const {
            return std::to_string(content);
        }

        SamTagID::SamTagID(std::string _content) : SamTagFreeString("ID", std::move(_content)) {

        }

        SamTagCO::SamTagCO(std::string _content) : SamTagFreeString("co", std::move(_content)) {

        }

        SamTagAN::SamTagAN(std::string _content) : SamTagFreeString("AN", std::move(_content)){
            std::regex rn(RNAME+std::string("(,")+RNAME+")*");
            if(!std::regex_match(SamTagAN::getContent(), rn)) {
                UTILS_DIE("Invalid sam AN value");
            }
        }

        SamTagM5::SamTagM5(std::string _content) : SamTagFreeString("M5", std::move(_content)){
            std::regex rn("^[a-f0-9]{32}$");
            if(!std::regex_match(SamTagM5::getContent(), rn)) {
                UTILS_DIE("Invalid sam M5 value");
            }
        }

        SamTagFO::SamTagFO(std::string _content) : SamTagFreeString("FO", std::move(_content)){
            std::regex rn("/\\*|[ACMGRSVTWYHKDBN]+/");
            if(!std::regex_match(SamTagFO::getContent(), rn)) {
                UTILS_DIE("Invalid sam FO value");
            }
        }
    }
}