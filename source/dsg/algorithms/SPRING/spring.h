#include <string>
#include "input/fastq/FastqFileReader.h"
#include "generation.h"

void call_reorder(std::string &working_dir, int max_readlen, int num_thr);
void call_encoder(std::string &working_dir, int max_readlen, int num_thr);

void generate_streams_SPRING(dsg::input::fastq::FastqFileReader &fastqFileReader1, dsg::input::fastq::FastqFileReader &fastqFileReader2, int num_thr, bool paired_end); 
