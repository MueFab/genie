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
        explicit TransformedPayload(gabac::DataBlock* _data);

        /**
         * @brief
         * @return
         */
        bool isEmpty() const;

        /**
         * @brief
         * @param _data
         */
        void swap(gabac::DataBlock* _data);
    };

    /**
     * @brief
     */
    class SubsequencePayload {
       private:
        std::vector<std::unique_ptr<TransformedPayload>> transformedPayloads;  //!< @brief
        GenSubIndex id;                                                 //!< @brief

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
        void add(std::unique_ptr<TransformedPayload> p);

        /**
         * @brief
         * @return
         */
        bool isEmpty() const;

        /**
         * @brief
         * @return
         */
        const std::vector<std::unique_ptr<TransformedPayload>>& getTransformedPayloads() const;
    };

    /**
     * @brief
     */
    class DescriptorPayload {
       private:
        std::vector<std::unique_ptr<SubsequencePayload>> subsequencePayloads;  //!< @brief
        GenDesc id;                                                  //!< @brief

       public:
        /**
         * @brief
         * @param _id
         */
        explicit DescriptorPayload(GenDesc _id) : id(_id) {}

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
        void add(std::unique_ptr<SubsequencePayload> p);

        /**
         * @brief
         * @return
         */
        bool isEmpty() const;

        /**
         * @brief
         * @return
         */
        const std::vector<std::unique_ptr<SubsequencePayload>>& getSubsequencePayloads() const;
    };

    /**
     * @brief
     * @param param
     * @param _record_num
     */
    explicit BlockPayloadSet(std::unique_ptr<format::mpegg_p2::ParameterSet> param, size_t _record_num);

    /**
     * @brief
     * @param i
     * @param p
     */
    void setPayload(GenDesc i, std::unique_ptr<DescriptorPayload> p);

    /**
     * @brief
     * @param i
     * @return
     */
    DescriptorPayload* getPayload(GenDesc i);

    /**
     * @brief
     * @param i
     * @return
     */
    std::unique_ptr<DescriptorPayload> movePayload(size_t i);

    /**
     * @brief
     * @return
     */
    size_t getRecordNum() const;

    /**
     * @brief
     * @return
     */
    format::mpegg_p2::ParameterSet* getParameters();

    /**
     * @brief
     * @return
     */
    const format::mpegg_p2::ParameterSet* getParameters() const;

    /**
     * @brief
     * @return
     */
    std::unique_ptr<format::mpegg_p2::ParameterSet> moveParameters();

    /**
     * @brief
     * @return
     */
    const std::vector<std::unique_ptr<DescriptorPayload>>& getPayloads() const;

   private:
    std::vector<std::unique_ptr<DescriptorPayload>> desc_pay;  //!< @brief

    size_t record_num;                                           //!< @brief
    std::unique_ptr<format::mpegg_p2::ParameterSet> parameters;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_BLOCK_PAYLOAD_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------