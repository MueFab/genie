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
 *
 */
class MpeggRawAu {
   public:
    /**
     *
     */
    struct SubDescriptor {
       private:
        gabac::DataBlock data;  //!<
        size_t position;        //!<

       public:
        /**
         *
         * @param wordsize
         */
        explicit SubDescriptor(size_t wordsize);

        /**
         *
         * @param d
         */
        explicit SubDescriptor(gabac::DataBlock* d);

        /**
         *
         * @param val
         */
        void push(uint64_t val);

        /**
         *
         */
        void inc();

        /**
         *
         * @return
         */
        uint64_t get() const;

        /**
         *
         * @return
         */
        bool end() const;

        /**
         *
         * @return
         */
        size_t rawSize() const;

        /**
         *
         * @return
         */
        size_t getWordSize() const;

        /**
         *
         * @return
         */
        const void* getData() const;

        void swap(gabac::DataBlock* block) { block->swap(&data); }
    };
    typedef std::vector<std::unique_ptr<SubDescriptor>> Descriptor;  //!<

    /**
     *
     * @param desc
     * @param sub
     * @return
     */
    SubDescriptor& get(GenomicDescriptor desc, GenomicSubsequence sub);

    void set(GenomicDescriptor desc, GenomicSubsequence sub, std::unique_ptr<SubDescriptor> data) {
        (*descriptors[uint8_t(desc)])[uint8_t(sub)] = std::move(data);
    }

    /**
     *
     * @param desc
     * @param sub
     * @return
     */
    const SubDescriptor& get(GenomicDescriptor desc, GenomicSubsequence sub) const;

    SubDescriptor& get(GenomicDescriptor desc, uint8_t sub) { return get(desc, GenomicSubsequence(sub)); }

    /**
     *
     * @param set
     */
    explicit MpeggRawAu(std::unique_ptr<format::mpegg_p2::ParameterSet> set);

    /**
     *
     * @param _parameters
     */
    void setParameters(std::unique_ptr<format::mpegg_p2::ParameterSet> _parameters) {
        parameters = std::move(_parameters);
    }

    /**
     *
     * @return
     */
    format::mpegg_p2::ParameterSet* getParameters() { return parameters.get(); }

    /**
     *
     * @return
     */
    std::unique_ptr<format::mpegg_p2::ParameterSet> moveParameters() { return std::move(parameters); }

    /**
     *
     * @return
     */
    const format::mpegg_p2::ParameterSet* getParameters() const { return parameters.get(); }

   private:
    std::vector<std::unique_ptr<Descriptor>> descriptors;        //!<
    std::unique_ptr<format::mpegg_p2::ParameterSet> parameters;  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MPEGG_RAW_AU_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------