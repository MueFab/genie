/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CLASSIFIERREGROUP_H
#define GENIE_CLASSIFIERREGROUP_H

// ---------------------------------------------------------------------------------------------------------------------

#include "classifier.h"
#include "record/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class ClassifierRegroup : public Classifier {
   private:
    using ClassBlock = std::vector<record::Chunk>;  //!<
    using SequenceBlock = std::vector<ClassBlock>;  //!<
    SequenceBlock classes;                          //!<
    uint16_t currentSeq;                            //!<
    size_t auSize;                                  //!<
    bool flushing;                                  //!<

   public:
    /**
     *
     * @param _auSize
     */
    explicit ClassifierRegroup(size_t _auSize);

    /**
     *
     * @return
     */
    record::Chunk getChunk() override;

    /**
     *
     * @param c
     */
    void add(record::Chunk&& c) override;

    /**
     *
     */
    void flush() override;

    /**
     *
     * @return
     */
    bool isFlushing() const override;

    /**
     *
     * @param cigar
     * @return
     */
    static record::ClassType classifyECigar(const std::string& cigar);

    /**
     *
     * @param seq
     * @return
     */
    static record::ClassType classifySeq(const std::string& seq);

    /**
     *
     * @param r
     * @return
     */
    static record::ClassType classify(const record::Record& r);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CLASSIFIERREGROUP_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------