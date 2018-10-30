#include "algorithms/SPRING/generate_new_fastq.h"
#include "algorithms/SPRING/reorder_compress_quality_id.h"
#include "algorithms/SPRING/util.h"
#include "input/fastq/FastqFileReader.h"

namespace spring {

void generate_new_fastq_se(dsg::input::fastq::FastqFileReader *fastqFileReader1,
  const std::string &temp_dir, const compression_params &cp) {

  uint32_t numreads = cp.num_reads;
  std::string basedir = temp_dir;
  std::string file_order = basedir + "/read_order.bin";
  std::string outfile_fastq  = basedir + "/new.fastq";
  uint32_t *order_array;
  // array containing index mapping position in original fastq to
  // position after reordering
  order_array = new uint32_t[numreads];
  generate_order(file_order, order_array, numreads);
  uint32_t str_array_size = numreads/8+1;
  // numreads/8+1 chosen so that these many FASTQ records can be stored in
  // memory without exceeding the RAM consumption of reordering stage
  std::string *read_array = new std::string[str_array_size];
  std::string *id_array = new std::string[str_array_size];
  std::string *quality_array = new std::string[str_array_size];

  std::ofstream fout_fastq(outfile_fastq);

  for (uint32_t i = 0; i <= num_reads_per_file / str_array_size; i++) {
    uint32_t num_reads_bin = str_array_size;
    if (i == num_reads_per_file / str_array_size)
      num_reads_bin = num_reads_per_file % str_array_size;
    if (num_reads_bin == 0) break;
    uint32_t start_read_bin = i * str_array_size;
    uint32_t end_read_bin = i * str_array_size + num_reads_bin;
    // Read the file and pick up lines corresponding to this bin
    fastqFileReader1->seekFromSet(0);
    std::vector<dsg::input::fastq::FastqRecord> fastqRecords;
    for (uint32_t j = 0; j < numreads; j++) {
      fastqFileReader1->readRecords(1, &fastqRecords);
      if (order_array[j] >= start_read_bin && order_array[j] < end_read_bin) {
        uint32_t index = order_array[j] - start_read_bin
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
  return;
}

} // namespace spring
