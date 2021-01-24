//////////////////////////////////////////////////////////////////////////////
// dprintf.h
// See header file for description.
//
#include <stdarg.h>
#include <stdio.h>
#include <openvr_driver.h>

#if defined(_WIN32)
#include <windows.h>
// disable vc fopen warning
#pragma warning(disable : 4996)  
#define vsprintf vsprintf_s
#endif

// TODO3: make this cross platform/use spdlog?
void dprintf(const char *fmt, ...)
{
    va_list args;
    char buffer[2048];

    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);

#if !defined(NDEBUG)
    static FILE *text_file_log;
    if (text_file_log == 0)
    {
        text_file_log = fopen("c:\\temp\\cbt.txt", "wt");
        if (!text_file_log)
        {
            text_file_log = fopen("c:\\temp\\cbt.txt", "wt");
        }
    }

    if (text_file_log)
    {
        fprintf(text_file_log, "%s", buffer);
        fflush(text_file_log);
    }

#ifdef WIN32
  OutputDebugStringA(buffer);
#endif

#endif

    if (vr::VRDriverContext() && vr::VRDriverLog())
    {
        vr::VRDriverLog()->Log(buffer);
    }

    
}
