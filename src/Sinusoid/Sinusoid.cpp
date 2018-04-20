
#include <iostream>
#include "Sinusoid.h"
#include "utilFunctions.h"
#include "ErrorDef.h"
#include "Fft.h"

Error_t CSinusoid::create(CSinusoid *&pCSinusoid)
{
    
    if (pCSinusoid) {
        return kUnknownError;
    }
    pCSinusoid = new CSinusoid ();
    return kNoError;
}

Error_t CSinusoid::destroy(CSinusoid *&pCSinusoid)
{
    
    if(!pCSinusoid)
        return kUnknownError;
    delete pCSinusoid;
    pCSinusoid = 0;
    return kNoError;
}


Error_t CSinusoid::init(int iBlockSize, int iHopSize, float fSampleRateInHz, float fMaxNSines, float fMinSinDur, float fFreqDevOffset, float fFreqDevSlope, float fAmpThresdB)
{
    
    if(m_bIsInitialized)
    {
        reset();
    }
    m_fSampleRateHz = fSampleRateInHz;
    
    setParam(CSinusoid::kNumFFT, iBlockSize);
    setParam(CSinusoid::kHopSize, iHopSize);
    setParam(CSinusoid::kMaxNSines, fMaxNSines);
    setParam(CSinusoid::kFreqDevSlope, fFreqDevSlope);
    setParam(CSinusoid::kFreqDevOffset, fFreqDevOffset);
    setParam(CSinusoid::kAmpThresdB, fAmpThresdB);
    setParam(CSinusoid::kMinSinDur, fMinSinDur);
    
    m_pCFft->initInstance(m_afParams[kNumFFT],2);
    
    m_piPeakLoc     = new int   [(int)m_afParams[CSinusoid::kMaxNSines]];
    m_pfIpMag       = new float [(int)m_afParams[CSinusoid::kMaxNSines]];
    m_pfIpPhase     = new float [(int)m_afParams[CSinusoid::kMaxNSines]];
    m_pfIpPeakLoc   = new float [(int)m_afParams[CSinusoid::kMaxNSines]];

    
    m_bIsInitialized = true;
    
    
    return kNoError;
}

Error_t CSinusoid::reset ()
{
    if(!m_bIsInitialized)
    {
        return kNotInitializedError;
    }
    
    CFft::destroyInstance(m_pCFft);
    delete m_piPeakLoc;
    delete m_pfIpPeakLoc;
    delete m_pfIpPhase;
    delete m_pfIpMag;
    
    return kNoError;
    
}

Error_t CSinusoid::analyze(float *pfInputBuffer, int NumFrames)
{
    float *pfMagSpectrum = new float [(int) m_afParams[CSinusoid::kNumFFT] +1];
    float *pfPhaseSpectrum = new float [(int) m_afParams[CSinusoid::kNumFFT] +1];
    CFft::complex_t *pfSpectrum = new CFft::complex_t [(int) m_afParams[CSinusoid::kNumFFT] *2];
    
    //Fft
    m_pCFft->doFft(pfSpectrum, pfInputBuffer);
    m_pCFft->getMagnitude(pfMagSpectrum, pfSpectrum);
    m_pCFft->getPhase(pfPhaseSpectrum, pfSpectrum);
    
    
    //Peak detection
    peakDetection(pfMagSpectrum);
    
    //Peak interpolation
    peakInterp(pfMagSpectrum, pfPhaseSpectrum);
    return kNoError;
    
}

Error_t CSinusoid::synthesize(float *pfOutputBuffer)
{
    float *pfReal = new float [(int) m_afParams[CSinusoid::kNumFFT] +1];
    float *pfImag = new float [(int) m_afParams[CSinusoid::kNumFFT] +1];
    genspecsines_C(m_pfIpPeakLoc, m_pfIpMag, m_pfIpPhase, m_iNumPeaksDetected, pfReal, pfImag, 1);
    
    
    return kNoError;
}

Error_t CSinusoid::peakDetection(float *pfMagSpectrum)
{
    int k = 0;
    for(int i = 1; i<m_afParams[kNumFFT]-1;i++)
    {
        if(pfMagSpectrum[i]>m_afParams[CSinusoid::kAmpThresdB] && pfMagSpectrum[i]>pfMagSpectrum[i-1] && pfMagSpectrum[i]>pfMagSpectrum[i+1])
        {
            m_piPeakLoc[k] = i;
            k++;
        }
    }
    m_iNumPeaksDetected = k;
    return kNoError;
    
}


Error_t CSinusoid::peakInterp(float *pfMagSpectrum, float *pfPhaseSpectrum)
{
    float fCurrVal = 0;
    float fLeftVal = 0;
    float fRightVal = 0;
    for(int i = 0;i<m_iNumPeaksDetected;i++)
    {
        fCurrVal = pfMagSpectrum[m_piPeakLoc[i]];
        fLeftVal = pfMagSpectrum[m_piPeakLoc[i]-1];
        fRightVal = pfMagSpectrum[m_piPeakLoc[i]+1];
        m_pfIpPeakLoc[i] = m_piPeakLoc[i] + 0.5*(fLeftVal-fRightVal)*(fLeftVal-2*fCurrVal+fRightVal);
        m_pfIpMag[i] = fCurrVal - 0.25*(fLeftVal-fRightVal)*(m_pfIpPeakLoc[i] - m_piPeakLoc[i]);
        
        //Need to do linear interpolation for phase Python code: ipphase = np.interp(iploc, np.arange(0, pX.size), pX)
    }
    
    return kNoError;
}


