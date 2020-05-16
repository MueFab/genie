#ifndef GENIE_CLASSIFIER_H
#define GENIE_CLASSIFIER_H

#include "genie/core/record/alignment_split/same-rec.h"

namespace genie {
namespace core {

class Classifier {
   public:
    virtual genie::core::record::Chunk getChunk() = 0;

    virtual void add(genie::core::record::Chunk&& c) = 0;

    virtual void flush() = 0;

    virtual bool isFlushing() const = 0;
};

class ClassifierRegroup : public Classifier {
   private:
    using ClassBlock = std::vector<genie::core::record::Chunk>;
    using SequenceBlock = std::vector<ClassBlock>;
    SequenceBlock classes;
    uint16_t currentSeq;
    size_t auSize;
    bool flushing;

   public:
    explicit ClassifierRegroup(size_t _auSize) : auSize(_auSize), flushing(false) {
        classes.resize(uint8_t(genie::core::record::ClassType::CLASS_U));
        for (auto& c : classes) {
            c.emplace_back();
        }
    }
    genie::core::record::Chunk getChunk() override {
        for (auto& c : classes) {
            if (c.size() > 1 || (!c.front().empty() && flushing)) {
                genie::core::record::Chunk chunk = std::move(c.front());
                c.erase(c.begin());
                if (c.empty()) {
                    c.emplace_back();
                }
                return chunk;
            }
        }
        if (flushing) {
            flushing = false;
        }
        return genie::core::record::Chunk();
    }

    void add(genie::core::record::Chunk&& c) override {
        genie::core::record::Chunk chunk = std::move(c);
        for (auto& r : chunk) {
            auto classtype = classify(r);
            r.setClassType(classtype);
            if (r.getClassID() < genie::core::record::ClassType::CLASS_HM &&
                r.getAlignmentSharedData().getSeqID() != currentSeq) {
                currentSeq = r.getAlignmentSharedData().getSeqID();
                for (size_t i = 0; i < uint8_t(genie::core::record::ClassType::CLASS_HM); ++i) {
                    classes[i].emplace_back();
                }
            }
            if (classes[uint8_t(classtype)].back().size() >= auSize) {
                classes[uint8_t(classtype)].emplace_back();
            }
            classes[uint8_t(classtype)].back().emplace_back(std::move(r));
        }
    }

    void flush() override { flushing = true; }

    bool isFlushing() const override { return flushing; }

    static genie::core::record::ClassType classifyECigar(const std::string& cigar) {
        for (const auto& c : cigar) {
            if (c == '+' || c == '-') {
                return genie::core::record::ClassType::CLASS_I;
            }
            if (genie::core::getAlphabetProperties(genie::core::AlphabetID::ACGTN).isIncluded(c)) {
                return genie::core::record::ClassType::CLASS_M;
            }
        }
        return genie::core::record::ClassType::CLASS_P;
    }

    static genie::core::record::ClassType classifySeq(const std::string& seq) {
        for (const auto& c : seq) {
            if (c == 'N') {
                return genie::core::record::ClassType::CLASS_N;
            }
        }
        return genie::core::record::ClassType::CLASS_P;
    }

    static genie::core::record::ClassType classify(const genie::core::record::Record& r) {
        if (r.getAlignments().empty()) {
            return genie::core::record::ClassType::CLASS_U;
        }
        if (r.getAlignments().front().getAlignmentSplits().size() + 1 < r.getSegments().size()) {
            return genie::core::record::ClassType::CLASS_HM;
        }

        genie::core::record::ClassType highestClass = genie::core::record::ClassType::CLASS_P;
        for (const auto& a : r.getAlignments().front().getAlignmentSplits()) {
            if (a->getType() != genie::core::record::AlignmentSplit::Type::SAME_REC) {
                continue;
            }
            auto& split = dynamic_cast<const genie::core::record::alignment_split::SameRec&>(*a);
            highestClass = std::max(highestClass, classifyECigar(split.getAlignment().getECigar()));
            if (highestClass == genie::core::record::ClassType::CLASS_I) {
                return highestClass;
            }
        }

        if (highestClass >= genie::core::record::ClassType::CLASS_M) {
            return highestClass;
        }

        for (const auto& s : r.getSegments()) {
            highestClass = std::max(highestClass, classifySeq(s.getSequence()));
            if (highestClass >= genie::core::record::ClassType::CLASS_N) {
                return highestClass;
            }
        }

        return highestClass;
    }
};

class ClassifierBypass : public Classifier {
    std::vector<genie::core::record::Chunk> vec;
    bool flushing = false;

   public:
    genie::core::record::Chunk getChunk() override {
        flushing = false;
        genie::core::record::Chunk ret;
        if (vec.empty()) {
            return ret;
        }
        ret = std::move(vec.front());
        vec.erase(vec.begin());
        return ret;
    }

    void add(genie::core::record::Chunk&& c) override { vec.emplace_back(std::move(c)); }

    void flush() override { flushing = true; }

    virtual bool isFlushing() const override { return flushing; }
};

}  // namespace core
}  // namespace genie

#endif  // GENIE_CLASSIFIER_H
