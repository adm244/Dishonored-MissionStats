/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

//IMPORTANT(adm244): SCRATCH VERSION JUST TO GET IT UP WORKING

/*
  TODO:
    - general code cleanup
*/

#include <windows.h>
#include <cstdio>

#ifdef Debug
#include <assert.h>
#else
#define assert
#endif

#include "types.h"
#include "detours.cpp"
#include "ini_parser.h"

internal HMODULE thisModule = 0;

#include "game\pointers.h"
#include "game\types.h"
#include "game\patches.h"

internal bool Initialize()
{
  if (!InitPatches()) {
    OutputDebugStringA("Initialize: Couldn't initialize patches.");
    return false;
  }
  
  return true;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  thisModule = (HMODULE)instance;
  
  if (reason == DLL_PROCESS_ATTACH) {
    if (!Initialize()) {
      return FALSE;
    }
  }
  
  return TRUE;
}
