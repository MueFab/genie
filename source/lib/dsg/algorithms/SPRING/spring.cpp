#include <algorithm>
#include <boost/filesystem.hpp>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>  // std::setw
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "algorithms/SPRING/encoder.h"
#include "algorithms/SPRING/params.h"
#include "algorithms/SPRING/pe_encode.h"
#include "algorithms/SPRING/preprocess.h"
#include "algorithms/SPRING/reorder.h"
#include "algorithms/SPRING/reorder_compress_quality_id.h"
#include "algorithms/SPRING/generate_read_streams.h"
#include "algorithms/SPRING/spring.h"
#include "algorithms/SPRING/util.h"
#include "input/fastq/FastqFileReader.h"

namespace spring {

void generate_streams_SPRING(
    dsg::input::fastq::FastqFileReader *fastqFileReader1,
    dsg::input::fastq::FastqFileReader *fastqFileReader2, int num_thr,
    bool paired_end, const std::string &working_dir) {
  // generate random temp directory in the working directory
  std::string temp_dir;
  while (true) {
    std::string random_str = "tmp." + spring::random_string(10);
    temp_dir = working_dir + "/" + random_str + '/';
    if (!boost::filesystem::exists(temp_dir)) break;
  }
  if (!boost::filesystem::create_directory(temp_dir)) {
    throw std::runtime_error("Cannot create temporary directory.");
  }
  std::cout << "Temporary directory: " << temp_dir << "\n";

  std::cout << "Starting compression...\n";
  auto compression_start = std::chrono::steady_clock::now();

  std::string infile_1, infile_2, outfile;
  bool preserve_quality = true, preserve_id = true;
  compression_params *cp_ptr = new compression_params;
  compression_params &cp = *cp_ptr;
  cp.paired_end = paired_end;
  cp.preserve_id = preserve_id;
  cp.preserve_quality = preserve_quality;
  cp.num_reads_per_block = NUM_READS_PER_BLOCK;
  cp.num_thr = num_thr;

  std::cout << "Preprocessing ...\n";
  auto preprocess_start = std::chrono::steady_clock::now();
  preprocess(fastqFileReader1, fastqFileReader2, temp_dir, cp);
  auto preprocess_end = std::chrono::steady_clock::now();
  std::cout << "Preprocessing done!\n";
  std::cout << "Time for this step: "
            << std::chrono::duration_cast<std::chrono::seconds>(
                   preprocess_end - preprocess_start)
                   .count()
            << " s\n";

//  if (!long_flag) {
    std::cout << "Reordering ...\n";
    auto reorder_start = std::chrono::steady_clock::now();
    call_reorder(temp_dir, cp);
    auto reorder_end = std::chrono::steady_clock::now();
    std::cout << "Reordering done!\n";
    std::cout << "Time for this step: "
              << std::chrono::duration_cast<std::chrono::seconds>(reorder_end -
                                                                  reorder_start)
                     .count()
              << " s\n";

    std::cout << "Encoding ...\n";
    auto encoder_start = std::chrono::steady_clock::now();
    call_encoder(temp_dir, cp);
    auto encoder_end = std::chrono::steady_clock::now();
    std::cout << "Encoding done!\n";
    std::cout << "Time for this step: "
              << std::chrono::duration_cast<std::chrono::seconds>(encoder_end -
                                                                  encoder_start)
                     .count()
              << " s\n";

    if (preserve_quality || preserve_id) {
      std::cout << "Reordering and compressing quality and/or ids ...\n";
      auto rcqi_start = std::chrono::steady_clock::now();
      reorder_compress_quality_id(temp_dir, cp);
      auto rcqi_end = std::chrono::steady_clock::now();
      std::cout << "Reordering and compressing quality and/or ids done!\n";
      std::cout << "Time for this step: "
                << std::chrono::duration_cast<std::chrono::seconds>(rcqi_end -
                                                                    rcqi_start)
                       .count()
                << " s\n";
    }

    if (paired_end) {
      std::cout << "Encoding pairing information ...\n";
      auto pe_encode_start = std::chrono::steady_clock::now();
      pe_encode(temp_dir, cp);
      auto pe_encode_end = std::chrono::steady_clock::now();
      std::cout << "Encoding pairing information done!\n";
      std::cout << "Time for this step: "
                << std::chrono::duration_cast<std::chrono::seconds>(
                       pe_encode_end - pe_encode_start)
                       .count()
                << " s\n";
    }

    std::cout << "Generating read streams ...\n";
    auto grs_start = std::chrono::steady_clock::now();
    generate_read_streams(temp_dir, cp);
    auto grs_end = std::chrono::steady_clock::now();
    std::cout << "Generating read streams done!\n";
    std::cout << "Time for this step: "
              << std::chrono::duration_cast<std::chrono::seconds>(grs_end -
                                                                  grs_start)
                     .count()
              << " s\n";
// }

  // Write compression params to a file
  std::string compression_params_file = temp_dir + "/cp.bin";
  std::ofstream f_cp(compression_params_file, std::ios::binary);
  f_cp.write((char *)&cp, sizeof(compression_params));
  f_cp.close();

  // Print out sizes of reads, quality and id after compression
//  namespace fs = boost::filesystem;
//  uint64_t size_read = 0;
//  uint64_t size_quality = 0;
//  uint64_t size_id = 0;
//  fs::path p{temp_dir};
//  fs::directory_iterator itr{p};
//  for (; itr != fs::directory_iterator{}; ++itr) {
//    std::string current_file = itr->path().filename().string();
//    switch (current_file[0]) {
//      case 'r':
//        size_read += fs::file_size(itr->path());
//        break;
//      case 'q':
//        size_quality += fs::file_size(itr->path());
//        break;
//      case 'i':
//        size_id += fs::file_size(itr->path());
//        break;
//    }
//  }
//  std::cout << "\n";
//  std::cout << "Sizes of streams after compression: \n";
//  std::cout << "Reads:      " << std::setw(12) << size_read << " bytes\n";
//  std::cout << "Quality:    " << std::setw(12) << size_quality << " bytes\n";
//  std::cout << "ID:         " << std::setw(12) << size_id << " bytes\n";
//
//  auto tar_start = std::chrono::steady_clock::now();
//  std::cout << "Creating tar archive ...";
//  std::string tar_command = "tar -cf " + outfile + " -C " + temp_dir + " . ";
//  int tar_status = std::system(tar_command.c_str());
//  if (tar_status != 0)
//    throw std::runtime_error("Error occurred during tar archive generation.");
//  std::cout << "Tar archive done!\n";
//  auto tar_end = std::chrono::steady_clock::now();
//  std::cout << "Time for this step: "
//            << std::chrono::duration_cast<std::chrono::seconds>(tar_end -
//                                                                tar_start)
//                   .count()
//            << " s\n";
//
  delete cp_ptr;
  auto compression_end = std::chrono::steady_clock::now();
  std::cout << "Compression done!\n";
  std::cout << "Total time for compression: "
            << std::chrono::duration_cast<std::chrono::seconds>(
                   compression_end - compression_start)
                   .count()
            << " s\n";

//  fs::path p1{outfile};
//  std::cout << "\n";
//  std::cout << "Total size: " << std::setw(12) << fs::file_size(p1)
//            << " bytes\n";
  return;
}

void call_reorder(const std::string &temp_dir, compression_params &cp) {
  size_t bitset_size_reorder = (2 * cp.max_readlen - 1) / 64 * 64 + 64;
  switch (bitset_size_reorder) {
    case 64:
      reorder_main<64>(temp_dir, cp);
      break;
    case 128:
      reorder_main<128>(temp_dir, cp);
      break;
    case 192:
      reorder_main<192>(temp_dir, cp);
      break;
    case 256:
      reorder_main<256>(temp_dir, cp);
      break;
    case 320:
      reorder_main<320>(temp_dir, cp);
      break;
    case 384:
      reorder_main<384>(temp_dir, cp);
      break;
    case 448:
      reorder_main<448>(temp_dir, cp);
      break;
    case 512:
      reorder_main<512>(temp_dir, cp);
      break;
    case 576:
      reorder_main<576>(temp_dir, cp);
      break;
    case 640:
      reorder_main<640>(temp_dir, cp);
      break;
    case 704:
      reorder_main<704>(temp_dir, cp);
      break;
    case 768:
      reorder_main<768>(temp_dir, cp);
      break;
    case 832:
      reorder_main<832>(temp_dir, cp);
      break;
    case 896:
      reorder_main<896>(temp_dir, cp);
      break;
    case 960:
      reorder_main<960>(temp_dir, cp);
      break;
    case 1024:
      reorder_main<1024>(temp_dir, cp);
      break;
    default:
      throw std::runtime_error("Wrong bitset size.");
  }
}

void call_encoder(const std::string &temp_dir, compression_params &cp) {
  size_t bitset_size_encoder = (3 * cp.max_readlen - 1) / 64 * 64 + 64;
  switch (bitset_size_encoder) {
    case 64:
      encoder_main<64>(temp_dir, cp);
      break;
    case 128:
      encoder_main<128>(temp_dir, cp);
      break;
    case 192:
      encoder_main<192>(temp_dir, cp);
      break;
    case 256:
      encoder_main<256>(temp_dir, cp);
      break;
    case 320:
      encoder_main<320>(temp_dir, cp);
      break;
    case 384:
      encoder_main<384>(temp_dir, cp);
      break;
    case 448:
      encoder_main<448>(temp_dir, cp);
      break;
    case 512:
      encoder_main<512>(temp_dir, cp);
      break;
    case 576:
      encoder_main<576>(temp_dir, cp);
      break;
    case 640:
      encoder_main<640>(temp_dir, cp);
      break;
    case 704:
      encoder_main<704>(temp_dir, cp);
      break;
    case 768:
      encoder_main<768>(temp_dir, cp);
      break;
    case 832:
      encoder_main<832>(temp_dir, cp);
      break;
    case 896:
      encoder_main<896>(temp_dir, cp);
      break;
    case 960:
      encoder_main<960>(temp_dir, cp);
      break;
    case 1024:
      encoder_main<1024>(temp_dir, cp);
      break;
    case 1088:
      encoder_main<1088>(temp_dir, cp);
      break;
    case 1152:
      encoder_main<1152>(temp_dir, cp);
      break;
    case 1216:
      encoder_main<1216>(temp_dir, cp);
      break;
    case 1280:
      encoder_main<1280>(temp_dir, cp);
      break;
    case 1344:
      encoder_main<1344>(temp_dir, cp);
      break;
    case 1408:
      encoder_main<1408>(temp_dir, cp);
      break;
    case 1472:
      encoder_main<1472>(temp_dir, cp);
      break;
    case 1536:
      encoder_main<1536>(temp_dir, cp);
      break;
    default:
      throw std::runtime_error("Wrong bitset size.");
  }
}

std::string random_string(size_t length) {
  auto randchar = []() -> char {
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

}  // namespace spring
