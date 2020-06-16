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

        GenSubIndex id;  //!< @brief
        size_t numSymbols;

       public:
        /**
         * @brief
         * @param wordsize
         */
        Subsequence(size_t wordsize, GenSubIndex _id);

        /**
         * @brief
         * @param d
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

        void annotateNumSymbols(size_t num) {
            numSymbols = num;
        }

        bool isEmpty() const {
            return !getNumSymbols();
        }

        size_t getRawSize() const {
            return data.getRawSize();
        }

        void write(util::BitWriter& writer) const {
            writer.writeBuffer(data.getData(), data.getRawSize());
        }

        Subsequence(GenSubIndex _id, size_t size, util::BitReader& reader)
            : data(0, 1), id(std::move(_id)), numSymbols(0) {
            data.reserve(size);
            for (size_t i = 0; i < size; ++i) {
                data.push_back(reader.read(8));
            }
        }

        Subsequence(GenSubIndex _id)
            : data(0, 1), id(_id), numSymbols(0) {
        }

        Subsequence(GenSubIndex _id, util::DataBlock&& dat)
            : data(std::move(dat)), id(std::move(_id)), numSymbols(0) {
        }

        void set(util::DataBlock&& dat) {
            data = std::move(dat);
        }

        void setPosition(size_t pos) {
            position = pos;
        }

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
         * @param type
         * @return
         */
        Subsequence& getTokenType(uint16_t pos, uint8_t type);

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

        size_t getWrittenSize() const {
            size_t overhead = getDescriptor(getID()).tokentype ? 0 : (subdesc.size() - 1) * sizeof(uint32_t);
            return std::accumulate(subdesc.begin(), subdesc.end(), overhead,
                                   [](size_t sum, const Subsequence& payload) {
                                     return payload.isEmpty() ? sum : sum + payload.getRawSize();
                                   });
        }

        void write(util::BitWriter& writer) const {
            if (this->id == GenDesc::RNAME || this->id == GenDesc::MSAR) {
                subdesc.front().write(writer);
                return;
            }
            for (size_t i = 0; i < subdesc.size(); ++i) {
                if (i < (subdesc.size() - 1)) {
                    writer.write(subdesc[i].getRawSize(), 32);
                }
                subdesc[i].write(writer);
            }
        }

        Descriptor(GenDesc _id, size_t count, size_t remainingSize,
                                                                util::BitReader& reader)
            : id(_id) {
            if (this->id == GenDesc::RNAME || this->id == GenDesc::MSAR) {
                subdesc.emplace_back(GenSubIndex{_id, 0}, remainingSize, reader);
                return;
            }
            for (size_t i = 0; i < count; ++i) {
                size_t s = 0;
                if (i < (count - 1)) {
                    s = reader.read(32);
                    remainingSize -= (s + 4);
                } else {
                    s = remainingSize;
                }
                if (s) {
                    subdesc.emplace_back(GenSubIndex{_id, i}, s, reader);
                } else {
                    subdesc.emplace_back(GenSubIndex{_id, i}, util::DataBlock(0, 4));
                }
            }
        }

        bool isEmpty() const{
            for(const auto& d : subdesc) {
                if(!d.isEmpty()) {
                    return false;
                }
            }
            return true;
        }

        Descriptor() : id(GenDesc(0)) {}
    };

    /**
     * @brief
     * @param desc
     * @param sub
     * @return
     */
    const Subsequence& get(GenSubIndex sub) const;

    /**
     * @brief
     * @param desc
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


    Descriptor&& move(GenDesc desc) {
        return std::move(get(desc));
    }

    /**
     *
     * @param desc
     * @return
     */
    const Descriptor& get(GenDesc desc) const;

    /**
     * @brief
     * @param desc
     * @param sub
     * @param data
     */
    void set(GenSubIndex sub, Subsequence&& data);

    void set(GenDesc sub, Descriptor&& data) {
        descriptors[uint8_t(sub)] = std::move(data);
    }

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
     * @brief
     * @param set
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

    stats::PerfStats& getStats() { return stats; }

    void setStats(stats::PerfStats&& _stats) { stats = std::move(_stats); }

   private:
    std::vector<Descriptor> descriptors;  //!< @brief
    parameter::ParameterSet parameters;   //!< @brief
    stats::PerfStats stats;

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