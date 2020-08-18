/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SELECTOR_HEAD_H
#define GENIE_SELECTOR_HEAD_H

// ---------------------------------------------------------------------------------------------------------------------

#include <functional>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

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
    virtual void route(Tin&& t, size_t mod_id, Section global_id);

    /**
     *
     * @return
     */
    static size_t defaultSelect(const Tin&);

   public:
    /**
     *
     */
    SelectorHead();

    /**
     *
     * @param mod
     */
    void add(genie::util::Drain<Tin>* mod);

    /**
     *
     * @param mod
     * @param index
     */
    void set(genie::util::Drain<Tin>* mod, size_t index);

    /**
     *
     * @param s
     */
    void setOperation(std::function<size_t(const Tin& t)> s);

    /**
     *
     */
    void flowIn(Tin&& t, const Section& id) override;

    /**
     *
     * @param pos
     */
    void flushIn(uint64_t& pos) override;

    /**
     *
     * @param id
     */
    void skipIn(const Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "selector-head.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SELECTOR_HEAD_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------