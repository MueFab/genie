#include "descriptor.h"
#include "bitwriter.h"
#include "make_unique.h"
#include "descriptor_configuration_present.h"

Descriptor::Descriptor() : class_specific_dec_cfg_flag(false), descriptor_configurations(0) {
    descriptor_configurations.push_back(make_unique<Descriptor_configuration_present>());
}

void Descriptor::setConfig(uint8_t index, std::unique_ptr<Descriptor_configuration> conf) {
    if (index > descriptor_configurations.size()) {
        GENIE_THROW_RUNTIME_EXCEPTION("Config index out of bounds.");
    }
    descriptor_configurations[index] = std::move(conf);
}

void Descriptor::setConfig(std::unique_ptr<Descriptor_configuration> conf) {
    if (class_specific_dec_cfg_flag) {
        GENIE_THROW_RUNTIME_EXCEPTION("Invalid setConfig() for non class-specific descriptor config.");
    }
    descriptor_configurations[0] = std::move(conf);
}

void Descriptor::enableClassSpecificConfigs(uint8_t numClasses) {
    if (class_specific_dec_cfg_flag) {
        GENIE_THROW_RUNTIME_EXCEPTION("Class specific configs already enabled");
    }
    class_specific_dec_cfg_flag = true;
    descriptor_configurations.resize(numClasses);
    for (size_t i = 1; i < descriptor_configurations.size(); ++i) {
        descriptor_configurations[i] = descriptor_configurations[0]->clone();
    }
}

void Descriptor::write(BitWriter *writer) const {
    writer->write(class_specific_dec_cfg_flag, 1);
    for (auto &i : descriptor_configurations) {
        i->write(writer);
    }
}