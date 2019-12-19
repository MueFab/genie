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
    };
    typedef std::vector<SubDescriptor> Descriptor;  //!<

    /**
     *
     */
    enum class GenomicDescriptor : uint8_t {
        POS = 0,
        RCOMP = 1,
        FLAGS = 2,
        MMPOS = 3,
        MMTYPE = 4,
        CLIPS = 5,
        UREADS = 6,
        RLEN = 7,
        PAIR = 8,
        MSCORE = 9,
        MMAP = 10,
        MSAR = 11,
        RTYPE = 12,
        RGROUP = 13,
        QV = 14,
        RNAME = 15,
        RFTP = 16,
        RFTT = 17
    };

    /**
     *
     */
    struct GenomicDescriptorProperties {
        std::string name;             //!<
        uint8_t number_subsequences;  //!<
    };
    static constexpr size_t NUM_DESCRIPTORS = 18;  //!<

    /**
     *
     */
    struct Alphabet {
        std::vector<char> lut;         //!<
        std::vector<char> inverseLut;  //!<
    };

    /**
     *
     * @return
     */
    static const std::vector<GenomicDescriptorProperties>& getDescriptorProperties();

    /**
     *
     * @param desc
     * @param sub
     * @return
     */
    SubDescriptor& get(GenomicDescriptor desc, size_t sub);

    /**
     *
     * @param desc
     * @param sub
     * @return
     */
    const SubDescriptor& get(GenomicDescriptor desc, size_t sub) const;

    /**
     *
     * @param id
     * @return
     */
    static const Alphabet& getAlphabetProperties(format::mpegg_p2::ParameterSet::AlphabetID id);

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
    std::vector<Descriptor> descriptors;                         //!<
    std::unique_ptr<format::mpegg_p2::ParameterSet> parameters;  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MPEGG_RAW_AU_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------