/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_ACCESS_UNIT_H_
#define SRC_GENIE_CORE_ACCESS_UNIT_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/mismatch_decoder.h"
#include "genie/core/parameter/parameter_set.h"
#include "genie/core/reference_manager.h"
#include "genie/core/stats/perf_stats.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class AccessUnit {
 public:
  /**
   * @brief
   */
  class Subsequence {
    util::DataBlock data_;  //!< @brief
    size_t position_{};     //!< @brief

    GenSubIndex id_;        //!< @brief
    size_t num_symbols_{};  //!< @brief

    util::DataBlock dependency_;                   //!< @brief
    std::unique_ptr<MismatchDecoder> mm_decoder_;  //!< @brief

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
    Subsequence AttachMismatchDecoder(std::unique_ptr<MismatchDecoder> mm);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] MismatchDecoder* GetMismatchDecoder() const;

    /**
     * @brief
     * @param word_size
     * @param id
     */
    Subsequence(uint8_t word_size, GenSubIndex id);

    /**
     * @brief
     * @param d
     * @param id
     */
    Subsequence(util::DataBlock d, GenSubIndex id);

    /**
     * @brief
     * @param val
     */
    void Push(uint64_t val);

    /**
     * @brief
     * @param val
     */
    void PushDependency(uint64_t val);

    /**
     * @brief
     */
    void Inc();

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint64_t Get(size_t lookahead = 0) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool end() const;

    /**
     * @brief
     */
    util::DataBlock&& Move();

    /**
     * @brief
     * @return
     */
    [[nodiscard]] GenSubIndex GetId() const;

    /**
     * @brief
     * @return
     */
    uint64_t Pull();

    /**
     * @brief
     * @return
     */
    [[nodiscard]] size_t GetNumSymbols() const;

    /**
     * @brief
     * @param num
     */
    void AnnotateNumSymbols(size_t num);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool IsEmpty() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] size_t GetRawSize() const;

    /**
     * @brief
     * @param writer
     */
    void Write(util::BitWriter& writer) const;

    /**
     * @brief
     * @param id
     * @param size
     * @param reader
     */
    Subsequence(GenSubIndex id, size_t size, util::BitReader& reader);

    /**
     * @brief
     * @param id
     */
    explicit Subsequence(GenSubIndex id);

    /**
     * @brief
     * @param id
     * @param dat
     */
    Subsequence(GenSubIndex id, util::DataBlock&& dat);

    /**
     * @brief
     * @param dat
     */
    void Set(util::DataBlock&& dat);

    /**
     * @brief
     * @param pos
     */
    void SetPosition(size_t pos);

    /**
     * @brief
     * @return
     */
    util::DataBlock& GetData();
  };

  /**
   * @brief
   */
  class Descriptor {
    std::vector<Subsequence> subdesc_;  //!< @brief
    GenDesc id_;                        //!< @brief

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
    [[nodiscard]] const Subsequence& Get(uint16_t sub) const;

    /**
     * @brief
     * @param pos
     * @param type
     * @return
     */
    Subsequence& GetTokenType(uint16_t pos, uint8_t type);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] GenDesc GetId() const;

    /**
     * @brief
     * @param sub
     */
    void Add(Subsequence&& sub);

    /**
     * @brief
     * @param id
     * @param sub
     */
    void Set(uint16_t id, Subsequence&& sub);

    /**
     * @brief
     * @param id
     */
    explicit Descriptor(GenDesc id);

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
    [[nodiscard]] const Subsequence* begin() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const Subsequence* end() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] size_t GetSize() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] size_t GetWrittenSize() const;

    /**
     * @brief
     * @param writer
     */
    void Write(util::BitWriter& writer) const;

    /**
     * @brief
     * @param id
     * @param count
     * @param remaining_size
     * @param reader
     */
    Descriptor(GenDesc id, size_t count, size_t remaining_size,
               util::BitReader& reader);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool IsEmpty() const;

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
  [[nodiscard]] const Subsequence& Get(GenSubIndex sub) const;

  /**
   * @brief
   * @param sub
   * @return
   */
  Subsequence& Get(GenSubIndex sub);

  /**
   *
   * @param desc
   * @return
   */
  Descriptor& Get(GenDesc desc);

  /**
   *
   * @param desc
   * @return
   */
  [[nodiscard]] const Descriptor& Get(GenDesc desc) const;

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
  void Push(const GenSubIndex& sub, uint64_t value);

  /**
   * @brief
   * @param sub
   * @param value
   */
  void PushDependency(const GenSubIndex& sub, uint64_t value);

  /**
   * @brief
   * @param sub
   * @return
   */
  bool IsEnd(const GenSubIndex& sub);

  /**
   * @brief
   * @param sub
   * @param lookahead
   * @return
   */
  uint64_t Peek(const GenSubIndex& sub, size_t lookahead = 0);

  /**
   * @brief
   * @param sub
   * @return
   */
  uint64_t Pull(const GenSubIndex& sub);

  /**
   * @brief
   * @param set
   * @param num_records
   */
  AccessUnit(parameter::EncodingSet&& set, size_t num_records);

  /**
   * @brief
   * @param parameters
   */
  void SetParameters(parameter::EncodingSet&& parameters);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const parameter::EncodingSet& GetParameters() const;

  /**
   * @brief
   * @return
   */
  parameter::EncodingSet& GetParameters();

  /**
   * @brief
   * @return
   */
  parameter::EncodingSet&& MoveParameters();

  /**
   * @brief
   * @return
   */
  [[nodiscard]] size_t GetNumReads() const;

  /**
   * @brief
   * @param recs
   */
  void SetNumReads(size_t recs);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] record::ClassType GetClassType() const;

  /**
   * @brief
   * @param type
   */
  void SetClassType(record::ClassType type);

  /**
   * @brief
   */
  void Clear();

  /**
   * @brief
   */
  void AddRecord();

  /**
   * @brief
   * @param ref
   */
  void SetReference(uint16_t ref);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint16_t GetReference() const;

  /**
   * @brief
   * @param pos
   */
  void SetMaxPos(uint64_t pos);

  /**
   * @brief
   * @param pos
   */
  void SetMinPos(uint64_t pos);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint64_t GetMaxPos() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint64_t GetMinPos() const;

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
  [[nodiscard]] const Descriptor* begin() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const Descriptor* end() const;

  /**
   * @brief
   * @return
   */
  stats::PerfStats& GetStats();

  /**
   * @brief
   * @param stats
   */
  void SetStats(stats::PerfStats&& stats);

  /**
   * @brief
   * @param ex
   * @param ref2_write
   */
  void SetReference(const ReferenceManager::ReferenceExcerpt& ex,
                    const std::vector<std::pair<size_t, size_t>>& ref2_write);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const ReferenceManager::ReferenceExcerpt& GetReferenceExcerpt()
      const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::vector<std::pair<size_t, size_t>>& GetRefToWrite()
      const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsReferenceOnly() const;

  /**
   * @brief
   * @param ref
   */
  void SetReferenceOnly(bool ref);

 private:
  std::vector<Descriptor> descriptors_;                  //!< @brief
  parameter::EncodingSet parameters_;                    //!< @brief
  stats::PerfStats stats_;                               //!< @brief
  ReferenceManager::ReferenceExcerpt reference_;         //!< @brief
  std::vector<std::pair<size_t, size_t>> ref_to_write_;  //!< @brief
  bool reference_only_;                                  //!< @brief

  size_t num_reads_;  //!< @brief

  record::ClassType type_;  //!< @brief
  uint64_t min_pos_;        //!< @brief
  uint64_t max_pos_;        //!< @brief

  uint16_t reference_sequence_;  //!< @brief
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_ACCESS_UNIT_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
