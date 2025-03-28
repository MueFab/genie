/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_ACCESS_UNIT_H_
#define SRC_GENIE_CORE_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <numeric>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/mismatch_decoder.h"
#include "genie/core/parameter/parameter_set.h"
#include "genie/core/reference_manager.h"
#include "genie/core/stats/perf_stats.h"
#include "genie/util/data_block.h"

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
        util::DataBlock data_;  //!< @brief
        size_t position_{};     //!< @brief

        GenSubIndex id_;       //!< @brief
        size_t numSymbols{};  //!< @brief

        util::DataBlock dependency;                  //!< @brief
        std::unique_ptr<MismatchDecoder> mmDecoder;  //!< @brief

     public:
        /**
         * @brief
         * @param sub
         * @return
         */
        Subsequence& operator=(const Subsequence& sub);

        /**
         * @brief
         * @param sub
         * @return
         */
        Subsequence& operator=(Subsequence&& sub) noexcept;

        /**
         * @brief
         * @param sub
         */
        Subsequence(const Subsequence& sub);

        /**
         * @brief
         * @param sub
         */
        Subsequence(Subsequence&& sub) noexcept;

        /**
         * @brief
         * @return
         */
        util::DataBlock* GetDependency();

        /**
         * @brief
         * @param mm
         */
        core::AccessUnit::Subsequence AttachMismatchDecoder(std::unique_ptr<MismatchDecoder> mm);

        /**
         * @brief
         * @return
         */
        MismatchDecoder* getMismatchDecoder() const;

        /**
         * @brief
         * @param wordSize
         * @param _id
         */
        Subsequence(uint8_t wordSize, GenSubIndex _id);

        /**
         * @brief
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
         * @param val
         */
        void pushDependency(uint64_t val);

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
         */
        util::DataBlock&& Move();

        /**
         * @brief
         * @return
         */
        GenSubIndex GetId() const;

        /**
         * @brief
         * @return
         */
        uint64_t pull();

        /**
         * @brief
         * @return
         */
        size_t GetNumSymbols() const;

        /**
         * @brief
         * @param num
         */
        void AnnotateNumSymbols(size_t num);

        /**
         * @brief
         * @return
         */
        bool IsEmpty() const;

        /**
         * @brief
         * @return
         */
        size_t GetRawSize() const;

        /**
         * @brief
         * @param writer
         */
        void Write(util::BitWriter& writer) const;

        /**
         * @brief
         * @param _id
         * @param size
         * @param reader
         */
        Subsequence(GenSubIndex _id, size_t size, util::BitReader& reader);

        /**
         * @brief
         * @param _id
         */
        explicit Subsequence(GenSubIndex _id);

        /**
         * @brief
         * @param _id
         * @param dat
         */
        Subsequence(GenSubIndex _id, util::DataBlock&& dat);

        /**
         * @brief
         * @param dat
         */
        void Set(util::DataBlock&& dat);

        /**
         * @brief
         * @param pos
         */
        void setPosition(size_t pos);

        /**
         * @brief
         * @return
         */
        util::DataBlock& getData();
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
        Subsequence& Get(uint16_t sub);

        /**
         * @brief
         * @param sub
         * @return
         */
        const Subsequence& Get(uint16_t sub) const;

        /**
         * @brief
         * @param pos
         * @param _type
         * @return
         */
        Subsequence& getTokenType(uint16_t pos, uint8_t _type);

        /**
         * @brief
         * @return
         */
        GenDesc GetId() const;

        /**
         * @brief
         * @param sub
         */
        void Add(Subsequence&& sub);

        /**
         * @brief
         * @param _id
         * @param sub
         */
        void Set(uint16_t _id, Subsequence&& sub);

        /**
         * @brief
         * @param _id
         */
        explicit Descriptor(GenDesc _id);

        /**
         * @brief
         * @return
         */
        Subsequence* begin();

        /**
         * @brief
         * @return
         */
        Subsequence* end();

        /**
         * @brief
         * @return
         */
        const Subsequence* begin() const;

        /**
         * @brief
         * @return
         */
        const Subsequence* end() const;

        /**
         * @brief
         * @return
         */
        size_t GetSize() const;

        /**
         * @brief
         * @return
         */
        size_t getWrittenSize() const;

        /**
         * @brief
         * @param writer
         */
        void write(util::BitWriter& writer) const;

        /**
         * @brief
         * @param _id
         * @param count
         * @param remainingSize
         * @param reader
         */
        Descriptor(GenDesc _id, size_t count, size_t remainingSize, util::BitReader& reader);

        /**
         * @brief
         * @return
         */
        bool isEmpty() const;

        /**
         * @brief
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
    void Set(GenSubIndex sub, Subsequence&& data);

    /**
     * @brief
     * @param sub
     * @param data
     */
    void Set(GenDesc sub, Descriptor&& data);

    /**
     * @brief
     * @param sub
     * @param value
     */
    void push(GenSubIndex sub, uint64_t value);

    /**
     * @brief
     * @param sub
     * @param value
     */
    void pushDependency(GenSubIndex sub, uint64_t value);

    /**
     * @brief
     * @param sub
     * @return
     */
    bool isEnd(GenSubIndex sub);

    /**
     * @brief
     * @param sub
     * @param lookahead
     * @return
     */
    uint64_t peek(GenSubIndex sub, size_t lookahead = 0);

    /**
     * @brief
     * @param sub
     * @return
     */
    uint64_t pull(GenSubIndex sub);

    /**
     * @brief
     * @param set
     * @param _numRecords
     */
    AccessUnit(parameter::EncodingSet&& set, size_t _numRecords);

    /**
     * @brief
     * @param _parameters
     */
    void setParameters(parameter::EncodingSet&& _parameters);

    /**
     * @brief
     * @return
     */
    const parameter::EncodingSet& getParameters() const;

    /**
     * @brief
     * @return
     */
    parameter::EncodingSet& getParameters();

    /**
     * @brief
     * @return
     */
    parameter::EncodingSet&& moveParameters();

    /**
     * @brief
     * @return
     */
    size_t getNumReads() const;

    /**
     * @brief
     * @param recs
     */
    void setNumReads(size_t recs);

    /**
     * @brief
     * @return
     */
    record::ClassType getClassType() const;

    /**
     * @brief
     * @param _type
     */
    void setClassType(record::ClassType _type);

    /**
     * @brief
     */
    void clear();

    /**
     * @brief
     */
    void addRecord();

    /**
     * @brief
     * @param ref
     */
    void setReference(uint16_t ref);

    /**
     * @brief
     * @return
     */
    uint16_t getReference();

    /**
     * @brief
     * @param pos
     */
    void setMaxPos(uint64_t pos);

    /**
     * @brief
     * @param pos
     */
    void setMinPos(uint64_t pos);

    /**
     * @brief
     * @return
     */
    uint64_t getMaxPos() const;

    /**
     * @brief
     * @return
     */
    uint64_t getMinPos() const;

    /**
     * @brief
     * @return
     */
    Descriptor* begin();

    /**
     * @brief
     * @return
     */
    Descriptor* end();

    /**
     * @brief
     * @return
     */
    const Descriptor* begin() const;

    /**
     * @brief
     * @return
     */
    const Descriptor* end() const;

    /**
     * @brief
     * @return
     */
    stats::PerfStats& getStats();

    /**
     * @brief
     * @param _stats
     */
    void setStats(stats::PerfStats&& _stats);

    /**
     * @brief
     * @param ex
     * @param ref2Write
     */
    void setReference(const ReferenceManager::ReferenceExcerpt& ex,
                      const std::vector<std::pair<size_t, size_t>>& ref2Write);

    /**
     * @brief
     * @return
     */
    const ReferenceManager::ReferenceExcerpt& getReferenceExcerpt() const;

    /**
     * @brief
     * @return
     */
    const std::vector<std::pair<size_t, size_t>>& getRefToWrite() const;

    /**
     * @brief
     * @return
     */
    bool isReferenceOnly() const;

    /**
     * @brief
     * @param ref
     */
    void setReferenceOnly(bool ref);

 private:
    std::vector<Descriptor> descriptors;                //!< @brief
    parameter::EncodingSet parameters;                  //!< @brief
    stats::PerfStats stats;                             //!< @brief
    ReferenceManager::ReferenceExcerpt reference;       //!< @brief
    std::vector<std::pair<size_t, size_t>> refToWrite;  //!< @brief
    bool referenceOnly{false};                          //!< @brief

    size_t numReads;  //!< @brief

    record::ClassType type{};  //!< @brief
    uint64_t minPos;           //!< @brief
    uint64_t maxPos;           //!< @brief

    uint16_t referenceSequence;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_ACCESS_UNIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
