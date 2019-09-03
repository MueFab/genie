#ifndef GENIE_DESCRIPTOR_SUBSEQUENCE_CFG_H
#define GENIE_DESCRIPTOR_SUBSEQUENCE_CFG_H

#include <memory>
#include <vector>

#include "transform_subseq_parameters.h"
#include "transform_subseq_cfg.h"

class BitWriter;

/**
 * ISO 23092-2 Section 8.3.1 table lines 5 to 12
 */
class Descriptor_subsequence_cfg {
private:
    uint16_t descriptor_subsequence_ID : 10; //!< line 5
    std::unique_ptr<Transform_subseq_parameters> transform_subseq_parameters; //!< lines 6 + 7
    std::vector<std::unique_ptr<Transform_subseq_cfg>> transformSubseq_cfgs; //!< lines 8 to 12

public:
    Descriptor_subsequence_cfg();

    Descriptor_subsequence_cfg(std::unique_ptr<Transform_subseq_parameters> _transform_subseq_parameters,
                               uint16_t descriptor_subsequence_ID);

    void addTransformSubseqCfg(std::unique_ptr<Transform_subseq_cfg> _transformSubseq_cfg);

    std::unique_ptr<Descriptor_subsequence_cfg> clone() const;

    virtual void write(BitWriter *writer) const;
};

#endif //GENIE_DESCRIPTOR_SUBSEQUENCE_CFG_H
