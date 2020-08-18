/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CORE_ACCESS_UNIT_H
#define GENIE_CORE_ACCESS_UNIT_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/stats/perf-stats.h>
#include <genie/util/data-block.h>
#include <numeric>
#include "constants.h"
#include "parameter/parameter_set.h"
#include "reference-manager.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief
 */
class AccessUnit {
   public:
    /**
     * @brief
     */
    class Subsequence {
       private:
        util::DataBlock data;  //!< @brief
        size_t position;       //!< @brief

        GenSubIndex id;     //!< @brief
        size_t numSymbols;  //!<

       public:
        /**
         *
         * @param wordsize
         * @param _id
         */
        Subsequence(size_t wordsize, GenSubIndex _id);

        /**
         *
         * @param d
         * @param _id
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

        /**
         *
         * @return
         */
        uint64_t pull();

        /**
         *
         * @return
         */
        size_t getNumSymbols() const;

        /**
         *
         * @param num
         */
        void annotateNumSymbols(size_t num);

        /**
         *
         * @return
         */
        bool isEmpty() const;

        /**
         *
         * @return
         */
        size_t getRawSize() const;

        /**
         *
         * @param writer
         */
        void write(util::BitWriter& writer) const;

        /**
         *
         * @param _id
         * @param size
         * @param reader
         */
        Subsequence(GenSubIndex _id, size_t size, util::BitReader& reader);

        /**
         *
         * @param _id
         */
        explicit Subsequence(GenSubIndex _id);

        /**
         *
         * @param _id
         * @param dat
         */
        Subsequence(GenSubIndex _id, util::DataBlock&& dat);

        /**
         *
         * @param dat
         */
        void set(util::DataBlock&& dat);

        /**
         *
         * @param pos
         */
        void setPosition(size_t pos);
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
         * @param sub
         * @return
         */
        Subsequence& get(uint16_t sub);

        /**
         *
         * @param sub
         * @return
         */
        const Subsequence& get(uint16_t sub) const;

        /**
         *
         * @param pos
         * @param _type
         * @return
         */
        Subsequence& getTokenType(uint16_t pos, uint8_t _type);

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
        void set(uint16_t _id, Subsequence&& sub);

        /**
         * @brief
         * @param _id
         */
        explicit Descriptor(GenDesc _id);

        /**
         *
         * @return
         */
        Subsequence* begin();

        /**
         *
         * @return
         */
        Subsequence* end();

        /**
         *
         * @return
         */
        const Subsequence* begin() const;

        /**
         *
         * @return
         */
        const Subsequence* end() const;

        /**
         *
         * @return
         */
        size_t getSize() const;

        /**
         *
         * @return
         */
        size_t getWrittenSize() const;

        /**
         *
         * @param writer
         */
        void write(util::BitWriter& writer) const;

        /**
         *
         * @param _id
         * @param count
         * @param remainingSize
         * @param reader
         */
        Descriptor(GenDesc _id, size_t count, size_t remainingSize, util::BitReader& reader);

        /**
         *
         * @return
         */
        bool isEmpty() const;

        /**
         *
         */
        Descriptor();
    };

    /**
     * @brief
     * @param sub
     * @return
     */
    const Subsequence& get(GenSubIndex sub) const;

    /**
     * @brief
     * @param sub
     * @return
     */
    Subsequence& get(GenSubIndex sub);

    /**
     *
     * @param desc
     * @return
     */
    Descriptor& get(GenDesc desc);

    /**
     *
     * @param desc
     * @return
     */
    const Descriptor& get(GenDesc desc) const;

    /**
     * @brief
     * @param sub
     * @param data
     */
    void set(GenSubIndex sub, Subsequence&& data);

    /**
     *
     * @param sub
     * @param data
     */
    void set(GenDesc sub, Descriptor&& data);

    /**
     *
     * @param sub
     * @param value
     */
    void push(GenSubIndex sub, uint64_t value);

    /**
     *
     * @param sub
     * @return
     */
    bool isEnd(GenSubIndex sub);

    /**
     *
     * @param sub
     * @param lookahead
     * @return
     */
    uint64_t peek(GenSubIndex sub, size_t lookahead = 0);

    /**
     *
     * @param sub
     * @return
     */
    uint64_t pull(GenSubIndex sub);

    /**
     *
     * @param set
     * @param _numRecords
     */
    AccessUnit(parameter::ParameterSet&& set, size_t _numRecords);

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
    size_t getNumReads() const;

    /**
     *
     * @param recs
     */
    void setNumReads(size_t recs);

    /**
     *
     * @return
     */
    record::ClassType getClassType() const;

    /**
     *
     * @param _type
     */
    void setClassType(record::ClassType _type);

    /**
     *
     */
    void clear();

    /**
     *
     */
    void addRecord();

    /**
     *
     * @param ref
     */
    void setReference(uint16_t ref);

    /**
     *
     * @return
     */
    uint16_t getReference();

    /**
     *
     * @param pos
     */
    void setMaxPos(uint64_t pos);

    /**
     *
     * @param pos
     */
    void setMinPos(uint64_t pos);

    /**
     *
     * @return
     */
    uint64_t getMaxPos() const;

    /**
     *
     * @return
     */
    uint64_t getMinPos() const;

    /**
     *
     * @return
     */
    Descriptor* begin();

    /**
     *
     * @return
     */
    Descriptor* end();

    /**
     *
     * @return
     */
    const Descriptor* begin() const;

    /**
     *
     * @return
     */
    const Descriptor* end() const;

    /**
     *
     * @return
     */
    stats::PerfStats& getStats();

    /**
     *
     * @param _stats
     */
    void setStats(stats::PerfStats&& _stats);

    /**
     *
     * @param ex
     * @param ref2Write
     */
    void setReference(const ReferenceManager::ReferenceExcerpt& ex,
                      const std::vector<std::pair<size_t, size_t>>& ref2Write);

    /**
     *
     * @return
     */
    const ReferenceManager::ReferenceExcerpt& getReferenceExcerpt() const;

    /**
     *
     * @return
     */
    const std::vector<std::pair<size_t, size_t>>& getRefToWrite() const;

    /**
     *
     * @return
     */
    bool isReferenceOnly() const;

    /**
     *
     * @param ref
     */
    void setReferenceOnly(bool ref);

   private:
    std::vector<Descriptor> descriptors;                //!< @brief
    parameter::ParameterSet parameters;                 //!< @brief
    stats::PerfStats stats;                             //!<
    ReferenceManager::ReferenceExcerpt reference;       //!<
    std::vector<std::pair<size_t, size_t>> refToWrite;  //!<
    bool referenceOnly{false};                          //!<

    size_t numReads;  //!< @brief

    record::ClassType type{};  //!<
    uint64_t minPos;           //!<
    uint64_t maxPos;           //!<

    uint16_t referenceSequence;  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ACCESS_UNIT_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------