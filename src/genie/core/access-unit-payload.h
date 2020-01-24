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
    class TransformedPayload {
       private:
        util::DataBlock payloadData;  //!< @brief
        size_t position{};

       public:
        /**
         * @brief
         * @param writer
         */
        void write(util::BitWriter& writer) const;

        /**
         * @brief
         * @param _data
         */
        explicit TransformedPayload(util::DataBlock _data, size_t pos);

        TransformedPayload(size_t size, util::BitReader& reader);

        /**
         * @brief
         * @return
         */
        bool isEmpty() const;

        /**
         * @brief
         * @param _data
         */
        util::DataBlock&& move();

        size_t getIndex() const;
    };

    /**
     * @brief
     */
    class SubsequencePayload {
       private:
        std::vector<TransformedPayload> transformedPayloads;  //!< @brief
        GenSubIndex id;                                       //!< @brief

       public:
        /**
         * @brief
         * @param _id
         */
        explicit SubsequencePayload(GenSubIndex _id);

        explicit SubsequencePayload(GenSubIndex _id, size_t remainingSize, size_t subseq_ctr, util::BitReader& reader);

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
        void add(TransformedPayload p);

        /**
         * @brief
         * @return
         */
        bool isEmpty() const;

        /**
         * @brief
         * @return
         */
        std::vector<TransformedPayload>& getTransformedPayloads();

        TransformedPayload& getTransformedPayload(size_t index);
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

        /**
         * @brief
         * @return
         */
        std::vector<SubsequencePayload>& getSubsequencePayloads();
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
     * @brief
     * @return
     */
    std::vector<DescriptorPayload>& getPayloads();

    void clear();

    uint16_t getReference() const;

    void setReference(uint16_t ref);

    void setMaxPos(uint64_t pos);

    void setMinPos(uint64_t pos);

    uint64_t getMaxPos() const;

    uint64_t getMinPos() const;

    void setRecordNum(size_t num);

   private:
    std::vector<DescriptorPayload> desc_pay;  //!< @brief

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