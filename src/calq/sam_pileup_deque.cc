#include "calq/sam_pileup_deque.h"

// -----------------------------------------------------------------------------

#include "calq/error_exception_reporter.h"
#include "calq/qual_encoder.h"

// -----------------------------------------------------------------------------

namespace calq {

// -----------------------------------------------------------------------------

SAMPileupDeque::SAMPileupDeque() : pileups_(), posMax_(0), posMin_(0) {}

// -----------------------------------------------------------------------------

SAMPileupDeque::~SAMPileupDeque() = default;

// -----------------------------------------------------------------------------

const SAMPileup& SAMPileupDeque::back() const {
    if (pileups_.empty()) {
        throwErrorException("Deque is empty");
    }
    return pileups_.back();
}

// -----------------------------------------------------------------------------

void SAMPileupDeque::clear() {
    pileups_.clear();
    posMax_ = 0;
    posMin_ = 0;
}

// -----------------------------------------------------------------------------

bool SAMPileupDeque::empty() const { return pileups_.empty(); }

// -----------------------------------------------------------------------------

const SAMPileup& SAMPileupDeque::front() const {
    if (pileups_.empty()) {
        throwErrorException("Deque is empty");
    }
    return pileups_.front();
}

// -----------------------------------------------------------------------------

size_t SAMPileupDeque::length() const { return posMax_ - posMin_ + 1; }

// -----------------------------------------------------------------------------

const SAMPileup& SAMPileupDeque::operator[](const size_t& n) const { return pileups_.at(n); }

// -----------------------------------------------------------------------------

void SAMPileupDeque::pop_back() {
    if (pileups_.empty()) {
        throwErrorException("Deque is empty");
    }
    pileups_.pop_back();
    posMax_--;
}

// -----------------------------------------------------------------------------

void SAMPileupDeque::pop_front() {
    if (pileups_.empty()) {
        throwErrorException("Deque is empty");
    }
    pileups_.pop_front();
    posMin_++;
}

// -----------------------------------------------------------------------------

size_t SAMPileupDeque::size() const { return pileups_.size(); }

// -----------------------------------------------------------------------------

void SAMPileupDeque::print() const {
    if (pileups_.empty()) {
        throwErrorException("Deque is empty");
    }

    for (auto const& samPileup : pileups_) {
        samPileup.print();
    }
}

// -----------------------------------------------------------------------------

uint32_t SAMPileupDeque::posMax() const { return posMax_; }

// -----------------------------------------------------------------------------

uint32_t SAMPileupDeque::posMin() const { return posMin_; }

// -----------------------------------------------------------------------------

void SAMPileupDeque::setPosMax(const uint32_t& posMax) {
    if (posMax < posMax_) {
        throwErrorException("posMax range");
    }
    posMax_ = posMax;
    pileups_.resize(length());
}

// -----------------------------------------------------------------------------

void SAMPileupDeque::setPosMin(const uint32_t& posMin) {
    if (posMin < posMin_) {
        throwErrorException("posMin range");
    }

    if (empty()) {
        posMin_ = posMin;
    } else {
        for (uint32_t i = posMin_; i < posMin; i++) {
            pop_front();
        }
    }
}

// -----------------------------------------------------------------------------
// TODO: look here
void SAMPileupDeque::add(const EncodingRead& r, uint8_t qvalOffset, uint8_t hqSoftClipThreshold) {
    if (this->empty()) {
        throwErrorException("samPileupQueue is empty");
    }
    if ((this->posMin() > r.posMin) || (this->posMax() < r.posMax - 1)) {
        throwErrorException("samPileupQueue does not overlap record");
    }

    size_t cigarIdx = 0;
    size_t cigarLen = r.cigar.length();
    size_t opLen = 0;  // length of current CIGAR operation
    size_t idx = 0;
    size_t pileupIdx = r.posMin - this->posMin();

    size_t softclips = 0;

    for (cigarIdx = 0; cigarIdx < cigarLen; cigarIdx++) {
        if (isdigit(r.cigar[cigarIdx])) {
            opLen = opLen * 10 + (size_t)r.cigar[cigarIdx] - (size_t)'0';
            continue;
        }

        const auto HQ_SOFTCLIP_THRESHOLD = static_cast<const char>(hqSoftClipThreshold + qvalOffset);

        switch (r.cigar[cigarIdx]) {
            case 'A':
            case 'C':
            case 'G':
            case 'T':
                opLen = 1;
                /* fall through */
            case 'M':
            case '=':
            case 'X':
                for (size_t i = 0; i < opLen; i++) {
                    this->pileups_[pileupIdx].pos = static_cast<uint32_t>(this->posMin() + pileupIdx);
                    this->pileups_[pileupIdx].seq += r.sequence[idx];
                    this->pileups_[pileupIdx].qual += r.qvalues[idx];
                    if (!r.reference.empty()) {
                        if (this->pileups_[pileupIdx].ref != 'N' &&
                            this->pileups_[pileupIdx].ref != r.reference[pileupIdx + this->posMin() - r.posMin]) {
                            throwErrorException(
                                "Non matching reference "
                                "between reads!");
                        }
                        this->pileups_[pileupIdx].ref = r.reference[pileupIdx + this->posMin() - r.posMin];
                    }

                    idx++;
                    pileupIdx++;
                }
                break;

            case 'S':
            case ')':
                for (int l = 0; l < static_cast<int>(opLen); ++l) {
                    if (r.qvalues[idx + l] >= HQ_SOFTCLIP_THRESHOLD) {
                        ++softclips;
                    }
                }
                /* fall through */
            case 'I':
            case '+':
                idx += opLen;
                break;
            case 'D':
            case '-':
            case 'N':
                pileupIdx += opLen;
                break;
            case 'H':
            case 'P':
                break;  // these have been clipped
            case '(':
            case '[':
            case ']':
                break;
            default:
                throwErrorException("Bad CIGAR string");
        }

        opLen = 0;
    }

    // Write clips
    for (size_t i = r.posMin - this->posMin(); i < pileupIdx; ++i) {
        this->pileups_[i].hq_softcounter += softclips;
    }
}

// -----------------------------------------------------------------------------

}  // namespace calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
