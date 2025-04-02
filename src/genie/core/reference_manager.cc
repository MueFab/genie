/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <string>
#include "genie/core/reference_manager.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

const uint64_t ReferenceManager::chunk_size_ = 1 * 1024 * 1024;  //!

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::touch(const std::string& name, size_t num) {
    if (cache_info_.size() < cache_size_) {
        cache_info_.push_front(std::make_pair(name, num));
        return;
    }

    for (auto it = cache_info_.begin(); it != cache_info_.end(); ++it) {
        if (it->first == name && it->second == num) {
            cache_info_.erase(it);
            cache_info_.push_front(std::make_pair(name, num));
            return;
        }
    }

    cache_info_.push_front(std::make_pair(name, num));
    auto& line = data_[cache_info_.back().first][cache_info_.back().second];
    cache_info_.pop_back();

    line->chunk.reset();
    line->memory.reset();
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::validateRefID(size_t id) {
    std::unique_lock<std::mutex> lock2(cache_info_lock_);
    for (size_t i = 0; i <= id; ++i) {
        auto s = std::to_string(i);
        data_[std::string(s.size() < 3 ? (3 - s.size()) : 0, '0') + s];
    }
}

// ---------------------------------------------------------------------------------------------------------------------

size_t ReferenceManager::Ref2Id(const std::string& ref) {
    std::unique_lock<std::mutex> lock2(cache_info_lock_);
    for (const auto& r : indices_) {
        if (r.second == ref) {
            return r.first;
        }
    }
    UTILS_DIE("Unknown reference");
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ReferenceManager::Id2Ref(size_t id) {
    std::unique_lock<std::mutex> lock2(cache_info_lock_);
    auto it = indices_.find(id);
    UTILS_DIE_IF(it == indices_.end(), "Unknown reference ID. Forgot to specify external reference?");
    return it->second;
}

// ---------------------------------------------------------------------------------------------------------------------

bool ReferenceManager::RefKnown(size_t id) {
    std::unique_lock<std::mutex> lock2(cache_info_lock_);
    auto it = indices_.find(id);
    return it != indices_.end();
}

// ---------------------------------------------------------------------------------------------------------------------

size_t ReferenceManager::GetChunkSize() { return chunk_size_; }

// ---------------------------------------------------------------------------------------------------------------------

bool ReferenceManager::ReferenceExcerpt::IsEmpty() const {
    for (const auto& p : data) {
        if (isMapped(p)) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::ReferenceExcerpt::Merge(ReferenceExcerpt& e) {
    if (this->ref_name.empty()) {
        *this = std::move(e);
        return;
    }
    extend(e.global_end);
    for (size_t i = e.global_start / chunk_size_; i < (e.global_end - 1) / chunk_size_ + 1; i++) {
        if (e.isMapped(i * chunk_size_)) {
            mapChunkAt(i * chunk_size_, e.GetChunkAt(i * chunk_size_));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

size_t ReferenceManager::ReferenceExcerpt::GetGlobalStart() const { return global_start; }

// ---------------------------------------------------------------------------------------------------------------------

size_t ReferenceManager::ReferenceExcerpt::GetGlobalEnd() const { return global_end; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ReferenceManager::ReferenceExcerpt::GetRefName() const { return ref_name; }

// ---------------------------------------------------------------------------------------------------------------------

std::shared_ptr<const std::string> ReferenceManager::ReferenceExcerpt::GetChunkAt(size_t pos) const {
    int id = static_cast<int>(size_t(pos / chunk_size_) - size_t(global_start / chunk_size_));
    UTILS_DIE_IF(id < 0 || id >= (int)data.size(), "Invalid index");
    return data[id];
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::ReferenceExcerpt::mapChunkAt(size_t pos, std::shared_ptr<const std::string> dat) {
    int id = static_cast<int>(size_t(pos / chunk_size_) - size_t(global_start / chunk_size_));
    UTILS_DIE_IF(id < 0 || id >= (int)data.size(), "Invalid index");
    data[id] = std::move(dat);
}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceManager::ReferenceExcerpt::ReferenceExcerpt() : ReferenceExcerpt("", 0, 1) {}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceManager::ReferenceExcerpt::ReferenceExcerpt(std::string name, size_t start, size_t end)
    : ref_name(std::move(name)),
      global_start(start),
      global_end(end),
      data(((global_end - 1) / chunk_size_) - (global_start / chunk_size_) + 1, undef_page()) {}

// ---------------------------------------------------------------------------------------------------------------------

const std::shared_ptr<const std::string>& ReferenceManager::ReferenceExcerpt::undef_page() {
    static std::shared_ptr<const std::string> ret = std::make_shared<const std::string>(chunk_size_, 'N');
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::ReferenceExcerpt::extend(size_t newEnd) {
    if (newEnd < global_end) {
        return;
    }
    size_t id = (newEnd - 1) / chunk_size_;
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
    return isMapped(data[(pos - (global_start - global_start % chunk_size_)) / chunk_size_]);
}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceManager::ReferenceExcerpt::Stepper::Stepper(const ReferenceExcerpt& e) {
    startVecIt = e.data.begin();
    vecIt = e.data.begin();
    endVecIt = e.data.end();
    stringPos = e.global_start % chunk_size_;
    curString = (**vecIt).data();
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::ReferenceExcerpt::Stepper::inc(size_t off) {
    stringPos += off;
    while (stringPos >= chunk_size_) {
        stringPos -= chunk_size_;
        vecIt++;
    }
    if (vecIt != endVecIt) {
        curString = vecIt->get()->data();
    }
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

ReferenceManager::ReferenceManager(size_t csize) : cache_size_(csize) {}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceManager::addRef(size_t index, std::unique_ptr<Reference> ref) {
    std::unique_lock<std::mutex> lock2(cache_info_lock_);
    UTILS_DIE_IF(indices_.find(index) != indices_.end(), "Ref index already taken");
    indices_.insert(std::make_pair(index, ref->getName()));
    auto sequence = data_.find(ref->getName());
    size_t curChunks = sequence == data_.end() ? 0 : sequence->second.size();
    for (size_t i = curChunks; i < (ref->getEnd() - 1) / chunk_size_ + 1; i++) {
        data_[ref->getName()].push_back(genie::util::make_unique<CacheLine>());
    }
    mgr_.registerRef(std::move(ref));
}

// ---------------------------------------------------------------------------------------------------------------------

std::shared_ptr<const std::string> ReferenceManager::loadAt(const std::string& name, size_t pos) {
    std::unique_lock<std::mutex> lock2(cache_info_lock_);
    size_t id = pos / chunk_size_;
    auto it = data_.find(name);

    // Invalid chunk
    if (it == data_.end()) {
        return ReferenceExcerpt::undef_page();
    }

    auto& cacheline = *it->second[id];
    lock2.unlock();

    // Very important that lock2 is released and we lock again in that specific order, to avoid deadlocks.
    std::lock_guard<std::mutex> lock1(cacheline.load_mutex);
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
    ret = std::make_shared<const std::string>(mgr_.getSequence(name, id * chunk_size_, (id + 1) * chunk_size_));
    lock2.lock();

    cacheline.chunk = ret;
    cacheline.memory = ret;
    touch(name, id);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceManager::ReferenceExcerpt ReferenceManager::Load(const std::string& name, size_t start, size_t end) {
    ReferenceExcerpt ret(name, start, end);
    for (size_t i = start / chunk_size_; i <= (end - 1) / chunk_size_; i++) {
        ret.mapChunkAt(i * chunk_size_, loadAt(name, i * chunk_size_));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::pair<size_t, size_t>> ReferenceManager::GetCoverage(const std::string& name) const {
    return mgr_.getCoverage(name);
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string> ReferenceManager::GetSequences() const {
    std::vector<std::string> ret;
    for (const auto& i : indices_) {
        ret.push_back(i.second);
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t ReferenceManager::GetLength(const std::string& name) {
    auto cov = GetCoverage(name);

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
