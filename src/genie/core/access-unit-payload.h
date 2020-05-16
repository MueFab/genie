/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ACCESS_UNIT_PAYLOAD_H
#define GENIE_ACCESS_UNIT_PAYLOAD_H

// ---------------------------------------------------------------------------------------------------------------------

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
        size_t numSymbols;

       public:
        size_t getNumSymbols() const { return numSymbols; }

        void annotateNumSymbols(size_t num) { numSymbols = num; }

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
        void set(util::DataBlock&& p) { payload = std::move(p); }

        /**
         * @brief return subsequence payload
         * @return payload
         */
        const util::DataBlock& get() const { return payload; }

        /**
         * @brief return subsequence payload
         * @return payload
         */
        util::DataBlock& get() { return payload; }

        /**
         * @brief move subsequence payload
         * @return payload
         */
        util::DataBlock&& move() { return std::move(payload); }

        /**
         * @brief
         * @return
         */
        bool isEmpty() const;

        /**
         * @brief return subsequence payload size
         * @return payload size
         */
        size_t getWrittenSize() const { return payload.getRawSize(); }
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

        explicit DescriptorPayload();

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

        SubsequencePayload* begin() { return &subsequencePayloads.front(); }

        SubsequencePayload* end() { return &subsequencePayloads.back() + 1; }

        const SubsequencePayload* begin() const { return &subsequencePayloads.front(); }

        const SubsequencePayload* end() const { return &subsequencePayloads.back() + 1; }

        size_t getWrittenSize() const {
            size_t overhead =
                getDescriptor(getID()).tokentype ? 0 : (subsequencePayloads.size() - 1) * sizeof(uint32_t);
            return std::accumulate(subsequencePayloads.begin(), subsequencePayloads.end(), overhead,
                                   [](size_t sum, const SubsequencePayload& payload) {
                                       return payload.isEmpty() ? sum : sum + payload.getWrittenSize();
                                   });
        }
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

    void clear();

    uint16_t getReference() const;

    void setReference(uint16_t ref);

    void setMaxPos(uint64_t pos);

    void setMinPos(uint64_t pos);

    uint64_t getMaxPos() const;

    uint64_t getMinPos() const;

    void setRecordNum(size_t num);

    DescriptorPayload* begin() { return &desc_pay.front(); }

    DescriptorPayload* end() { return &desc_pay.back() + 1; }

    const DescriptorPayload* begin() const { return &desc_pay.front(); }

    const DescriptorPayload* end() const { return &desc_pay.back() + 1; }

    core::record::ClassType getClassType() const { return type; }

    void setClassType(core::record::ClassType _type) { type = _type; }

   private:
    std::vector<DescriptorPayload> desc_pay;  //!< @brief

    core::record::ClassType type{};
    size_t record_num;                   //!< @brief
    parameter::ParameterSet parameters;  //!< @brief

    uint16_t reference{};
    uint64_t minPos{};
    uint64_t maxPos{};
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ACCESS_UNIT_PAYLOAD_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
