#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "Synthesis.h"

//#include "Util.h"
#include "Vector.h"
#include "Vibrato.h"

SUITE(Vibrato)
{
    struct VibratoData
    {
        VibratoData()
        {
            
            m_ppfTime    = new float* [1];
            m_ppfResult  = new float* [1];
            
            m_ppfTime[0] = new float [m_iBlockSize];
            m_ppfResult[0] = new float [m_iBlockSize];

            CVibrato::create(m_pCVibrato);
            m_pCVibrato->init(1,44100,1);
        }
        
        ~VibratoData()
        {
            CVibrato::destroy(m_pCVibrato);
            
            
            delete [] m_ppfTime[0];
            delete [] m_ppfResult[0];
            delete [] m_ppfTime;
            delete [] m_ppfResult;
            
            
        }
        
        CVibrato                *m_pCVibrato  = 0;
        float                   **m_ppfTime     = 0;
        float                   **m_ppfResult     = 0;

        int                     m_iBlockSize  = 2048;

    };
    
    TEST_FIXTURE(VibratoData, BasicTest)
    {
        m_pCVibrato->setParam(CVibrato::kParamDelay, 0.01);
        m_pCVibrato->setParam(CVibrato::kParamWidth, 0.01);
        m_pCVibrato->setParam(CVibrato::kParamModulation, 10);
        CHECK_CLOSE(0.01,m_pCVibrato->getParam(CVibrato::kParamDelay),1e-5);
        CHECK_CLOSE(0.01,m_pCVibrato->getParam(CVibrato::kParamWidth),1e-5);
        CHECK_CLOSE(10,m_pCVibrato->getParam(CVibrato::kParamModulation),1e-5);
        
        //Preventing width from being greater than delay
        m_pCVibrato->setParam(CVibrato::kParamWidth, 0.1);
        CHECK_CLOSE(0.01,m_pCVibrato->getParam(CVibrato::kParamWidth),1e-5);
        
        //Adjusting Width to new delay
        m_pCVibrato->setParam(CVibrato::kParamDelay, 0.001);
        CHECK_CLOSE(0.001,m_pCVibrato->getParam(CVibrato::kParamWidth),1e-5);
        
        //Delay goes beyond range
        m_pCVibrato->setParam(CVibrato::kParamDelay, 1.1);
        CHECK_CLOSE(0.001,m_pCVibrato->getParam(CVibrato::kParamDelay),1e-5);
    }
    TEST_FIXTURE(VibratoData, ZeroWidthTest)
    {
        int iCounter = 0;
        //Create a 1 sample delay
        m_pCVibrato->setParam(CVibrato::kParamDelay, 1/44100.0);
        m_pCVibrato->setParam(CVibrato::kParamWidth, 0);
        m_pCVibrato->setParam(CVibrato::kParamModulation, 10);
        
        CSynthesis::generateSine(m_ppfTime[0], 2.F, 1.F*m_iBlockSize, m_iBlockSize, 1.F, 0);
        
        m_pCVibrato->process(m_ppfTime, m_ppfResult, m_iBlockSize, iCounter);
        
        for(int i = 0;i<m_iBlockSize-1;i++)
        {
            
            CHECK_CLOSE(m_ppfTime[0][i],m_ppfResult[0][i+1],1e-5);
        }

        m_pCVibrato->setParam(CVibrato::kParamDelay, 5/44100.0);
        m_pCVibrato->process(m_ppfTime, m_ppfResult, m_iBlockSize, iCounter);
        
        for(int i = 0;i<m_iBlockSize-5;i++)
        {
            
            CHECK_CLOSE(m_ppfTime[0][i],m_ppfResult[0][i+5],1e-5);
        }
        
    }
    TEST_FIXTURE(VibratoData, DCInputTest)
    {
        int iCounter = 0;

        for(int i = 0;i<m_iBlockSize;i++)
        {
            m_ppfTime[0][i] = 0.5;
        }
        m_pCVibrato->setParam(CVibrato::kParamDelay, 5/44100.0);
        m_pCVibrato->setParam(CVibrato::kParamWidth, 0.01);
        m_pCVibrato->setParam(CVibrato::kParamModulation, 10);
        
        m_pCVibrato->process(m_ppfTime, m_ppfResult, m_iBlockSize, iCounter);
        //Delay still happens and at the boundary there is a blip
        for(int i = 0;i<m_iBlockSize-5;i++)
        {
            
            CHECK_CLOSE(m_ppfTime[0][i],m_ppfResult[0][i+5],1e-1);
        }
        
    }
    
    TEST_FIXTURE(VibratoData,ZeroInputTest)
    {
        int iCounter = 0;

        for(int i = 0;i<m_iBlockSize;i++)
        {
            m_ppfTime[0][i] = 0;
            m_ppfResult[0][i] = 1;
        }
        
        m_pCVibrato->setParam(CVibrato::kParamDelay, 5/44100.0);
        m_pCVibrato->setParam(CVibrato::kParamWidth, 0.01);
        m_pCVibrato->setParam(CVibrato::kParamModulation, 10);
        
        m_pCVibrato->process(m_ppfTime, m_ppfResult, m_iBlockSize, iCounter);
        
        CHECK_ARRAY_CLOSE(m_ppfTime[0], m_ppfResult[0], m_iBlockSize, 1e-5);

    }
    
    TEST_FIXTURE(VibratoData, BlockSizeTest)
    {
        int iCounter = 0;

        m_pCVibrato->setParam(CVibrato::kParamDelay, 1/44100.0);
        m_pCVibrato->setParam(CVibrato::kParamWidth, 0);
        m_pCVibrato->setParam(CVibrato::kParamModulation, 10);
        
        

        for(int iBlockSize = 1;iBlockSize<1000;iBlockSize++)
        {
            delete [] m_ppfTime[0];
            delete [] m_ppfResult[0];
            iCounter = 0;
            m_ppfTime[0] = new float [iBlockSize];
            m_ppfResult[0] = new float [iBlockSize];
            CSynthesis::generateSine(m_ppfTime[0], 2.F, 1.F*iBlockSize, iBlockSize, 1.F, 0);
        
            m_pCVibrato->process(m_ppfTime, m_ppfResult, iBlockSize, iCounter);
        
            for(int i = 0;i<iBlockSize-1;i++)
            {
                CHECK_CLOSE(m_ppfTime[0][i],m_ppfResult[0][i+1],1e-5);
            }

        }
    }
    
    TEST_FIXTURE(VibratoData,MultichannelTest)
    {
        
        m_pCVibrato->reset();
        m_pCVibrato->init(1,44100,3);
        delete [] m_ppfTime[0];
        delete [] m_ppfResult[0];
        delete []m_ppfTime;
        delete []m_ppfResult;
        int iCounter = 0;
        m_pCVibrato->setParam(CVibrato::kParamDelay, 1/44100.0);
        m_pCVibrato->setParam(CVibrato::kParamWidth, 0);
        m_pCVibrato->setParam(CVibrato::kParamModulation, 10);
        
        m_ppfTime    = new float* [3];
        m_ppfResult  = new float* [3];
        
        
        
        for(int c = 0;c<3;c++)
        {
            m_ppfTime[c] = new float [m_iBlockSize];
            m_ppfResult[c] = new float [m_iBlockSize];
            CSynthesis::generateSine(m_ppfTime[c], 2.F, 1.F*m_iBlockSize, m_iBlockSize, 1.F, 0);

        }
        
        m_pCVibrato->process(m_ppfTime, m_ppfResult, m_iBlockSize, iCounter);
        for(int c = 0;c<3;c++)
        {
            for(int i = 0;i<m_iBlockSize-1;i++)
            {
                
                CHECK_CLOSE(m_ppfTime[c][i],m_ppfResult[c][i+1],1e-5);
            }
        }

        
    }
    }


#endif //WITH_TESTS
