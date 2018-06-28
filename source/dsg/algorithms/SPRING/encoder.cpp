#include <omp.h>
#include <algorithm>
#include <array>
#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <numeric>
#include <string>
#include <string>
#include <vector>
#include "algorithms/SPRING/encoder.h"

struct encoder_global {
	uint32_t numreads, numreads_s, numreads_N;
	int numdict_s = 2;
	uint32_t dict_start[2];
	uint32_t dict_end[2];
	int thresh_s = 24;
	int maxsearch = 1000;
	int max_readlen, num_thr;

	typedef std::bitset<3 * MAX_READ_LEN> bitset;



	std::string basedir;
	std::string infile;
	std::string infile_flag;
	std::string infile_pos;
	std::string infile_seq;
	std::string infile_RC;
	std::string infile_readlength;
	std::string infile_N;
	std::string outfile_N;
	std::string outfile_seq;
	std::string outfile_pos;
	std::string outfile_noise;
	std::string outfile_noisepos;
	std::string outfile_singleton;
	std::string infile_order;
	std::string infile_order_N;
	std::string infilenumreads;

	char longtochar[] = {'A', 'C', 'G', 'T', 'N'};
	long chartolong[128];
	char enc_noise[128][128];

	// Some global arrays (some initialized in setglobalarrays())
	char revinttochar[8] = {'A', 'N', 'G', '#',
				'C', '#', 'T', '#'};  // used in bitsettostring
	char inttochar[] = {'A', 'C', 'G', 'T', 'N'};
	char chartorevchar[128];             // A-T etc for reverse complement
	bitset basemask[MAX_READ_LEN][128];  // bitset for A,G,C,T,N at each position
	// used in stringtobitset, chartobitset and bitsettostring
	bitset positionmask[MAX_READ_LEN];  // bitset for each position (1 at two bits
					    // and 0 elsewhere)
	// used in bitsettostring
	bitset mask63;  // bitset with 63 bits set to 1 (used in bitsettostring for
			// conversion to ullong)
};

struct contig_reads {
  std::string read;
  int64_t pos;
  char RC;
  uint32_t order;
  uint8_t read_length;
};

void correct_order(uint32_t *order_s, encoder_global &eg);
// correct singleton and clean read order to their actual position in fastq (as
// opposed to pos in clean reads)

void readsingletons(bitset *read, uint32_t *order_s, uint8_t *read_lengths_s, encoder_global &eg);

std::string bitsettostring(bitset b, uint8_t readlen, encoder_global &eg);

void writetofile(bitset *read, encoder_global &eg);

void encode(bitset *read, bbhashdict *dict, uint32_t *order_s,
            uint8_t *read_lengths_s, encoder_global &eg);

void packbits(encoder_global &eg);

std::string buildcontig(std::list<contig_reads> &current_contig,
                        uint32_t list_size, encoder_global &eg);  // using lists to enable faster
                                              // insertion of singletons

void writecontig(std::string &ref, std::list<contig_reads> &current_contig,
                 std::ofstream &f_seq, std::ofstream &f_pos,
                 std::ofstream &f_noise, std::ofstream &f_noisepos,
                 std::ofstream &f_order, std::ofstream &f_RC,
                 std::ofstream &f_readlength, encoder_global &eg);

void getDataParams(encoder_global &eg);

void setglobalarrays(encoder_global &eg);

void encoder_main(std::string &working_dir, int max_readlen, int num_thr) {
  encoder_global eg;
  eg.basedir = std::string(argv[1]);
  eg.infile = eg.basedir + "/temp.dna";
  eg.infile_pos = eg.basedir + "/temppos.txt";
  eg.infile_flag = eg.basedir + "/tempflag.txt";
  eg.infile_order = eg.basedir + "/read_order.bin";
  eg.infile_order_N = eg.basedir + "/read_order_N.bin";
  eg.infile_RC = eg.basedir + "/read_rev.txt";
  eg.infile_readlength = eg.basedir + "/read_lengths.bin";
  eg.infile_N = eg.basedir + "/input_N.dna";
  eg.outfile_N = eg.basedir + "/unaligned_N.txt";
  eg.outfile_seq = eg.basedir + "/read_seq.txt";
  eg.outfile_pos = eg.basedir + "/read_pos.txt";
  eg.outfile_noise = eg.basedir + "/read_noise.txt";
  eg.outfile_noisepos = eg.basedir + "/read_noisepos.txt";
  eg.outfile_singleton = eg.basedir + "/unaligned_singleton.txt";
  eg.infilenumreads = eg.basedir + "/numreads.bin";

  eg.max_readlen = max_readlen;
  eg.num_thr = num_thr;

  omp_set_num_threads(eg.num_thr);
  getDataParams(eg);  // populate numreads
  setglobalarrays(eg);
  bitset *read = new bitset[eg.numreads_s + eg.numreads_N];
  uint32_t *order_s = new uint32_t[eg.numreads_s + eg.numreads_N];
  uint8_t *read_lengths_s = new uint8_t[eg.numreads_s + eg.numreads_N];
  readsingletons(read, order_s, read_lengths_s);
  correct_order(order_s);

  bbhashdict *dict = new bbhashdict[eg.numdict_s];
  if (eg.max_readlen > 50) {
    dict[0].start = 0;
    dict[0].end = 20;
    dict[1].start = 21;
    dict[1].end = 41;
  } else {
    dict[0].start = 0;
    dict[0].end = 20 * eg.max_readlen / 50;
    dict[1].start = 20 * eg.max_readlen / 50 + 1;
    dict[1]end = 41 * eg.max_readlen / 50;
  }

  constructdictionary(read, dict, read_lengths_s);
  encode(read, dict, order_s, read_lengths_s, eg);
  delete[] read;
  delete[] dict;
  remove(eg.infile_N.c_str());
  return 0;
}

void encode(bitset *read, bbhashdict *dict, uint32_t *order_s,
            uint8_t *read_lengths_s, encoder_global &eg) {
  omp_lock_t *read_lock = new omp_lock_t[eg.numreads_s + eg.numreads_N];
  omp_lock_t *dict_lock = new omp_lock_t[eg.numreads_s + eg.numreads_N];
  for (int j = 0; j < eg.numreads_s + eg.numreads_N; j++) {
    omp_init_lock(&read_lock[j]);
    omp_init_lock(&dict_lock[j]);
  }
  bool *remainingreads = new bool[eg.numreads_s + eg.numreads_N];
  std::fill(remainingreads, remainingreads + eg.numreads_s + eg.numreads_N, 1);

  bitset mask1[eg.numdict_s];
  generateindexmasks(mask1);
  bitset mask[MAX_READ_LEN][MAX_READ_LEN];
  generatemasks(mask);
  std::cout << "Encoding reads\n";
#pragma omp parallel
  {
    int tid = omp_get_thread_num();
    std::ifstream f(eg.infile + '.' + std::to_string(tid));
    std::ifstream in_flag(eg.infile_flag + '.' + std::to_string(tid));
    std::ifstream in_pos(eg.infile_pos + '.' + std::to_string(tid),
                         std::ios::binary);
    std::ifstream in_order(eg.infile_order + '.' + std::to_string(tid),
                           std::ios::binary);
    std::ifstream in_RC(eg.infile_RC + '.' + std::to_string(tid));
    std::ifstream in_readlength(eg.infile_readlength + '.' + std::to_string(tid),
                                std::ios::binary);
    std::ofstream f_seq(eg.outfile_seq + '.' + std::to_string(tid));
    std::ofstream f_pos(eg.outfile_pos + '.' + std::to_string(tid));
    std::ofstream f_noise(eg.outfile_noise + '.' + std::to_string(tid));
    std::ofstream f_noisepos(eg.outfile_noisepos + '.' + std::to_string(tid));
    std::ofstream f_order(eg.infile_order + '.' + std::to_string(tid) + ".tmp",
                          std::ios::binary);
    std::ofstream f_RC(eg.infile_RC + '.' + std::to_string(tid) + ".tmp");
    std::ofstream f_readlength(
        eg.infile_readlength + '.' + std::to_string(tid) + ".tmp",
        std::ios::binary);
    int64_t dictidx[2];  // to store the start and end index (end not inclusive)
                         // in the dict read_id array
    uint32_t startposidx;  // index in startpos
    uint64_t ull;
    bool flag = 0;
    // flag to check if match was found or not
    std::string current, ref;
    bitset forward_bitset, reverse_bitset, b;
    char c, rc;
    std::list<contig_reads> current_contig;
    int64_t p;
    uint8_t rl;
    uint32_t ord, list_size = 0;  // list_size variable introduced because
                                  // list::size() was running very slowly
                                  // on UIUC machine
    std::list<uint32_t> deleted_rids[eg.numdict_s];
    bool done = false;
    while (!done) {
      if (!(in_flag >> c)) done = true;
      if (!done) {
        std::getline(f, current);
        rc = in_RC.get();
        in_pos.read((char *)&p, sizeof(int64_t));
        in_order.read((char *)&ord, sizeof(uint32_t));
        in_readlength.read((char *)&rl, sizeof(uint8_t));
      }
      if (c == '0' || done || list_size > 10000000)  // limit on list size so
                                                     // that memory doesn't get
                                                     // too large
      {
        if (list_size != 0) {
          // sort contig according to pos
          current_contig.sort([](const contig_reads &a, const contig_reads &b) {
            return a.pos < b.pos;
          });
          // make first pos zero and shift all pos values accordingly
          auto current_contig_it = current_contig.begin();
          int64_t first_pos = (*current_contig_it).pos;
          for (; current_contig_it != current_contig.end(); ++current_contig_it)
            (*current_contig_it).pos -= first_pos;

          ref = buildcontig(current_contig, list_size);
          // try to align the singleton reads to ref
          // first create bitsets from first readlen positions of ref
          forward_bitset.reset();
          reverse_bitset.reset();
          if (ref.size() >= eg.max_readlen) {
            stringtobitset(ref.substr(0, eg.max_readlen), eg.max_readlen,
                           forward_bitset);
            stringtobitset(
                reverse_complement(ref.substr(0, eg.max_readlen), eg.max_readlen),
                eg.max_readlen, reverse_bitset);
            for (long j = 0; j < ref.size() - eg.max_readlen + 1; j++) {
              // search for singleton reads
              for (int rev = 0; rev < 2; rev++) {
                for (int l = 0; l < eg.numdict_s; l++) {
                  if (!rev)
                    b = forward_bitset & mask1[l];
                  else
                    b = reverse_bitset & mask1[l];
                  ull = (b >> 3 * dict[l].start).to_ullong();
                  startposidx = dict[l].bphf->lookup(ull);
                  if (startposidx >= dict[l].numkeys)  // not found
                    continue;
                  // check if any other thread is modifying same dictpos
                  if (!omp_test_lock(&dict_lock[startposidx])) continue;
                  dict[l].findpos(dictidx, startposidx);
                  if (dict[l].empty_bin[startposidx])  // bin is empty
                  {
                    omp_unset_lock(&dict_lock[startposidx]);
                    continue;
                  }
                  uint64_t ull1 =
                      ((read[dict[l].read_id[dictidx[0]]] & mask1[l]) >>
                       3 * dict[l].start)
                          .to_ullong();
                  if (ull ==
                      ull1)  // checking if ull is actually the key for this bin
                  {
                    for (int64_t i = dictidx[1] - 1;
                         i >= dictidx[0] && i >= dictidx[1] - maxsearch; i--) {
                      auto rid = dict[l].read_id[i];
                      int hamming;
                      if (!rev)
                        hamming = ((forward_bitset ^ read[rid]) &
                                   mask[0][eg.max_readlen - read_lengths_s[rid]])
                                      .count();
                      else
                        hamming = ((reverse_bitset ^ read[rid]) &
                                   mask[0][eg.max_readlen - read_lengths_s[rid]])
                                      .count();
                      if (hamming <= eg.thresh_s) {
                        omp_set_lock(&read_lock[rid]);
                        if (remainingreads[rid]) {
                          remainingreads[rid] = 0;
                          flag = 1;
                        }
                        omp_unset_lock(&read_lock[rid]);
                      }
                      if (flag == 1)  // match found
                      {
                        flag = 0;
                        list_size++;
                        char rc = rev ? 'r' : 'd';
                        long pos =
                            rev ? (j + eg.max_readlen - read_lengths_s[rid]) : j;
                        std::string read_string =
                            rev ? reverse_complement(
                                      bitsettostring(read[rid],
                                                     read_lengths_s[rid]),
                                      read_lengths_s[rid])
                                : bitsettostring(read[rid],
                                                 read_lengths_s[rid]);
                        current_contig.push_back({read_string, pos, rc,
                                                  order_s[rid],
                                                  read_lengths_s[rid]});
                        for (int l1 = 0; l1 < eg.numdict_s; l1++) {
                          if (read_lengths_s[rid] > dict[l1].end)
                            deleted_rids[l1].push_back(rid);
                        }
                      }
                    }
                  }
                  omp_unset_lock(&dict_lock[startposidx]);
                  // delete from dictionaries
                  for (int l1 = 0; l1 < eg.numdict_s; l1++)
                    for (auto it = deleted_rids[l1].begin();
                         it != deleted_rids[l1].end();) {
                      b = read[*it] & mask1[l1];
                      ull = (b >> 3 * dict[l1].start).to_ullong();
                      startposidx = dict[l1].bphf->lookup(ull);
                      if (!omp_test_lock(&dict_lock[startposidx])) {
                        ++it;
                        continue;
                      }
                      dict[l1].findpos(dictidx, startposidx);
                      dict[l1].remove(dictidx, startposidx, *it);
                      it = deleted_rids[l1].erase(it);
                      omp_unset_lock(&dict_lock[startposidx]);
                    }
                }
              }
              if (j !=
                  ref.size() -
                      eg.max_readlen)  // not at last position,shift bitsets
              {
                forward_bitset >>= 3;
                forward_bitset = forward_bitset & mask[0][0];
                forward_bitset |=
                    basemask[eg.max_readlen - 1][ref[j + eg.max_readlen]];
                reverse_bitset <<= 3;
                reverse_bitset = reverse_bitset & mask[0][0];
                reverse_bitset |=
                    basemask[0][eg.chartorevchar[ref[j + eg.max_readlen]]];
              }

            }  // end for
          }    // end if
          // sort contig according to pos
          current_contig.sort([](const contig_reads &a, const contig_reads &b) {
            return a.pos < b.pos;
          });
          writecontig(ref, current_contig, f_seq, f_pos, f_noise, f_noisepos,
                      f_order, f_RC, f_readlength, eg);
        }
        if (!done) {
          current_contig = {{current, p, rc, ord, rl}};
          list_size = 1;
        }
      } else if (c == '1')  // read found during rightward search
      {
        current_contig.push_back({current, p, rc, ord, rl});
        list_size++;
      }
    }
    f.close();
    in_flag.close();
    in_pos.close();
    in_order.close();
    in_RC.close();
    in_readlength.close();
    f_seq.close();
    f_pos.close();
    f_noise.close();
    f_noisepos.close();
    f_order.close();
    f_readlength.close();
    f_RC.close();
  }

  // Combine files produced by the threads
  std::ofstream f_order(eg.infile_order);
  std::ofstream f_readlength(eg.infile_readlength);
  for (int tid = 0; tid < eg.num_thr; tid++) {
    std::ifstream in_order(eg.infile_order + '.' + std::to_string(tid) + ".tmp");
    std::ifstream in_readlength(eg.infile_readlength + '.' + std::to_string(tid) +
                                ".tmp");
    f_order << in_order.rdbuf();
    f_order.clear();  // clear error flag in case in_order is empty
    f_readlength << in_readlength.rdbuf();
    f_readlength.clear();  // clear error flag in case in_readlength is empty

    remove((eg.infile_order + '.' + std::to_string(tid)).c_str());
    remove((eg.infile_order + '.' + std::to_string(tid) + ".tmp").c_str());
    remove((eg.infile_readlength + '.' + std::to_string(tid)).c_str());
    remove((eg.infile_readlength + '.' + std::to_string(tid) + ".tmp").c_str());
    rename((eg.infile_RC + '.' + std::to_string(tid) + ".tmp").c_str(),
           (eg.infile_RC + '.' + std::to_string(tid)).c_str());
    remove((eg.infile_RC + '.' + std::to_string(tid) + ".tmp").c_str());
    remove((eg.infile_flag + '.' + std::to_string(tid)).c_str());
    remove((eg.infile_pos + '.' + std::to_string(tid)).c_str());
    remove((eg.infile + '.' + std::to_string(tid)).c_str());
  }
  f_order.close();
  f_readlength.close();
  // write remaining singleton reads now
  std::ofstream f_singleton(eg.outfile_singleton);
  f_order.open(eg.infile_order, std::ios::binary | std::ofstream::app);
  f_readlength.open(eg.infile_readlength, std::ios::binary | std::ofstream::app);
  std::ofstream f_N(eg.outfile_N);
  uint32_t matched_s = eg.numreads_s;
  for (uint32_t i = 0; i < eg.numreads_s; i++)
    if (remainingreads[i] == 1) {
      matched_s--;
      f_order.write((char *)&order_s[i], sizeof(uint32_t));
      f_readlength.write((char *)&read_lengths_s[i], sizeof(uint8_t));
      f_singleton << bitsettostring(read[i], read_lengths_s[i]);
    }
  uint32_t matched_N = eg.numreads_N;
  for (uint32_t i = eg.numreads_s; i < eg.numreads_s + eg.numreads_N; i++)
    if (remainingreads[i] == 1) {
      matched_N--;
      f_N << bitsettostring(read[i], read_lengths_s[i]);
      f_order.write((char *)&order_s[i], sizeof(uint32_t));
      f_readlength.write((char *)&read_lengths_s[i], sizeof(uint8_t));
    }
  f_order.close();
  f_readlength.close();
  f_N.close();
  f_singleton.close();
  delete[] remainingreads;
  packbits(eg);
  std::cout << "Encoding done:\n";
  std::cout << matched_s << " singleton reads were aligned\n";
  std::cout << matched_N << " reads with N were aligned\n";
  return;
}

std::string buildcontig(std::list<contig_reads> &current_contig,
                        uint32_t list_size, encoder_global &eg) {
  if (list_size == 1) return (current_contig.front()).read;
  auto current_contig_it = current_contig.begin();
  int64_t currentpos = 0, currentsize = 0, to_insert;
  std::vector<std::array<long, 4>> count;
  for (; current_contig_it != current_contig.end(); ++current_contig_it) {
    if (current_contig_it == current_contig.begin())  // first read
      to_insert = (*current_contig_it).read_length;
    else {
      currentpos = (*current_contig_it).pos;
      if (currentpos + (*current_contig_it).read_length > currentsize)
        to_insert = currentpos + (*current_contig_it).read_length - currentsize;
      else
        to_insert = 0;
    }
    count.insert(count.end(), to_insert, {0, 0, 0, 0});
    currentsize = currentsize + to_insert;
    for (long i = 0; i < (*current_contig_it).read_length; i++)
      count[currentpos + i][chartolong[(*current_contig_it).read[i]]] += 1;
  }
  std::string ref(count.size(), 'A');
  for (long i = 0; i < count.size(); i++) {
    long max = 0, indmax = 0;
    for (long j = 0; j < 4; j++)
      if (count[i][j] > max) {
        max = count[i][j];
        indmax = j;
      }
    ref[i] = longtochar[indmax];
  }
  return ref;
}

void writecontig(std::string &ref, std::list<contig_reads> &current_contig,
                 std::ofstream &f_seq, std::ofstream &f_pos,
                 std::ofstream &f_noise, std::ofstream &f_noisepos,
                 std::ofstream &f_order, std::ofstream &f_RC,
                 std::ofstream &f_readlength, encoder_global &eg) {
  f_seq << ref;
  char c;
  long prevj = 0;
  auto current_contig_it = current_contig.begin();
  long currentpos, prevpos = 0;
  for (; current_contig_it != current_contig.end(); ++current_contig_it) {
    currentpos = (*current_contig_it).pos;
    prevj = 0;
    for (long j = 0; j < (*current_contig_it).read_length; j++)
      if ((*current_contig_it).read[j] != ref[currentpos + j]) {
        f_noise << enc_noise[ref[currentpos + j]][(*current_contig_it).read[j]];
        c = j - prevj;
        f_noisepos << c;
        prevj = j;
      }
    f_noise << "\n";
    if (current_contig_it == current_contig.begin())
      c = eg.max_readlen;
    else
      c = currentpos - prevpos;
    f_pos << c;
    f_order.write((char *)&((*current_contig_it).order), sizeof(uint32_t));
    f_readlength.write((char *)&((*current_contig_it).read_length),
                       sizeof(uint8_t));
    f_RC << (*current_contig_it).RC;
    prevpos = currentpos;
  }
  return;
}

void packbits(encoder_global &eg) {
#pragma omp parallel
  {
    int tid = omp_get_thread_num();
    // seq
    std::ifstream in_seq(eg.outfile_seq + '.' + std::to_string(tid));
    std::ofstream f_seq(eg.outfile_seq + '.' + std::to_string(tid) + ".tmp",
                        std::ios::binary);
    std::ofstream f_seq_tail(eg.outfile_seq + '.' + std::to_string(tid) + ".tail");
    uint64_t file_len = 0;
    char c;
    while (in_seq >> std::noskipws >> c) file_len++;
    uint8_t basetoint[128];
    basetoint['A'] = 0;
    basetoint['C'] = 1;
    basetoint['G'] = 2;
    basetoint['T'] = 3;

    in_seq.close();
    in_seq.open(eg.outfile_seq + '.' + std::to_string(tid));
    char dnabase[8];
    uint8_t dnabin;
    for (uint64_t i = 0; i < file_len / 4; i++) {
      in_seq.read(dnabase, 4);

      dnabin = 64 * basetoint[dnabase[3]] + 16 * basetoint[dnabase[2]] +
               4 * basetoint[dnabase[1]] + basetoint[dnabase[0]];
      f_seq.write((char *)&dnabin, sizeof(uint8_t));
    }
    f_seq.close();
    in_seq.read(dnabase, file_len % 4);
    for (int i = 0; i < file_len % 4; i++) f_seq_tail << dnabase[i];
    f_seq_tail.close();
    in_seq.close();
    remove((eg.outfile_seq + '.' + std::to_string(tid)).c_str());
    rename((eg.outfile_seq + '.' + std::to_string(tid) + ".tmp").c_str(),
           (eg.outfile_seq + '.' + std::to_string(tid)).c_str());

    // rev
    std::ifstream in_rev(eg.infile_RC + '.' + std::to_string(tid));
    std::ofstream f_rev(eg.infile_RC + '.' + std::to_string(tid) + ".tmp",
                        std::ios::binary);
    std::ofstream f_rev_tail(eg.infile_RC + '.' + std::to_string(tid) + ".tail");
    file_len = 0;
    while (in_rev >> std::noskipws >> c) file_len++;
    basetoint['d'] = 0;
    basetoint['r'] = 1;

    in_rev.close();
    in_rev.open(eg.infile_RC + '.' + std::to_string(tid));
    for (uint64_t i = 0; i < file_len / 8; i++) {
      in_rev.read(dnabase, 8);
      dnabin = 128 * basetoint[dnabase[7]] + 64 * basetoint[dnabase[6]] +
               32 * basetoint[dnabase[5]] + 16 * basetoint[dnabase[4]] +
               8 * basetoint[dnabase[3]] + 4 * basetoint[dnabase[2]] +
               2 * basetoint[dnabase[1]] + basetoint[dnabase[0]];
      f_rev.write((char *)&dnabin, sizeof(uint8_t));
    }
    f_rev.close();
    in_rev.read(dnabase, file_len % 8);
    for (int i = 0; i < file_len % 8; i++) f_rev_tail << dnabase[i];
    f_rev_tail.close();
    remove((eg.infile_RC + '.' + std::to_string(tid)).c_str());
    rename((eg.infile_RC + '.' + std::to_string(tid) + ".tmp").c_str(),
           (eg.infile_RC + '.' + std::to_string(tid)).c_str());
  }
  // singleton
  std::ifstream in_singleton(eg.outfile_singleton);
  std::ofstream f_singleton(eg.outfile_singleton + ".tmp", std::ios::binary);
  std::ofstream f_singleton_tail(eg.outfile_singleton + ".tail");
  uint64_t file_len = 0;
  char c;
  while (in_singleton >> std::noskipws >> c) file_len++;
  uint8_t basetoint[128];
  basetoint['A'] = 0;
  basetoint['C'] = 1;
  basetoint['G'] = 2;
  basetoint['T'] = 3;
  in_singleton.close();
  in_singleton.open(eg.outfile_singleton);
  char dnabase[8];
  uint8_t dnabin;
  for (uint64_t i = 0; i < file_len / 4; i++) {
    in_singleton.read(dnabase, 4);

    dnabin = 64 * basetoint[dnabase[3]] + 16 * basetoint[dnabase[2]] +
             4 * basetoint[dnabase[1]] + basetoint[dnabase[0]];
    f_singleton.write((char *)&dnabin, sizeof(uint8_t));
  }
  f_singleton.close();
  in_singleton.read(dnabase, file_len % 4);
  for (int i = 0; i < file_len % 4; i++) f_singleton_tail << dnabase[i];
  f_singleton_tail.close();
  in_singleton.close();
  remove((eg.outfile_singleton).c_str());
  rename((eg.outfile_singleton + ".tmp").c_str(), (eg.outfile_singleton).c_str());
  return;
}

void setglobalarrays(encoder_global &eg) {
  eg.chartorevchar['A'] = 'T';
  eg.chartorevchar['C'] = 'G';
  eg.chartorevchar['G'] = 'C';
  eg.chartorevchar['T'] = 'A';
  eg.chartorevchar['N'] = 'N';

  for (int i = 0; i < 63; i++) mask63[i] = 1;
  for (int i = 0; i < max_readlen; i++) {
    eg.basemask[i]['A'][3 * i] = 0;
    eg.basemask[i]['A'][3 * i + 1] = 0;
    eg.basemask[i]['A'][3 * i + 2] = 0;
    eg.basemask[i]['C'][3 * i] = 0;
    eg.basemask[i]['C'][3 * i + 1] = 0;
    eg.basemask[i]['C'][3 * i + 2] = 1;
    eg.basemask[i]['G'][3 * i] = 0;
    eg.basemask[i]['G'][3 * i + 1] = 1;
    eg.basemask[i]['G'][3 * i + 2] = 0;
    eg.basemask[i]['T'][3 * i] = 0;
    eg.basemask[i]['T'][3 * i + 1] = 1;
    eg.basemask[i]['T'][3 * i + 2] = 1;
    eg.basemask[i]['N'][3 * i] = 1;
    eg.basemask[i]['N'][3 * i + 1] = 0;
    eg.basemask[i]['N'][3 * i + 2] = 0;
    eg.positionmask[i][3 * i] = 1;
    eg.positionmask[i][3 * i + 1] = 1;
    eg.positionmask[i][3 * i + 2] = 1;
  }

  // enc_noise uses substitution statistics from Minoche et al.
  eg.enc_noise['A']['C'] = '0';
  eg.enc_noise['A']['G'] = '1';
  eg.enc_noise['A']['T'] = '2';
  eg.enc_noise['A']['N'] = '3';
  eg.enc_noise['C']['A'] = '0';
  eg.enc_noise['C']['G'] = '1';
  eg.enc_noise['C']['T'] = '2';
  eg.enc_noise['C']['N'] = '3';
  eg.enc_noise['G']['T'] = '0';
  eg.enc_noise['G']['A'] = '1';
  eg.enc_noise['G']['C'] = '2';
  eg.enc_noise['G']['N'] = '3';
  eg.enc_noise['T']['G'] = '0';
  eg.enc_noise['T']['C'] = '1';
  eg.enc_noise['T']['A'] = '2';
  eg.enc_noise['T']['N'] = '3';
  eg.enc_noise['N']['A'] = '0';
  eg.enc_noise['N']['G'] = '1';
  eg.enc_noise['N']['C'] = '2';
  eg.enc_noise['N']['T'] = '3';
  eg.chartolong['A'] = 0;
  eg.chartolong['C'] = 1;
  eg.chartolong['G'] = 2;
  eg.chartolong['T'] = 3;
  eg.chartolong['N'] = 4;
  return;
}

void getDataParams(encoder_global &eg) {
  std::ifstream f_numreads(eg.infilenumreads, std::ios::binary);
  uint32_t numreads_clean, numreads_total;
  f_numreads.read((char *)&numreads_clean, sizeof(uint32_t));
  f_numreads.read((char *)&numreads_total, sizeof(uint32_t));
  f_numreads.close();

  std::ifstream myfile_s(eg.infile + ".singleton", std::ifstream::in);
  numreads_s = 0;
  std::string line;
  while (std::getline(myfile_s, line)) ++eg.numreads_s;
  myfile_s.close();
  eg.numreads = numreads_clean - eg.numreads_s;
  eg.numreads_N = numreads_total - numreads_clean;

  std::cout << "Maximum Read length: " << eg.max_readlen << std::endl;
  std::cout << "Number of non-singleton reads: " << eg.numreads << std::endl;
  std::cout << "Number of singleton reads: " << eg.numreads_s << std::endl;
  std::cout << "Number of reads with N: " << eg.numreads_N << std::endl;
}

void correct_order(uint32_t *order_s, encoder_global &eg) {
  uint32_t numreads_total = eg.numreads + eg.numreads_s + eg.numreads_N;
  bool *read_flag_N = new bool[numreads_total]();
  // bool array indicating N reads
  for (uint32_t i = 0; i < eg.numreads_N; i++) {
    read_flag_N[order_s[eg.numreads_s + i]] = true;
  }

  uint32_t *cumulative_N_reads = new uint32_t[eg.numreads + eg.numreads_s];
  // number of reads occuring before pos in clean reads
  uint32_t pos_in_clean = 0, num_N_reads_till_now = 0;
  for (uint32_t i = 0; i < numreads_total; i++) {
    if (read_flag_N[i] == true)
      num_N_reads_till_now++;
    else
      cumulative_N_reads[pos_in_clean++] = num_N_reads_till_now;
  }

  // First correct the order for singletons
  for (uint32_t i = 0; i < eg.numreads_s; i++)
    order_s[i] += cumulative_N_reads[order_s[i]];

  // Now correct for clean reads (this is stored on file)
  for (int tid = 0; tid < eg.num_thr; tid++) {
    std::ifstream fin_order(eg.infile_order + '.' + std::to_string(tid),
                            std::ios::binary);
    std::ofstream fout_order(eg.infile_order + '.' + std::to_string(tid) + ".tmp",
                             std::ios::binary);
    uint32_t pos;
    fin_order.read((char *)&pos, sizeof(uint32_t));
    while (!fin_order.eof()) {
      pos += cumulative_N_reads[pos];
      fout_order.write((char *)&pos, sizeof(uint32_t));
      fin_order.read((char *)&pos, sizeof(uint32_t));
    }
    fin_order.close();
    fout_order.close();
    remove((eg.infile_order + '.' + std::to_string(tid)).c_str());
    rename((eg.infile_order + '.' + std::to_string(tid) + ".tmp").c_str(),
           (eg.infile_order + '.' + std::to_string(tid)).c_str());
  }
  remove(eg.infile_order_N.c_str());
  delete[] read_flag_N;
  delete[] cumulative_N_reads;
  return;
}

void readsingletons(bitset *read, uint32_t *order_s, uint8_t *read_lengths_s, encoder_global &eg) {
  // not parallelized right now since these are very small number of reads
  std::ifstream f(eg.infile + ".singleton", std::ifstream::in);
  std::string s;
  for (uint32_t i = 0; i < eg.numreads_s; i++) {
    std::getline(f, s);
    read_lengths_s[i] = s.length();
    stringtobitset(s, read_lengths_s[i], read[i]);
  }
  f.close();
  f.open(eg.infile_N);
  for (uint32_t i = eg.numreads_s; i < eg.numreads_s + eg.numreads_N; i++) {
    std::getline(f, s);
    read_lengths_s[i] = s.length();
    stringtobitset(s, read_lengths_s[i], read[i]);
  }
  std::ifstream f_order_s(eg.infile_order + ".singleton", std::ios::binary);
  for (uint32_t i = 0; i < eg.numreads_s; i++)
    f_order_s.read((char *)&order_s[i], sizeof(uint32_t));
  f_order_s.close();
  std::ifstream f_order_N(eg.infile_order_N, std::ios::binary);
  for (uint32_t i = eg.numreads_s; i < eg.numreads_s + eg.numreads_N; i++)
    f_order_N.read((char *)&order_s[i], sizeof(uint32_t));
  f_order_N.close();
}

std::string bitsettostring(bitset b, uint8_t readlen, encoder_global &eg) {
  char s[MAX_READ_LEN + 1];
  s[readlen] = '\0';
  unsigned long long ull, rem;
  for (int i = 0; i < 3 * readlen / 63 + 1; i++) {
    ull = (b & eg.mask63).to_ullong();
    b >>= 63;
    for (int j = 21 * i; j < 21 * i + 21 && j < readlen; j++) {
      s[j] = eg.revinttochar[ull % 8];
      ull /= 8;
    }
  }
  std::string s1 = s;
  return s1;
}
