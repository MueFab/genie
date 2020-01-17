/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ACCESS_UNIT_RAW_H
#define GENIE_ACCESS_UNIT_RAW_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/data-block.h>
#include "constants.h"
#include "parameter/parameter_set.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief
 */
class AccessUnitRaw {
   public:
    /**
     * @brief
     */
    class Subsequence {
       private:
        util::DataBlock data;  //!< @brief
        size_t position;       //!< @brief

        GenSubIndex id;  //!< @brief

       public:
        /**
         * @brief
         * @param wordsize
         */
        Subsequence(size_t wordsize, GenSubIndex _id);

        /**
         * @brief
         * @param d
         */
        Subsequence(util::DataBlock d, GenSubIndex _id);

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

        uint64_t pull();
    };

    /**
     * @brief
     */
    class Descriptor {
       private:
        std::vector<Subsequence> subdesc;  //!< @brief
        GenDesc id;                        //!< @brief

       public:
        /**
         * @brief
         * @return
         */
        const std::vector<Subsequence>& getSubsequences() const;

        /**
         * @brief
         * @param sub
         * @return
         */
        Subsequence& getSubsequence(uint8_t sub);

        /**
         * @brief
         * @return
         */
        GenDesc getID() const;

        /**
         * @brief
         * @param sub
         */
        void add(Subsequence&& sub);

        /**
         * @brief
         * @param _id
         * @param sub
         */
        void set(uint8_t _id, Subsequence&& sub);

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
    const Subsequence& get(GenSubIndex sub) const;

    /**
     * @brief
     * @param desc
     * @param sub
     * @return
     */
    Subsequence& get(GenSubIndex sub);

    /**
     * @brief
     * @param desc
     * @param sub
     * @return
     */
    Subsequence& get(GenDesc desc, uint8_t sub);

    /**
     * @brief
     * @param desc
     * @param sub
     * @param data
     */
    void set(GenSubIndex sub, Subsequence&& data);

    /**
     *
     * @param sub
     * @param value
     */
    void push(GenSubIndex sub, uint64_t value);

    bool isEnd(GenSubIndex sub);

    uint64_t peek(GenSubIndex sub, size_t lookahead = 0);

    /**
     *
     * @param sub
     * @return
     */
    uint64_t pull(GenSubIndex sub);

    /**
     * @brief
     * @return
     */
    const std::vector<Descriptor>& getDescriptorStreams() const;

    /**
     * @brief
     * @param set
     */
    AccessUnitRaw(parameter::ParameterSet&& set, size_t _numRecords);

    /**
     * @brief
     * @param _parameters
     */
    void setParameters(parameter::ParameterSet&& _parameters);

    /**
     * @brief
     * @return
     */
    const parameter::ParameterSet& getParameters() const;

    /**
     * @brief
     * @return
     */
    parameter::ParameterSet& getParameters();

    /**
     * @brief
     * @return
     */
    parameter::ParameterSet&& moveParameters();

    /**
     * @brief
     * @return
     */
    size_t getNumRecords() const;

    void clear();

    void addRecord();

    void setReference(uint16_t ref);

    uint16_t getReference();

    void setMaxPos(uint64_t pos);

    void setMinPos(uint64_t pos);

    uint64_t getMaxPos() const;

    uint64_t getMinPos() const;

   private:
    std::vector<Descriptor> descriptors;  //!< @brief
    parameter::ParameterSet parameters;   //!< @brief

    size_t numRecords;  //!< @brief

    uint64_t minPos;
    uint64_t maxPos;

    uint16_t referenceSequence;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ACCESS_UNIT_RAW_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------