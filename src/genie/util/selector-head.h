/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_SELECTOR_HEAD_H_
#define SRC_GENIE_UTIL_SELECTOR_HEAD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <functional>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief
 * @tparam Tin
 */
template <typename Tin>
class SelectorHead : public genie::util::Drain<Tin> {
 private:
    std::vector<genie::util::Drain<Tin>*> mods;  //!< @brief
    std::function<size_t(const Tin& t)> select;  //!< @brief

 protected:
    /**
     * @brief
     * @param t
     * @param mod_id
     * @param global_id
     */
    virtual void route(Tin&& t, size_t mod_id, Section global_id);

    /**
     * @brief
     * @return
     */
    static size_t defaultSelect(const Tin&);

 public:
    /**
     * @brief
     */
    SelectorHead();

    /**
     * @brief
     * @param mod
     */
    void add(genie::util::Drain<Tin>* mod);

    /**
     * @brief
     * @param mod
     * @param index
     */
    void set(genie::util::Drain<Tin>* mod, size_t index);

    /**
     * @brief
     * @param s
     */
    void setOperation(std::function<size_t(const Tin& t)> s);

    /**
     * @brief
     */
    void flowIn(Tin&& t, const Section& id) override;

    /**
     * @brief
     * @param pos
     */
    void flushIn(uint64_t& pos) override;

    /**
     * @brief
     * @param id
     */
    void skipIn(const Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/selector-head.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SELECTOR_HEAD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
