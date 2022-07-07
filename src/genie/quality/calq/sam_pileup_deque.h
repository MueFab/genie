#ifndef CALQ_SAM_PILEUP_DEQUE_H_
#define CALQ_SAM_PILEUP_DEQUE_H_

// -----------------------------------------------------------------------------

#include <deque>

// -----------------------------------------------------------------------------

#include "sam_pileup.h"

// -----------------------------------------------------------------------------

namespace calq {

// -----------------------------------------------------------------------------

struct EncodingRead;

// -----------------------------------------------------------------------------

class SAMPileupDeque
{
    friend class SAMRecord;

 public:
    SAMPileupDeque();
    ~SAMPileupDeque();

    const SAMPileup& back() const;
    void clear();
    bool empty() const;
    const SAMPileup& front() const;
    size_t length() const;
    const SAMPileup& operator[](const size_t& n) const;
    void pop_back();
    void pop_front();
    size_t size() const;

    void print() const;

    uint32_t posMax() const;
    uint32_t posMin() const;

    void setPosMax(const uint32_t& posMax);
    void setPosMin(const uint32_t& posMin);

    void add(const EncodingRead& r,
             uint8_t qvalOffset,
             uint8_t hqSoftClipThreshold
    );

 private:
    std::deque<SAMPileup> pileups_;
    uint32_t posMax_;
    uint32_t posMin_;
};

// -----------------------------------------------------------------------------

}  // namespace calq

// -----------------------------------------------------------------------------

#endif  // CALQ_SAM_PILEUP_DEQUE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
