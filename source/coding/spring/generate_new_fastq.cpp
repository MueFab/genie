#include "spring/generate_new_fastq.h"
#include "spring/util.h"
#include "fileio/fastq_file_reader.h"
#include <iostream>

namespace spring {

void generate_new_fastq_se(dsg::input::fastq::FastqFileReader *fastqFileReader1,
  const std::string &temp_dir, const compression_params &cp) {

  uint32_t numreads = cp.num_reads;
  std::string basedir = temp_dir;
  std::string file_order = basedir + "/read_order.bin";
  std::string outfile_fastq  = basedir + "/new.fastq";
  bool preserve_order = cp.preserve_order;
  uint32_t *order_array;
  // array containing index mapping position in original fastq to
  // position after reordering
  order_array = new uint32_t[numreads];
  if (!preserve_order) {
    generate_order_array(file_order, order_array, numreads);
  } else {
    for (uint32_t i = 0; i < numreads; i++)
      order_array[i] = i; // TODO: this is NOT the most efficient method for order preserving
  }
  uint32_t str_array_size = numreads/8+1;
  // numreads/8+1 chosen so that these many FASTQ records can be stored in
  // memory without exceeding the RAM consumption of reordering stage
  std::string *read_array = new std::string[str_array_size];
  std::string *id_array = new std::string[str_array_size];
  std::string *quality_array = new std::string[str_array_size];

  std::ofstream fout_fastq(outfile_fastq);

  for (uint32_t i = 0; i <= numreads / str_array_size; i++) {
    uint32_t num_reads_bin = str_array_size;
    if (i == numreads / str_array_size)
      num_reads_bin = numreads % str_array_size;
    if (num_reads_bin == 0) break;
    uint32_t start_read_bin = i * str_array_size;
    uint32_t end_read_bin = i * str_array_size + num_reads_bin;
    // Read the file and pick up lines corresponding to this bin
    fastqFileReader1->seekFromSet(0);
    std::vector<dsg::input::fastq::FastqRecord> fastqRecords;
    for (uint32_t j = 0; j < numreads; j++) {
      fastqFileReader1->readRecords(1, &fastqRecords);
      if (order_array[j] >= start_read_bin && order_array[j] < end_read_bin) {
        uint32_t index = order_array[j] - start_read_bin;
        read_array[index] = fastqRecords[0].sequence;
        id_array[index] = fastqRecords[0].title;
        quality_array[index] = fastqRecords[0].qualityScores;
      }
    }
    for (uint32_t j = 0; j < num_reads_bin; j++) {
      fout_fastq << id_array[j] << "\n";
      fout_fastq << read_array[j] << "\n";
      fout_fastq << "+\n";
      fout_fastq << quality_array[j] << "\n";
    }
  }
  fout_fastq.close();
  delete[] order_array;
  delete[] id_array;
  delete[] read_array;
  delete[] quality_array;
  return;
}

void generate_new_fastq_pe(dsg::input::fastq::FastqFileReader *fastqFileReader1,
  dsg::input::fastq::FastqFileReader *fastqFileReader2,
  const std::string &temp_dir, const compression_params &cp) {
  dsg::input::fastq::FastqFileReader *fastqFileReader[2] =
                                          {fastqFileReader1, fastqFileReader2};
  uint32_t numreads = cp.num_reads;
  uint32_t numreads_by_2 = numreads/2;
  std::string basedir = temp_dir;
  std::string file_order = basedir + "/read_order_single.bin";
  std::string outfile_fastq  = basedir + "/new.fastq";
  bool preserve_order = cp.preserve_order;
  uint32_t *order_array;
  // array containing index mapping position in original fastq to
  // position after reordering
  order_array = new uint32_t[numreads_by_2];
  if (!preserve_order) {
    generate_order_array(file_order, order_array, numreads_by_2);
  } else {
    for (uint32_t i = 0; i < numreads_by_2; i++) {
      order_array[i] = i; // TODO: this is NOT the most efficient method for order preserving
    }
  }
  uint32_t str_array_size = numreads_by_2/8+1;
  // numreads/8+1 chosen so that these many FASTQ records can be stored in
  // memory without exceeding the RAM consumption of reordering stage
  std::string *read_array_1 = new std::string[str_array_size];
  std::string *id_array = new std::string[str_array_size];
  std::string *quality_array_1 = new std::string[str_array_size];
  std::string *read_array_2 = new std::string[str_array_size];
  std::string *quality_array_2 = new std::string[str_array_size];

  std::ofstream fout_fastq(outfile_fastq);

  for (uint32_t i = 0; i <= numreads_by_2 / str_array_size; i++) {
    uint32_t num_reads_bin = str_array_size;
    if (i == numreads_by_2 / str_array_size)
      num_reads_bin = numreads_by_2 % str_array_size;
    if (num_reads_bin == 0) break;
    uint32_t start_read_bin = i * str_array_size;
    uint32_t end_read_bin = i * str_array_size + num_reads_bin;
    // Read the file and pick up lines corresponding to this bin
    fastqFileReader[0]->seekFromSet(0);
    fastqFileReader[1]->seekFromSet(0);
    std::vector<dsg::input::fastq::FastqRecord> fastqRecords_1, fastqRecords_2;
    for (uint32_t j = 0; j < numreads/2; j++) {
      fastqFileReader[0]->readRecords(1, &fastqRecords_1);
      fastqFileReader[1]->readRecords(1, &fastqRecords_2);
      if (order_array[j] >= start_read_bin && order_array[j] < end_read_bin) {
        uint32_t index = order_array[j] - start_read_bin;
        read_array_1[index] = fastqRecords_1[0].sequence;
        read_array_2[index] = fastqRecords_2[0].sequence;
        id_array[index] = fastqRecords_1[0].title;
        quality_array_1[index] = fastqRecords_1[0].qualityScores;
        quality_array_2[index] = fastqRecords_2[0].qualityScores;
      }
    }
    for (uint32_t j = 0; j < num_reads_bin; j++) {
      fout_fastq << id_array[j] << "\n";
      fout_fastq << read_array_1[j] << "\n";
      fout_fastq << "+\n";
      fout_fastq << quality_array_1[j] << "\n";
      fout_fastq << id_array[j] << "\n";
      fout_fastq << read_array_2[j] << "\n";
      fout_fastq << "+\n";
      fout_fastq << quality_array_2[j] << "\n";
    }
  }
  fout_fastq.close();
  delete[] order_array;
  delete[] id_array;
  delete[] read_array_1;
  delete[] quality_array_1;
  delete[] read_array_2;
  delete[] quality_array_2;
  return;
}

} // namespace spring
