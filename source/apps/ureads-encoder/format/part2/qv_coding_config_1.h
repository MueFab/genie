#ifndef GENIE_QV_CODING_CONFIG_1_H
#define GENIE_QV_CODING_CONFIG_1_H

#include "qv_coding_config.h"
#include <memory>

class BitWriter;

class Parameter_set_qvps {
private:
/*    uint8_t qv_num_codebooks_total : 4;
    std::vector<Qv_codebook> qv_codebooks;

public:
    Parameter_set_qvps();

    void addCodeBook(const Qv_codebook &book); */
public:
    virtual void write(BitWriter *writer) const;
};

/**
* ISO 23092-2 Section 3.3.2 table 7 lines 34 to 38
*/
class Qv_coding_config_1 : public Qv_coding_config {
public:
    /**
    * ISO 23092-2 Section 3.3.2.2.1
    */
    enum class Qvps_preset_ID {
        ASCII = 0, //!< 3.3.2.2.1.1
        OFFSET33_RANGE41 = 1, //!< 3.3.2.2.1.2
        OFFSET64_RANGE40 = 2 //!< 3.3.2.2.1.3
    };

    Qv_coding_config_1();

    explicit Qv_coding_config_1(Qvps_preset_ID _qvps_preset_ID, bool _reverse_flag);

    void setQvps(std::unique_ptr<Parameter_set_qvps> _parameter_set_qvps);

    void write(BitWriter *writer) const override;

private:
    uint8_t qvps_flag : 1; //!< Line 34
    std::unique_ptr<Parameter_set_qvps> parameter_set_qvps; //!< Line 36
    std::unique_ptr<Qvps_preset_ID> qvps_preset_ID; //!< : 4; Line 38
};

#endif //GENIE_QV_CODING_CONFIG_1_H
