/* Description:Pre-definitions for exterior libraries.
 * Language:C++
 * Author:***
 *
 */

#ifndef LIB_EXT_LIB
#define LIB_EXT_LIB

#ifdef EXTLIB_FFTW
#ifdef LIBENV_CPLR_VS
#pragma comment(lib, "libfftw3f-3.lib")
#endif
#include "fftw/fftw3.h"
#endif

#ifdef EXTLIB_FREEIMAGE
#ifdef LIBENV_CPLR_VS
#pragma comment(lib, "FreeImage.lib")
#endif
#include "FreeImage/FreeImage.h"
#endif

#ifdef EXTLIB_FFMPEG
#ifdef LIBENV_CPLR_VS
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "postproc.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")
#endif
#endif

#endif