#include <cmath>  // abs
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <genie/generation.h>

#include "spring/generate_ref_streams_eru.h"
#include "spring/util.h"
#include "spring/params.h"

namespace spring {

generated_aus_ref generate_ref_streams(const std::string &temp_dir, const compression_params &cp) {

  std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> descriptorFilesPerAU;
  std::vector<uint64_t> start_au_ref;
  std::vector<uint64_t> end_au_ref;

  std::string basedir = temp_dir;

  std::string file_subseq_6_0 = basedir + "/ref_subseq_6_0"; // ureads
  std::string file_subseq_7_0 = basedir + "/ref_subseq_7_0"; // rlen

  std::string file_seq = basedir + "/read_seq.txt";

  // char_to_int
  int64_t char_to_int[128];
  char_to_int[(uint8_t)'A'] = 0;
  char_to_int[(uint8_t)'C'] = 1;
  char_to_int[(uint8_t)'G'] = 2;
  char_to_int[(uint8_t)'T'] = 3;
  char_to_int[(uint8_t)'N'] = 4;

  std::vector<int64_t> subseq_6_0;
  std::vector<int64_t> subseq_7_0;
  
  uint64_t len_uread = LEN_UREAD_REF;
  uint64_t num_ureads_in_AU = NUM_UREADS_IN_AU_REF;

  std::string seq = read_file_as_string(file_seq); 

  uint64_t seq_len = seq.size();
  
  uint64_t num_AU = 1 + (seq_len-1)/(len_uread*num_ureads_in_AU);
  
  for (uint64_t i = 0; i < num_AU; i++) {
    std::map<uint8_t, std::map<uint8_t, std::string>> listDescriptorFiles;
    subseq_6_0.clear();
    subseq_7_0.clear();
    uint64_t seq_start = i*num_ureads_in_AU*len_uread;
    uint64_t seq_end = (i+1)*num_ureads_in_AU*len_uread;
    if (seq_end > seq_len)
      seq_end = seq_len;
    uint64_t num_ureads = 1 + (seq_end-seq_start - 1)/(len_uread);
    for (uint64_t j = 0; j < num_ureads; j++) {
      uint64_t uread_start = seq_start + j*len_uread;
      uint64_t uread_end = seq_start + (j+1)*len_uread;
      if (uread_end > seq_end)
        uread_end = seq_end;
      for (uint64_t k = uread_start; k < uread_end; k++)
        subseq_6_0.push_back(char_to_int[(uint8_t)seq[k]]);
      subseq_7_0.push_back(uread_end-uread_start);
    }
    // write vectors to files
    std::string filename6_0 = file_subseq_6_0 + '.' + std::to_string(i);
    std::string filename7_0 = file_subseq_7_0 + '.' + std::to_string(i);

    write_vector_to_file(subseq_6_0, filename6_0);
    listDescriptorFiles[6][0]=filename6_0;
    write_vector_to_file(subseq_7_0, filename7_0);
    listDescriptorFiles[7][0]=filename7_0;

    descriptorFilesPerAU.push_back(listDescriptorFiles);
    start_au_ref.push_back(seq_start);
    end_au_ref.push_back(seq_end);
  }

  return generated_aus_ref(descriptorFilesPerAU, start_au_ref, end_au_ref);
}

}  // namespace spring
