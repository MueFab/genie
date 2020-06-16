#ifndef GENIE_FACTORY_H
#define GENIE_FACTORY_H

#include <memory>
#include "exceptions.h"

namespace genie {
namespace util {

class GenericFactory {
   public:
    virtual ~GenericFactory() = default;
};

template <typename T>
class Factory : public GenericFactory {
   private:
    std::map<uint8_t, std::function<std::unique_ptr<T>(util::BitReader&)>> factories;

   public:
    void registerType(uint8_t id, const std::function<std::unique_ptr<T>(util::BitReader&)>& factory) {
        factories.insert(std::make_pair(id, factory));
    }

    uint8_t registerType(const std::function<std::unique_ptr<T>(util::BitReader&)>& factory) {
        uint8_t id = 0;
        auto it = factories.find(id);
        while (it != factories.end()) {
            id++;
            it = factories.find(id);
        }
        factories.insert(std::make_pair(id, factory));
        return id;
    }

    void unregisterType(uint8_t id) { factories.erase(id); }

    std::unique_ptr<T> create(uint8_t id, util::BitReader& reader) {
        auto it = factories.find(id);
        UTILS_DIE_IF(it == factories.end(), "Unknown implementation in factory");
        return it->second(reader);
    }
};
}  // namespace util
}  // namespace genie

#endif  // GENIE_FACTORY_H
