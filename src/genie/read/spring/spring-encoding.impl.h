/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_SPRING_ENCODING_IMPL_H_
#define SRC_GENIE_READ_SPRING_SPRING_ENCODING_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <list>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::spring {

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
encoder_global_b<bitset_size>::encoder_global_b(int max_readlen_param) {
    max_readlen = max_readlen_param;
    basemask = new std::bitset<bitset_size> *[max_readlen_param];
    for (int i = 0; i < max_readlen_param; i++) basemask[i] = new std::bitset<bitset_size>[128];
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
encoder_global_b<bitset_size>::~encoder_global_b() {
    for (int i = 0; i < max_readlen; i++) delete[] basemask[i];
    delete[] basemask;
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
std::string bitsettostring(std::bitset<bitset_size> b, const uint16_t readlen,
                           const encoder_global_b<bitset_size> &egb) {
    // destroys bitset b
    static const char revinttochar[8] = {'A', 'N', 'G', 0, 'C', 0, 'T', 0};
    std::string s;
    s.resize(readlen);
    uint64_t ull;
    for (int i = 0; i < 3 * readlen / 63 + 1; i++) {
        ull = (b & egb.mask63).to_ullong();
        b >>= 63;
        for (int j = 21 * i; j < 21 * i + 21 && j < readlen; j++) {
            s[j] = revinttochar[ull % 8];
            ull /= 8;
        }
    }
    return s;
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void encode(std::bitset<bitset_size> *read, bbhashdict *dict, uint32_t *order_s, uint16_t *read_lengths_s,
            const encoder_global &eg, const encoder_global_b<bitset_size> &egb) {
    static const int thresh_s = THRESH_ENCODER;
    static const int maxsearch = MAX_SEARCH_ENCODER;
#ifdef GENIE_USE_OPENMP
    auto *read_lock = new omp_lock[eg.numreads_s + eg.numreads_N];
    auto *dict_lock = new omp_lock[eg.numreads_s + eg.numreads_N];
#endif
    bool *remainingreads = new bool[eg.numreads_s + eg.numreads_N];
    std::fill(remainingreads, remainingreads + eg.numreads_s + eg.numreads_N, 1);

    auto *mask1 = new std::bitset<bitset_size>[eg.numdict_s];
    generateindexmasks<bitset_size>(mask1, dict, eg.numdict_s, 3);
    auto **mask = new std::bitset<bitset_size> *[eg.max_readlen];
    for (int i = 0; i < eg.max_readlen; i++) mask[i] = new std::bitset<bitset_size>[eg.max_readlen];
    generatemasks<bitset_size>(mask, eg.max_readlen, 3);

    //
    // This is the 3rd hottest parallel region in genie (behind gabac
    // paralelization and 3rd parallel region in reorder.h).  It shows
    // good load balancing and benefits from parallelization.
    //
    std::cerr << "Encoding reads\n";
#ifdef GENIE_USE_OPENMP
#pragma omp parallel num_threads(eg.num_thr)
#endif
    {
#ifdef GENIE_USE_OPENMP
        int tid = omp_get_thread_num();
#else
        int tid = 0;
#endif
        std::ifstream f(eg.infile + '.' + std::to_string(tid), std::ios::binary);
        std::ifstream in_flag(eg.infile_flag + '.' + std::to_string(tid));
        std::ifstream in_pos(eg.infile_pos + '.' + std::to_string(tid), std::ios::binary);
        std::ifstream in_order(eg.infile_order + '.' + std::to_string(tid), std::ios::binary);
        std::ifstream in_RC(eg.infile_RC + '.' + std::to_string(tid));
        std::ifstream in_readlength(eg.infile_readlength + '.' + std::to_string(tid), std::ios::binary);
        std::ofstream f_seq(eg.outfile_seq + '.' + std::to_string(tid));
        std::ofstream f_pos(eg.outfile_pos + '.' + std::to_string(tid), std::ios::binary);
        std::ofstream f_noise(eg.outfile_noise + '.' + std::to_string(tid));
        std::ofstream f_noisepos(eg.outfile_noisepos + '.' + std::to_string(tid), std::ios::binary);
        std::ofstream f_order(eg.infile_order + '.' + std::to_string(tid) + ".tmp", std::ios::binary);
        std::ofstream f_RC(eg.infile_RC + '.' + std::to_string(tid) + ".tmp");
        std::ofstream f_readlength(eg.infile_readlength + '.' + std::to_string(tid) + ".tmp", std::ios::binary);
        int64_t dictidx[2];  // to store the start and end index (end not inclusive)
        // in the dict read_id array
        uint64_t startposidx;  // index in startpos
        uint64_t ull;
        uint64_t abs_pos = 0;  // absolute position in reference (total length
        // of all contigs till now)
        bool flag = false;
        // flag to check if match was found or not
        std::string current, ref;
        std::bitset<bitset_size> forward_bitset, reverse_bitset, b;
        char c = '0', rc = 'd';
        std::list<contig_reads> current_contig;
        int64_t p = 0;
        uint16_t rl = 0;
        uint32_t ord = 0, list_size = 0;  // list_size variable introduced because
        // list::size() was running very slowly
        // on UIUC machine
        auto *deleted_rids = new std::list<uint32_t>[eg.numdict_s];
        bool done = false;
        while (!done) {
            if (!(in_flag >> c)) done = true;
            if (!done) {
                read_dna_from_bits(current, f);
                rc = static_cast<char>(in_RC.get());
                in_pos.read(reinterpret_cast<char *>(&p), sizeof(int64_t));
                in_order.read(reinterpret_cast<char *>(&ord), sizeof(uint32_t));
                in_readlength.read(reinterpret_cast<char *>(&rl), sizeof(uint16_t));
            }
            if (c == '0' || done || list_size > 10000000) {  // limit on list size so
                                                             // that memory doesn't get
                                                             // too large
                if (list_size != 0) {
                    // sort contig according to pos
                    current_contig.sort([](const contig_reads &ar, const contig_reads &br) { return ar.pos < br.pos; });
                    // make first pos zero and shift all pos values accordingly
                    auto current_contig_it = current_contig.begin();
                    int64_t first_pos = (*current_contig_it).pos;
                    for (; current_contig_it != current_contig.end(); ++current_contig_it)
                        (*current_contig_it).pos -= first_pos;

                    ref = buildcontig(current_contig, list_size);
                    if (static_cast<int64_t>(ref.size()) >= eg.max_readlen && (eg.numreads_s + eg.numreads_N > 0)) {
                        // try to align the singleton reads to ref
                        // first create bitsets from first readlen positions of ref
                        forward_bitset.reset();
                        reverse_bitset.reset();
                        stringtobitset(ref.substr(0, eg.max_readlen), static_cast<uint16_t>(eg.max_readlen),
                                       forward_bitset, egb.basemask);
                        stringtobitset(reverse_complement(ref.substr(0, eg.max_readlen), eg.max_readlen),
                                       static_cast<uint16_t>(eg.max_readlen), reverse_bitset, egb.basemask);
                        for (int64_t j = 0; j < static_cast<int64_t>(ref.size()) - eg.max_readlen + 1; j++) {
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
#ifdef GENIE_USE_OPENMP
                                    if (!dict_lock[startposidx].test()) continue;
#else
                                        // if we are single-threaded we do not need to continue
                                        // because nobody else could possibly try to modify the same
                                        // dictpos
#endif
                                    dict[l].findpos(dictidx, startposidx);
                                    if (dict[l].empty_bin[startposidx]) {  // bin is empty
#ifdef GENIE_USE_OPENMP
                                        dict_lock[startposidx].unset();
#endif
                                        continue;
                                    }
                                    uint64_t ull1 =
                                        ((read[dict[l].read_id[dictidx[0]]] & mask1[l]) >> 3 * dict[l].start)
                                            .to_ullong();
                                    if (ull == ull1) {  // checking if ull is actually the key for this bin
                                        for (int64_t i = dictidx[1] - 1; i >= dictidx[0] && i >= dictidx[1] - maxsearch;
                                             i--) {
                                            auto rid = dict[l].read_id[i];
                                            int hamming;
                                            if (!rev)
                                                hamming =
                                                    static_cast<int>(((forward_bitset ^ read[rid]) &
                                                                      mask[0][eg.max_readlen - read_lengths_s[rid]])
                                                                         .count());
                                            else
                                                hamming =
                                                    static_cast<int>(((reverse_bitset ^ read[rid]) &
                                                                      mask[0][eg.max_readlen - read_lengths_s[rid]])
                                                                         .count());
                                            if (hamming <= thresh_s) {
#ifdef GENIE_USE_OPENMP
                                                read_lock[rid].set();
#endif
                                                if (remainingreads[rid]) {
                                                    remainingreads[rid] = false;
                                                    flag = true;
                                                }
#ifdef GENIE_USE_OPENMP
                                                read_lock[rid].unset();
#endif
                                            }
                                            if (flag == 1) {  // match found
                                                flag = false;
                                                list_size++;
                                                char l_rc = rev ? 'r' : 'd';
                                                int64_t pos = rev ? (j + eg.max_readlen - read_lengths_s[rid]) : j;
                                                std::string read_string =
                                                    rev ? reverse_complement(bitsettostring<bitset_size>(
                                                                                 read[rid], read_lengths_s[rid], egb),
                                                                             read_lengths_s[rid])
                                                        : bitsettostring<bitset_size>(read[rid], read_lengths_s[rid],
                                                                                      egb);
                                                current_contig.push_back(
                                                    {read_string, pos, l_rc, order_s[rid], read_lengths_s[rid]});
                                                for (int l1 = 0; l1 < eg.numdict_s; l1++) {
                                                    if (read_lengths_s[rid] > dict[l1].end)
                                                        deleted_rids[l1].push_back(rid);
                                                }
                                            }
                                        }
                                    }
#ifdef GENIE_USE_OPENMP
                                    dict_lock[startposidx].unset();
#endif
                                    // delete from dictionaries
                                    for (int l1 = 0; l1 < eg.numdict_s; l1++)
                                        for (auto it = deleted_rids[l1].begin(); it != deleted_rids[l1].end();) {
                                            b = read[*it] & mask1[l1];
                                            ull = (b >> 3 * dict[l1].start).to_ullong();
                                            startposidx = dict[l1].bphf->lookup(ull);
#ifdef GENIE_USE_OPENMP
                                            if (!dict_lock[startposidx].test()) {
                                                ++it;
                                                continue;
                                            }
#else
                                            // nothing to be done
#endif
                                            dict[l1].findpos(dictidx, startposidx);
                                            dict[l1].remove(dictidx, startposidx, *it);
                                            it = deleted_rids[l1].erase(it);
#ifdef GENIE_USE_OPENMP
                                            dict_lock[startposidx].unset();
#endif
                                        }
                                }
                            }
                            if (j != static_cast<int64_t>(ref.size()) -
                                         eg.max_readlen) {  // not at last position,shift bitsets
                                forward_bitset >>= 3;
                                forward_bitset = forward_bitset & mask[0][0];
                                forward_bitset |=
                                    egb.basemask[eg.max_readlen - 1][static_cast<uint8_t>(ref[j + eg.max_readlen])];
                                reverse_bitset <<= 3;
                                reverse_bitset = reverse_bitset & mask[0][0];
                                reverse_bitset |=
                                    egb.basemask[0]
                                                [static_cast<uint8_t>(chartorevchar[(uint8_t)ref[j + eg.max_readlen]])];
                            }
                        }  // end for
                    }      // end if
                    // sort contig according to pos
                    current_contig.sort([](const contig_reads &ar, const contig_reads &br) { return ar.pos < br.pos; });
                    writecontig(ref, current_contig, f_seq, f_pos, f_noise, f_noisepos, f_order, f_RC, f_readlength,
                                abs_pos);
                }
                if (!done) {
                    current_contig = {{current, p, rc, ord, rl}};
                    list_size = 1;
                }
            } else if (c == '1') {  // read found during rightward search
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
        delete[] deleted_rids;
    }  // end omp parallel

    auto *file_len_seq_thr = new uint64_t[eg.num_thr];
    for (int tid = 0; tid < eg.num_thr; tid++) {
        std::ifstream in_seq(eg.outfile_seq + '.' + std::to_string(tid));
        in_seq.seekg(0, std::ifstream::end);
        file_len_seq_thr[tid] = in_seq.tellg();
        in_seq.close();
    }
    // Combine files produced by the threads
    std::ofstream f_order(eg.infile_order, std::ios::binary);
    std::ofstream f_readlength(eg.infile_readlength, std::ios::binary);
    std::ofstream f_noisepos(eg.outfile_noisepos, std::ios::binary);
    std::ofstream f_noise(eg.outfile_noise);
    std::ofstream f_RC(eg.infile_RC);
    std::ofstream f_seq(eg.outfile_seq);
    for (int tid = 0; tid < eg.num_thr; tid++) {
        std::ifstream in_seq(eg.outfile_seq + '.' + std::to_string(tid));
        std::ifstream in_order(eg.infile_order + '.' + std::to_string(tid) + ".tmp", std::ios::binary);
        std::ifstream in_readlength(eg.infile_readlength + '.' + std::to_string(tid) + ".tmp", std::ios::binary);
        std::ifstream in_RC(eg.infile_RC + '.' + std::to_string(tid) + ".tmp");
        std::ifstream in_noisepos(eg.outfile_noisepos + '.' + std::to_string(tid), std::ios::binary);
        std::ifstream in_noise(eg.outfile_noise + '.' + std::to_string(tid));
        f_seq << in_seq.rdbuf();
        f_seq.clear();
        f_order << in_order.rdbuf();
        f_order.clear();  // clearStreamState error flag in case in_order is empty
        f_noisepos << in_noisepos.rdbuf();
        f_noisepos.clear();  // clearStreamState error flag in case in_noisepos is empty
        f_noise << in_noise.rdbuf();
        f_noise.clear();  // clearStreamState error flag in case in_noise is empty
        f_readlength << in_readlength.rdbuf();
        f_readlength.clear();  // clearStreamState error flag in case in_readlength is empty
        f_RC << in_RC.rdbuf();
        f_RC.clear();  // clearStreamState error flag in case in_RC is empty

        remove((eg.outfile_seq + '.' + std::to_string(tid)).c_str());
        remove((eg.infile_order + '.' + std::to_string(tid)).c_str());
        remove((eg.infile_order + '.' + std::to_string(tid) + ".tmp").c_str());
        remove((eg.infile_readlength + '.' + std::to_string(tid)).c_str());
        remove((eg.infile_readlength + '.' + std::to_string(tid) + ".tmp").c_str());
        remove((eg.outfile_noisepos + '.' + std::to_string(tid)).c_str());
        remove((eg.outfile_noise + '.' + std::to_string(tid)).c_str());
        remove((eg.infile_RC + '.' + std::to_string(tid) + ".tmp").c_str());
        remove((eg.infile_RC + '.' + std::to_string(tid)).c_str());
        remove((eg.infile_flag + '.' + std::to_string(tid)).c_str());
        remove((eg.infile_pos + '.' + std::to_string(tid)).c_str());
        remove((eg.infile + '.' + std::to_string(tid)).c_str());
    }
    f_order.close();
    f_readlength.close();
    // write remaining singleton reads now
    std::ofstream f_unaligned(eg.outfile_unaligned, std::ios::binary);
    f_order.open(eg.infile_order, std::ios::binary | std::ofstream::app);
    f_readlength.open(eg.infile_readlength, std::ios::binary | std::ofstream::app);
    uint32_t matched_s = eg.numreads_s;
    uint64_t len_unaligned = 0;

    for (uint32_t i = 0; i < eg.numreads_s; i++)
        if (remainingreads[i] == 1) {
            matched_s--;
            f_order.write(reinterpret_cast<char *>(&order_s[i]), sizeof(uint32_t));
            f_readlength.write(reinterpret_cast<char *>(&read_lengths_s[i]), sizeof(uint16_t));
            std::string unaligned_read = bitsettostring<bitset_size>(read[i], read_lengths_s[i], egb);
            write_dnaN_in_bits(unaligned_read, f_unaligned);
            len_unaligned += read_lengths_s[i];
        }
    uint32_t matched_N = eg.numreads_N;
    for (uint32_t i = eg.numreads_s; i < eg.numreads_s + eg.numreads_N; i++)
        if (remainingreads[i] == 1) {
            matched_N--;
            std::string unaligned_read = bitsettostring<bitset_size>(read[i], read_lengths_s[i], egb);
            write_dnaN_in_bits(unaligned_read, f_unaligned);
            f_order.write(reinterpret_cast<char *>(&order_s[i]), sizeof(uint32_t));
            f_readlength.write(reinterpret_cast<char *>(&read_lengths_s[i]), sizeof(uint16_t));
            len_unaligned += read_lengths_s[i];
        }
    f_order.close();
    f_readlength.close();
    f_unaligned.close();
    delete[] remainingreads;
#ifdef GENIE_USE_OPENMP
    delete[] dict_lock;
    delete[] read_lock;
#endif
    for (int i = 0; i < eg.max_readlen; i++) delete[] mask[i];
    delete[] mask;
    delete[] mask1;

    // write length of unaligned array
    std::ofstream f_unaligned_count(eg.outfile_unaligned + ".count", std::ios::binary);
    f_unaligned_count.write(reinterpret_cast<char *>(&len_unaligned), sizeof(uint64_t));
    f_unaligned_count.close();

    // convert read_pos into 8 byte non-diff (absolute)
    // positions
    uint64_t abs_pos = 0;
    uint64_t abs_pos_thr;
    std::ofstream fout_pos(eg.outfile_pos, std::ios::binary);
    for (int tid = 0; tid < eg.num_thr; tid++) {
        std::ifstream fin_pos(eg.outfile_pos + '.' + std::to_string(tid), std::ios::binary);
        fin_pos.read(reinterpret_cast<char *>(&abs_pos_thr), sizeof(uint64_t));
        while (!fin_pos.eof()) {
            abs_pos_thr += abs_pos;
            fout_pos.write(reinterpret_cast<char *>(&abs_pos_thr), sizeof(uint64_t));
            fin_pos.read(reinterpret_cast<char *>(&abs_pos_thr), sizeof(uint64_t));
        }
        fin_pos.close();
        remove((eg.outfile_pos + '.' + std::to_string(tid)).c_str());
        abs_pos += file_len_seq_thr[tid];
    }
    fout_pos.close();
    delete[] file_len_seq_thr;

    std::cerr << "Encoding done:\n";
    std::cerr << matched_s << " singleton reads were aligned\n";
    std::cerr << matched_N << " reads with N were aligned\n";
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void setglobalarrays(encoder_global &eg, encoder_global_b<bitset_size> &egb) {
    for (int i = 0; i < 63; i++) egb.mask63[i] = 1;
    for (int i = 0; i < eg.max_readlen; i++) {
        egb.basemask[i][static_cast<uint8_t>('A')][3 * i] = 0;
        egb.basemask[i][static_cast<uint8_t>('A')][3 * i + 1] = 0;
        egb.basemask[i][static_cast<uint8_t>('A')][3 * i + 2] = 0;
        egb.basemask[i][static_cast<uint8_t>('C')][3 * i] = 0;
        egb.basemask[i][static_cast<uint8_t>('C')][3 * i + 1] = 0;
        egb.basemask[i][static_cast<uint8_t>('C')][3 * i + 2] = 1;
        egb.basemask[i][static_cast<uint8_t>('G')][3 * i] = 0;
        egb.basemask[i][static_cast<uint8_t>('G')][3 * i + 1] = 1;
        egb.basemask[i][static_cast<uint8_t>('G')][3 * i + 2] = 0;
        egb.basemask[i][static_cast<uint8_t>('T')][3 * i] = 0;
        egb.basemask[i][static_cast<uint8_t>('T')][3 * i + 1] = 1;
        egb.basemask[i][static_cast<uint8_t>('T')][3 * i + 2] = 1;
        egb.basemask[i][static_cast<uint8_t>('N')][3 * i] = 1;
        egb.basemask[i][static_cast<uint8_t>('N')][3 * i + 1] = 0;
        egb.basemask[i][static_cast<uint8_t>('N')][3 * i + 2] = 0;
    }

    // enc_noise uses substitution statistics from Minoche et al.
    eg.enc_noise[static_cast<uint8_t>('A')][static_cast<uint8_t>('C')] = '0';
    eg.enc_noise[static_cast<uint8_t>('A')][static_cast<uint8_t>('G')] = '1';
    eg.enc_noise[static_cast<uint8_t>('A')][static_cast<uint8_t>('T')] = '2';
    eg.enc_noise[static_cast<uint8_t>('A')][static_cast<uint8_t>('N')] = '3';
    eg.enc_noise[static_cast<uint8_t>('C')][static_cast<uint8_t>('A')] = '0';
    eg.enc_noise[static_cast<uint8_t>('C')][static_cast<uint8_t>('G')] = '1';
    eg.enc_noise[static_cast<uint8_t>('C')][static_cast<uint8_t>('T')] = '2';
    eg.enc_noise[static_cast<uint8_t>('C')][static_cast<uint8_t>('N')] = '3';
    eg.enc_noise[static_cast<uint8_t>('G')][static_cast<uint8_t>('T')] = '0';
    eg.enc_noise[static_cast<uint8_t>('G')][static_cast<uint8_t>('A')] = '1';
    eg.enc_noise[static_cast<uint8_t>('G')][static_cast<uint8_t>('C')] = '2';
    eg.enc_noise[static_cast<uint8_t>('G')][static_cast<uint8_t>('N')] = '3';
    eg.enc_noise[static_cast<uint8_t>('T')][static_cast<uint8_t>('G')] = '0';
    eg.enc_noise[static_cast<uint8_t>('T')][static_cast<uint8_t>('C')] = '1';
    eg.enc_noise[static_cast<uint8_t>('T')][static_cast<uint8_t>('A')] = '2';
    eg.enc_noise[static_cast<uint8_t>('T')][static_cast<uint8_t>('N')] = '3';
    eg.enc_noise[static_cast<uint8_t>('N')][static_cast<uint8_t>('A')] = '0';
    eg.enc_noise[static_cast<uint8_t>('N')][static_cast<uint8_t>('G')] = '1';
    eg.enc_noise[static_cast<uint8_t>('N')][static_cast<uint8_t>('C')] = '2';
    eg.enc_noise[static_cast<uint8_t>('N')][static_cast<uint8_t>('T')] = '3';
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void readsingletons(std::bitset<bitset_size> *read, uint32_t *order_s, uint16_t *read_lengths_s,
                    const encoder_global &eg, const encoder_global_b<bitset_size> &egb) {
    // not parallelized right now since these are very small number of reads
    std::ifstream f(eg.infile + ".singleton", std::ifstream::in | std::ios::binary);
    std::string s;
    for (uint32_t i = 0; i < eg.numreads_s; i++) {
        read_dna_from_bits(s, f);
        read_lengths_s[i] = static_cast<uint16_t>(s.length());
        stringtobitset<bitset_size>(s, read_lengths_s[i], read[i], egb.basemask);
    }
    f.close();
    remove((eg.infile + ".singleton").c_str());
    f.open(eg.infile_N, std::ios::binary);
    for (uint32_t i = eg.numreads_s; i < eg.numreads_s + eg.numreads_N; i++) {
        read_dnaN_from_bits(s, f);
        read_lengths_s[i] = static_cast<uint16_t>(s.length());
        stringtobitset<bitset_size>(s, read_lengths_s[i], read[i], egb.basemask);
    }
    std::ifstream f_order_s(eg.infile_order + ".singleton", std::ios::binary);
    for (uint32_t i = 0; i < eg.numreads_s; i++)
        f_order_s.read(reinterpret_cast<char *>(&order_s[i]), sizeof(uint32_t));
    f_order_s.close();
    remove((eg.infile_order + ".singleton").c_str());
    std::ifstream f_order_N(eg.infile_order_N, std::ios::binary);
    for (uint32_t i = eg.numreads_s; i < eg.numreads_s + eg.numreads_N; i++)
        f_order_N.read(reinterpret_cast<char *>(&order_s[i]), sizeof(uint32_t));
    f_order_N.close();
}

// ---------------------------------------------------------------------------------------------------------------------

template <size_t bitset_size>
void encoder_main(const std::string &temp_dir, const compression_params &cp) {
    auto *egb_ptr = new encoder_global_b<bitset_size>(cp.max_readlen);
    auto *eg_ptr = new encoder_global;
    encoder_global_b<bitset_size> &egb = *egb_ptr;
    encoder_global &eg = *eg_ptr;

    eg.basedir = temp_dir;
    eg.infile = eg.basedir + "/temp.dna";
    eg.infile_pos = eg.basedir + "/temppos.txt";
    eg.infile_flag = eg.basedir + "/tempflag.txt";
    eg.infile_order = eg.basedir + "/read_order.bin";
    eg.infile_order_N = eg.basedir + "/read_order_N.bin";
    eg.infile_RC = eg.basedir + "/read_rev.txt";
    eg.infile_readlength = eg.basedir + "/read_lengths.bin";
    eg.infile_N = eg.basedir + "/input_N.dna";
    eg.outfile_seq = eg.basedir + "/read_seq.txt";
    eg.outfile_pos = eg.basedir + "/read_pos.bin";
    eg.outfile_noise = eg.basedir + "/read_noise.txt";
    eg.outfile_noisepos = eg.basedir + "/read_noisepos.bin";
    eg.outfile_unaligned = eg.basedir + "/read_unaligned.txt";

    eg.max_readlen = cp.max_readlen;
    eg.num_thr = cp.num_thr;

    getDataParams(eg, cp);  // populate numreads
    setglobalarrays<bitset_size>(eg, egb);
    auto *read = new std::bitset<bitset_size>[eg.numreads_s + eg.numreads_N];
    auto *order_s = new uint32_t[eg.numreads_s + eg.numreads_N];
    auto *read_lengths_s = new uint16_t[eg.numreads_s + eg.numreads_N];
    readsingletons<bitset_size>(read, order_s, read_lengths_s, eg, egb);
    remove(eg.infile_N.c_str());
    correct_order(order_s, eg);

    auto *dict = new bbhashdict[eg.numdict_s];
    if (eg.max_readlen > 50) {
        dict[0].start = 0;
        dict[0].end = 20;
        dict[1].start = 21;
        dict[1].end = 41;
    } else {
        dict[0].start = 0;
        dict[0].end = 20 * eg.max_readlen / 50;
        dict[1].start = 20 * eg.max_readlen / 50 + 1;
        dict[1].end = 41 * eg.max_readlen / 50;
    }

    if (eg.numreads_s + eg.numreads_N > 0)
        constructdictionary<bitset_size>(read, dict, read_lengths_s, eg.numdict_s, eg.numreads_s + eg.numreads_N, 3,
                                         eg.basedir, eg.num_thr);

    encode<bitset_size>(read, dict, order_s, read_lengths_s, eg, egb);
    delete[] read;
    delete[] dict;
    delete[] order_s;
    delete[] read_lengths_s;
    delete eg_ptr;
    delete egb_ptr;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::spring

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_SPRING_ENCODING_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
