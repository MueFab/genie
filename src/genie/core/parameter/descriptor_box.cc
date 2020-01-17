/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "descriptor_box.h"
#include <genie/util/bitwriter.h>
#include <genie/util/exceptions.h>
#include <genie/util/make-unique.h>
#include "parameter/descriptor_present/descriptor_present.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

// ---------------------------------------------------------------------------------------------------------------------

DescriptorBox::DescriptorBox() : class_specific_dec_cfg_flag(false), descriptor_configurations(0) {
    descriptor_configurations.push_back(util::make_unique<desc_pres::DescriptorPresent>());
}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorBox::DescriptorBox(size_t num_classes, GenDesc desc, util::BitReader& reader) {
    class_specific_dec_cfg_flag = reader.read(1);
    if (class_specific_dec_cfg_flag == 0) {
        descriptor_configurations.emplace_back(Descriptor::factory(desc, reader));
    } else {
        for (size_t i = 0; i < num_classes; ++i) {
            descriptor_configurations.emplace_back(Descriptor::factory(desc, reader));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorBox::setClassSpecific(uint8_t index, std::unique_ptr<Descriptor> conf) {
    if (index > descriptor_configurations.size()) {
        UTILS_THROW_RUNTIME_EXCEPTION("Config index out of bounds.");
    }
    descriptor_configurations[index] = std::move(conf);
}

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorBox::set(std::unique_ptr<Descriptor> conf) {
    if (class_specific_dec_cfg_flag) {
        UTILS_THROW_RUNTIME_EXCEPTION("Invalid setConfig() for non class-specific descriptor config.");
    }
    descriptor_configurations[0] = std::move(conf);
}

// ---------------------------------------------------------------------------------------------------------------------

const Descriptor* DescriptorBox::get() const { return descriptor_configurations.front().get(); }

// ---------------------------------------------------------------------------------------------------------------------

const Descriptor* DescriptorBox::getClassSpecific(uint8_t index) const {
    return descriptor_configurations[index].get();
}

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorBox::enableClassSpecific(uint8_t numClasses) {
    if (class_specific_dec_cfg_flag) {
        return;
    }
    class_specific_dec_cfg_flag = true;
    descriptor_configurations.resize(numClasses);
    for (size_t i = 1; i < descriptor_configurations.size(); ++i) {
        descriptor_configurations[i] = descriptor_configurations[0]->clone();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool DescriptorBox::isClassSpecific() const { return class_specific_dec_cfg_flag; }

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorBox::write(util::BitWriter& writer) const {
    writer.write(class_specific_dec_cfg_flag, 1);
    for (auto& i : descriptor_configurations) {
        i->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<DescriptorBox> DescriptorBox::clone() const {
    auto ret = util::make_unique<DescriptorBox>();
    ret->descriptor_configurations.clear();
    ret->class_specific_dec_cfg_flag = class_specific_dec_cfg_flag;
    for (const auto& d : descriptor_configurations) {
        ret->descriptor_configurations.push_back(d->clone());
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------