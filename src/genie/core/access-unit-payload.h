/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ACCESS_UNIT_PAYLOAD_H
#define GENIE_ACCESS_UNIT_PAYLOAD_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/stats/perf-stats.h>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/data-block.h>
#include <numeric>
#include "constants.h"
#include "parameter/parameter_set.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief
 */
class AccessUnitPayload {
   public:
    /**
     * @brief
     */
    class SubsequencePayload {
       private:
        GenSubIndex id;           //!< @brief
        util::DataBlock payload;  //!< @brief
        size_t numSymbols;        //!<

       public:
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
         * @brief
         * @param _id
         */
        explicit SubsequencePayload(GenSubIndex _id);

        /**
         * @brief
         * @param _id
         * @param _payload
         */
        explicit SubsequencePayload(GenSubIndex _id, util::DataBlock&& _payload);

        /**
         * @brief
         * @param _id
         * @param remainingSize
         * @param reader
         */
        explicit SubsequencePayload(GenSubIndex _id, size_t remainingSize, util::BitReader& reader);

        /**
         * @brief
         * @return
         */
        GenSubIndex getID() const;

        /**
         * @brief
         * @param writer
         */
        void write(util::BitWriter& writer) const;

        /**
         * @brief
         * @param p
         */
        void set(util::DataBlock&& p);

        /**
         * @brief return subsequence payload
         * @return payload
         */
        const util::DataBlock& get() const;

        /**
         * @brief return subsequence payload
         * @return payload
         */
        util::DataBlock& get();

        /**
         * @brief move subsequence payload
         * @return payload
         */
        util::DataBlock&& move();

        /**
         * @brief
         * @return
         */
        bool isEmpty() const;

        /**
         * @brief return subsequence payload size
         * @return payload size
         */
        size_t getWrittenSize() const;
    };

    /**
     * @brief
     */
    class DescriptorPayload {
       private:
        std::vector<SubsequencePayload> subsequencePayloads;  //!< @brief
        GenDesc id;                                           //!< @brief

       public:
        /**
         * @brief
         * @param _id
         */
        explicit DescriptorPayload(GenDesc _id);

        /**
         *
         */
        explicit DescriptorPayload();

        /**
         *
         * @param _id
         * @param count
         * @param remainingSize
         * @param reader
         */
        explicit DescriptorPayload(GenDesc _id, size_t count, size_t remainingSize, util::BitReader& reader);

        /**
         * @brief
         * @return
         */
        GenDesc getID() const;

        /**
         * @brief
         * @param writer
         */
        void write(util::BitWriter& writer) const;

        /**
         * @brief
         * @param p
         */
        void add(SubsequencePayload p);

        /**
         * @brief
         * @return
         */
        bool isEmpty() const;

        /**
         *
         * @return
         */
        SubsequencePayload* begin();

        /**
         *
         * @return
         */
        SubsequencePayload* end();

        /**
         *
         * @return
         */
        const SubsequencePayload* begin() const;

        /**
         *
         * @return
         */
        const SubsequencePayload* end() const;

        /**
         *
         * @return
         */
        size_t getWrittenSize() const;
    };

    /**
     * @brief
     * @param param
     * @param _record_num
     */
    explicit AccessUnitPayload(parameter::ParameterSet param, size_t _record_num);

    /**
     * @brief
     * @param i
     * @param p
     */
    void setPayload(GenDesc i, DescriptorPayload p);

    /**
     * @brief
     * @param i
     * @return
     */
    DescriptorPayload& getPayload(GenDesc i);

    /**
     * @brief
     * @param i
     * @return
     */
    DescriptorPayload&& movePayload(size_t i);

    /**
     * @brief
     * @return
     */
    size_t getRecordNum() const;

    /**
     * @brief
     * @return
     */
    parameter::ParameterSet& getParameters();

    /**
     * @brief
     * @return
     */
    const parameter::ParameterSet& getParameters() const;

    /**
     * @brief
     * @return
     */
    parameter::ParameterSet&& moveParameters();

    /**
     *
     */
    void clear();

    /**
     *
     * @return
     */
    uint16_t getReference() const;

    /**
     *
     * @param ref
     */
    void setReference(uint16_t ref);

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
     * @param num
     */
    void setRecordNum(size_t num);

    /**
     *
     * @return
     */
    DescriptorPayload* begin();

    /**
     *
     * @return
     */
    DescriptorPayload* end();

    /**
     *
     * @return
     */
    const DescriptorPayload* begin() const;

    /**
     *
     * @return
     */
    const DescriptorPayload* end() const;

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
     * @return
     */
    stats::PerfStats& getStats();

    /**
     *
     * @param s
     */
    void setStats(stats::PerfStats&& s);

   private:
    std::vector<DescriptorPayload> desc_pay;  //!< @brief

    stats::PerfStats stats;

    record::ClassType type{};
    size_t record_num;                   //!< @brief
    parameter::ParameterSet parameters;  //!< @brief

    uint16_t reference{};  //!<
    uint64_t minPos{};     //!<
    uint64_t maxPos{};     //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ACCESS_UNIT_PAYLOAD_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
