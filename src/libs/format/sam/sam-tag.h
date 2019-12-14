#ifndef GENIE_SAM_TAG_H
#define GENIE_SAM_TAG_H

#include <string>
#include <vector>
#include <regex>
#include <util/exceptions.h>

namespace format {
    namespace sam {
        class SamTag {
        private:
            std::string name;
        protected:
            explicit SamTag(std::string _name);

        public:
            static constexpr const char* const RNAME = "[0-9A-Za-z!#$%&+./:;?@^_|~-][0-9A-Za-z!#$%&*+./:;=?@^_|~-]*";
            std::string getName() const;

            virtual std::string getContent() const = 0;

            virtual ~SamTag () = default;

        };

        class SamTagVN : public SamTag {
        private:
            std::string content;
        public:
            explicit SamTagVN(std::string _content);

            std::string getContent() const override;
        };

        class SamTagSO : public SamTag {
        public:
            enum class SortingOrder : uint8_t {
                UNKNOWN,
                UNSORTED,
                QUERYNAME,
                COORDINATE
            };
        private:
            SortingOrder content;
        public:
            explicit SamTagSO(SortingOrder _content);
            std::string getContent() const override;

            static std::string sortingOrderString(SortingOrder opt);
        };

        class SamTagTP : public SamTag {
        public:
            enum class Topology : uint8_t {
                LINEAR,
                CIRCULAR
            };
        private:
            Topology content;
        public:
            explicit SamTagTP(Topology _content);
            std::string getContent() const override;

            static std::string topologyString(Topology opt);
        };

        class SamTagPL : public SamTag {
        public:
            enum class Platform : uint8_t {
                CAPILLARY,
                LS454,
                ILLUMINA,
                SOLID,
                HELICOS,
                IONTORRENT,
                ONT,
                PACBIO
            };
        private:
            Platform content;
        public:
            explicit SamTagPL(Platform _content);
            std::string getContent() const override;

            static std::string platformString(Platform opt);
        };

        class SamTagGO : public SamTag {
        public:
            enum class Grouping : uint8_t {
                NONE,
                QUERY,
                REFERENCE
            };
        private:
            Grouping content;
        public:
            explicit SamTagGO(Grouping _content);

            std::string getContent() const override;

            static std::string groupingString(Grouping opt);
        };

        class SamTagSS : public SamTag {
        private:
            std::string content;
        public:
            explicit SamTagSS(SamTagSS* ss, const std::string& _sorting);

            std::string getContent() const override;
        };

        class SamTagFreeString : public SamTag {
        private:
            std::string content;
        public:
            explicit SamTagFreeString(std::string _name, std::string _content);

            std::string getContent() const override;
        };

        class SamTagSN : public SamTag {
        private:
            std::string content;
        public:
            explicit SamTagSN(std::string _content);

            std::string getContent() const override;
        };

        class SamTagLN : public SamTag {
        private:
            int32_t content;
        public:
            explicit SamTagLN(int32_t _content);

            std::string getContent() const override;
        };

        class SamTagID : public SamTagFreeString {
        public:
            explicit SamTagID(std::string _content);
        };

        class SamTagCO : public SamTagFreeString {
        public:
            explicit SamTagCO(std::string _content);
        };

        class SamTagAN : public SamTagFreeString {
        public:
            explicit SamTagAN(std::string _content);
        };

        class SamTagM5 : public SamTagFreeString {
        public:
            explicit SamTagM5(std::string _content);
        };

        class SamTagFO : public SamTagFreeString {
        public:
            explicit SamTagFO(std::string _content);
        };

        class SamTagDT : public SamTagFreeString {
        public:
            explicit SamTagDT(std::string _content);
        };

        // Free Text: AS, DS, SP, UR, ID, BC, CN, KS, LB, PG, PI, PM, PU, SM, PN, CL, PP
        // TODO: AH, DT
    }
}

#endif //GENIE_SAM_TAG_H
