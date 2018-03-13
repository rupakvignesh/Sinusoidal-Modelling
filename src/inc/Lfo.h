#if !defined(__Lfo_hdr__)
#define __Lfo_hdr__

#include <iostream>
#include <math.h>
#include "ErrorDef.h"
#include "RingBuffer.h"

using std::cout;

#define PI 3.14159265

class Lfo{

public: 
	Lfo(float fModFreqInSamples) :
    m_fParamModulation(fModFreqInSamples),
	m_RingBuff(0)
{
    if(fModFreqInSamples == 0)
        m_iBuffLength = 1;
    else
        m_iBuffLength = ceil(1/fModFreqInSamples);
    
	m_RingBuff = new CRingBuffer<float>(m_iBuffLength);
    computeWaveTable(m_iBuffLength, m_RingBuff);
}

virtual ~Lfo()
    {
        delete m_RingBuff;
    }
    
float getWaveTableValue(int iReadIdx)
    {
        return m_RingBuff->get(iReadIdx);
    }
private:
    //const int m_BuffLen = 2048;
    float m_fParamModulation;
    CRingBuffer<float>* m_RingBuff;
    int m_iBuffLength;
    
    Error_t computeWaveTable(int iBuffLength,CRingBuffer<float>* m_RingBuff){
    
        for(int i = 0;i<m_iBuffLength;i++)
        {
            m_RingBuff->putPostInc(sin(2*PI*m_fParamModulation*(i+1)));
        }
        
        return kNoError;
    }
    
};

#endif // #if !defined(__Vibrato_hdr__)
