#include "sam-line.h"

namespace format {
    namespace sam {
        size_t SamLine::countTag(const std::string &_name) const {
            size_t counter = 0;
            for (const auto &t : tags) {
                if (t->getName() == _name) {
                    ++counter;
                }
            }
            return counter;
        }

        void SamLine::addTag(std::unique_ptr<SamTag> tag) {
            auto entry = std::find(optionalMap.begin(), optionalMap.end(), tag->getName());
            if (entry == optionalMap.end() || countTag(tag->getName())) {
                UTILS_DIE("Tag " + tag->getName() + " already present or not valid for this line");
            }
            tags.push_back(std::move(tag));
        }

        SamLine::SamLine(std::string _name, std::vector<std::string> map)
                : name(std::move(_name)), optionalMap(std::move(map)) {

        }

        std::string SamLine::toString() const {
            std::string ret;
            ret += '@' + name;
            for (const auto &t : tags) {
                ret += ' ' + t->getName() + ':' + t->getContent();
            }
            return ret;
        }

        SamLineHD::SamLineHD(std::unique_ptr<SamTagVN> vn)
                : SamLine(
                "HD",
                {
                        "VN",
                        "SO",
                        "GO",
                        "SS"
                }
        ) {
            SamLineHD::addTag(std::move(vn));
        }

        SamLineSQ::SamLineSQ(std::unique_ptr<SamTagSN> sn, std::unique_ptr<SamTagLN> ln)
                : SamLine(
                "SQ",
                {
                        "SN",
                        "LN",
                        "AH",
                        "AN",
                        "AS",
                        "DS",
                        "M5",
                        "SP",
                        "TP",
                        "UR"
                }
        ) {
            SamLineSQ::addTag(std::move(sn));
            SamLineSQ::addTag(std::move(ln));
        }

        SamLineRG::SamLineRG(std::unique_ptr<SamTagID> id)
                : SamLine(
                "RG",
                {
                        "ID",
                        "BC",
                        "CN",
                        "DS",
                        "DT",
                        "FO",
                        "KS",
                        "LB",
                        "PG",
                        "PI",
                        "PL",
                        "PM",
                        "PU",
                        "SM"
                }
        ) {
            SamLineRG::addTag(std::move(id));
        }

        SamLinePG::SamLinePG(std::unique_ptr<SamTagID> id)
                : SamLine(
                "PG",
                {
                        "ID",
                        "PN",
                        "CL",
                        "PP",
                        "DS",
                        "VN"
                }
        ) {
            SamLinePG::addTag(std::move(id));
        }

        SamLineCO::SamLineCO(std::unique_ptr<SamTagCO> co)
                : SamLine(
                "RG",
                {
                        "co"
                }
        ) {
            SamLineCO::addTag(std::move(co));
        }

    }
}