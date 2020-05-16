#ifndef GENIE_SIDESELECTOR_H
#define GENIE_SIDESELECTOR_H

namespace genie {
namespace util {

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

}  // namespace util
}  // namespace genie

#endif  // GENIE_SIDESELECTOR_H
