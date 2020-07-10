/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_REFERENCE_MANAGER_H
#define GENIE_REFERENCE_MANAGER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/make-unique.h>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
#include "reference-source.h"
#include "reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class ReferenceCollection {
   private:
    std::map<std::string, std::vector<std::unique_ptr<Reference>>> refs;  //!<

   public:
    /**
     *
     * @param name
     * @param _start
     * @param _end
     * @return
     */
    std::string getSequence(const std::string& name, uint64_t _start, uint64_t _end) const;

    std::vector<std::pair<size_t, size_t>> getCoverage(const std::string& name) const {
        std::vector<std::pair<size_t, size_t>> ret;
        auto it = refs.find(name);
        if(it == refs.end()) {
            return ret;
        }
        for (const auto& s : it->second) {
            ret.emplace_back(s->getStart(), s->getEnd());
        }
        return ret;
    }

    std::vector<std::string> getSequences() const {
        std::vector<std::string> ret;
        for (const auto& s : refs) {
            ret.emplace_back(s.first);
        }
        return ret;
    }

    /**
     *
     * @param ref
     */
    void registerRef(std::unique_ptr<Reference> ref);

    /**
     *
     * @param ref
     */
    void registerRef(std::vector<std::unique_ptr<Reference>>&& ref);
};

class ReferenceManager {
   private:
    ReferenceCollection mgr;

    struct CacheLine {
        std::shared_ptr<const std::string> chunk;  //!<
        std::weak_ptr<const std::string> memory;   //!<
        std::mutex loadMutex;
    };

    std::map<std::string, std::vector<std::unique_ptr<CacheLine>>> data;
    std::deque<std::pair<std::string, size_t>> cacheInfo;
    std::mutex cacheInfoLock;
    uint64_t cacheSize;  //!<
    static const uint64_t CHUNK_SIZE;

    void touch(const std::string& name, size_t num) {
        if (cacheInfo.size() < cacheSize) {
            cacheInfo.push_front(std::make_pair(name, num));
            return;
        }

        for (auto it = cacheInfo.begin(); it != cacheInfo.end(); ++it) {
            if (it->first == name && it->second == num) {
                cacheInfo.erase(it);
                cacheInfo.push_front(std::make_pair(name, num));
                return;
            }
        }

        cacheInfo.push_front(std::make_pair(name, num));
        auto& line = data[cacheInfo.back().first][cacheInfo.back().second];
        cacheInfo.pop_back();

        line->chunk.reset();
        line->memory.reset();
    }

   public:

    size_t ref2ID(const std::string& ref) const {
        size_t ctr = 0;
        for(const auto& r : data) {
            if(r.first == ref) {
                return ctr;
            }
            ctr++;
        }
        UTILS_DIE("Unknown reference");
    }

    std::string ID2Ref(size_t id) const {
        size_t ctr = 0;
        for(const auto& r : data) {
            if(id == ctr) {
                return r.first;
            }
            ctr++;
        }
        UTILS_DIE("Unknown reference");
    }

    void addReferenceSource(ReferenceSource& source) { mgr.registerRef(source.generateRefHandles()); }

    static size_t getChunkSize() { return CHUNK_SIZE; }

    struct ReferenceExcerpt {
       private:
        std::string ref_name;
        size_t global_start;
        size_t global_end;
        std::vector<std::shared_ptr<const std::string>> data;

       public:
        bool isEmpty() const {
            for (const auto& p : data) {
                if (isMapped(p)) {
                    return false;
                }
            }
            return true;
        }

        void merge(ReferenceExcerpt& e) {
            if (this->ref_name.empty()) {
                *this = std::move(e);
                return;
            }
            extend(e.global_end);
            for (size_t i = e.global_start; i < e.global_end; i += CHUNK_SIZE) {
                if (e.isMapped(i)) {
                    mapChunkAt(i, e.getChunkAt(i));
                }
            }
        }

        size_t getDataStart() const {
            return global_start - global_start % CHUNK_SIZE;
        }

        size_t getDataEnd() const {
            return global_end - global_end % CHUNK_SIZE + CHUNK_SIZE;
        }

        size_t getGlobalStart() const { return global_start; }

        size_t getGlobalEnd() const { return global_end; }

        const std::string& getRefName() const { return ref_name; }

        std::shared_ptr<const std::string> getChunkAt(size_t pos) const {
            int id = (pos - (global_start - global_start % CHUNK_SIZE)) / CHUNK_SIZE;
            UTILS_DIE_IF(id < 0 || id >= (int)data.size(), "Invalid index");
            return data[id];
        }

        void mapChunkAt(size_t pos, std::shared_ptr<const std::string> dat) {
            int id = (pos - (global_start - global_start % CHUNK_SIZE)) / CHUNK_SIZE;
            UTILS_DIE_IF(id < 0 || id >= (int)data.size(), "Invalid index");
            data[id] = std::move(dat);
        }

        void mapSection(size_t start, size_t end, ReferenceManager* mgr) {
            for (size_t i = start; i < end; i += CHUNK_SIZE) {
                if (isMapped(i)) {
                    continue;
                }
                mapChunkAt(i, mgr->loadAt(ref_name, i));
            }
        }

        ReferenceExcerpt() : ReferenceExcerpt("", 0, 1) {}

        ReferenceExcerpt(std::string name, size_t start, size_t end)
            : ref_name(std::move(name)),
              global_start(start),
              global_end(end),
              data(((global_end - global_end % CHUNK_SIZE + CHUNK_SIZE) - (global_start - global_start % CHUNK_SIZE) -
                    1) / CHUNK_SIZE +
                       1,
                   undef_page()) {}

        static const std::shared_ptr<const std::string>& undef_page() {
            static std::shared_ptr<const std::string> ret = std::make_shared<const std::string>(CHUNK_SIZE, 'N');
            return ret;
        }

        void extend(size_t newEnd) {
            if (newEnd < global_end) {
                return;
            }
            size_t id = (newEnd - (global_start - global_start % CHUNK_SIZE)) / CHUNK_SIZE;
            for (size_t i = data.size() - 1; i < id; ++i) {
                data.push_back(undef_page());
            }
            global_end = newEnd;
        }

        static bool isMapped(const std::shared_ptr<const std::string>& page) {
            return page.get() != undef_page().get();
        }

        bool isMapped(size_t pos) const {
            return isMapped(data[(pos - (global_start - global_start % CHUNK_SIZE)) / CHUNK_SIZE]);
        }

        void unMapAt(size_t pos) {
            data[(pos - (global_start - global_start % CHUNK_SIZE)) / CHUNK_SIZE] = undef_page();
        }

        struct Stepper {
           private:
            std::vector<std::shared_ptr<const std::string>>::const_iterator startVecIt;
            std::vector<std::shared_ptr<const std::string>>::const_iterator vecIt;
            std::vector<std::shared_ptr<const std::string>>::const_iterator endVecIt;
            size_t stringPos;
            const char* curString;

           public:
            explicit Stepper(const ReferenceExcerpt& e) {
                startVecIt = e.data.begin();
                vecIt = e.data.begin();
                endVecIt = e.data.end();
                stringPos = e.global_start % CHUNK_SIZE;
                curString = (**vecIt).data();
            }
            void inc(size_t off = 1) {
                stringPos += off;
                while (stringPos >= CHUNK_SIZE) {
                    stringPos -= CHUNK_SIZE;
                    vecIt++;
                }
                curString = vecIt->get()->data();
            }

            void setPos(size_t pos) {
                stringPos = pos % CHUNK_SIZE;
                vecIt = startVecIt + pos / CHUNK_SIZE;
                curString = vecIt->get()->data();
            }

            bool end() const { return vecIt >= endVecIt; }

            char get() const { return curString[stringPos]; }
        };

        Stepper getStepper() const { return Stepper(*this); }

        ReferenceExcerpt getSubExcerpt(size_t start, size_t end) const {
            UTILS_DIE_IF(start < global_start || end > global_end, "SubExcerpt can't be bigger than parent");
            ReferenceExcerpt ret(ref_name, start, end);
            for (size_t i = start; i < end; i += CHUNK_SIZE) {
                ret.mapChunkAt(i, getChunkAt(i));
            }
            return ret;
        }

        std::string getString(size_t start, size_t end) const {
            UTILS_DIE_IF(start < global_start || end > global_end, "String can't be bigger than reference excerpt");
            auto stepper = getStepper();
            std::string ret;
            stepper.inc(start - global_start);
            for (size_t i = start; i < end; ++i) {
                ret += stepper.get();
                stepper.inc();
            }
            return ret;
        }
    };

    explicit ReferenceManager(size_t csize) : cacheSize(csize) {}

    void buildCache() {
        std::map<std::string, size_t> lengths;
        data.clear();

        for (const auto& s : mgr.getSequences()) {
            size_t max = 0;
            auto cov = mgr.getCoverage(s);
            for (const auto& p : cov) {
                max = std::max(max, p.second);
            }
            lengths.insert(std::make_pair(s, max));
        }

        for (const auto& s : lengths) {
            for (size_t i = 0; i < (s.second - 1) / CHUNK_SIZE + 1; i++) {
                data[s.first].push_back(genie::util::make_unique<CacheLine>());
            }
        }
    }

    std::shared_ptr<const std::string> loadAt(const std::string& name, size_t pos) {
        size_t id = pos / CHUNK_SIZE;
        auto it = data.find(name);

        // Invalid chunk
        if (it == data.end()) {
            return ReferenceExcerpt::undef_page();
        }

        std::lock_guard<std::mutex> lock1(it->second[id]->loadMutex);
        std::unique_lock<std::mutex> lock2(cacheInfoLock);

        // Chunk already loaded
        auto ret = it->second[id]->chunk;
        if (ret) {
            touch(name, id);
            return ret;
        }

        // Try quick load. Maybe another thread unloaded this reference but it is still in memory, reachable via weak
        // ptr.
        ret = it->second[id]->memory.lock();
        if (ret) {
            it->second[id]->chunk = ret;
            it->second[id]->memory = ret;
            touch(name, id);
            return ret;
        }

        // Reference is not in memory. We have to do a slow read from disc...
        // Loading mutex keeps locked for this chunk, but other chunks can be accessed, main lock is opened.
        lock2.unlock();
        ret = std::make_shared<const std::string>(mgr.getSequence(name, id * CHUNK_SIZE, (id + 1) * CHUNK_SIZE));
        lock2.lock();

        it->second[id]->chunk = ret;
        it->second[id]->memory = ret;
        touch(name, id);
        return ret;
    }

    ReferenceExcerpt load(const std::string& name, size_t start, size_t end) {
        ReferenceExcerpt ret(name, start, end);
        for (size_t i = start; i < end; i += CHUNK_SIZE) {
            ret.mapChunkAt(i, loadAt(name, i));
        }
        return ret;
    }

    std::vector<std::pair<size_t, size_t>> getCoverage(const std::string& name) const { return mgr.getCoverage(name); }

    std::vector<std::string> getSequences() const { return mgr.getSequences(); }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_REFERENCE_MANAGER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------