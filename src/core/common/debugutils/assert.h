#include "../core.h"

#ifdef DEBUG
#include <assert.h>
#define ccAssert(x) assert(x)
#else
#define ccAssert(x) 
#endif
