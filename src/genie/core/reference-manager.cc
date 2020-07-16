/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "reference-manager.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

const uint64_t ReferenceManager::CHUNK_SIZE = 1 * 1024 * 1024;  //!

// ---------------------------------------------------------------------------------------------------------------------

std::string ReferenceCollection::getSequence(const std::string& name, uint64_t _start, uint64_t _end) const {
    auto it = refs.find(name);
    if (it == refs.end()) {
        return "";
    }
    uint64_t position = _start;
    std::string ret;
    ret.reserve(_end - _start);
    while (true) {
        size_t nearest = std::numeric_limits<size_t>::max();
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
            if (position >= _end) {
                return ret;
            }

            if (it2->get()->getStart() > position) {
                nearest = std::min(nearest, it2->get()->getStart());
            }

            if (it2->get()->getStart() <= position && it2->get()->getEnd() > position) {
                nearest = std::numeric_limits<size_t>::max();
                ret += it2->get()->getSequence(position, std::min(it2->get()->getEnd(), _end));
                position = it2->get()->getEnd();
                it2 = it->second.begin();
            }
        }

        if (position >= _end) {
            return ret;
        }

        if (nearest == std::numeric_limits<size_t>::max()) {
            ret += std::string(_end - position, 'N');
            return ret;
        }
        ret += std::string(nearest - position, 'N');
        position = nearest;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceCollection::registerRef(std::unique_ptr<Reference> ref) {
    refs[ref->getName()].emplace_back(std::move(ref));
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceCollection::registerRef(std::vector<std::unique_ptr<Reference>>&& ref) {
    auto vec = std::move(ref);
    for (auto& v : vec) {
        registerRef(std::move(v));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::pair<size_t, size_t>> ReferenceCollection::getCoverage(const std::string& name) const {
    std::vector<std::pair<size_t, size_t>> ret;
    auto it = refs.find(name);
    if (it == refs.end()) {
        return ret;
    }
    for (const auto& s : it->second) {
        ret.emplace_back(s->getStart(), s->getEnd());
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string> ReferenceCollection::getSequences() const {
    std::vector<std::string> ret;
    for (const auto& s : refs) {
        ret.emplace_back(s.first);
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::touch(const std::string& name, size_t num) {
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

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::validateRefID(size_t id) {
    std::unique_lock<std::mutex> lock2(cacheInfoLock);
    for (size_t i = 0; i <= id; ++i) {
        auto s = std::to_string(i);
        data[std::string(s.size() < 3 ? (3 - s.size()) : 0, '0') + s];
    }
}

// ---------------------------------------------------------------------------------------------------------------------

size_t ReferenceManager::ref2ID(const std::string& ref) {
    std::unique_lock<std::mutex> lock2(cacheInfoLock);
    size_t ctr = 0;
    for (const auto& r : data) {
        if (r.first == ref) {
            return ctr;
        }
        ctr++;
    }
    UTILS_DIE("Unknown reference");
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ReferenceManager::ID2Ref(size_t id) {
    std::unique_lock<std::mutex> lock2(cacheInfoLock);
    size_t ctr = 0;
    for (const auto& r : data) {
        if (id == ctr) {
            return r.first;
        }
        ctr++;
    }
    UTILS_DIE("Unknown reference");
}

// ---------------------------------------------------------------------------------------------------------------------

size_t ReferenceManager::getChunkSize() { return CHUNK_SIZE; }

// ---------------------------------------------------------------------------------------------------------------------

bool ReferenceManager::ReferenceExcerpt::isEmpty() const {
    for (const auto& p : data) {
        if (isMapped(p)) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::ReferenceExcerpt::merge(ReferenceExcerpt& e) {
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

// ---------------------------------------------------------------------------------------------------------------------

size_t ReferenceManager::ReferenceExcerpt::getDataStart() const { return global_start - (global_start % CHUNK_SIZE); }

// ---------------------------------------------------------------------------------------------------------------------

size_t ReferenceManager::ReferenceExcerpt::getDataEnd() const {
    return global_end - (global_end % CHUNK_SIZE) + CHUNK_SIZE;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t ReferenceManager::ReferenceExcerpt::getGlobalStart() const { return global_start; }

// ---------------------------------------------------------------------------------------------------------------------

size_t ReferenceManager::ReferenceExcerpt::getGlobalEnd() const { return global_end; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ReferenceManager::ReferenceExcerpt::getRefName() const { return ref_name; }

// ---------------------------------------------------------------------------------------------------------------------

std::shared_ptr<const std::string> ReferenceManager::ReferenceExcerpt::getChunkAt(size_t pos) const {
    int id = (pos - (global_start - global_start % CHUNK_SIZE)) / CHUNK_SIZE;
    UTILS_DIE_IF(id < 0 || id >= (int)data.size(), "Invalid index");
    return data[id];
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::ReferenceExcerpt::mapChunkAt(size_t pos, std::shared_ptr<const std::string> dat) {
    int id = (pos - (global_start - global_start % CHUNK_SIZE)) / CHUNK_SIZE;
    UTILS_DIE_IF(id < 0 || id >= (int)data.size(), "Invalid index");
    data[id] = std::move(dat);
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::ReferenceExcerpt::mapSection(size_t start, size_t end, ReferenceManager* mgr) {
    for (size_t i = start; i < end; i += CHUNK_SIZE) {
        if (isMapped(i)) {
            continue;
        }
        mapChunkAt(i, mgr->loadAt(ref_name, i));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceManager::ReferenceExcerpt::ReferenceExcerpt() : ReferenceExcerpt("", 0, 1) {}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceManager::ReferenceExcerpt::ReferenceExcerpt(std::string name, size_t start, size_t end)
    : ref_name(std::move(name)),
      global_start(start),
      global_end(end),
      data((((global_end - 1) - (global_end - 1) % CHUNK_SIZE + CHUNK_SIZE) -
            (global_start - global_start % CHUNK_SIZE) - 1) /
                   CHUNK_SIZE +
               1,
           undef_page()) {}

// ---------------------------------------------------------------------------------------------------------------------

const std::shared_ptr<const std::string>& ReferenceManager::ReferenceExcerpt::undef_page() {
    static std::shared_ptr<const std::string> ret = std::make_shared<const std::string>(CHUNK_SIZE, 'N');
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::ReferenceExcerpt::extend(size_t newEnd) {
    if (newEnd < global_end) {
        return;
    }
    size_t id = (newEnd - (global_start - global_start % CHUNK_SIZE)) / CHUNK_SIZE;
    for (size_t i = data.size() - 1; i < id; ++i) {
        data.push_back(undef_page());
    }
    global_end = newEnd;
}

// ---------------------------------------------------------------------------------------------------------------------

bool ReferenceManager::ReferenceExcerpt::isMapped(const std::shared_ptr<const std::string>& page) {
    return page.get() != undef_page().get();
}

// ---------------------------------------------------------------------------------------------------------------------

bool ReferenceManager::ReferenceExcerpt::isMapped(size_t pos) const {
    return isMapped(data[(pos - (global_start - global_start % CHUNK_SIZE)) / CHUNK_SIZE]);
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::ReferenceExcerpt::unMapAt(size_t pos) {
    data[(pos - (global_start - global_start % CHUNK_SIZE)) / CHUNK_SIZE] = undef_page();
}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceManager::ReferenceExcerpt::Stepper::Stepper(const ReferenceExcerpt& e) {
    startVecIt = e.data.begin();
    vecIt = e.data.begin();
    endVecIt = e.data.end();
    stringPos = e.global_start % CHUNK_SIZE;
    curString = (**vecIt).data();
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::ReferenceExcerpt::Stepper::inc(size_t off) {
    stringPos += off;
    while (stringPos >= CHUNK_SIZE) {
        stringPos -= CHUNK_SIZE;
        vecIt++;
    }
    curString = vecIt->get()->data();
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::ReferenceExcerpt::Stepper::setPos(size_t pos) {
    stringPos = pos % CHUNK_SIZE;
    vecIt = startVecIt + pos / CHUNK_SIZE;
    curString = vecIt->get()->data();
}

// ---------------------------------------------------------------------------------------------------------------------

bool ReferenceManager::ReferenceExcerpt::Stepper::end() const { return vecIt >= endVecIt; }

// ---------------------------------------------------------------------------------------------------------------------

char ReferenceManager::ReferenceExcerpt::Stepper::get() const { return curString[stringPos]; }

// ---------------------------------------------------------------------------------------------------------------------

ReferenceManager::ReferenceExcerpt::Stepper ReferenceManager::ReferenceExcerpt::getStepper() const {
    return Stepper(*this);
}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceManager::ReferenceExcerpt ReferenceManager::ReferenceExcerpt::getSubExcerpt(size_t start, size_t end) const {
    UTILS_DIE_IF(start < global_start || end > global_end, "SubExcerpt can't be bigger than parent");
    ReferenceExcerpt ret(ref_name, start, end);
    for (size_t i = start; i < end; i += CHUNK_SIZE) {
        ret.mapChunkAt(i, getChunkAt(i));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ReferenceManager::ReferenceExcerpt::getString(size_t start, size_t end) const {
    if (start < global_start || end > global_end) {
        UTILS_DIE("String can't be bigger than reference excerpt");
    }
    auto stepper = getStepper();
    std::string ret;
    stepper.inc(start - global_start);
    for (size_t i = start; i < end; ++i) {
        ret += stepper.get();
        stepper.inc();
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceManager::ReferenceManager(size_t csize) : cacheSize(csize) {}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::addRef(std::unique_ptr<Reference> ref) {
    std::unique_lock<std::mutex> lock2(cacheInfoLock);
    auto sequence = data.find(ref->getName());
    size_t curChunks = sequence == data.end() ? 0 : sequence->second.size();
    for (size_t i = curChunks; i < (ref->getEnd() - 1) / CHUNK_SIZE + 1; i++) {
        data[ref->getName()].push_back(genie::util::make_unique<CacheLine>());
    }
    mgr.registerRef(std::move(ref));
}

// ---------------------------------------------------------------------------------------------------------------------

std::shared_ptr<const std::string> ReferenceManager::loadAt(const std::string& name, size_t pos) {
    std::unique_lock<std::mutex> lock2(cacheInfoLock);
    size_t id = pos / CHUNK_SIZE;
    auto it = data.find(name);

    // Invalid chunk
    if (it == data.end()) {
        return ReferenceExcerpt::undef_page();
    }

    auto& cacheline = *it->second[id];
    lock2.unlock();

    // Very important that lock2 is released and we lock again in that specific order, to avoid deadlocks.
    std::lock_guard<std::mutex> lock1(cacheline.loadMutex);
    lock2.lock();

    // Chunk already loaded
    auto ret = cacheline.chunk;
    if (ret) {
        touch(name, id);
        return ret;
    }

    // Try quick load. Maybe another thread unloaded this reference but it is still in memory, reachable via weak
    // ptr.
    ret = cacheline.memory.lock();
    if (ret) {
        cacheline.chunk = ret;
        cacheline.memory = ret;
        touch(name, id);
        return ret;
    }

    // Reference is not in memory. We have to do a slow read from disc...
    // Loading mutex keeps locked for this chunk, but other chunks can be accessed, main lock is opened.
    lock2.unlock();
    ret = std::make_shared<const std::string>(mgr.getSequence(name, id * CHUNK_SIZE, (id + 1) * CHUNK_SIZE));
    lock2.lock();

    cacheline.chunk = ret;
    cacheline.memory = ret;
    touch(name, id);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceManager::ReferenceExcerpt ReferenceManager::load(const std::string& name, size_t start, size_t end) {
    ReferenceExcerpt ret(name, start, end);
    for (size_t i = start; i < end; i += CHUNK_SIZE) {
        ret.mapChunkAt(i, loadAt(name, i));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::pair<size_t, size_t>> ReferenceManager::getCoverage(const std::string& name) const {
    return mgr.getCoverage(name);
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string> ReferenceManager::getSequences() const { return mgr.getSequences(); }

// ---------------------------------------------------------------------------------------------------------------------

size_t ReferenceManager::getLength(const std::string& name) {
    auto cov = getCoverage(name);
    size_t ret = 0;
    for (const auto& c : cov) {
        ret = std::max(c.second, ret);
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------