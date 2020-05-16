#ifndef GENIE_SELECTOR_H
#define GENIE_SELECTOR_H

#include <functional>
#include <vector>
#include "exceptions.h"
#include "source.h"
#include "thread-manager.h"
namespace genie {
namespace util {

template <typename Tout>
class SelectorTail : public genie::util::Source<Tout>, public genie::util::Drain<Tout> {
   private:
    size_t dryCtr{};
    size_t modNum{};
    size_t position{};

   public:
    explicit SelectorTail() : modNum(0), position(0) {}

    void addMod() { modNum++; }

    void setNum(size_t num) { modNum = num; }

    void flowIn(Tout&& t, const Section& id) override { genie::util::Source<Tout>::flowOut(std::move(t), id); }

    void dryIn() override {
        size_t num = ++dryCtr;
        if (num == modNum) {
            // Output signal once every module contained finished
            dryCtr = 0;
            genie::util::Source<Tout>::dryOut();
        }
    }

    void skipIn(const Section& id) override {
        if (id.strongSkip) {
            genie::util::Source<Tout>::skipOut(id);
        }
    }
};

template <typename Tin>
class SelectorHead : public genie::util::Drain<Tin> {
   private:
    std::vector<genie::util::Drain<Tin>*> mods;
    std::function<size_t(const Tin& t)> select;

   protected:
    virtual void route(Tin&& t, size_t mod_id, Section global_id) {
        for (size_t i = 0; i < mods.size(); ++i) {
            if (i == mod_id) {
                mods[i]->flowIn(std::move(t), global_id);
            } else {
                mods[i]->skipIn(Section{global_id.start, global_id.length, false});
            }
        }
    }

    static size_t defaultSelect(const Tin&) { return 0; }

   public:
    SelectorHead() : select(&defaultSelect) {}
    explicit SelectorHead(std::function<size_t(const Tin& t)> _selector) : select(_selector) {}

    void add(genie::util::Drain<Tin>* mod) { mods.push_back(mod); }

    void set(genie::util::Drain<Tin>* mod, size_t index) { mods[index] = mod; }

    void setOperation(std::function<size_t(const Tin& t)> s) { select = s; }
    void flowIn(Tin&& t, const Section& id) override {
        Tin in = std::move(t);
        size_t dest = select(in);
        route(std::move(in), dest, id);
    }

    void dryIn() override {
        for (const auto& m : mods) {
            m->dryIn();
        }
    }

    void skipIn(const Section& id) override {
        for (const auto& m : mods) {
            m->skipIn(id);
        }
    }
};

template <typename Tin, typename Tout>
class Selector : public genie::util::Drain<Tin>, public genie::util::Source<Tout> {
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

    void setBranch(genie::util::Drain<Tin>* entry, genie::util::Source<Tout>* out, size_t index) {
        head.set(entry, index);
        out->setDrain(&tail);
    }

    void setOperation(std::function<size_t(const Tin& t)> s) { head.setOperation(s); }

    void flowIn(Tin&& t, const util::Section& id) override { head.flowIn(std::move(t), id); }

    void dryIn() override { head.dryIn(); }

    void skipIn(const Section& id) override { head.skipIn(id); }

    void setDrain(genie::util::Drain<Tout>* d) override { tail.setDrain(d); }
};

template <typename Coder, typename Ret, typename... Args>
class SideSelector {
   private:
    std::vector<Coder*> mods;
    std::function<size_t(Args...)> select;

    static size_t defaultSelect(Args...) { return 0; }

   public:
    SideSelector() : select(&defaultSelect) {}
    void setMod(Coder* mod, size_t index) { mods[index] = mod; }
    void addMod(Coder* mod) { mods.emplace_back(mod); }
    void setSelection(std::function<size_t(Args...)> _select) { select = _select; }
    Ret process(Args... param) {
        size_t index = select(param...);
        UTILS_DIE_IF(index >= mods.size(), "Invalid index in SideSelector");
        return mods[index]->process(param...);
    }
};

}  // namespace util
}  // namespace genie

#endif  // GENIE_SELECTOR_H
