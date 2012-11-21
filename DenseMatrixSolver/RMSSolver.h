#ifdef WIN32
// for windows dlls

#ifdef RMSSOLVER_DLL_EXPORT
// define symbol RMS_DLL_EXPORT when compiling dll
#define RMSSOLVER_ITEM __declspec(dllexport)
#else
// otherwise, automagically define this for dll clients
#define RMSSOLVER_ITEM __declspec(dllimport)
#endif

#else
// for a static library or other platforms

#define RMSSOLVER_ITEM

#endif

#ifndef NULL
#define NULL 0
#endif


