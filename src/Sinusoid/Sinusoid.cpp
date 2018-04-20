
#include <iostream>
#include "Sinusoid.h"
#include "utilFunctions.h"
#include "ErrorDef.h"

Error_t CSinusoid::create(CSinusoid *&pCSinusoid){
    
    if (pCSinusoid) {
        return kUnknownError;
    }
    pCSinusoid = new CSinusoid ();
    return kNoError;
}

Error_t CSinusoid::destroy(CSinusoid *&pCSinusoid){
    
    if(!pCSinusoid)
        return kUnknownError;
    delete pCSinusoid;
    pCSinusoid = 0;
    return kNoError;
}

CSinusoid::CSinusoid()
{

}

CSinusoid::~CSinusoid()
{
    
}

Error_t CSinusoid::init(int iBlockSize, int iHopSize, float fSampleRateInHz, float fMaxNSines, float fMinSinDur, float fFreqDevOffset, float fFreqDevSlope, float fAmpThresdB){
    
    if(m_bIsInitialized){
        reset();
    }
    
    m_bIsInitialized = true;
    
    
    return kNoError;
}

Error_t CSinusoid::reset (){
    if(!m_bIsInitialized){
        return kNotInitializedError;
    }
    
    for(int i=0; i<m_afParams[kMaxNSines]; i++){
        delete [] m_ppfXTMag[i];
        delete [] m_ppfXTFreq[i];
        delete [] m_ppfXTPhase[i];
    }
    
    delete m_ppfXTMag;
    delete m_ppfXTFreq;
    delete m_ppfXTPhase;
    
}

