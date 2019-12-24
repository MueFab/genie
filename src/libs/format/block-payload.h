#ifndef GENIE_BLOCK_PAYLOAD_H
#define GENIE_BLOCK_PAYLOAD_H

#include <sstream>

#include <coding/mpegg-raw-au.h>

class TransformedPayload {
    gabac::DataBlock data;

   public:
    void write(util::BitWriter* writer) const {
        for (size_t i = 0; i < data.size(); ++i) {
            writer->write(data.get(i), data.getWordSize() * 8);
        }
    }

    explicit TransformedPayload(gabac::DataBlock* _data) : data(0, 1) { data.swap(_data); }

    bool isEmpty() const { return data.empty(); }

    gabac::DataBlock* get() { return &data; }
};

class SubDescriptorPayload {
    std::vector<std::unique_ptr<TransformedPayload>> trans_pay;

    std::vector<TransformedPayload*> trans_pay_shadow;

   public:
    void write(util::BitWriter* writer) const {
        for (size_t i = 0; i < trans_pay.size(); ++i) {
            std::stringstream ss;
            util::BitWriter tmp_writer(&ss);
            trans_pay[i]->write(writer);
            tmp_writer.flush();
            uint64_t bits = tmp_writer.getBitsWritten();
            if (i != trans_pay.size() - 1) {
                writer->write(bits / 8, 32);
            }
            writer->write(&ss);
        }
    }

    void add(std::unique_ptr<TransformedPayload> p) { trans_pay_shadow.push_back(p.get()); trans_pay.push_back(std::move(p)); }

    bool isEmpty() const {
        if (trans_pay.empty()) {
            return true;
        }
        for (const auto& p : trans_pay) {
            if (!p->isEmpty()) {
                return false;
            }
        }
        return true;
    }

    TransformedPayload* getTransformedPayload(uint8_t index) const { return trans_pay[index].get(); }

    size_t getNumPayloads() const { return trans_pay.size(); }

    const std::vector<TransformedPayload*>& get() const {
        return trans_pay_shadow;
    }
};

class DescriptorPayload {
   private:
    std::vector<std::unique_ptr<SubDescriptorPayload>> sub_pay;

   public:
    void write(util::BitWriter* writer) const {
        for (size_t i = 0; i < sub_pay.size(); ++i) {
            std::stringstream ss;
            util::BitWriter tmp_writer(&ss);
            sub_pay[i]->write(writer);
            tmp_writer.flush();
            uint64_t bits = tmp_writer.getBitsWritten();
            if (i != sub_pay.size() - 1) {
                writer->write(bits / 8, 32);
            }
            writer->write(&ss);
        }
    }

    void add(std::unique_ptr<SubDescriptorPayload> p) { sub_pay.push_back(std::move(p)); }

    bool isEmpty() const {
        if (sub_pay.empty()) {
            return true;
        }
        for (const auto& p : sub_pay) {
            if (!p->isEmpty()) {
                return false;
            }
        }
        return true;
    }

    SubDescriptorPayload* getSubDescriptorPayload(GenomicSubsequence index) const { return sub_pay[uint8_t (index)].get(); }
};

class BlockPayloadSet {
   private:
    std::vector<std::unique_ptr<DescriptorPayload>> desc_pay;
    size_t record_num;
    std::unique_ptr<format::mpegg_p2::ParameterSet> parameters;

   public:
    explicit BlockPayloadSet(std::unique_ptr<format::mpegg_p2::ParameterSet> param, size_t _record_num = 0)
        : desc_pay(getDescriptors().size()), record_num(_record_num), parameters(std::move(param)) {}

    void setPayload(GenomicDescriptor i, std::unique_ptr<DescriptorPayload> p) {
        desc_pay[uint8_t(i)] = std::move(p);
    }

    DescriptorPayload* getPayload(GenomicDescriptor i) { return desc_pay[uint8_t(i)].get(); }

    std::unique_ptr<DescriptorPayload> movePayload(size_t i) { return std::move(desc_pay[i]); }

    size_t getRecordNum() const { return record_num; }

    format::mpegg_p2::ParameterSet* getParameters() { return parameters.get(); }

    const format::mpegg_p2::ParameterSet* getParameters() const { return parameters.get(); }

    std::unique_ptr<format::mpegg_p2::ParameterSet> moveParameters() { return std::move(parameters); }
};

#endif  // GENIE_BLOCK_PAYLOAD_H
