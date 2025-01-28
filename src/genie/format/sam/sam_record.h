/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_RECORD_H_
#define SRC_GENIE_FORMAT_SAM_SAM_RECORD_H_

// -----------------------------------------------------------------------------

#include <htslib/sam.h>

#include <string>
#include <vector>
#include <optional>

// -----------------------------------------------------------------------------

namespace genie::format::sam {

/**
 * @brief
 */
class SamRecord {
 public:

  struct Tag {
    std::string tag;  //!< @brief Tag name
    std::string value;  //!< @brief Tag value
  };

  std::string qname_;  //!< @brief Query template name
  uint16_t flag_;      //!< @brief Flag
  int32_t rid_;        //!< @brief Reference sequence ID
  uint32_t pos_;       //!< @brief Position
  uint8_t mapq_;       //!< @brief Mapping Quality
  std::string cigar_;  //!< @brief CIGAR
  int32_t mate_rid_;   //!< @brief Mate reference sequence ID
  uint32_t mate_pos_;  //!< @brief Mate position
  std::string seq_;    //!< @brief Read sequence
  std::string qual_;   //!< @brief
  std::vector<Tag> optional_fields_;

  /**
   * @brief
   * @param rec
   * @return
   */
  bool operator==(const SamRecord& rec) const {
    return qname_ == rec.qname_ && flag_ == rec.flag_ && rid_ == rec.rid_ &&
           pos_ == rec.pos_ && mapq_ == rec.mapq_ && cigar_ == rec.cigar_ &&
           mate_rid_ == rec.mate_rid_ && mate_pos_ == rec.mate_pos_ &&
           seq_ == rec.seq_ && qual_ == rec.qual_;
  }

  /**
   * @brief
   * @param int_base
   * @return
   */
  static char FourBitBase2Char(uint8_t int_base);

  /**
   * @brief
   * @param sam_alignment
   * @return
   */
  static std::string GetCigarString(const bam1_t* sam_alignment);

  /**
   * @brief
   * @param sam_alignment
   * @return
   */
  static std::string GetSeqString(const bam1_t* sam_alignment);

  /**
   * @brief
   * @param sam_alignment
   * @return
   */
  static std::string GetQualString(const bam1_t* sam_alignment);

  /**
   * @brief
   * @param token
   * @return
   */
  static char ConvertCigar2ECigarChar(char token);

  /**
   * @brief
   * @param token
   * @return
   */
  static int StepSequence(char token);

  /**
   * @brief
   * @param cigar
   * @param seq
   * @return
   */
  static std::string ConvertCigar2ECigar(const std::string& cigar,
                                         const std::string& seq);

  /**
   * @brief
   */
  SamRecord();

  /**
   * @brief
   * @param sam_alignment
   */
  explicit SamRecord(const bam1_t* sam_alignment);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::string& GetQname() const;

  /**
   * @brief
   * @return
   */
  std::string&& MoveQname();

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint16_t GetFlag() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] int32_t GetRid() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint32_t GetPos() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetMapq() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::string& GetCigar() const;

  /**
   * @brief
   * @return
   */
  std::string&& MoveCigar();

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::string GetECigar() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] int32_t GetMrid() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint32_t GetMPos() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::string& GetSeq() const;

  /**
   * @brief
   * @return
   */
  std::string&& MoveSeq();

  /**
   * @brief
   * @param seq
   */
  void SetSeq(std::string&& seq);

  /**
   * @brief
   * @param seq
   */
  void SetSeq(const std::string& seq);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::string& GetQual() const;

  /**
   * @brief
   * @return
   */
  std::string&& MoveQual();

  /**
   * @brief
   * @param qual
   */
  void SetQual(const std::string& qual);

  /**
   * @brief
   * @param flag
   * @return
   */
  [[nodiscard]] bool CheckFlag(uint16_t flag) const;

  /**
   * @brief
   * @param flag
   * @return
   */
  [[nodiscard]] bool CheckNFlag(uint16_t flag) const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsUnmapped() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsMateUnmapped() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsPrimary() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsSecondary() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsDuplicates() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsSupplementary() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsPaired() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsRead1() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsRead2() const;

  /**
   * @brief SamRecord are correctly oriented with respect to one another,
   * i.e. that one of the mate pairs maps to the forward strand and the other
   * maps to the reverse strand. If the mates don't map in a proper pair, that
   * may mean that both reads map to the forward or reverse strand. This
   * includes that the reads are mapped to the same chromosomes.
   * @return
   */
  [[nodiscard]] bool IsProperlyPaired() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsPairedAndBothMapped() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsReverse() const;

  /**
   * @brief
   * @param r
   * @return
   */
  [[nodiscard]] bool IsPairOf(const SamRecord& r) const;

  void write(std::ostream& os) const;

  explicit SamRecord(std::ifstream& is);

  void ParseOptionalField(const std::string& field);
};

using SamRecordPair = std::pair<SamRecord, std::optional<SamRecord>>;

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_RECORD_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
