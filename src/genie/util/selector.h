/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SELECTOR_H
#define GENIE_SELECTOR_H

// ---------------------------------------------------------------------------------------------------------------------

#include <functional>
#include <vector>
#include "exceptions.h"
#include "source.h"
#include "thread-manager.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 *
 * @tparam Tout
 */
template <typename Tout>
class SelectorTail : public genie::util::Source<Tout>, public genie::util::Drain<Tout> {
   private:
    size_t dryCtr{};    //!<
    size_t modNum{};    //!<
    size_t position{};  //!<

   public:
    /**
     *
     */
    explicit SelectorTail() : modNum(0), position(0) {}

    /**
     *
     */
    void addMod() { modNum++; }

    /**
     *
     * @param num
     */
    void setNum(size_t num) { modNum = num; }

    /**
     *
     * @param t
     * @param id
     */
    void flowIn(Tout&& t, const Section& id) override { genie::util::Source<Tout>::flowOut(std::move(t), id); }

    /**
     *
     * @param pos
     */
    void flushIn(size_t& pos) override {
        size_t num = ++dryCtr;
        if (num == modNum) {
            // Output signal once every module contained finished
            dryCtr = 0;
            genie::util::Source<Tout>::flushOut(pos);
        }
    }

    /**
     *
     * @param id
     */
    void skipIn(const Section& id) override {
        if (id.strongSkip) {
            genie::util::Source<Tout>::skipOut(id);
        }
    }
};

/**
 *
 * @tparam Tin
 */
template <typename Tin>
class SelectorHead : public genie::util::Drain<Tin> {
   private:
    std::vector<genie::util::Drain<Tin>*> mods;  //!<
    std::function<size_t(const Tin& t)> select;  //!<

   protected:
    /**
     *
     * @param t
     * @param mod_id
     * @param global_id
     */
    virtual void route(Tin&& t, size_t mod_id, Section global_id) {
        for (size_t i = 0; i < mods.size(); ++i) {
            if (i == mod_id) {
                mods[i]->flowIn(std::move(t), global_id);
            } else {
                mods[i]->skipIn(Section{global_id.start, global_id.length, false});
            }
        }
    }

    /**
     *
     * @return
     */
    static size_t defaultSelect(const Tin&) { return 0; }

   public:
    /**
     *
     */
    SelectorHead() : select(&defaultSelect) {}

    /**
     *
     * @param _selector
     */
    explicit SelectorHead(std::function<size_t(const Tin& t)> _selector) : select(_selector) {}

    /**
     *
     * @param mod
     */
    void add(genie::util::Drain<Tin>* mod) { mods.push_back(mod); }

    /**
     *
     * @param mod
     * @param index
     */
    void set(genie::util::Drain<Tin>* mod, size_t index) { mods[index] = mod; }

    /**
     *
     * @param s
     */
    void setOperation(std::function<size_t(const Tin& t)> s) { select = s; }

    /**
     *
     */
    void flowIn(Tin&& t, const Section& id) override {
        Tin in = std::move(t);
        size_t dest = select(in);
        route(std::move(in), dest, id);
    }

    /**
     *
     * @param pos
     */
    void flushIn(size_t& pos) override {
        for (const auto& m : mods) {
            m->flushIn(pos);
        }
    }

    /**
     *
     * @param id
     */
    void skipIn(const Section& id) override {
        bool first = true;
        for (const auto& m : mods) {
            if (first) {
                m->skipIn(id);
                first = false;
            } else {
                Section locID = id;
                locID.strongSkip = false;
                m->skipIn(locID);
            }
        }
    }
};

/**
 *
 * @tparam Tin
 * @tparam Tout
 */
template <typename Tin, typename Tout>
class Selector : public genie::util::Drain<Tin>, public genie::util::Source<Tout> {
   private:
    SelectorHead<Tin> head;   //!<
    SelectorTail<Tout> tail;  //!<

   public:
    /**
     *
     * @param select
     */
    explicit Selector(std::function<size_t(const Tin& t)> select) : head(select) { head.setTail(&tail); }

    /**
     *
     */
    Selector() = default;

    /**
     *
     */
    void addBranch(genie::util::Drain<Tin>* entry, genie::util::Source<Tout>* out) {
        head.add(entry);
        out->setDrain(&tail);
    }

    /**
     *
     */
    void setBranch(genie::util::Drain<Tin>* entry, genie::util::Source<Tout>* out, size_t index) {
        head.set(entry, index);
        out->setDrain(&tail);
    }

    /**
     *
     * @param s
     */
    void setOperation(std::function<size_t(const Tin& t)> s) { head.setOperation(s); }

    /**
     *
     * @param t
     * @param id
     */
    void flowIn(Tin&& t, const util::Section& id) override { head.flowIn(std::move(t), id); }

    /**
     *
     * @param pos
     */
    void flushIn(size_t& pos) override { head.flushIn(pos); }

    /**
     *
     * @param id
     */
    void skipIn(const Section& id) override { head.skipIn(id); }

    /**
     *
     */
    void setDrain(genie::util::Drain<Tout>* d) override { tail.setDrain(d); }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SELECTOR_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------