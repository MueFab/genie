#ifndef GENIE_SAM_LINE_H
#define GENIE_SAM_LINE_H


#include <memory>
#include <vector>
#include "sam-tag.h"

namespace format {
    namespace sam {
        class SamLine {
        private:
            std::string name;
            std::vector<std::unique_ptr<SamTag>> tags;
            std::vector<std::string> optionalMap;
        protected:
            size_t countTag(const std::string &_name) const;

            virtual void addTag(std::unique_ptr<SamTag> tag);

        public:
            explicit SamLine(std::string _name, std::vector<std::string> map);

            std::string toString() const;
        };

        class SamLineHD : public SamLine {
        public:
            explicit SamLineHD(std::unique_ptr<SamTagVN> vn);
        };

        class SamLineSQ : public SamLine {
        public:
            explicit SamLineSQ(std::unique_ptr<SamTagSN> sn, std::unique_ptr<SamTagLN> ln);
        };

        class SamLineRG : public SamLine {
        public:
            explicit SamLineRG(std::unique_ptr<SamTagID> id);
        };

        class SamLinePG : public SamLine {
        public:
            explicit SamLinePG(std::unique_ptr<SamTagID> id);
        };

        class SamLineCO : public SamLine {
        public:
            explicit SamLineCO(std::unique_ptr<SamTagCO> co);
        };
    }
}

#endif //GENIE_SAM_LINE_H
