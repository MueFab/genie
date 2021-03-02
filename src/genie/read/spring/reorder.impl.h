/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_REORDER_IMPL_H_
#define SRC_GENIE_READ_SPRING_REORDER_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
reorder_global<bitset_size>::reorder_global(int max_readlen_param) {
    basemask = new std::bitset<bitset_size> *[max_readlen_param];
    for (int i = 0; i < max_readlen_param; i++) basemask[i] = new std::bitset<bitset_size>[128];
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
reorder_global<bitset_size>::~reorder_global() {
    for (int i = 0; i < max_readlen; i++) delete[] basemask[i];
    delete[] basemask;
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void bitsettostring(std::bitset<bitset_size> b, char *s, const uint16_t readlen,
                    const reorder_global<bitset_size> &rg) {
    // destroys bitset b
    static const char revinttochar[4] = {'A', 'G', 'C', 'T'};
    uint64_t ull;
    for (int i = 0; i < 2 * readlen / 64 + 1; i++) {
        ull = (b & rg.mask64).to_ullong();
        b >>= 64;
        for (int j = 32 * i; j < 32 * i + 32 && j < readlen; j++) {
            s[j] = revinttochar[ull % 4];
            ull /= 4;
        }
    }
    s[readlen] = '\0';
    return;
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void setglobalarrays(reorder_global<bitset_size> &rg) {
    for (int i = 0; i < 64; i++) rg.mask64[i] = 1;
    for (int i = 0; i < rg.max_readlen; i++) {
        rg.basemask[i][(uint8_t)'A'][2 * i] = 0;
        rg.basemask[i][(uint8_t)'A'][2 * i + 1] = 0;
        rg.basemask[i][(uint8_t)'C'][2 * i] = 0;
        rg.basemask[i][(uint8_t)'C'][2 * i + 1] = 1;
        rg.basemask[i][(uint8_t)'G'][2 * i] = 1;
        rg.basemask[i][(uint8_t)'G'][2 * i + 1] = 0;
        rg.basemask[i][(uint8_t)'T'][2 * i] = 1;
        rg.basemask[i][(uint8_t)'T'][2 * i + 1] = 1;
    }
    return;
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void updaterefcount(std::bitset<bitset_size> &cur, std::bitset<bitset_size> &ref, std::bitset<bitset_size> &revref,
                    int **count, const bool resetcount, const bool rev, const int shift, const uint16_t cur_readlen,
                    int &ref_len, const reorder_global<bitset_size> &rg) {
    // for var length, shift represents shift of start positions, if read length is
    // small, may not need to shift actually
    static const char inttochar[4] = {'A', 'C', 'T', 'G'};
    auto chartoint = [](uint8_t a) { return (a & 0x06) >> 1; };  // inverse of above
    char s[MAX_READ_LEN + 1], s1[MAX_READ_LEN + 1], *current;
    bitsettostring<bitset_size>(cur, s, cur_readlen, rg);
    if (rev == false) {
        current = s;
    } else {
        reverse_complement(s, s1, cur_readlen);
        current = s1;
    }

    if (resetcount == true) {  // resetcount - unmatched read so start over
        std::fill(count[0], count[0] + rg.max_readlen, 0);
        std::fill(count[1], count[1] + rg.max_readlen, 0);
        std::fill(count[2], count[2] + rg.max_readlen, 0);
        std::fill(count[3], count[3] + rg.max_readlen, 0);
        for (int i = 0; i < cur_readlen; i++) {
            count[chartoint((uint8_t)current[i])][i] = 1;
        }
        ref_len = cur_readlen;
    } else {
        if (!rev) {
            // shift count
            for (int i = 0; i < ref_len - shift; i++) {
                for (int j = 0; j < 4; j++) count[j][i] = count[j][i + shift];
                if (i < cur_readlen) count[chartoint((uint8_t)current[i])][i] += 1;
            }

            // for the new positions set count to 1
            for (int i = ref_len - shift; i < cur_readlen; i++) {
                for (int j = 0; j < 4; j++) count[j][i] = 0;
                count[chartoint((uint8_t)current[i])][i] = 1;
            }
            ref_len = std::max<int>(ref_len - shift, cur_readlen);
        } else {  // reverse case is quite complicated in the variable length case
            if (cur_readlen - shift >= ref_len) {
                for (int i = cur_readlen - shift - ref_len; i < cur_readlen - shift; i++) {
                    for (int j = 0; j < 4; j++) count[j][i] = count[j][i - (cur_readlen - shift - ref_len)];
                    count[chartoint((uint8_t)current[i])][i] += 1;
                }
                for (int i = 0; i < cur_readlen - shift - ref_len; i++) {
                    for (int j = 0; j < 4; j++) count[j][i] = 0;
                    count[chartoint((uint8_t)current[i])][i] = 1;
                }
                for (int i = cur_readlen - shift; i < cur_readlen; i++) {
                    for (int j = 0; j < 4; j++) count[j][i] = 0;
                    count[chartoint((uint8_t)current[i])][i] = 1;
                }
                ref_len = cur_readlen;
            } else if (ref_len + shift <= rg.max_readlen) {
                for (int i = ref_len - cur_readlen + shift; i < ref_len; i++)
                    count[chartoint((uint8_t)current[i - (ref_len - cur_readlen + shift)])][i] += 1;
                for (int i = ref_len; i < ref_len + shift; i++) {
                    for (int j = 0; j < 4; j++) count[j][i] = 0;
                    count[chartoint((uint8_t)current[i - (ref_len - cur_readlen + shift)])][i] = 1;
                }
                ref_len = ref_len + shift;
            } else {
                for (int i = 0; i < rg.max_readlen - shift; i++) {
                    for (int j = 0; j < 4; j++) count[j][i] = count[j][i + (ref_len + shift - rg.max_readlen)];
                }
                for (int i = rg.max_readlen - cur_readlen; i < rg.max_readlen - shift; i++) {
                    count[chartoint((uint8_t)current[i - (rg.max_readlen - cur_readlen)])][i] += 1;
                }
                for (int i = rg.max_readlen - shift; i < rg.max_readlen; i++) {
                    for (int j = 0; j < 4; j++) count[j][i] = 0;
                    count[chartoint((uint8_t)current[i - (rg.max_readlen - cur_readlen)])][i] = 1;
                }
                ref_len = rg.max_readlen;
            }
        }
        // find max of each position to get ref

        for (int i = 0; i < ref_len; i++) {
            int max = 0, indmax = 0;
            for (int j = 0; j < 4; j++)
                if (count[j][i] > max) {
                    max = count[j][i];
                    indmax = j;
                }
            current[i] = inttochar[indmax];
        }
    }
    chartobitset<bitset_size>(current, ref_len, ref, rg.basemask);
    char revcurrent[MAX_READ_LEN + 1];
    reverse_complement(current, revcurrent, ref_len);
    chartobitset<bitset_size>(revcurrent, ref_len, revref, rg.basemask);

    return;
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void readDnaFile(std::bitset<bitset_size> *read, uint16_t *read_lengths, const reorder_global<bitset_size> &rg) {
    std::ifstream f(rg.infile[0], std::ifstream::in|std::ios::binary);
    for (uint32_t i = 0; i < rg.numreads_array[0]; i++) {
      f.read((char*)&read_lengths[i],sizeof(uint16_t));
      uint16_t num_bytes_to_read = ((uint32_t)read_lengths[i]+4-1)/4;
      f.read((char*)&read[i],num_bytes_to_read);
    }
    f.close();
    remove(rg.infile[0].c_str());
    if (rg.paired_end) {
      f.open(rg.infile[1], std::ifstream::in|std::ios::binary);
      for (uint32_t i = rg.numreads_array[0]; i < rg.numreads_array[0] + rg.numreads_array[1]; i++) {
        f.read((char*)&read_lengths[i],sizeof(uint16_t));
        uint16_t num_bytes_to_read = ((uint32_t)read_lengths[i]+4-1)/4;
        f.read((char*)&read[i],num_bytes_to_read);
      }
      f.close();
      remove(rg.infile[1].c_str());
    }
    return;
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
bool search_match(const std::bitset<bitset_size> &ref, std::bitset<bitset_size> *mask1,
#ifdef GENIE_USE_OPENMP
                  omp_lock *dict_lock, omp_lock *read_lock,
#endif
                  std::bitset<bitset_size> **mask, uint16_t *read_lengths, bool *remainingreads,
                  std::bitset<bitset_size> *read, bbhashdict *dict, uint32_t &k, const bool rev, const int shift,
                  const int &ref_len, const reorder_global<bitset_size> &rg) {
    static const unsigned int thresh = THRESH_REORDER;
    const int maxsearch = MAX_SEARCH_REORDER;
    std::bitset<bitset_size> b;
    uint64_t ull;
    int64_t dictidx[2];  // to store the start and end index (end not inclusive)
    // in the dict read_id array
    uint64_t startposidx;  // index in startpos
    bool flag = 0;
    for (int l = 0; l < rg.numdict; l++) {
        // check if dictionary index is within bound
        if (!rev) {
            if (dict[l].end + shift >= ref_len) continue;
        } else {
            if (dict[l].end >= ref_len + shift || dict[l].start <= shift) continue;
        }
        b = ref & mask1[l];
        ull = (b >> 2 * dict[l].start).to_ullong();
        startposidx = dict[l].bphf->lookup(ull);
        if (startposidx >= dict[l].numkeys)  // not found
            continue;
            // check if any other thread is modifying same dictpos
#ifdef GENIE_USE_OPENMP
        dict_lock[reorder_lock_idx(startposidx)].set();
#endif
        dict[l].findpos(dictidx, startposidx);
        if (dict[l].empty_bin[startposidx]) {  // bin is empty
#ifdef GENIE_USE_OPENMP
            dict_lock[reorder_lock_idx(startposidx)].unset();
#endif
            continue;
        }
        uint64_t ull1 = ((read[dict[l].read_id[dictidx[0]]] & mask1[l]) >> 2 * dict[l].start).to_ullong();
        if (ull == ull1) {  // checking if ull is actually the key for this bin
            for (int64_t i = dictidx[1] - 1; i >= dictidx[0] && i >= dictidx[1] - maxsearch; i--) {
                auto rid = dict[l].read_id[i];
                size_t hamming;
                if (!rev) {
                    hamming = ((ref ^ read[rid]) &
                               mask[0][rg.max_readlen - std::min<int>(ref_len - shift, read_lengths[rid])])
                                  .count();
                } else {
                    hamming = ((ref ^ read[rid]) &
                               mask[shift][rg.max_readlen - std::min<int>(ref_len + shift, read_lengths[rid])])
                                  .count();
                }
                if (hamming <= thresh) {
#ifdef GENIE_USE_OPENMP
                    read_lock[reorder_lock_idx(rid)].set();
#endif
                    if (remainingreads[rid]) {
                        remainingreads[rid] = 0;
                        k = rid;
                        flag = 1;
                    }
#ifdef GENIE_USE_OPENMP
                    read_lock[reorder_lock_idx(rid)].unset();
#endif
                    if (flag == 1) break;
                }
            }
        }
#ifdef GENIE_USE_OPENMP
        dict_lock[reorder_lock_idx(startposidx)].unset();
#endif
        if (flag == 1) break;
    }
    return flag;
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void reorder(std::bitset<bitset_size> *read, bbhashdict *dict, uint16_t *read_lengths,
             const reorder_global<bitset_size> &rg) {
#ifdef GENIE_USE_OPENMP
    const uint32_t num_locks = NUM_LOCKS_REORDER;  // limits on number of locks (power of 2 for fast mod)
    omp_lock *dict_lock = new omp_lock[num_locks];
    omp_lock *read_lock = new omp_lock[num_locks];
#endif
    std::bitset<bitset_size> **mask = new std::bitset<bitset_size> *[rg.max_readlen];
    for (int i = 0; i < rg.max_readlen; i++) mask[i] = new std::bitset<bitset_size>[rg.max_readlen];
    generatemasks<bitset_size>(mask, rg.max_readlen, 2);
    std::bitset<bitset_size> *mask1 = new std::bitset<bitset_size>[rg.numdict];
    generateindexmasks<bitset_size>(mask1, dict, rg.numdict, 2);
    bool *remainingreads = new bool[rg.numreads];
    std::fill(remainingreads, remainingreads + rg.numreads, 1);

    // we go through remainingreads array from behind as that speeds up deletion
    // from bin arrays

    //
    // The following parallel region shows up in the execution profile
    // as being the hottest in all of genie/spring.  The workload is
    // well-balanced and it benefits nicely from parallelization.
    //
    // A lot of other loops / regions must be executed with exactly
    // the same # of threads, as they access files created on a
    // per-thread basis (with the tid as part of the name) between
    // parallel regions.
    //
    uint32_t firstread = 0;
    uint32_t *unmatched = new uint32_t[rg.num_thr];
#ifdef GENIE_USE_OPENMP
#pragma omp parallel num_threads(rg.num_thr)
#endif
    {
#ifdef GENIE_USE_OPENMP
        int tid = omp_get_thread_num();
#else
        int tid = 0;  // set thread ID to zero if not using OpenMP
#endif
        std::string tid_str = std::to_string(tid);
        std::ofstream foutRC(rg.outfileRC + '.' + tid_str, std::ofstream::out);
        std::ofstream foutflag(rg.outfileflag + '.' + tid_str, std::ofstream::out);
        std::ofstream foutpos(rg.outfilepos + '.' + tid_str, std::ofstream::out | std::ios::binary);
        std::ofstream foutorder(rg.outfileorder + '.' + tid_str, std::ofstream::out | std::ios::binary);
        std::ofstream foutorder_s(rg.outfileorder + ".singleton." + tid_str, std::ofstream::out | std::ios::binary);
        std::ofstream foutlength(rg.outfilereadlength + '.' + tid_str, std::ofstream::out | std::ios::binary);

        unmatched[tid] = 0;
        std::bitset<bitset_size> ref, revref, b;

        int64_t first_rid = 0;
        // first_rid represents first read of contig, used for left searching

        // variables for early stopping
        bool stop_searching = false;
        uint32_t num_reads_thr = 0;
        uint32_t num_unmatched_past_1M_thr = 0;

        int **count = new int *[4];
        for (int i = 0; i < 4; i++) count[i] = new int[rg.max_readlen];
        int64_t dictidx[2];  // to store the start and end index (end not inclusive)
        // in the dict read_id array
        uint64_t startposidx;  // index in startpos
        bool flag = 0, done = 0, prev_unmatched = false, left_search_start = false, left_search = false;
        // left_search_start - true when left search is starting (to avoid double
        // deletion of first read)
        // left_search - true during left search - needed so that we know when to
        // pick arbitrary read
        int64_t current, prev;
        uint64_t ull;
        int ref_len;
        int64_t ref_pos = 0, cur_read_pos = 0;
        // starting positions of the ref and the current read in the contig, can be
        // negative during left search or due to RC
        // useful for sorting according to starting position in the encoding stage.

        int64_t remainingpos = rg.numreads - 1;  // used for searching next unmatched read when no match is found
#ifdef GENIE_USE_OPENMP
#pragma omp critical
#endif
        {  // doing initial setup and first read
            current = firstread;
            // some fix below to make sure no errors occurs when we have very few
            // reads (comparable to num_threads). basically if read already taken,
            // this thread just gives up
            if (rg.numreads == 0) {
                done = true;
            } else if (remainingreads[current] == 0) {
                done = true;
            } else {
                remainingreads[current] = 0;
                unmatched[tid]++;
            }
            firstread += rg.numreads / rg.num_thr;  // spread out first read equally
        }
#ifdef GENIE_USE_OPENMP
#pragma omp barrier
#endif
        if (!done) {
            updaterefcount<bitset_size>(read[current], ref, revref, count, true, false, 0, read_lengths[current],
                                        ref_len, rg);
            cur_read_pos = 0;
            ref_pos = 0;
            first_rid = current;
            prev_unmatched = true;
            prev = current;
        }
        while (!done) {
            if (num_reads_thr % 1000000 == 0) {
                if (num_unmatched_past_1M_thr > STOP_CRITERIA_REORDER * 1000000) {
                    stop_searching = true;
                }
                num_unmatched_past_1M_thr = 0;
            }
            num_reads_thr++;
            // delete the read from the corresponding dictionary bins (unless we are
            // starting left search)
            if (!left_search_start) {
                for (int l = 0; l < rg.numdict; l++) {
                    if (read_lengths[current] <= dict[l].end) continue;
                    b = read[current] & mask1[l];
                    ull = (b >> 2 * dict[l].start).to_ullong();
                    startposidx = dict[l].bphf->lookup(ull);
                    // check if any other thread is modifying same dictpos
#ifdef GENIE_USE_OPENMP
                    dict_lock[reorder_lock_idx(startposidx)].set();
#endif
                    dict[l].findpos(dictidx, startposidx);
                    dict[l].remove(dictidx, startposidx, current);
#ifdef GENIE_USE_OPENMP
                    dict_lock[reorder_lock_idx(startposidx)].unset();
#endif
                }
            } else {
                left_search_start = false;
            }
            flag = 0;
            uint32_t k;
            if (!stop_searching) {
                for (int shift = 0; shift < rg.maxshift; shift++) {
                    // find forward match
                    flag = search_match<bitset_size>(ref, mask1,
#ifdef GENIE_USE_OPENMP
                                                     dict_lock, read_lock,
#endif
                                                     mask, read_lengths, remainingreads, read, dict, k, false, shift,
                                                     ref_len, rg);
                    if (flag == 1) {
                        current = k;
                        int ref_len_old = ref_len;
                        updaterefcount<bitset_size>(read[current], ref, revref, count, false, false, shift,
                                                    read_lengths[current], ref_len, rg);
                        if (!left_search) {
                            cur_read_pos = ref_pos + shift;
                            ref_pos = cur_read_pos;
                        } else {
                            cur_read_pos = ref_pos + ref_len_old - shift - read_lengths[current];
                            ref_pos = ref_pos + ref_len_old - shift - ref_len;
                        }
                        if (prev_unmatched == true) {  // prev read not singleton, write it now
                            foutRC << 'd';
                            foutorder.write(reinterpret_cast<char *>(&prev), sizeof(uint32_t));
                            foutflag << 0;  // for unmatched
                            int64_t zero = 0;
                            foutpos.write(reinterpret_cast<char *>(&zero), sizeof(int64_t));
                            foutlength.write(reinterpret_cast<char *>(&read_lengths[prev]), sizeof(uint16_t));
                        }
                        foutRC << (left_search ? 'r' : 'd');
                        foutorder.write(reinterpret_cast<char *>(&current), sizeof(uint32_t));
                        foutflag << 1;  // for matched
                        foutpos.write(reinterpret_cast<char *>(&cur_read_pos), sizeof(int64_t));
                        foutlength.write(reinterpret_cast<char *>(&read_lengths[current]), sizeof(uint16_t));

                        prev_unmatched = false;
                        break;
                    }

                    // find reverse match
                    flag = search_match<bitset_size>(revref, mask1,
#ifdef GENIE_USE_OPENMP
                                                     dict_lock, read_lock,
#endif
                                                     mask, read_lengths, remainingreads, read, dict, k, true, shift,
                                                     ref_len, rg);

                    if (flag == 1) {
                        current = k;
                        int ref_len_old = ref_len;
                        updaterefcount<bitset_size>(read[current], ref, revref, count, false, true, shift,
                                                    read_lengths[current], ref_len, rg);
                        if (!left_search) {
                            cur_read_pos = ref_pos + ref_len_old + shift - read_lengths[current];
                            ref_pos = ref_pos + ref_len_old + shift - ref_len;
                        } else {
                            cur_read_pos = ref_pos - shift;
                            ref_pos = cur_read_pos;
                        }
                        if (prev_unmatched == true) {  // prev read not singleton, write it now
                            foutRC << 'd';
                            foutorder.write(reinterpret_cast<char *>(&prev), sizeof(uint32_t));
                            foutflag << 0;  // for unmatched
                            int64_t zero = 0;
                            foutpos.write(reinterpret_cast<char *>(&zero), sizeof(int64_t));
                            foutlength.write(reinterpret_cast<char *>(&read_lengths[prev]), sizeof(uint16_t));
                        }
                        foutRC << (left_search ? 'd' : 'r');
                        foutorder.write(reinterpret_cast<char *>(&current), sizeof(uint32_t));
                        foutflag << 1;  // for matched
                        foutpos.write(reinterpret_cast<char *>(&cur_read_pos), sizeof(int64_t));
                        foutlength.write(reinterpret_cast<char *>(&read_lengths[current]), sizeof(uint16_t));

                        prev_unmatched = false;
                        break;
                    }

                    revref <<= 2;
                    ref >>= 2;
                }
            }
            if (flag == 0) {
                // no match found
                num_unmatched_past_1M_thr++;
                if (!left_search) {  // start left search
                    left_search = true;
                    left_search_start = true;
                    // update ref and count with RC of first_read
                    updaterefcount<bitset_size>(read[first_rid], ref, revref, count, true, true, 0,
                                                read_lengths[first_rid], ref_len, rg);
                    ref_pos = 0;
                    cur_read_pos = 0;
                } else {  // left search done, now pick arbitrary read and start new
                          // contig
                    left_search = false;
                    for (int64_t j = remainingpos; j >= 0; j--) {
                        if (remainingreads[j] == 1) {
#ifdef GENIE_USE_OPENMP
                            read_lock[reorder_lock_idx(j)].set();
#endif
                            if (remainingreads[j]) {  // checking again inside critical block
                                current = j;
                                remainingpos = j - 1;
                                remainingreads[j] = 0;
                                flag = 1;
                                unmatched[tid]++;
                            }
#ifdef GENIE_USE_OPENMP
                            read_lock[reorder_lock_idx(j)].unset();
#endif
                            if (flag == 1) break;
                        }
                    }
                    if (flag == 0) {
                        if (prev_unmatched == true) {  // last read was singleton, write it now
                            foutorder_s.write(reinterpret_cast<char *>(&prev), sizeof(uint32_t));
                        }
                        done = 1;  // no reads left
                    } else {
                        updaterefcount<bitset_size>(read[current], ref, revref, count, true, false, 0,
                                                    read_lengths[current], ref_len, rg);
                        ref_pos = 0;
                        cur_read_pos = 0;
                        if (prev_unmatched == true) {  // prev read singleton, write it now
                            foutorder_s.write(reinterpret_cast<char *>(&prev), sizeof(uint32_t));
                        }
                        prev_unmatched = true;
                        first_rid = current;
                        prev = current;
                    }
                }
            }
        }  // while (!done) end

        foutRC.close();
        foutorder.close();
        foutflag.close();
        foutpos.close();
        foutorder_s.close();
        foutlength.close();
        for (int i = 0; i < 4; i++) delete[] count[i];
        delete[] count;
    }  // parallel end

    delete[] remainingreads;
#ifdef GENIE_USE_OPENMP
    delete[] dict_lock;
    delete[] read_lock;
#endif
    std::cout << "Reordering done, " << std::accumulate(unmatched, unmatched + rg.num_thr, 0) << " were unmatched\n";
    for (int i = 0; i < rg.max_readlen; i++) delete[] mask[i];
    delete[] mask;
    delete[] mask1;
    delete[] unmatched;
    return;
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void writetofile(std::bitset<bitset_size> *read, uint16_t *read_lengths, reorder_global<bitset_size> &rg) {
    //
    // This loop must be executed in parallel with the same #threads
    // that was used for the hot loop in reorder(), for correctness.
    // It also shows up in the execution profile, and is worth
    // parallelizing anyway.
    //
    // FIXME re-code the loop so that is doesn't necessarily need to
    // execute on the same #threads.
    //
    std::vector<uint32_t> numreads_s_thr(rg.num_thr, 0);
#ifdef GENIE_USE_OPENMP
#pragma omp parallel num_threads(rg.num_thr)
#endif
    {
#ifdef GENIE_USE_OPENMP
        uint32_t tid = omp_get_thread_num();
#else
        uint32_t tid = 0;  // set thread ID to zero if not using OpenMP
#endif
        std::string tid_str = std::to_string(tid);
        std::ofstream fout(rg.outfile + '.' + tid_str, std::ofstream::out|std::ios::binary);
        std::ofstream fout_s(rg.outfile + ".singleton." + tid_str, std::ofstream::out|std::ios::binary);
        std::ifstream finRC(rg.outfileRC + '.' + tid_str, std::ifstream::in);
        std::ifstream finorder(rg.outfileorder + '.' + tid_str, std::ifstream::in | std::ios::binary);
        std::ifstream finorder_s(rg.outfileorder + ".singleton." + tid_str, std::ifstream::in | std::ios::binary);
        char s[MAX_READ_LEN + 1], s1[MAX_READ_LEN + 1];
        uint32_t current;
        char c;
        while (finRC >> std::noskipws >> c)  // read character by character
        {
            finorder.read((char *)&current, sizeof(uint32_t));
            if (c == 'd') {
              uint16_t num_bytes_to_write = ((uint32_t)read_lengths[current] + 4 - 1)/4;
      	      fout.write((char*)&read_lengths[current],sizeof(uint16_t));
      	      fout.write((char*)&read[current], num_bytes_to_write);
            } else {
      	       bitsettostring<bitset_size>(read[current], s, read_lengths[current], rg);
      	       reverse_complement(s, s1, read_lengths[current]);
      	       write_dna_in_bits(s1, fout);
            }
        }
        finorder_s.read(reinterpret_cast<char *>(&current), sizeof(uint32_t));
        while (!finorder_s.eof()) {
            numreads_s_thr[tid]++;
            uint16_t num_bytes_to_write = ((uint32_t)read_lengths[current] + 4 - 1)/4;
            fout_s.write((char*)&read_lengths[current],sizeof(uint16_t));
            fout_s.write((char*)&read[current], num_bytes_to_write);
            finorder_s.read((char *)&current, sizeof(uint32_t));
        }

        fout.close();
        fout_s.close();
        finRC.close();
        finorder.close();
        finorder_s.close();
    }

    uint32_t numreads_s = 0;
    for (int i = 0; i < rg.num_thr; i++)
    numreads_s += numreads_s_thr[i];
    // write numreads_s to a file
    std::ofstream fout_s_count(rg.outfile + ".singleton"+".count", std::ofstream::out|std::ios::binary);
    fout_s_count.write((char*)&numreads_s, sizeof(uint32_t));
    fout_s_count.close();

    // Now combine the num_thr order files
    std::ofstream fout_s(rg.outfile + ".singleton", std::ofstream::out|std::ios::binary);
    std::ofstream foutorder_s(rg.outfileorder + ".singleton", std::ofstream::out | std::ios::binary);
    for (int tid = 0; tid < rg.num_thr; tid++) {
        std::string tid_str = std::to_string(tid);
        std::ifstream fin_s(rg.outfile + ".singleton." + tid_str, std::ifstream::in|std::ios::binary);
        std::ifstream finorder_s(rg.outfileorder + ".singleton." + tid_str, std::ifstream::in | std::ios::binary);

        fout_s << fin_s.rdbuf();  // write entire file
        foutorder_s << finorder_s.rdbuf();

        // clear error flags which occur on rdbuffing empty file
        fout_s.clear();
        foutorder_s.clear();

        fin_s.close();
        finorder_s.close();

        remove((rg.outfile + ".singleton." + tid_str).c_str());
        remove((rg.outfileorder + ".singleton." + tid_str).c_str());
    }
    fout_s.close();
    foutorder_s.close();
    return;
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void reorder_main(const std::string &temp_dir, const compression_params &cp) {
    reorder_global<bitset_size> *rg_pointer = new reorder_global<bitset_size>(cp.max_readlen);
    reorder_global<bitset_size> &rg = *rg_pointer;
    rg.basedir = temp_dir;
    rg.infile[0] = rg.basedir + "/input_clean_1.dna";
    rg.infile[1] = rg.basedir + "/input_clean_2.dna";
    rg.outfile = rg.basedir + "/temp.dna";
    rg.outfileRC = rg.basedir + "/read_rev.txt";
    rg.outfileflag = rg.basedir + "/tempflag.txt";
    rg.outfilepos = rg.basedir + "/temppos.txt";
    rg.outfileorder = rg.basedir + "/read_order.bin";
    rg.outfilereadlength = rg.basedir + "/read_lengths.bin";

    rg.max_readlen = cp.max_readlen;
    rg.num_thr = cp.num_thr;
    rg.paired_end = cp.paired_end;
    rg.maxshift = rg.max_readlen / 2;
    bbhashdict *dict = new bbhashdict[rg.numdict];
    dict[0].start = rg.max_readlen > 100 ? rg.max_readlen / 2 - 32 : rg.max_readlen / 2 - rg.max_readlen * 32 / 100;
    dict[0].end = rg.max_readlen / 2 - 1;
    dict[1].start = rg.max_readlen / 2;
    dict[1].end =
        rg.max_readlen > 100 ? rg.max_readlen / 2 - 1 + 32 : rg.max_readlen / 2 - 1 + rg.max_readlen * 32 / 100;

    rg.numreads = cp.num_reads_clean[0] + cp.num_reads_clean[1];
    rg.numreads_array[0] = cp.num_reads_clean[0];
    rg.numreads_array[1] = cp.num_reads_clean[1];

    setglobalarrays(rg);
    std::bitset<bitset_size> *read = new std::bitset<bitset_size>[rg.numreads];
    uint16_t *read_lengths = new uint16_t[rg.numreads];
    std::cout << "Reading file\n";
    readDnaFile<bitset_size>(read, read_lengths, rg);
    if (rg.numreads > 0) {
        std::cout << "Constructing dictionaries\n";
        constructdictionary<bitset_size>(read, dict, read_lengths, rg.numdict, rg.numreads, 2, rg.basedir, rg.num_thr);
    }
    std::cout << "Reordering reads\n";
    reorder<bitset_size>(read, dict, read_lengths, rg);
    std::cout << "Writing to file\n";
    writetofile<bitset_size>(read, read_lengths, rg);
    delete[] read;
    delete[] dict;
    delete[] read_lengths;
    delete rg_pointer;
    std::cout << "Done!\n";
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_REORDER_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
