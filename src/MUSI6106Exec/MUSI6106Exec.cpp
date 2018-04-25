
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "Sinusoid.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
        sOutputFilePath;

    static const int        kBlockSize = 2048;
    
     static const int        kHopSize = 1024;

    clock_t                 time = 0;

    float                   **ppfAudioData = 0;
    
    float                   **ppfOutputBuffer = 0;
    
    float                   *pfInputBuffer = 0;
    
    float                   *pfOutputMusic = 0;
    
    float                   *pfOldOutput    = 0;

    CAudioFileIf            *phAudioFile = 0;

    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;

    float                   fSampleRateInHz = 44100;
    CAudioFileIf            *pCInstance = 0;
    CSinusoid               *pCSinusoid = 0;

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 2)
    {
        cout << "Missing audio input path!";
        return -1;
    }
    else if(argc == 2)
    {
        sInputFilePath = argv[1];
        sOutputFilePath = sInputFilePath + ".txt";
        
    }
    

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phAudioFile->getFileSpec(stFileSpec);

    //////////////////////////////////////////////////////////////////////////////
    // open the output wav file
    
    CAudioFileIf::create(pCInstance);
    pCInstance->openFile("Output.wav",CAudioFileIf::kFileWrite,&stFileSpec);

    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
    hOutputFile.open(sOutputFilePath.c_str(), std::ios::out);
    if (!hOutputFile.is_open())
    {
        cout << "Text file open error!";
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioData[i] = new float[kHopSize];
    
    ppfOutputBuffer= new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfOutputBuffer[i] = new float[kBlockSize];
    
    pfInputBuffer = new float [kBlockSize];
    
    pfOutputMusic = new float [kHopSize];
    
    pfOldOutput = new float [kHopSize];
    

    //////////////////////////////////////////////////////////////////////////////
    // Set Sinusoid parameters
    
    CSinusoid::create(pCSinusoid);
    pCSinusoid->init(kBlockSize, kHopSize, fSampleRateInHz, 1024, 0, 0, 0, -80);
    
    time = clock();
    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output file
    while (!phAudioFile->isEof())
    {
        long long iNumFrames = kHopSize;
        phAudioFile->readData(ppfAudioData, iNumFrames);
        for(int i = 0;i<iNumFrames;i++)
        {
            pfInputBuffer[i+kHopSize] = ppfAudioData[0][i];
        }
        
        pCSinusoid->analyze(pfInputBuffer);
        pCSinusoid->synthesize(ppfOutputBuffer[0]);
        
        for(int i = 0;i<iNumFrames;i++)
        {
            pfOutputMusic[i] = ppfOutputBuffer[0][i+kHopSize]+pfOldOutput[i];
            pfOldOutput[i] = ppfOutputBuffer[0][i];
            pfInputBuffer[i] = pfInputBuffer[i+kHopSize];
            
        }
        pCInstance->writeData(ppfOutputBuffer, iNumFrames);
        cout << "\r" << "reading and writing";

        for (int i = 0; i < iNumFrames; i++)
        {
            
                hOutputFile << pfOutputMusic[i] << endl;
        }
    }

    cout << "\nreading/writing done in: \t" << (clock() - time)*1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phAudioFile);
    CAudioFileIf::destroy(pCInstance);
    CSinusoid::destroy(pCSinusoid);
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        delete [] ppfOutputBuffer[i];
        delete [] ppfAudioData[i];
    }
    
    delete ppfAudioData;
    delete ppfOutputBuffer;
    delete pfInputBuffer;
    delete pfOldOutput;
    delete pfOutputMusic;
 

}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2018 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

