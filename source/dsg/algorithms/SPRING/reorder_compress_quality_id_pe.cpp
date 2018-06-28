#include <omp.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <string>
#include <vector>
#include "cluster.h"
#include "codebook.h"
#include "qv_compressor.h"
#include "sam_block.h"
#include "input/fastq/FastqFileReader.h"
#include "algorithms/SPRING/reorder_compress_quality_id.h"

struct reorder_compress_quality_id_global {
	uint8_t paired_id_code;
	bool preserve_order, paired_end, preserve_quality, preserve_id;
	std::string quality_mode;
	double quality_ratio;
	uint32_t numreads, numreads_by_2;
	int max_readlen, num_thr;
	char illumina_binning_table[128];

	std::string infile_id_1;
	std::string infile_id_2;
	std::string infile_order;
	std::string outfile_order;
	std::string infilenumreads;
	std::string basedir;
};

void generate_order(reorder_compress_quality_id_global &rg);
// generate reordering information for the two separate files (pairs) from
// read_order.bin

void reorder_quality(dsg::FastqFileReader &fastqFileReader1, dsg::FastqFileReader &fastqFileReader2, reorder_compress_quality_id_global &rg);
void reorder_id(reorder_compress_quality_id_global &rg);
void encode(FILE *fout, struct qv_options_t *opts, uint32_t max_readlen,
            uint32_t numreads, char *quality_array, uint8_t *read_lengths,
            std::string &infile_order, uint64_t startpos, reorder_compress_quality_id_global &rg);
void illumina_binning(char *quality, uint8_t readlen);
void generate_illumina_binning_table();

int reorder_compress_quality_id(std::string &working_dir, int max_readlen, int num_thr, bool preserve_order, bool paired_end, bool preserve_quality, bool preserve_id, dsg::FastqFileReader &fastqFileReader1, dsg::FastqFileReader &fastqFileReader2, std::string quality_mode, double quality_ratio) {
  reorder_compress_quality_id_global rg;	
  rg.basedir = working_dir;

  rg.infile_id_1 = rg.basedir + "/input_1.id";
  rg.infile_id_2 = rg.basedir + "/input_2.id";
  rg.infile_order = rg.basedir + "/read_order.bin";
  rg.outfile_order = rg.basedir + "/read_order.bin.tmp";
  rg.infilenumreads = rg.basedir + "/numreads.bin";

  rg.max_readlen = max_readlen;
  rg.num_thr = num_thr;
  rg.preserve_order = preserve_order;
  rg.paired_end = paired_end;
  rg.preserve_quality = preserve_quality;
  rg.preserve_id = preserve_id;
  if (rg.preserve_quality == true) {
    rg.quality_mode = quality_mode;
    rg.quality_ratio = quality_ratio;
  }
  if (rg.preserve_quality == false && rg.preserve_id == false) return 0;
  // fill illumina binning table
  generate_illumina_binning_table();

  omp_set_num_threads(rg.num_thr);
  std::ifstream f_numreads(rg.infilenumreads, std::ios::binary);
  f_numreads.seekg(4);
  f_numreads.read((char *)&rg.numreads, sizeof(uint32_t));
  f_numreads.read((char *)&rg.paired_id_code, sizeof(uint8_t));
  f_numreads.close();
  rg.numreads_by_2 = rg.numreads / 2;
  if (paired_end == false) rg.numreads_by_2 = rg.numreads;
  generate_order(rg);
  std::cout << "Compressing qualities and/or ids\n";
  if (preserve_quality == true) reorder_quality(fastqFileReader1, fastqFileReader2, rg);
  if (preserve_id == true) reorder_id(rg);
  if (paired_end == true) remove((rg.basedir + "/read_order.bin.tmp").c_str());
  return 0;
}

void generate_order(reorder_compress_quality_id_global &rg) {
  if (rg.preserve_order == true)  // write fake order information in this case to
                                 // provide common interface
  {
    std::ofstream fout_order(rg.outfile_order, std::ios::binary);
    for (uint32_t i = 0; i < rg.numreads_by_2; i++) {
      fout_order.write((char *)&i, sizeof(uint32_t));
    }
    fout_order.close();
  } else {
    if (paired_end == false)
      rg.outfile_order = rg.infile_order;
    else {
      std::ifstream fin_order(rg.infile_order, std::ios::binary);
      std::ofstream fout_order(rg.outfile_order, std::ios::binary);
      uint32_t order;
      for (uint32_t i = 0; i < rg.numreads; i++) {
        fin_order.read((char *)&order, sizeof(uint32_t));
        if (order < rg.numreads_by_2) {
          fout_order.write((char *)&order, sizeof(uint32_t));
        }
      }
      fin_order.close();
      fout_order.close();
    }
  }
  return;
}

void reorder_quality(dsg::FastqFileReader &fastqFileReader1, dsg::FastqFileReader &fastqFileReader2, reorder_compress_quality_id_global &rg) {
  std::string infile_fastq[2] = {infile_fastq_1, infile_fastq_2};
  std::string line;
  char line_ch[max_readlen + 1];
  uint8_t cur_readlen;
  if ((quality_mode == "bsc" || quality_mode == "illumina_binning_bsc") &&
      preserve_order == true)
  // just write to file without newlines
  {
    for (int k = 0; k < 2; k++) {
      if (k == 1 && paired_end == false) continue;
      std::ifstream f_in(infile_fastq[k]);
      std::ofstream f_out(basedir + "/quality_" + std::to_string(k + 1) +
                          ".txt");
      for (uint64_t i = 0; i < numreads_by_2; i++) {
        std::getline(f_in, line);
        std::getline(f_in, line);
        std::getline(f_in, line);
        f_in.getline(line_ch, max_readlen + 1);
        cur_readlen = f_in.gcount() - 1;
        if (quality_mode == "illumina_binning_bsc")
          illumina_binning(line_ch, cur_readlen);
        f_out.write(line_ch, cur_readlen);
      }
      f_in.close();
      f_out.close();
    }
    return;
  }
  char *quality = new char[(uint64_t)numreads_by_2 * (max_readlen + 1)];
  uint8_t *read_lengths = new uint8_t[numreads_by_2];

  for (int k = 0; k < 2; k++) {
    if (k == 1 && paired_end == false) continue;
    std::ifstream f_in(infile_fastq[k]);

    for (uint64_t i = 0; i < numreads_by_2; i++) {
      std::getline(f_in, line);
      std::getline(f_in, line);
      std::getline(f_in, line);
      f_in.getline((quality + i * (max_readlen + 1)), max_readlen + 1);
      read_lengths[i] = f_in.gcount() - 1;
      if (quality_mode == "illumina_binning_qvz" ||
          quality_mode == "illumina_binning_bsc")
        illumina_binning(quality + i * (max_readlen + 1), read_lengths[i]);
    }
    f_in.close();

    if (quality_mode == "bsc" || quality_mode == "illumina_binning_bsc")
    // just write to file without newlines
    {
      std::ofstream f_out(basedir + "/quality_" + std::to_string(k + 1) +
                          ".txt");
      std::ifstream f_order(outfile_order, std::ios::binary);
      uint32_t order;
      for (uint64_t i = 0; i < numreads_by_2; i++) {
        f_order.read((char *)&order, sizeof(uint32_t));
        f_out.write(quality + (uint64_t)order * (max_readlen + 1),
                    read_lengths[order]);
      }
      f_out.close();
      continue;
    }
#pragma omp parallel
    {
      int tid = omp_get_thread_num();
      uint64_t start = uint64_t(tid) * (numreads_by_2 / omp_get_num_threads());
      uint32_t numreads_thr = numreads_by_2 / omp_get_num_threads();
      if (tid == omp_get_num_threads() - 1)
        numreads_thr =
            numreads_by_2 - numreads_thr * (omp_get_num_threads() - 1);
      struct qv_options_t opts;
      opts.verbose = 0;
      opts.stats = 0;
      opts.ratio = quality_ratio;
      opts.clusters = 1;
      opts.uncompressed = 0;
      opts.distortion = DISTORTION_MSE;
      opts.cluster_threshold = 4;
      std::string output_name_str =
          (basedir + "/compressed_quality_" + std::to_string(k + 1) + ".bin." +
           std::to_string(tid));
      const char *output_name = output_name_str.c_str();
      FILE *fout;
      fout = fopen(output_name, "wb");
      opts.mode = MODE_FIXED;
      encode(fout, &opts, max_readlen, numreads_thr, quality, read_lengths,
             outfile_order, start * sizeof(uint32_t));
    }
  }
  delete[] quality;
  delete[] read_lengths;
  return;
}

void reorder_id(reorder_compress_quality_id_global &rg) {
  std::string *id = new std::string[rg.numreads_by_2];
  std::string infile_id[2] = {rg.infile_id_1, rg.infile_id_2};
  for (int k = 0; k < 2; k++) {
    if (k == 1 && paired_end == false) continue;
    if (paired_id_code != 0 && k == 1) break;
    std::ifstream f_in(infile_id[k]);

    for (uint64_t i = 0; i < numreads_by_2; i++) std::getline(f_in, id[i]);
    f_in.close();
#pragma omp parallel
    {
      int tid = omp_get_thread_num();
      uint64_t start = uint64_t(tid) * (numreads_by_2 / omp_get_num_threads());
      uint32_t numreads_thr = numreads_by_2 / omp_get_num_threads();
      if (tid == omp_get_num_threads() - 1)
        numreads_thr =
            numreads_by_2 - numreads_thr * (omp_get_num_threads() - 1);
      std::ifstream f_order(outfile_order, std::ios::binary);
      f_order.seekg(start * sizeof(uint32_t));
      std::string outfile_compressed_id_str =
          (basedir + "/compressed_id_" + std::to_string(k + 1) + ".bin." +
           std::to_string(tid));
      const char *outfile_compressed_id = outfile_compressed_id_str.c_str();
      struct compressor_info_t comp_info;
      comp_info.id_array = id;
      comp_info.f_order = &f_order;
      comp_info.numreads = numreads_thr;
      comp_info.mode = COMPRESSION;
      comp_info.fcomp = fopen(outfile_compressed_id, "w");
      compress((void *)&comp_info);
      fclose(comp_info.fcomp);
      f_order.close();
    }
  }
  delete[] id;
  return;
}

void illumina_binning(char *quality, uint8_t readlen) {
  for (uint8_t i = 0; i < readlen; i++)
    quality[i] = illumina_binning_table[quality[i]];
  return;
}

void generate_illumina_binning_table() {
  for (uint8_t i = 0; i <= 33 + 1; i++) illumina_binning_table[i] = 33 + 0;
  for (uint8_t i = 33 + 2; i <= 33 + 9; i++) illumina_binning_table[i] = 33 + 6;
  for (uint8_t i = 33 + 10; i <= 33 + 19; i++)
    illumina_binning_table[i] = 33 + 15;
  for (uint8_t i = 33 + 20; i <= 33 + 24; i++)
    illumina_binning_table[i] = 33 + 22;
  for (uint8_t i = 33 + 25; i <= 33 + 29; i++)
    illumina_binning_table[i] = 33 + 27;
  for (uint8_t i = 33 + 30; i <= 33 + 34; i++)
    illumina_binning_table[i] = 33 + 33;
  for (uint8_t i = 33 + 35; i <= 33 + 39; i++)
    illumina_binning_table[i] = 33 + 37;
  for (uint8_t i = 33 + 40; i <= 127; i++) illumina_binning_table[i] = 33 + 40;
  return;
}
