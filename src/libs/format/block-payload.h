/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_BLOCK_PAYLOAD_H
#define GENIE_BLOCK_PAYLOAD_H

// ---------------------------------------------------------------------------------------------------------------------

#include <coding/mpegg-raw-au.h>
#include <sstream>

/**
 * @brief
 */
class BlockPayloadSet {
   public:
    /**
     * @brief
     */
    class TransformedPayload {
       private:
        gabac::DataBlock payloadData;  //!< @brief
        size_t position;

       public:
        /**
         * @brief
         * @param writer
         */
        void write(util::BitWriter* writer) const;

        /**
         * @brief
         * @param _data
         */
        explicit TransformedPayload(gabac::DataBlock _data, size_t pos);

        /**
         * @brief
         * @return
         */
        bool isEmpty() const;

        /**
         * @brief
         * @param _data
         */
        gabac::DataBlock&& move();

        size_t getIndex() const { return position; }
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
        explicit SubsequencePayload(GenSubIndex _id) : id(_id) {}

        /**
         * @brief
         * @return
         */
        GenSubIndex getID() const { return id; }

        /**
         * @brief
         * @param writer
         */
        void write(util::BitWriter* writer) const;

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

        TransformedPayload& getTransformedPayload(size_t index) { return transformedPayloads[index]; }
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
        explicit DescriptorPayload(GenDesc _id) : id(_id) {}

        explicit DescriptorPayload() : id(GenDesc(0)) {}

        /**
         * @brief
         * @return
         */
        GenDesc getID() const { return id; }

        /**
         * @brief
         * @param writer
         */
        void write(util::BitWriter* writer) const;

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
    explicit BlockPayloadSet(format::mpegg_p2::ParameterSet param, size_t _record_num);

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
    format::mpegg_p2::ParameterSet& getParameters();

    /**
     * @brief
     * @return
     */
    const format::mpegg_p2::ParameterSet& getParameters() const;

    /**
     * @brief
     * @return
     */
    format::mpegg_p2::ParameterSet&& moveParameters();

    /**
     * @brief
     * @return
     */
    std::vector<DescriptorPayload>& getPayloads();

    void clear() { *this = BlockPayloadSet(format::mpegg_p2::ParameterSet(), 0); }

   private:
    std::vector<DescriptorPayload> desc_pay;  //!< @brief

    size_t record_num;                          //!< @brief
    format::mpegg_p2::ParameterSet parameters;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BLOCK_PAYLOAD_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------