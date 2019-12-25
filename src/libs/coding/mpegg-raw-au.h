/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MPEGG_RAW_AU_H
#define GENIE_MPEGG_RAW_AU_H

// ---------------------------------------------------------------------------------------------------------------------

#include <format/mpegg_p2/parameter_set.h>
#include <cstdint>
#include <string>
#include <vector>

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
        gabac::DataBlock data;  //!< @brief
        size_t position;        //!< @brief

        GenomicSubsequence id;  //!< @brief

       public:
        /**
         * @brief
         * @param wordsize
         */
        SubDescriptor(size_t wordsize, GenomicSubsequence _id);

        /**
         * @brief
         * @param d
         */
        SubDescriptor(gabac::DataBlock* d, GenomicSubsequence _id);

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
        uint64_t get() const;

        /**
         * @brief
         * @return
         */
        bool end() const;

        /**
         * @brief
         * @param block
         */
        void swap(gabac::DataBlock* block);

        /**
         * @brief
         * @return
         */
        GenomicSubsequence getID() const;
    };

    /**
     * @brief
     */
    class Descriptor {
       private:
        std::vector<std::unique_ptr<SubDescriptor>> subdesc;  //!< @brief
        GenomicDescriptor id;                                 //!< @brief

       public:
        /**
         * @brief
         * @return
         */
        const std::vector<std::unique_ptr<SubDescriptor>>& getSubsequences() const;

        /**
         * @brief
         * @param sub
         * @return
         */
        SubDescriptor* getSubsequence(GenomicSubsequence sub);

        /**
         * @brief
         * @return
         */
        GenomicDescriptor getID() const;

        /**
         * @brief
         * @param sub
         */
        void add(std::unique_ptr<SubDescriptor> sub);

        /**
         * @brief
         * @param _id
         * @param sub
         */
        void set(GenomicSubsequence _id, std::unique_ptr<SubDescriptor> sub);

        /**
         * @brief
         * @param _id
         */
        explicit Descriptor(GenomicDescriptor _id);
    };

    /**
     * @brief
     * @param desc
     * @param sub
     * @return
     */
    const SubDescriptor& get(GenomicDescriptor desc, GenomicSubsequence sub) const;

    /**
     * @brief
     * @param desc
     * @param sub
     * @return
     */
    SubDescriptor& get(GenomicDescriptor desc, GenomicSubsequence sub);

    /**
     * @brief
     * @param desc
     * @param sub
     * @return
     */
    SubDescriptor& get(GenomicDescriptor desc, uint8_t sub);

    /**
     * @brief
     * @return
     */
    const std::vector<std::unique_ptr<Descriptor>>& getDescriptorStreams() const;

    /**
     * @brief
     * @param desc
     * @param sub
     * @param data
     */
    void set(GenomicDescriptor desc, GenomicSubsequence sub, std::unique_ptr<SubDescriptor> data);

    /**
     * @brief
     * @param set
     */
    explicit MpeggRawAu(std::unique_ptr<format::mpegg_p2::ParameterSet> set, size_t _numRecords);

    /**
     * @brief
     * @param _parameters
     */
    void setParameters(std::unique_ptr<format::mpegg_p2::ParameterSet> _parameters);

    /**
     * @brief
     * @return
     */
    const format::mpegg_p2::ParameterSet* getParameters() const;

    /**
     * @brief
     * @return
     */
    format::mpegg_p2::ParameterSet* getParameters();

    /**
     * @brief
     * @return
     */
    std::unique_ptr<format::mpegg_p2::ParameterSet> moveParameters();

    /**
     * @brief
     * @return
     */
    size_t getNumRecords() const;

   private:
    std::vector<std::unique_ptr<Descriptor>> descriptors;        //!< @brief
    std::unique_ptr<format::mpegg_p2::ParameterSet> parameters;  //!< @brief

    size_t numRecords;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MPEGG_RAW_AU_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------