/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_SPRING_ENCODING_IMPL_H_
#define SRC_GENIE_READ_SPRING_SPRING_ENCODING_IMPL_H_

// -----------------------------------------------------------------------------

#include <iostream>
#include <list>
#include <string>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

// -----------------------------------------------------------------------------
template <size_t BitsetSize>
EncoderGlobalB<BitsetSize>::EncoderGlobalB(const int max_read_len_param) {
  max_read_len = max_read_len_param;
  base_mask = new std::bitset<BitsetSize>*[max_read_len_param];
  for (int i = 0; i < max_read_len_param; i++)
    base_mask[i] = new std::bitset<BitsetSize>[128];
}

// -----------------------------------------------------------------------------
template <size_t BitsetSize>
EncoderGlobalB<BitsetSize>::~EncoderGlobalB() {
  for (int i = 0; i < max_read_len; i++) delete[] base_mask[i];
  delete[] base_mask;
}

// -----------------------------------------------------------------------------
template <size_t BitsetSize>
std::string BitsetToString(std::bitset<BitsetSize> b, const uint16_t read_len,
                           const EncoderGlobalB<BitsetSize>& egb) {
  // destroys bitset b
  static constexpr char rev_int_to_char[8] = {'A', 'N', 'G', 0, 'C', 0, 'T', 0};
  std::string s;
  s.resize(read_len);
  for (int i = 0; i < 3 * read_len / 63 + 1; i++) {
    uint64_t ull = (b & egb.mask63).to_ullong();
    b >>= 63;
    for (int j = 21 * i; j < 21 * i + 21 && j < read_len; j++) {
      s[j] = rev_int_to_char[ull % 8];
      ull /= 8;
    }
  }
  return s;
}

// -----------------------------------------------------------------------------
template <size_t BitsetSize>
void Encode(std::bitset<BitsetSize>* read, BbHashDict* dict, uint32_t* order_s,
            uint16_t* read_lengths_s, const EncoderGlobal& eg,
            const EncoderGlobalB<BitsetSize>& egb) {
  static constexpr int kThresh_S = kThresh_Encoder;
  static constexpr int kMaxSearch = kMax_Search_Encoder;
#ifdef GENIE_USE_OPENMP
  auto* read_lock = new OmpLock[eg.num_reads_s + eg.num_reads_n];
  auto* dict_lock = new OmpLock[eg.num_reads_s + eg.num_reads_n];
#endif
  auto remaining_reads = new bool[eg.num_reads_s + eg.num_reads_n];
  std::fill(remaining_reads, remaining_reads + eg.num_reads_s + eg.num_reads_n,
            1);

  auto* mask1 = new std::bitset<BitsetSize>[eg.num_dict_s];
  GenerateIndexMasks<BitsetSize>(mask1, dict, eg.num_dict_s, 3);
  auto** mask = new std::bitset<BitsetSize>*[eg.max_read_len];
  for (int i = 0; i < eg.max_read_len; i++)
    mask[i] = new std::bitset<BitsetSize>[eg.max_read_len];
  GenerateMasks<BitsetSize>(mask, eg.max_read_len, 3);

  //
  // This is the 3rd hottest parallel region in genie (behind gabac
  // parallelization and 3rd parallel region in reorder.h).  It shows
  // good load balancing and benefits from parallelization.
  //
  std::cerr << "Encoding reads\n";
#ifdef GENIE_USE_OPENMP
#pragma omp parallel num_threads(eg.num_thr) default(none)            \
    shared(eg, egb, dict, mask1, mask, read, order_s, read_lengths_s, \
               remaining_reads, read_lock, dict_lock, kCharToRevChar)
#endif
  {
#ifdef GENIE_USE_OPENMP
    int tid = omp_get_thread_num();
#else
    int tid = 0;
#endif
    std::ifstream f(eg.infile + '.' + std::to_string(tid), std::ios::binary);
    UTILS_DIE_IF(!f, "Cannot open file to read: " + eg.infile + '.' +
                         std::to_string(tid));
    std::ifstream in_flag(eg.infile_flag + '.' + std::to_string(tid));
    UTILS_DIE_IF(!in_flag, "Cannot open file to read: " + eg.infile_flag + '.' +
                               std::to_string(tid));
    std::ifstream in_pos(eg.infile_pos + '.' + std::to_string(tid),
                         std::ios::binary);
    UTILS_DIE_IF(!in_pos, "Cannot open file to read: " + eg.infile_pos + '.' +
                              std::to_string(tid));
    std::ifstream in_order(eg.infile_order + '.' + std::to_string(tid),
                           std::ios::binary);
    UTILS_DIE_IF(!in_order, "Cannot open file to read: " + eg.infile_order +
                                '.' + std::to_string(tid));
    std::ifstream in_rc(eg.infile_rc + '.' + std::to_string(tid));
    UTILS_DIE_IF(!in_rc, "Cannot open file to read: " + eg.infile_rc + '.' +
                             std::to_string(tid));
    std::ifstream in_read_length(
        eg.infile_read_length + '.' + std::to_string(tid), std::ios::binary);
    UTILS_DIE_IF(!in_read_length,
                 "Cannot open file to read: " + eg.infile_read_length + '.' +
                     std::to_string(tid));
    std::ofstream f_seq(eg.outfile_seq + '.' + std::to_string(tid));
    UTILS_DIE_IF(!f_seq, "Cannot open file to write: " + eg.outfile_seq + '.' +
                             std::to_string(tid));
    std::ofstream f_pos(eg.outfile_pos + '.' + std::to_string(tid),
                        std::ios::binary);
    UTILS_DIE_IF(!f_pos, "Cannot open file to write: " + eg.outfile_pos + '.' +
                             std::to_string(tid));
    std::ofstream f_noise(eg.outfile_noise + '.' + std::to_string(tid));
    UTILS_DIE_IF(!f_noise, "Cannot open file to write: " + eg.outfile_noise +
                               '.' + std::to_string(tid));
    std::ofstream f_noise_pos(eg.outfile_noise_pos + '.' + std::to_string(tid),
                              std::ios::binary);
    UTILS_DIE_IF(!f_noise_pos,
                 "Cannot open file to write: " + eg.outfile_noise_pos + '.' +
                     std::to_string(tid));
    std::ofstream f_order(eg.infile_order + '.' + std::to_string(tid) + ".tmp",
                          std::ios::binary);
    UTILS_DIE_IF(!f_order, "Cannot open file to write: " + eg.infile_order +
                               '.' + std::to_string(tid) + ".tmp");
    std::ofstream f_rc(eg.infile_rc + '.' + std::to_string(tid) + ".tmp");
    UTILS_DIE_IF(!f_rc, "Cannot open file to write: " + eg.infile_rc + '.' +
                            std::to_string(tid) + ".tmp");
    std::ofstream f_read_length(
        eg.infile_read_length + '.' + std::to_string(tid) + ".tmp",
        std::ios::binary);
    UTILS_DIE_IF(!f_read_length,
                 "Cannot open file to write: " + eg.infile_read_length + '.' +
                     std::to_string(tid) + ".tmp");
    // in the dict read_id array
    uint64_t start_pos_idx;  // index in start_pos
    uint64_t ull;
    uint64_t abs_pos = 0;  // absolute position in reference (total length
    // of all contigs till now)
    bool flag = false;
    // flag to check if match was found or not
    std::string current, ref;
    std::bitset<BitsetSize> forward_bitset, reverse_bitset, b;
    char c = '0', rc = 'd';
    std::list<ContigReads> current_contig;
    int64_t p = 0;
    uint16_t rl = 0;
    uint32_t ord = 0,
             list_size = 0;  // list_size variable introduced because
    // list::Size() was running very slowly
    // on UIUC machine
    auto* deleted_rids = new std::list<uint32_t>[eg.num_dict_s];
    bool done = false;
    while (!done) {
      if (!(in_flag >> c)) done = true;
      if (!done) {
        ReadDnaFromBits(current, f);
        rc = static_cast<char>(in_rc.get());
        in_pos.read(reinterpret_cast<char*>(&p), sizeof(int64_t));
        in_order.read(reinterpret_cast<char*>(&ord), sizeof(uint32_t));
        in_read_length.read(reinterpret_cast<char*>(&rl), sizeof(uint16_t));
      }
      if (c == '0' || done || list_size > 10000000) {  // limit on list Size so
        // that memory doesn't get
        // too large
        if (list_size != 0) {
          // sort contig according to pos
          current_contig.sort([](const ContigReads& ar, const ContigReads& br) {
            return ar.pos < br.pos;
          });
          // make first pos zero and shift all pos values accordingly
          auto current_contig_it = current_contig.begin();
          int64_t first_pos = current_contig_it->pos;
          for (; current_contig_it != current_contig.end(); ++current_contig_it)
            current_contig_it->pos -= first_pos;

          ref = BuildContig(current_contig, list_size);
          if (static_cast<int64_t>(ref.size()) >= eg.max_read_len &&
              eg.num_reads_s + eg.num_reads_n > 0) {
            // try to align the singleton reads to ref
            // first create bitsets from first read_len positions of
            // ref
            forward_bitset.reset();
            reverse_bitset.reset();
            StringToBitset(ref.substr(0, eg.max_read_len),
                           static_cast<uint16_t>(eg.max_read_len),
                           forward_bitset, egb.base_mask);
            StringToBitset(ReverseComplement(ref.substr(0, eg.max_read_len),
                                             eg.max_read_len),
                           static_cast<uint16_t>(eg.max_read_len),
                           reverse_bitset, egb.base_mask);
            for (int64_t j = 0;
                 j < static_cast<int64_t>(ref.size()) - eg.max_read_len + 1;
                 j++) {
              // search for singleton reads
              for (int rev = 0; rev < 2; rev++) {
                for (int l = 0; l < eg.num_dict_s; l++) {
                  int64_t dictidx[2];
                  if (!rev)
                    b = forward_bitset & mask1[l];
                  else
                    b = reverse_bitset & mask1[l];
                  ull = (b >> 3 * dict[l].start_).to_ullong();
                  start_pos_idx = dict[l].boo_hash_fun_->lookup(ull);
                  if (start_pos_idx >= dict[l].num_keys_)  // not found
                    continue;
                  // check if any other thread is modifying
                  // same dictpos
#ifdef GENIE_USE_OPENMP
                  if (!dict_lock[start_pos_idx].Test()) continue;
#else
                  // if we are single-threaded we do not need
                  // to continue because nobody else could
                  // possibly try to modify the same dictpos
#endif
                  dict[l].FindPos(dictidx, start_pos_idx);
                  if (dict[l].empty_bin_[start_pos_idx]) {  // bin is empty
#ifdef GENIE_USE_OPENMP
                    dict_lock[start_pos_idx].Unset();
#endif
                    continue;
                  }
                  uint64_t ull1 =
                      ((read[dict[l].read_id_[dictidx[0]]] & mask1[l]) >>
                       3 * dict[l].start_)
                          .to_ullong();
                  if (ull == ull1) {  // checking if ull is actually
                                      // the key for this bin
                    for (int64_t i = dictidx[1] - 1;
                         i >= dictidx[0] && i >= dictidx[1] - kMaxSearch; i--) {
                      auto rid = dict[l].read_id_[i];
                      int hamming;
                      if (!rev)
                        hamming = static_cast<int>(
                            ((forward_bitset ^ read[rid]) &
                             mask[0][eg.max_read_len - read_lengths_s[rid]])
                                .count());
                      else
                        hamming = static_cast<int>(
                            ((reverse_bitset ^ read[rid]) &
                             mask[0][eg.max_read_len - read_lengths_s[rid]])
                                .count());
                      if (hamming <= kThresh_S) {
#ifdef GENIE_USE_OPENMP
                        read_lock[rid].Set();
#endif
                        if (remaining_reads[rid]) {
                          remaining_reads[rid] = false;
                          flag = true;
                        }
#ifdef GENIE_USE_OPENMP
                        read_lock[rid].Unset();
#endif
                      }
                      if (flag == 1) {  // match found
                        flag = false;
                        list_size++;
                        char l_rc = rev ? 'r' : 'd';
                        int64_t pos =
                            rev ? (j + eg.max_read_len - read_lengths_s[rid])
                                : j;
                        std::string read_string =
                            rev ? ReverseComplement(
                                      BitsetToString<BitsetSize>(
                                          read[rid], read_lengths_s[rid], egb),
                                      read_lengths_s[rid])
                                : BitsetToString<BitsetSize>(
                                      read[rid], read_lengths_s[rid], egb);
                        current_contig.push_back({read_string, pos, l_rc,
                                                  order_s[rid],
                                                  read_lengths_s[rid]});
                        for (int l1 = 0; l1 < eg.num_dict_s; l1++) {
                          if (read_lengths_s[rid] > dict[l1].end_)
                            deleted_rids[l1].push_back(rid);
                        }
                      }
                    }
                  }
#ifdef GENIE_USE_OPENMP
                  dict_lock[start_pos_idx].Unset();
#endif
                  // delete from dictionaries
                  for (int l1 = 0; l1 < eg.num_dict_s; l1++)
                    for (auto it = deleted_rids[l1].begin();
                         it != deleted_rids[l1].end();) {
                      b = read[*it] & mask1[l1];
                      ull = (b >> 3 * dict[l1].start_).to_ullong();
                      start_pos_idx = dict[l1].boo_hash_fun_->lookup(ull);
#ifdef GENIE_USE_OPENMP
                      if (!dict_lock[start_pos_idx].Test()) {
                        ++it;
                        continue;
                      }
#else
                    // nothing to be done
#endif
                      dict[l1].FindPos(dictidx, start_pos_idx);
                      dict[l1].Remove(dictidx, start_pos_idx, *it);
                      it = deleted_rids[l1].erase(it);
#ifdef GENIE_USE_OPENMP
                      dict_lock[start_pos_idx].Unset();
#endif
                    }
                }
              }
              if (j != static_cast<int64_t>(ref.size()) -
                           eg.max_read_len) {  // not at last
                                               // position,shift
                                               // bitsets
                forward_bitset >>= 3;
                forward_bitset = forward_bitset & mask[0][0];
                forward_bitset |=
                    egb.base_mask[eg.max_read_len - 1][static_cast<uint8_t>(
                        ref[j + eg.max_read_len])];
                reverse_bitset <<= 3;
                reverse_bitset = reverse_bitset & mask[0][0];
                reverse_bitset |= egb.base_mask[0][static_cast<uint8_t>(
                    kCharToRevChar[static_cast<uint8_t>(
                        ref[j + eg.max_read_len])])];
              }
            }  // end for
          }  // end if
          // sort contig according to pos
          current_contig.sort([](const ContigReads& ar, const ContigReads& br) {
            return ar.pos < br.pos;
          });
          WriteContig(ref, current_contig, f_seq, f_pos, f_noise, f_noise_pos,
                      f_order, f_rc, f_read_length, abs_pos);
        }
        if (!done) {
          current_contig = {{current, p, rc, ord, rl}};
          list_size = 1;
        }
      } else if (c == '1') {  // read found during rightward search
        current_contig.push_back({current, p, rc, ord, rl});
        list_size++;
      }
    }
    f.close();
    in_flag.close();
    in_pos.close();
    in_order.close();
    in_rc.close();
    in_read_length.close();
    f_seq.close();
    f_pos.close();
    f_noise.close();
    f_noise_pos.close();
    f_order.close();
    f_read_length.close();
    f_rc.close();
    delete[] deleted_rids;
  }  // end omp parallel

  auto* file_len_seq_thr = new uint64_t[eg.num_thr];
  for (int tid = 0; tid < eg.num_thr; tid++) {
    std::ifstream in_seq(eg.outfile_seq + '.' + std::to_string(tid));
    UTILS_DIE_IF(!in_seq, "Cannot open file to write: " + eg.outfile_seq + '.' +
                              std::to_string(tid));
    in_seq.seekg(0, std::ifstream::end);
    file_len_seq_thr[tid] = in_seq.tellg();
    in_seq.close();
  }
  // Combine files produced by the threads
  std::ofstream f_order(eg.infile_order, std::ios::binary);
  std::ofstream f_read_length(eg.infile_read_length, std::ios::binary);
  std::ofstream f_noise_pos(eg.outfile_noise_pos, std::ios::binary);
  std::ofstream f_noise(eg.outfile_noise);
  std::ofstream f_rc(eg.infile_rc);
  std::ofstream f_seq(eg.outfile_seq);
  for (int tid = 0; tid < eg.num_thr; tid++) {
    std::ifstream in_seq(eg.outfile_seq + '.' + std::to_string(tid));
    UTILS_DIE_IF(!in_seq, "Cannot open file to read: " + eg.outfile_seq + '.' +
                              std::to_string(tid));
    std::ifstream in_order(eg.infile_order + '.' + std::to_string(tid) + ".tmp",
                           std::ios::binary);
    UTILS_DIE_IF(!in_order, "Cannot open file to read: " + eg.infile_order +
                                '.' + std::to_string(tid) + ".tmp");
    std::ifstream in_read_length(
        eg.infile_read_length + '.' + std::to_string(tid) + ".tmp",
        std::ios::binary);
    UTILS_DIE_IF(!in_read_length,
                 "Cannot open file to read: " + eg.infile_read_length + '.' +
                     std::to_string(tid) + ".tmp");
    std::ifstream in_rc(eg.infile_rc + '.' + std::to_string(tid) + ".tmp");
    UTILS_DIE_IF(!in_rc, "Cannot open file to read: " + eg.infile_rc + '.' +
                             std::to_string(tid) + ".tmp");
    std::ifstream in_noise_pos(eg.outfile_noise_pos + '.' + std::to_string(tid),
                               std::ios::binary);
    UTILS_DIE_IF(!in_noise_pos,
                 "Cannot open file to read: " + eg.outfile_noise_pos + '.' +
                     std::to_string(tid));
    std::ifstream in_noise(eg.outfile_noise + '.' + std::to_string(tid));
    UTILS_DIE_IF(!in_noise, "Cannot open file to read: " + eg.outfile_noise +
                                '.' + std::to_string(tid));
    f_seq << in_seq.rdbuf();
    f_seq.clear();
    f_order << in_order.rdbuf();
    f_order.clear();  // clearStreamState error flag in case in_order is empty
    f_noise_pos << in_noise_pos.rdbuf();
    f_noise_pos.clear();  // clearStreamState error flag in case in_noisepos
                          // is empty
    f_noise << in_noise.rdbuf();
    f_noise.clear();  // clearStreamState error flag in case in_noise is empty
    f_read_length << in_read_length.rdbuf();
    f_read_length.clear();  // clearStreamState error flag in case
                            // in_readlength is empty
    f_rc << in_rc.rdbuf();
    f_rc.clear();  // clearStreamState error flag in case in_RC is empty

    remove((eg.outfile_seq + '.' + std::to_string(tid)).c_str());
    remove((eg.infile_order + '.' + std::to_string(tid)).c_str());
    remove((eg.infile_order + '.' + std::to_string(tid) + ".tmp").c_str());
    remove((eg.infile_read_length + '.' + std::to_string(tid)).c_str());
    remove(
        (eg.infile_read_length + '.' + std::to_string(tid) + ".tmp").c_str());
    remove((eg.outfile_noise_pos + '.' + std::to_string(tid)).c_str());
    remove((eg.outfile_noise + '.' + std::to_string(tid)).c_str());
    remove((eg.infile_rc + '.' + std::to_string(tid) + ".tmp").c_str());
    remove((eg.infile_rc + '.' + std::to_string(tid)).c_str());
    remove((eg.infile_flag + '.' + std::to_string(tid)).c_str());
    remove((eg.infile_pos + '.' + std::to_string(tid)).c_str());
    remove((eg.infile + '.' + std::to_string(tid)).c_str());
  }
  f_order.close();
  f_read_length.close();
  // write remaining singleton reads now
  std::ofstream f_unaligned(eg.outfile_unaligned, std::ios::binary);
  f_order.open(eg.infile_order, std::ios::binary | std::ofstream::app);
  f_read_length.open(eg.infile_read_length,
                     std::ios::binary | std::ofstream::app);
  uint32_t matched_s = eg.num_reads_s;
  uint64_t len_unaligned = 0;

  for (uint32_t i = 0; i < eg.num_reads_s; i++)
    if (remaining_reads[i] == 1) {
      matched_s--;
      f_order.write(reinterpret_cast<char*>(&order_s[i]), sizeof(uint32_t));
      f_read_length.write(reinterpret_cast<char*>(&read_lengths_s[i]),
                          sizeof(uint16_t));
      std::string unaligned_read =
          BitsetToString<BitsetSize>(read[i], read_lengths_s[i], egb);
      WriteDnaNInBits(unaligned_read, f_unaligned);
      len_unaligned += read_lengths_s[i];
    }
  uint32_t matched_n = eg.num_reads_n;
  for (uint32_t i = eg.num_reads_s; i < eg.num_reads_s + eg.num_reads_n; i++)
    if (remaining_reads[i] == 1) {
      matched_n--;
      std::string unaligned_read =
          BitsetToString<BitsetSize>(read[i], read_lengths_s[i], egb);
      WriteDnaNInBits(unaligned_read, f_unaligned);
      f_order.write(reinterpret_cast<char*>(&order_s[i]), sizeof(uint32_t));
      f_read_length.write(reinterpret_cast<char*>(&read_lengths_s[i]),
                          sizeof(uint16_t));
      len_unaligned += read_lengths_s[i];
    }
  f_order.close();
  f_read_length.close();
  f_unaligned.close();
  delete[] remaining_reads;
#ifdef GENIE_USE_OPENMP
  delete[] dict_lock;
  delete[] read_lock;
#endif
  for (int i = 0; i < eg.max_read_len; i++) delete[] mask[i];
  delete[] mask;
  delete[] mask1;

  // write length of unaligned array
  std::ofstream f_unaligned_count(eg.outfile_unaligned + ".count",
                                  std::ios::binary);
  f_unaligned_count.write(reinterpret_cast<char*>(&len_unaligned),
                          sizeof(uint64_t));
  f_unaligned_count.close();

  // convert read_pos into 8 byte non-diff (absolute)
  // positions
  uint64_t abs_pos = 0;
  uint64_t abs_pos_thr;
  std::ofstream file_out_pos(eg.outfile_pos, std::ios::binary);
  for (int tid = 0; tid < eg.num_thr; tid++) {
    const auto fin_pos_path = eg.outfile_pos + '.' + std::to_string(tid);
    std::ifstream fin_pos(fin_pos_path, std::ios::binary);
    UTILS_DIE_IF(!fin_pos, "Cannot open file to read: " + fin_pos_path);
    fin_pos.read(reinterpret_cast<char*>(&abs_pos_thr), sizeof(uint64_t));
    while (!fin_pos.eof()) {
      abs_pos_thr += abs_pos;
      file_out_pos.write(reinterpret_cast<char*>(&abs_pos_thr),
                         sizeof(uint64_t));
      fin_pos.read(reinterpret_cast<char*>(&abs_pos_thr), sizeof(uint64_t));
    }
    fin_pos.close();
    remove((eg.outfile_pos + '.' + std::to_string(tid)).c_str());
    abs_pos += file_len_seq_thr[tid];
  }
  file_out_pos.close();
  delete[] file_len_seq_thr;

  std::cerr << "Encoding done:\n";
  std::cerr << matched_s << " singleton reads were aligned\n";
  std::cerr << matched_n << " reads with N were aligned\n";
}

// -----------------------------------------------------------------------------
template <size_t BitsetSize>
void SetGlobalArrays(EncoderGlobal& eg, EncoderGlobalB<BitsetSize>& egb) {
  for (int i = 0; i < 63; i++) egb.mask63[i] = 1;
  for (int i = 0; i < eg.max_read_len; i++) {
    egb.base_mask[i][static_cast<uint8_t>('A')][3 * i] = 0;
    egb.base_mask[i][static_cast<uint8_t>('A')][3 * i + 1] = 0;
    egb.base_mask[i][static_cast<uint8_t>('A')][3 * i + 2] = 0;
    egb.base_mask[i][static_cast<uint8_t>('C')][3 * i] = 0;
    egb.base_mask[i][static_cast<uint8_t>('C')][3 * i + 1] = 0;
    egb.base_mask[i][static_cast<uint8_t>('C')][3 * i + 2] = 1;
    egb.base_mask[i][static_cast<uint8_t>('G')][3 * i] = 0;
    egb.base_mask[i][static_cast<uint8_t>('G')][3 * i + 1] = 1;
    egb.base_mask[i][static_cast<uint8_t>('G')][3 * i + 2] = 0;
    egb.base_mask[i][static_cast<uint8_t>('T')][3 * i] = 0;
    egb.base_mask[i][static_cast<uint8_t>('T')][3 * i + 1] = 1;
    egb.base_mask[i][static_cast<uint8_t>('T')][3 * i + 2] = 1;
    egb.base_mask[i][static_cast<uint8_t>('N')][3 * i] = 1;
    egb.base_mask[i][static_cast<uint8_t>('N')][3 * i + 1] = 0;
    egb.base_mask[i][static_cast<uint8_t>('N')][3 * i + 2] = 0;
  }

  // enc_noise uses substitution statistics from Minoche et al.
  eg.enc_noise[static_cast<uint8_t>('A')][static_cast<uint8_t>('C')] = '0';
  eg.enc_noise[static_cast<uint8_t>('A')][static_cast<uint8_t>('G')] = '1';
  eg.enc_noise[static_cast<uint8_t>('A')][static_cast<uint8_t>('T')] = '2';
  eg.enc_noise[static_cast<uint8_t>('A')][static_cast<uint8_t>('N')] = '3';
  eg.enc_noise[static_cast<uint8_t>('C')][static_cast<uint8_t>('A')] = '0';
  eg.enc_noise[static_cast<uint8_t>('C')][static_cast<uint8_t>('G')] = '1';
  eg.enc_noise[static_cast<uint8_t>('C')][static_cast<uint8_t>('T')] = '2';
  eg.enc_noise[static_cast<uint8_t>('C')][static_cast<uint8_t>('N')] = '3';
  eg.enc_noise[static_cast<uint8_t>('G')][static_cast<uint8_t>('T')] = '0';
  eg.enc_noise[static_cast<uint8_t>('G')][static_cast<uint8_t>('A')] = '1';
  eg.enc_noise[static_cast<uint8_t>('G')][static_cast<uint8_t>('C')] = '2';
  eg.enc_noise[static_cast<uint8_t>('G')][static_cast<uint8_t>('N')] = '3';
  eg.enc_noise[static_cast<uint8_t>('T')][static_cast<uint8_t>('G')] = '0';
  eg.enc_noise[static_cast<uint8_t>('T')][static_cast<uint8_t>('C')] = '1';
  eg.enc_noise[static_cast<uint8_t>('T')][static_cast<uint8_t>('A')] = '2';
  eg.enc_noise[static_cast<uint8_t>('T')][static_cast<uint8_t>('N')] = '3';
  eg.enc_noise[static_cast<uint8_t>('N')][static_cast<uint8_t>('A')] = '0';
  eg.enc_noise[static_cast<uint8_t>('N')][static_cast<uint8_t>('G')] = '1';
  eg.enc_noise[static_cast<uint8_t>('N')][static_cast<uint8_t>('C')] = '2';
  eg.enc_noise[static_cast<uint8_t>('N')][static_cast<uint8_t>('T')] = '3';
}

// -----------------------------------------------------------------------------
template <size_t BitsetSize>
void ReadSingletons(std::bitset<BitsetSize>* read, uint32_t* order_s,
                    uint16_t* read_lengths_s, const EncoderGlobal& eg,
                    const EncoderGlobalB<BitsetSize>& egb) {
  // not parallelized right now since these are very small number of reads
  std::ifstream f(eg.infile + ".singleton",
                  std::ifstream::in | std::ios::binary);
  UTILS_DIE_IF(!f, "Cannot open file to read: " + eg.infile + ".singleton");
  std::string s;
  for (uint32_t i = 0; i < eg.num_reads_s; i++) {
    ReadDnaFromBits(s, f);
    read_lengths_s[i] = static_cast<uint16_t>(s.length());
    StringToBitset<BitsetSize>(s, read_lengths_s[i], read[i], egb.base_mask);
  }
  f.close();
  remove((eg.infile + ".singleton").c_str());
  f.open(eg.infile_n, std::ios::binary);
  for (uint32_t i = eg.num_reads_s; i < eg.num_reads_s + eg.num_reads_n; i++) {
    ReadDnaNFromBits(s, f);
    read_lengths_s[i] = static_cast<uint16_t>(s.length());
    StringToBitset<BitsetSize>(s, read_lengths_s[i], read[i], egb.base_mask);
  }
  std::ifstream f_order_s(eg.infile_order + ".singleton", std::ios::binary);
  UTILS_DIE_IF(!f_order_s,
               "Cannot open file to read: " + eg.infile_order + ".singleton");
  for (uint32_t i = 0; i < eg.num_reads_s; i++)
    f_order_s.read(reinterpret_cast<char*>(&order_s[i]), sizeof(uint32_t));
  f_order_s.close();
  remove((eg.infile_order + ".singleton").c_str());
  std::ifstream f_order_n(eg.infile_order_n, std::ios::binary);
  UTILS_DIE_IF(!f_order_n, "Cannot open file to read: " + eg.infile_order_n);
  for (uint32_t i = eg.num_reads_s; i < eg.num_reads_s + eg.num_reads_n; i++)
    f_order_n.read(reinterpret_cast<char*>(&order_s[i]), sizeof(uint32_t));
  f_order_n.close();
}

// -----------------------------------------------------------------------------
template <size_t BitsetSize>
void EncoderMain(const std::string& temp_dir, const CompressionParams& cp) {
  auto* egb_ptr = new EncoderGlobalB<BitsetSize>(cp.max_read_len);
  auto* eg_ptr = new EncoderGlobal;
  EncoderGlobalB<BitsetSize>& egb = *egb_ptr;
  EncoderGlobal& eg = *eg_ptr;

  eg.basedir = temp_dir;
  eg.infile = eg.basedir + "/temp.dna";
  eg.infile_pos = eg.basedir + "/temp_pos.txt";
  eg.infile_flag = eg.basedir + "/temp_flag.txt";
  eg.infile_order = eg.basedir + "/read_order.bin";
  eg.infile_order_n = eg.basedir + "/read_order_N.bin";
  eg.infile_rc = eg.basedir + "/read_rev.txt";
  eg.infile_read_length = eg.basedir + "/read_lengths.bin";
  eg.infile_n = eg.basedir + "/input_N.dna";
  eg.outfile_seq = eg.basedir + "/read_seq.txt";
  eg.outfile_pos = eg.basedir + "/read_pos.bin";
  eg.outfile_noise = eg.basedir + "/read_noise.txt";
  eg.outfile_noise_pos = eg.basedir + "/read_noise_pos.bin";
  eg.outfile_unaligned = eg.basedir + "/read_unaligned.txt";

  eg.max_read_len = static_cast<int>(cp.max_read_len);
  eg.num_thr = cp.num_thr;

  GetDataParams(eg, cp);  // populate num_reads
  SetGlobalArrays<BitsetSize>(eg, egb);
  auto* read = new std::bitset<BitsetSize>[eg.num_reads_s + eg.num_reads_n];
  auto* order_s = new uint32_t[eg.num_reads_s + eg.num_reads_n];
  auto* read_lengths_s = new uint16_t[eg.num_reads_s + eg.num_reads_n];
  ReadSingletons<BitsetSize>(read, order_s, read_lengths_s, eg, egb);
  remove(eg.infile_n.c_str());
  CorrectOrder(order_s, eg);

  auto* dict = new BbHashDict[eg.num_dict_s];
  if (eg.max_read_len > 50) {
    dict[0].start_ = 0;
    dict[0].end_ = 20;
    dict[1].start_ = 21;
    dict[1].end_ = 41;
  } else {
    dict[0].start_ = 0;
    dict[0].end_ = 20 * eg.max_read_len / 50;
    dict[1].start_ = 20 * eg.max_read_len / 50 + 1;
    dict[1].end_ = 41 * eg.max_read_len / 50;
  }

  if (eg.num_reads_s + eg.num_reads_n > 0)
    ConstructDictionary<BitsetSize>(read, dict, read_lengths_s, eg.num_dict_s,
                                    eg.num_reads_s + eg.num_reads_n, 3,
                                    eg.basedir, eg.num_thr);

  Encode<BitsetSize>(read, dict, order_s, read_lengths_s, eg, egb);
  delete[] read;
  delete[] dict;
  delete[] order_s;
  delete[] read_lengths_s;
  delete eg_ptr;
  delete egb_ptr;
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_SPRING_ENCODING_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
