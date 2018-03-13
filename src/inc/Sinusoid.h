#if !defined(__Sinusoid_hdr__)
#define __Sinusoid_hdr__

#include "ErrorDef.h"

class CSinusoid
{
public:
    static Error_t create(CSinusoid *& pCSinusoid);
    static Error_t destroy(CSinusoid *& pCSinusoid);
    Error_t init();
    Error_t setParam();
    float   getParam();
    Error_t process();
    
    
private:
};

#endif
