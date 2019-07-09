/**
 * @file
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_CONTEXT_MODEL_H_
#define GABAC_CONTEXT_MODEL_H_

#include "cabac_tables.h"

namespace gabac {


class ContextModel
{
 public:
    explicit ContextModel(
            unsigned char state
    );

    ~ContextModel();

    /**
     * @note Inline definition for performance as suggested by NCSA
     */
    unsigned char getState() const;

    /**
     * @note Inline definition for performance as suggested by NCSA
     */
    unsigned char getMps() const;

    /**
     * @note Inline definition for performance as suggested by NCSA
     */
    void updateLps();

    /**
     * @note Inline definition for performance as suggested by NCSA
     */
    void updateMps();

 private:
    unsigned char m_state;
};


inline unsigned char ContextModel::getState() const{
    return m_state >> 1u;
}

inline unsigned char ContextModel::getMps() const{
    return m_state & static_cast<unsigned char>(0x1u);
}

inline void ContextModel::updateLps(){
    m_state = cabactables::nextStateLps[m_state];
}

inline void ContextModel::updateMps(){
    m_state = cabactables::nextStateMps[m_state];
}


}  // namespace gabac


#endif  // GABAC_CONTEXT_MODEL_H_
