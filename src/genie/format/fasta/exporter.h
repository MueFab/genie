#ifndef GENIE_EXPRTER_H
#define GENIE_EXPRTER_H

#include <genie/core/format-exporter.h>
#include <genie/util/ordered-lock.h>
#include <genie/util/ordered-section.h>
#include <genie/util/original-source.h>
#include <genie/util/thread-manager.h>

namespace genie {
namespace format {
namespace fasta {

class FastaSource : public util::OriginalSource, public util::Source<std::string> {
   private:
    std::ostream* outfile;
    util::OrderedLock outlock;
    core::ReferenceManager* refMgr;
    std::map<std::string, size_t> accu_lengths;
    size_t line_length;
   public:
    FastaSource(std::ostream* _outfile, core::ReferenceManager* _refMgr) : outfile(_outfile), refMgr(_refMgr), line_length(0) {
        auto seqs = refMgr->getSequences();
        size_t pos = 0;
        for(const auto& s : seqs) {
            pos += (refMgr->getLength(s) - 1) / refMgr->getChunkSize() + 1;
            accu_lengths[s] = pos;
        }
    }
    bool pump(size_t& id, std::mutex& lock) override {
        util::Section loc_id = {0, 1, false};
        {
            std::lock_guard<std::mutex> guard(lock);
            loc_id.start = id;
            id++;
        }

        if(loc_id.start >= accu_lengths.rbegin()->second) {
            return false;
        }
        std::string seq;
        size_t pos_old = 0;
        size_t pos = 0;
        for(const auto& s : accu_lengths) {
            if(loc_id.start < s.second) {
                pos = loc_id.start - pos_old;
                seq = s.first;
                break;
            }
            pos_old = s.second;
        }

        auto string = refMgr->loadAt(seq, pos * refMgr->getChunkSize());
        size_t actual_length = loc_id.start ==(accu_lengths[seq] - 1) ? refMgr->getLength(seq) % refMgr->getChunkSize() : string->length();

        std::cout << "Decompressing " << seq << " [" << pos * refMgr->getChunkSize() << ", " << pos * refMgr->getChunkSize() + actual_length << "]" << std::endl;

        util::OrderedSection outSec(&outlock, loc_id);
        if(pos == 0) {
            if(line_length > 0) {
                outfile->write("\n", 1);
                line_length = 0;
            }
            outfile->write(">", 1);
            outfile->write(seq.c_str(), seq.length());
            outfile->write("\n", 1);
        }
        size_t s_pos = 0;
        while(true) {
            size_t length = std::min(50 - line_length, actual_length - s_pos);
            outfile->write(&(*string)[s_pos], length);
            s_pos += length;
            line_length += length;
            if (line_length == 50) {
                outfile->write("\n", 1);
                line_length = 0;
            }
            if(s_pos == actual_length) {
                return true;
            }
        }
    }

    void flushIn(size_t&) override {
    }
};

class Exporter : public core::FormatExporter {
   private:
    core::ReferenceManager* refMgr;
    std::ostream* outfile;
    size_t num_threads;
   public:
    Exporter(core::ReferenceManager* _refMgr, std::ostream* out, size_t _num_threads) : refMgr(_refMgr), outfile(out), num_threads(_num_threads) {

    }

    void flushIn(size_t& ) override {
        util::ThreadManager mgr(num_threads);
        FastaSource source(outfile, refMgr);
        std::vector<util::OriginalSource*> vec;
        vec.push_back(&source);
        mgr.setSource(vec);
        mgr.run();
    }

    void flowIn(core::record::Chunk&& , const util::Section& ) override {
    }
};

}
}
}

#endif  // GENIE_EXPRTER_H
