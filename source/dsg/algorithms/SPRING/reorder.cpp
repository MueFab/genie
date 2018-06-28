#include <omp.h>
#include <algorithm>
#include <atomic>
#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <string>
#include <vector>
#include "algorithms/SPRING/reorder.h"

template<size_t bitset_size>
struct reorder_global {
	uint32_t num_locks =
	    0x1000000;  // limits on number of locks (power of 2 for fast mod)

	uint32_t numreads = 0;

	int maxshift, thresh = 4, numdict = 2, maxsearch = 1000, num_thr, max_readlen;

	std::string basedir;
	std::string infile;
	std::string infilenumreads;
	std::string outfile;
	std::string outfileRC;
	std::string outfileflag;
	std::string outfilepos;
	std::string outfileorder;
	std::string outfilereadlength;

	// Some global arrays (some initialized in setglobalarrays())
	char revinttochar[4] = {'A', 'G', 'C', 'T'};  // used in bitsettostring
	char inttochar[] = {'A', 'C', 'G', 'T'};
	char chartorevchar[128];  // A-T etc for reverse complement
	int chartoint[128];       // A-0,C-1 etc. used in updaterefcount

	bitset basemask[MAX_READ_LEN][128];  // bitset for A,G,C,T at each position
	// used in stringtobitset, chartobitset and bitsettostring
	bitset positionmask[MAX_READ_LEN];  // bitset for each position (1 at two bits
					    // and 0 elsewhere)
	// used in bitsettostring
	bitset mask64;  // bitset with 64 bits set to 1 (used in bitsettostring for
			// conversion to ullong)
};

void bitsettostring(bitset b, char *s, uint8_t readlen, reorder_global &rg);

void readDnaFile(bitset *read, uint8_t *read_lengths, reorder_global &rg);

void reorder(bitset *read, bbhashdict *dict, uint8_t *read_lengths, reorder_global &rg);

bool search_match(bitset &ref, bitset *mask1, omp_lock_t *dict_lock,
                  omp_lock_t *read_lock,
                  bitset mask[MAX_READ_LEN][MAX_READ_LEN],
                  uint8_t *read_lengths, bool *remainingreads, bitset *read,
                  bbhashdict *dict, uint32_t &k, bool rev, int shift,
                  int &ref_len, reorder_global &rg);

void writetofile(bitset *read, uint8_t *read_lengths, reorder_global &rg);

void updaterefcount(bitset &cur, bitset &ref, bitset &revref,
                    int count[][MAX_READ_LEN], bool resetcount, bool rev,
                    int shift, uint8_t cur_readlen, int &ref_len, reorder_global &rg);
// update the clean reference and count matrix

void chartobitset(char *s, uint8_t readlen, bitset &b, reorder_global &rg);

void setglobalarrays(reorder_global &rg);
// setting arrays like chartoint etc.

void reorder_main(std::string &working_dir, int max_readlen, int num_thr) {
  reorder_global rg;
  rg.basedir = working_dir;
  rg.infile = basedir + "/input_clean.dna";
  rg.outfile = basedir + "/temp.dna";
  rg.outfileRC = basedir + "/read_rev.txt";
  rg.outfileflag = basedir + "/tempflag.txt";
  rg.outfilepos = basedir + "/temppos.txt";
  rg.outfileorder = basedir + "/read_order.bin";
  rg.outfilereadlength = basedir + "/read_lengths.bin";
  rg.infilenumreads = basedir + "/numreads.bin";

  rg.max_readlen = max_readlen;
  rg.num_thr = num_thr;
  rg.maxshift = rg.max_readlen / 2;
  bbhashdict *dict = new bbhashdict[rg.numdict];
  dict[0].start = rg.max_readlen > 100 ? rg.max_readlen / 2 - 32
                                    : rg.max_readlen / 2 - rg.max_readlen * 32 / 100;
  dict[0].end = rg.max_readlen / 2 - 1;
  dict[1].start = rg.max_readlen / 2;
  dict[1].end = rg.max_readlen > 100
                    ? rg.max_readlen / 2 - 1 + 32
                    : rg.max_readlen / 2 - 1 + rg.max_readlen * 32 / 100;

  std::ifstream f_numreads(rg.infilenumreads, std::ios::binary);
  f_numreads.read((char *)&rg.numreads, sizeof(uint32_t));
  omp_set_num_threads(rg.num_thr);
  setglobalarrays(rg);
  bitset *read = new bitset[rg.numreads];
  uint8_t *read_lengths = new uint8_t[rg.numreads];
  std::cout << "Reading file: " << rg.infile << std::endl;
  readDnaFile(read, read_lengths, rg);

  std::cout << "Constructing dictionaries\n";
  constructdictionary(read, dict, read_lengths, rg);
  std::cout << "Reordering reads\n";
  reorder(read, dict, read_lengths, rg);
  std::cout << "Writing to file\n";
  writetofile(read, read_lengths, rg);
  delete[] read;
  delete[] dict;
  std::cout << "Done!\n";
  return 0;
}

void setglobalarrays(reorder_global &rg) {
  rg.chartorevchar['A'] = 'T';
  rg.chartorevchar['C'] = 'G';
  rg.chartorevchar['G'] = 'C';
  rg.chartorevchar['T'] = 'A';
  rg.chartoint['A'] = 0;
  rg.chartoint['C'] = 1;
  rg.chartoint['G'] = 2;
  rg.chartoint['T'] = 3;

  for (int i = 0; i < 64; i++) mask64[i] = 1;
  for (int i = 0; i < max_readlen; i++) {
    rg.basemask[i]['A'][2 * i] = 0;
    rg.basemask[i]['A'][2 * i + 1] = 0;
    rg.basemask[i]['C'][2 * i] = 0;
    rg.basemask[i]['C'][2 * i + 1] = 1;
    rg.basemask[i]['G'][2 * i] = 1;
    rg.basemask[i]['G'][2 * i + 1] = 0;
    rg.basemask[i]['T'][2 * i] = 1;
    rg.basemask[i]['T'][2 * i + 1] = 1;
    rg.positionmask[i][2 * i] = 1;
    rg.positionmask[i][2 * i + 1] = 1;
  }
  return;
}

void readDnaFile(bitset *read, uint8_t *read_lengths, reorder_global &rg) {
#pragma omp parallel
  {
    int tid = omp_get_thread_num();
    std::ifstream f(rg.infile, std::ifstream::in);
    std::string s;
    uint32_t i = 0;
    while (std::getline(f, s)) {
      if (i % rg.num_thr == tid) {
        read_lengths[i] = s.length();
        stringtobitset(s, read_lengths[i], read[i], rg);
        i++;
      } else {
        i++;
        continue;
      }
    }
    f.close();
  }
  remove(rg.infile.c_str());
  return;
}

void reorder(bitset *read, bbhashdict *dict, uint8_t *read_lengths, reorder_global &rg) {
  omp_lock_t *dict_lock = new omp_lock_t[rg.num_locks];
  omp_lock_t *read_lock = new omp_lock_t[rg.num_locks];
  for (int j = 0; j < rg.num_locks; j++) {
    omp_init_lock(&dict_lock[j]);
    omp_init_lock(&read_lock[j]);
  }
  bitset mask[MAX_READ_LEN][MAX_READ_LEN];
  generatemasks(mask);
  bitset mask1[numdict];
  generateindexmasks(mask1);
  bool *remainingreads = new bool[rg.numreads];
  std::fill(remainingreads, remainingreads + rg.numreads, 1);

  // we go through remainingreads array from behind as that speeds up deletion
  // from bin arrays

  uint32_t firstread = 0, unmatched[rg.num_thr];
#pragma omp parallel
  {
    int tid = omp_get_thread_num();
    std::string tid_str = std::to_string(tid);
    std::ofstream foutRC(rg.outfileRC + '.' + tid_str, std::ofstream::out);
    std::ofstream foutflag(rg.outfileflag + '.' + tid_str, std::ofstream::out);
    std::ofstream foutpos(rg.outfilepos + '.' + tid_str,
                          std::ofstream::out | std::ios::binary);
    std::ofstream foutorder(rg.outfileorder + '.' + tid_str,
                            std::ofstream::out | std::ios::binary);
    std::ofstream foutorder_s(rg.outfileorder + ".singleton." + tid_str,
                              std::ofstream::out | std::ios::binary);
    std::ofstream foutlength(rg.outfilereadlength + '.' + tid_str,
                             std::ofstream::out | std::ios::binary);

    unmatched[tid] = 0;
    bitset ref, revref, b;

    uint32_t first_rid;
    // first_rid represents first read of contig, used for left searching
    int count[4][MAX_READ_LEN];
    int64_t dictidx[2];  // to store the start and end index (end not inclusive)
                         // in the dict read_id array
    uint32_t startposidx;  // index in startpos
    bool flag = 0, done = 0, prev_unmatched = false, left_search_start = false,
         left_search = false;
    // left_search_start - true when left search is starting (to avoid double
    // deletion of first read)
    // left_search - true during left search - needed so that we know when to
    // pick arbitrary read
    uint32_t current, prev;
    uint64_t ull;
    int ref_len;
    int64_t ref_pos, cur_read_pos;
    // starting positions of the ref and the current read in the contig, can be
    // negative during left search or due to RC
    // useful for sorting according to starting position in the encoding stage.

    int64_t remainingpos =
        rg.numreads -
        1;  // used for searching next unmatched read when no match is found
#pragma omp critical
    {  // doing initial setup and first read
      current = firstread;
      firstread +=
          rg.numreads / omp_get_num_threads();  // spread out first read equally
      remainingreads[current] = 0;
      unmatched[tid]++;
    }
#pragma omp barrier
    updaterefcount(read[current], ref, revref, count, true, false, 0,
                   read_lengths[current], ref_len, rg);
    cur_read_pos = 0;
    ref_pos = 0;
    first_rid = current;
    prev_unmatched = true;
    prev = current;
    while (!done) {
      // delete the read from the corresponding dictionary bins (unless we are
      // starting left search)
      if (!left_search_start) {
        for (int l = 0; l < rg.numdict; l++) {
          if (read_lengths[current] <= dict[l].end) continue;
          b = read[current] & mask1[l];
          ull = (b >> 2 * dict[l].start).to_ullong();
          startposidx = dict[l].bphf->lookup(ull);
          // check if any other thread is modifying same dictpos
          omp_set_lock(&dict_lock[startposidx & 0xFFFFFF]);
          dict[l].findpos(dictidx, startposidx);
          dict[l].remove(dictidx, startposidx, current);
          omp_unset_lock(&dict_lock[startposidx & 0xFFFFFF]);
        }
      } else {
        left_search_start = false;
      }
      flag = 0;
      uint32_t k;
      for (int shift = 0; shift < rg.maxshift; shift++) {
        // find forward match
        flag =
            search_match(ref, mask1, dict_lock, read_lock, mask, read_lengths,
                         remainingreads, read, dict, k, false, shift, ref_len, rg);
        if (flag == 1) {
          current = k;
          int ref_len_old = ref_len;
          updaterefcount(read[current], ref, revref, count, false, false, shift,
                         read_lengths[current], ref_len, rg);
          if (!left_search) {
            cur_read_pos = ref_pos + shift;
            ref_pos = cur_read_pos;
          } else {
            cur_read_pos =
                ref_pos + ref_len_old - shift - read_lengths[current];
            ref_pos = ref_pos + ref_len_old - shift - ref_len;
          }
          if (prev_unmatched == true)  // prev read not singleton, write it now
          {
            foutRC << 'd';
            foutorder.write((char *)&prev, sizeof(uint32_t));
            foutflag << 0;  // for unmatched
            int64_t zero = 0;
            foutpos.write((char *)&zero, sizeof(int64_t));
            foutlength.write((char *)&read_lengths[prev], sizeof(uint8_t));
          }
          foutRC << (left_search ? 'r' : 'd');
          foutorder.write((char *)&current, sizeof(uint32_t));
          foutflag << 1;  // for matched
          foutpos.write((char *)&cur_read_pos, sizeof(int64_t));
          foutlength.write((char *)&read_lengths[current], sizeof(uint8_t));

          prev_unmatched = false;
          break;
        }

        // find reverse match
        flag = search_match(revref, mask1, dict_lock, read_lock, mask,
                            read_lengths, remainingreads, read, dict, k, true,
                            shift, ref_len, rg);
        if (flag == 1) {
          current = k;
          int ref_len_old = ref_len;
          updaterefcount(read[current], ref, revref, count, false, true, shift,
                         read_lengths[current], ref_len, rg);
          if (!left_search) {
            cur_read_pos =
                ref_pos + ref_len_old + shift - read_lengths[current];
            ref_pos = ref_pos + ref_len_old + shift - ref_len;
          } else {
            cur_read_pos = ref_pos - shift;
            ref_pos = cur_read_pos;
          }
          if (prev_unmatched == true)  // prev read not singleton, write it now
          {
            foutRC << 'd';
            foutorder.write((char *)&prev, sizeof(uint32_t));
            foutflag << 0;  // for unmatched
            int64_t zero = 0;
            foutpos.write((char *)&zero, sizeof(int64_t));
            foutlength.write((char *)&read_lengths[prev], sizeof(uint8_t));
          }
          foutRC << (left_search ? 'd' : 'r');
          foutorder.write((char *)&current, sizeof(uint32_t));
          foutflag << 1;  // for matched
          foutpos.write((char *)&cur_read_pos, sizeof(int64_t));
          foutlength.write((char *)&read_lengths[current], sizeof(uint8_t));

          prev_unmatched = false;
          break;
        }

        revref <<= 2;
        ref >>= 2;
      }
      if (flag == 0)  // no match found
      {
        if (!left_search)  // start left search
        {
          left_search = true;
          left_search_start = true;
          // update ref and count with RC of first_read
          updaterefcount(read[first_rid], ref, revref, count, true, true, 0,
                         read_lengths[first_rid], ref_len, rg);
          ref_pos = 0;
          cur_read_pos = 0;
        } else  // left search done, now pick arbitrary read and start new
                // contig
        {
          left_search = false;
          for (int64_t j = remainingpos; j >= 0; j--) {
            if (remainingreads[j] == 1) {
              omp_set_lock(&read_lock[j & 0xffffff]);
              if (remainingreads[j])  // checking again inside critical block
              {
                current = j;
                remainingpos = j - 1;
                remainingreads[j] = 0;
                flag = 1;
                unmatched[tid]++;
              }
              omp_unset_lock(&read_lock[j & 0xffffff]);
              if (flag == 1) break;
            }
          }
          if (flag == 0) {
            if (prev_unmatched ==
                true)  // last read was singleton, write it now
            {
              foutorder_s.write((char *)&prev, sizeof(uint32_t));
            }
            done = 1;  // no reads left
          } else {
            updaterefcount(read[current], ref, revref, count, true, false, 0,
                           read_lengths[current], ref_len, rg);
            ref_pos = 0;
            cur_read_pos = 0;
            if (prev_unmatched == true)  // prev read singleton, write it now
            {
              foutorder_s.write((char *)&prev, sizeof(uint32_t));
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
  }  // parallel end

  delete[] remainingreads;
  std::cout << "Reordering done, "
            << std::accumulate(unmatched, unmatched + num_thr, 0)
            << " were unmatched\n";
  return;
}

bool search_match(bitset &ref, bitset *mask1, omp_lock_t *dict_lock,
                  omp_lock_t *read_lock,
                  bitset mask[MAX_READ_LEN][MAX_READ_LEN],
                  uint8_t *read_lengths, bool *remainingreads, bitset *read,
                  bbhashdict *dict, uint32_t &k, bool rev, int shift,
                  int &ref_len, reorder_global &rg) {
  bitset b;
  uint64_t ull;
  int64_t dictidx[2];    // to store the start and end index (end not inclusive)
                         // in the dict read_id array
  uint32_t startposidx;  // index in startpos
  bool flag = 0;
  for (int l = 0; l < rg.numdict; l++) {
    // check if dictionary index is within bound
    if (!rev) {
      if (dict[l].end + shift >= ref_len) continue;
    } else {
      if (dict[l].end >= ref_len + shift || dict[l].start <= shift) continue;
    }
    b = ref & mask1[l];
    ull = (b >> 2 * dict[l]start).to_ullong();
    startposidx = dict[l].bphf->lookup(ull);
    if (startposidx >= dict[l].numkeys)  // not found
      continue;
    // check if any other thread is modifying same dictpos
    omp_set_lock(&dict_lock[startposidx & 0xFFFFFF]);
    dict[l].findpos(dictidx, startposidx);
    if (dict[l].empty_bin[startposidx])  // bin is empty
    {
      omp_unset_lock(&dict_lock[startposidx & 0xFFFFFF]);
      continue;
    }
    uint64_t ull1 =
        ((read[dict[l].read_id[dictidx[0]]] & mask1[l]) >> 2 * dict[l].start)
            .to_ullong();
    if (ull == ull1)  // checking if ull is actually the key for this bin
    {
      for (int64_t i = dictidx[1] - 1;
           i >= dictidx[0] && i >= dictidx[1] - rg.maxsearch; i--) {
        auto rid = dict[l].read_id[i];
        int hamming;
        if (!rev)
          hamming = ((ref ^ read[rid]) &
                     mask[0][rg.max_readlen -
                             std::min<int>(ref_len - shift, read_lengths[rid])])
                        .count();
        else
          hamming =
              ((ref ^ read[rid]) &
               mask[shift][rg.max_readlen -
                           std::min<int>(ref_len + shift, read_lengths[rid])])
                  .count();
        if (hamming <= thresh) {
          omp_set_lock(&read_lock[rid & 0xFFFFFF]);
          if (remainingreads[rid]) {
            remainingreads[rid] = 0;
            k = rid;
            flag = 1;
          }
          omp_unset_lock(&read_lock[rid & 0xFFFFFF]);
          if (flag == 1) break;
        }
      }
    }
    omp_unset_lock(&dict_lock[startposidx & 0xFFFFFF]);
    if (flag == 1) break;
  }
  return flag;
}

void writetofile(bitset *read, uint8_t *read_lengths, reorder_global &rg) {
// convert bitset to string for all num_thr files in parallel
#pragma omp parallel
  {
    int tid = omp_get_thread_num();
    std::string tid_str = std::to_string(tid);
    std::ofstream fout(rg.outfile + '.' + tid_str, std::ofstream::out);
    std::ofstream fout_s(rg.outfile + ".singleton." + tid_str, std::ofstream::out);
    std::ifstream finRC(rg.outfileRC + '.' + tid_str, std::ifstream::in);
    std::ifstream finorder(rg.outfileorder + '.' + tid_str,
                           std::ifstream::in | std::ios::binary);
    std::ifstream finorder_s(rg.outfileorder + ".singleton." + tid_str,
                             std::ifstream::in | std::ios::binary);
    char s[MAX_READ_LEN + 1], s1[MAX_READ_LEN + 1];
    uint32_t current;
    char c;
    while (finRC >> std::noskipws >> c)  // read character by character
    {
      finorder.read((char *)&current, sizeof(uint32_t));
      bitsettostring(read[current], s, read_lengths[current]);
      if (c == 'd') {
        fout << s << "\n";
      } else {
        reverse_complement(s, s1, read_lengths[current]);
        fout << s1 << "\n";
      }
    }
    finorder_s.read((char *)&current, sizeof(uint32_t));
    while (!finorder_s.eof()) {
      bitsettostring(read[current], s, read_lengths[current]);
      fout_s << s << "\n";
      finorder_s.read((char *)&current, sizeof(uint32_t));
    }

    fout.close();
    fout_s.close();
    finRC.close();
    finorder.close();
    finorder_s.close();
  }

  // Now combine the num_thr order files
  std::ofstream fout_s(rg.outfile + ".singleton", std::ofstream::out);
  std::ofstream foutorder_s(rg.outfileorder + ".singleton",
                            std::ofstream::out | std::ios::binary);
  for (int tid = 0; tid < rg.num_thr; tid++) {
    std::string tid_str = std::to_string(tid);
    std::ifstream fin_s(rg.outfile + ".singleton." + tid_str, std::ifstream::in);
    std::ifstream finorder_s(rg.outfileorder + ".singleton." + tid_str,
                             std::ifstream::in | std::ios::binary);

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

void bitsettostring(bitset b, char *s, uint8_t readlen, reorder_global &rg) {
  unsigned long long ull;
  for (int i = 0; i < 2 * readlen / 64 + 1; i++) {
    ull = (b & rg.mask64).to_ullong();
    b >>= 64;
    for (int j = 32 * i; j < 32 * i + 32 && j < readlen; j++) {
      s[j] = rg.revinttochar[ull % 4];
      ull /= 4;
    }
  }
  s[readlen] = '\0';
  return;
}

void updaterefcount(bitset &cur, bitset &ref, bitset &revref,
                    int count[][MAX_READ_LEN], bool resetcount, bool rev,
                    int shift, uint8_t cur_readlen, int &ref_len, reorder_global &rg)
// for var length, shift represents shift of start positions, if read length is
// small, may not need to shift actually
{
  char s[MAX_READ_LEN + 1], s1[MAX_READ_LEN + 1], *current;
  bitsettostring(cur, s, cur_readlen);
  if (rev == false)
    current = s;
  else {
    reverse_complement(s, s1, cur_readlen);
    current = s1;
  }

  if (resetcount == true)  // resetcount - unmatched read so start over
  {
    std::memset(count, 0, sizeof(count[0][0]) * 4 * MAX_READ_LEN);
    for (int i = 0; i < cur_readlen; i++) {
      count[rg.chartoint[current[i]]][i] = 1;
    }
    ref_len = cur_readlen;
  } else {
    if (!rev) {
      // shift count
      for (int i = 0; i < ref_len - shift; i++) {
        for (int j = 0; j < 4; j++) count[j][i] = count[j][i + shift];
        if (i < cur_readlen) count[rg.chartoint[current[i]]][i] += 1;
      }

      // for the new positions set count to 1
      for (int i = ref_len - shift; i < cur_readlen; i++) {
        for (int j = 0; j < 4; j++) count[j][i] = 0;
        count[rg.chartoint[current[i]]][i] = 1;
      }
      ref_len = std::max<int>(ref_len - shift, cur_readlen);
    } else  // reverse case is quite complicated in the variable length case
    {
      if (cur_readlen - shift >= ref_len) {
        for (int i = cur_readlen - shift - ref_len; i < cur_readlen - shift;
             i++) {
          for (int j = 0; j < 4; j++)
            count[j][i] = count[j][i - (cur_readlen - shift - ref_len)];
          count[rg.chartoint[current[i]]][i] += 1;
        }
        for (int i = 0; i < cur_readlen - shift - ref_len; i++) {
          for (int j = 0; j < 4; j++) count[j][i] = 0;
          count[rg.chartoint[current[i]]][i] = 1;
        }
        for (int i = cur_readlen - shift; i < cur_readlen; i++) {
          for (int j = 0; j < 4; j++) count[j][i] = 0;
          count[rg.chartoint[current[i]]][i] = 1;
        }
        ref_len = cur_readlen;
      } else if (ref_len + shift <= max_readlen) {
        for (int i = ref_len - cur_readlen + shift; i < ref_len; i++)
          count[rg.chartoint[current[i - (ref_len - cur_readlen + shift)]]][i] +=
              1;
        for (int i = ref_len; i < ref_len + shift; i++) {
          for (int j = 0; j < 4; j++) count[j][i] = 0;
          count[rg.chartoint[current[i - (ref_len - cur_readlen + shift)]]][i] = 1;
        }
        ref_len = ref_len + shift;
      } else {
        for (int i = 0; i < max_readlen - shift; i++) {
          for (int j = 0; j < 4; j++)
            count[j][i] = count[j][i + (ref_len + shift - rg.max_readlen)];
        }
        for (int i = rg.max_readlen - cur_readlen; i < rg.max_readlen - shift; i++)
          count[rg.chartoint[current[i - (rg.max_readlen - cur_readlen)]]][i] += 1;
        for (int i = rg.max_readlen - shift; i < rg.max_readlen; i++) {
          for (int j = 0; j < 4; j++) count[j][i] = 0;
          count[rg.chartoint[current[i - (rg.max_readlen - cur_readlen)]]][i] = 1;
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
      current[i] = rg.inttochar[indmax];
    }
  }
  chartobitset(current, ref_len, ref);
  char revcurrent[MAX_READ_LEN];
  reverse_complement(current, revcurrent, ref_len);
  chartobitset(revcurrent, ref_len, revref);

  return;
}
