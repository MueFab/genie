#ifndef GENIE_INDUSTRIAL_PARK_H
#define GENIE_INDUSTRIAL_PARK_H

#include <functional>
#include <map>
#include <typeindex>
#include "bitreader.h"
#include "factory.h"
#include "make-unique.h"

namespace genie {
namespace util {

class IndustrialPark {
   private:
    std::map<std::type_index, std::unique_ptr<GenericFactory>> factories;

    template <typename T>
    Factory<T>* findAndCreate() {
        auto type = std::type_index(typeid(T));
        auto it = factories.find(type);
        if (it == factories.end()) {
            factories.insert(std::make_pair(type, util::make_unique<Factory<T>>()));
            it = factories.find(type);
        }
        return reinterpret_cast<Factory<T>*>(it->second.get());
    }

    template <typename T>
    Factory<T>* findAndFail() const {
        auto type = std::type_index(typeid(T));
        auto it = factories.find(type);
        if (it == factories.end()) {
            UTILS_DIE("Unknown factory type");
        }
        return reinterpret_cast<Factory<T>*>(it->second.get());
    }

   public:
    template <typename T>
    void registerConstructor(uint8_t id, const std::function<std::unique_ptr<T>(util::BitReader&)>& constructor) {
        findAndCreate<T>()->registerType(id, constructor);
    }

    template <typename T>
    uint8_t registerConstructor(const std::function<std::unique_ptr<T>(util::BitReader&)>& constructor) {
        return findAndCreate<T>()->registerType(constructor);
    }

    template <typename T>
    void unregisterConstructor(uint8_t id) {
        findAndFail<T>()->unregisterType(id);
    }

    template <typename T>
    std::unique_ptr<T> construct(uint8_t id, util::BitReader& reader) const {
        return findAndFail<T>()->create(id, reader);
    }
};
}  // namespace util
}  // namespace genie

#endif  // GENIE_INDUSTRIAL_PARK_H
