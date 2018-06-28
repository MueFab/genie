#include <bitset>
#include <string>
#include "algorithms/SPRING/BooPHF.h"

int MAX_READ_LEN = 255;
typedef boomphf::SingleHashFunctor<u_int64_t> hasher_t;
typedef boomphf::mphf<u_int64_t, hasher_t> boophf_t;

class bbhashdict {
 public:
  boophf_t *bphf;
  uint32_t start;
  uint32_t end;	
  uint32_t numkeys;
  uint32_t dict_numreads;  // number of reads in this dict (for variable length)
  uint32_t *startpos;
  uint32_t *read_id;
  bool *empty_bin;
  void findpos(int64_t *dictidx, uint32_t &startposidx);
  void remove(int64_t *dictidx, uint32_t &startposidx, uint32_t current);
  bbhashdict() {
    bphf = NULL;
    startpos = NULL;
    read_id = NULL;
  }
  ~bbhashdict() {
    delete[] startpos;
    delete[] read_id;
    delete[] empty_bin;
    delete bphf;
  }
};

template<size_t bitset_size>
void stringtobitset(std::string s, uint8_t readlen, std::bitset<bitset_size> &b, std::bitset<bitset_size> basemask[256][128]);

template<size_t bitset_size>
void constructdictionary(std::bitset<bitset_size> *read, bbhashdict *dict, uint8_t *read_lengths, int numdict, uint32_t numreads, int bpb);

template<size_t MAX_READ_LEN, size_t bitset_size>
void generatemasks(std::bitset<bitset_size> mask[MAX_READ_LEN][MAX_READ_LEN], uint8_t max_readlen, int bpb);

void reverse_complement(char *s, char *s1, uint8_t readlen, char chartorevchar[128]);

template<size_t bitset_size>
void chartobitset(char *s, uint8_t readlen, std::bitset<bitset_size> &b, std::bitset<bitset_size> basemask[256][128]);

std::string reverse_complement(std::string s, uint8_t readlen, char chartorevchar[128]);

template<size_t bitset_size>
void generateindexmasks(std::bitset<bitset_size> *mask1, bbhashdict *dict, int numdict, int bpb);
