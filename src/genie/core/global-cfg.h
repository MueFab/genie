#ifndef GENIE_GLOBAL_CFG_H
#define GENIE_GLOBAL_CFG_H

#include <genie/util/exceptions.h>
#include <genie/util/make-unique.h>
#include <genie/util/industrial-park.h>
#include <parameter/descriptor_present/decoder.h>
#include <functional>
#include <map>
#include <typeindex>
#include <typeinfo>

namespace genie {
namespace core {

class GlobalCfg {
   private:
    GlobalCfg() = default;
    GlobalCfg(GlobalCfg& cfg) = default;

    util::IndustrialPark fpark;
   public:

    util::IndustrialPark& getIndustrialPark() {
        return fpark;
    }

    static GlobalCfg& getSingleton() {
        static GlobalCfg cfg;
        return cfg;
    }
};

}  // namespace core
}  // namespace genie

#endif  // GENIE_GLOBAL_CFG_H
