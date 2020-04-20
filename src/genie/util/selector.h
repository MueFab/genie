#ifndef GENIE_SELECTOR_H
#define GENIE_SELECTOR_H

#include <functional>
#include <vector>
#include "source.h"
#include "exceptions.h"
#include "thread-manager.h"
namespace genie {
namespace util {

class SelectorStash {
   private:
    std::vector<size_t> globalIDs;  // One per thread
   protected:
    virtual size_t retrieve() { return globalIDs[genie::util::ThreadManager::threadID]; }

   public:
    explicit SelectorStash(size_t num_Threads) : globalIDs(num_Threads, 0) {}
    virtual void notify(size_t global) { globalIDs[genie::util::ThreadManager::threadID] = global; }
    ~SelectorStash() = default;
};

template <typename Tout>
class SelectorTail : public genie::util::Source<Tout>, public genie::util::Drain<Tout>, public SelectorStash {
   private:
    std::atomic<size_t> dryCtr{};
    size_t modNum{};

   public:
    explicit SelectorTail(size_t thread_num) : SelectorStash(thread_num), modNum(0) {}
    SelectorTail() : SelectorStash(1) {}

    void setNum(size_t num) { modNum = num; }

    void flowIn(Tout&& t, size_t) override { genie::util::Source<Tout>::flowOut(std::move(t), retrieve()); }

    void dryIn() override {
        size_t num = ++dryCtr;
        if (num == modNum) {
            dryCtr = 0;
            genie::util::Source<Tout>::dryOut();
        }
    }
};

template <typename Tin>
class SelectorHead : public genie::util::Drain<Tin> {
   private:
    static constexpr size_t MAX_MODULES = 16;
    std::array<std::atomic<size_t>, MAX_MODULES> nextIDs{};  // One per module
    std::vector<genie::util::Drain<Tin>*> mods;
    SelectorStash* stash{};
    std::function<size_t(const Tin& t)> select;

   protected:
    virtual void route(Tin&& t, size_t mod_id, size_t global_id) {
        size_t local_id = nextIDs[mod_id]++;
        if (stash) {
            stash->notify(global_id);
        }
        mods[mod_id]->flowIn(std::move(t), local_id);
    }

    static size_t defaultSelect(const Tin&) { return 0; }

   public:
    SelectorHead() : select(&defaultSelect) {}
    explicit SelectorHead(std::function<size_t(const Tin& t)> _selector) : stash(nullptr), select(_selector) {}
    void add(genie::util::Drain<Tin>* mod) {
        UTILS_DIE_IF(mods.size() >= MAX_MODULES, "Too many modules in SelectorHead");
        mods.push_back(mod);
    }
    void setTail(SelectorStash* st) { stash = st; }
    void setOperation(std::function<size_t(const Tin& t)> s) { select = s; }
    void flowIn(Tin&& t, size_t id) override {
        Tin in = std::move(t);
        size_t dest = select(in);
        route(std::move(in), dest, id);
    }

    void dryIn() override {
        for (const auto& m : mods) {
            m->dryIn();
        }
    }
};

template <typename Tin, typename Tout>
class Selector : public genie::util::Drain<Tin> {
   private:
    SelectorHead<Tin> head;
    SelectorTail<Tout> tail;

   public:
    explicit Selector(std::function<size_t(const Tin& t)> select) : head(select) { head.setTail(&tail); }
    Selector() = default;

    void addBranch(genie::util::Drain<Tin>* entry, genie::util::Source<Tout>* out) {
        head.add(entry);
        out->setDrain(&tail);
    }
    void setOperation(std::function<size_t(const Tin& t)> s) { head.setOperation(s); }

    void flowIn(Tin&& t, size_t id) override { head.flowIn(std::move(t), id); }

    void dryIn() override { head.dryIn(); }

    virtual void setDrain(genie::util::Drain<Tout>* d) { tail.setDrain(d); }
};

template <typename Coder, typename Ret, typename... Args>
class SideSelector {
   private:
    std::vector<Coder*> mods;
    std::function<size_t(Args...)> select;

    static size_t defaultSelect(Args...) { return 0; }

   public:
    SideSelector() : select(&defaultSelect) {}
    void addMod(Coder* mod) { mods.emplace_back(mod); }
    void setSelection(std::function<size_t(Args...)> _select) { select = _select; }
    Ret process(Args... param) {
        size_t index = select(param...);
        UTILS_DIE_IF(index >= mods.size(), "Invalid index in SideSelector");
        return mods[index]->process(param...);
    }
};

}
}

#endif  // GENIE_SELECTOR_H
