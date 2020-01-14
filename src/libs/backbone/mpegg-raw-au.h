/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MPEGG_RAW_AU_H
#define GENIE_MPEGG_RAW_AU_H

// ---------------------------------------------------------------------------------------------------------------------

#include <mpegg_p2/parameter_set.h>
#include <cstdint>
#include <string>
#include <vector>
#include "constants.h"

namespace genie {

/**
 * @brief
 */
class MpeggRawAu {
   public:
    /**
     * @brief
     */
    class SubDescriptor {
       private:
        util::DataBlock data;  //!< @brief
        size_t position;       //!< @brief

        GenSubIndex id;  //!< @brief

       public:
        /**
         * @brief
         * @param wordsize
         */
        SubDescriptor(size_t wordsize, GenSubIndex _id);

        /**
         * @brief
         * @param d
         */
        SubDescriptor(util::DataBlock d, GenSubIndex _id);

        /**
         * @brief
         * @param val
         */
        void push(uint64_t val);

        /**
         * @brief
         */
        void inc();

        /**
         * @brief
         * @return
         */
        uint64_t get(size_t lookahead = 0) const;

        /**
         * @brief
         * @return
         */
        bool end() const;

        /**
         * @brief
         * @param block
         */
        util::DataBlock&& move();

        /**
         * @brief
         * @return
         */
        GenSubIndex getID() const;

        uint64_t pull() {
            if (end()) {
                UTILS_DIE("Tried to read descriptor that has already ended");
            }
            return data.get(position++);
        }
    };

    /**
     * @brief
     */
    class Descriptor {
       private:
        std::vector<SubDescriptor> subdesc;  //!< @brief
        GenDesc id;                          //!< @brief

       public:
        /**
         * @brief
         * @return
         */
        const std::vector<SubDescriptor>& getSubsequences() const;

        /**
         * @brief
         * @param sub
         * @return
         */
        SubDescriptor& getSubsequence(uint8_t sub);

        /**
         * @brief
         * @return
         */
        GenDesc getID() const;

        /**
         * @brief
         * @param sub
         */
        void add(SubDescriptor&& sub);

        /**
         * @brief
         * @param _id
         * @param sub
         */
        void set(uint8_t _id, SubDescriptor&& sub);

        /**
         * @brief
         * @param _id
         */
        explicit Descriptor(GenDesc _id);
    };

    /**
     * @brief
     * @param desc
     * @param sub
     * @return
     */
    const SubDescriptor& get(GenSubIndex sub) const;

    /**
     * @brief
     * @param desc
     * @param sub
     * @return
     */
    SubDescriptor& get(GenSubIndex sub);

    /**
     * @brief
     * @param desc
     * @param sub
     * @return
     */
    SubDescriptor& get(GenDesc desc, uint8_t sub);

    /**
     * @brief
     * @param desc
     * @param sub
     * @param data
     */
    void set(GenSubIndex sub, SubDescriptor&& data);

    /**
     *
     * @param sub
     * @param value
     */
    void push(GenSubIndex sub, uint64_t value) { get(sub).push(value); }

    bool isEnd(GenSubIndex sub) { return get(sub).end(); }

    uint64_t peek(GenSubIndex sub, size_t lookahead = 0) { return get(sub).get(lookahead); }

    /**
     *
     * @param sub
     * @return
     */
    uint64_t pull(GenSubIndex sub) { return get(sub).pull(); }

    /**
     * @brief
     * @return
     */
    const std::vector<Descriptor>& getDescriptorStreams() const;

    /**
     * @brief
     * @param set
     */
    MpeggRawAu(mpegg_p2::ParameterSet&& set, size_t _numRecords);

    /**
     * @brief
     * @param _parameters
     */
    void setParameters(mpegg_p2::ParameterSet&& _parameters);

    /**
     * @brief
     * @return
     */
    const mpegg_p2::ParameterSet& getParameters() const;

    /**
     * @brief
     * @return
     */
    mpegg_p2::ParameterSet& getParameters();

    /**
     * @brief
     * @return
     */
    mpegg_p2::ParameterSet&& moveParameters();

    /**
     * @brief
     * @return
     */
    size_t getNumRecords() const;

    void clear() { *this = MpeggRawAu(mpegg_p2::ParameterSet(), 0); }

    void addRecord() { numRecords++; }

    void setReference(uint16_t ref) { referenceSequence = ref; }

    uint16_t getReference() { return referenceSequence; }

    void setMaxPos(uint64_t pos) { maxPos = pos; }

    void setMinPos(uint64_t pos) { minPos = pos; }

    uint64_t getMaxPos() const { return maxPos; }

    uint64_t getMinPos() const { return minPos; }

   private:
    std::vector<Descriptor> descriptors;  //!< @brief
    mpegg_p2::ParameterSet parameters;    //!< @brief

    size_t numRecords;  //!< @brief

    uint64_t minPos;
    uint64_t maxPos;

    uint16_t referenceSequence;
};
}  // namespace genie
// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MPEGG_RAW_AU_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------