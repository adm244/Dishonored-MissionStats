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
  DONE:
    - Locate where mission stats are stored in memory
    - Detect if ghost playthrough has been ruinned
    - Show location discovery message with a custom text
    
  TODO:
    - Draw all mission stats on a screen
    - Draw if specific mission stat has changed
    - Ability to lock a specific mission stat value
*/

#define PSAPI_VERSION 1
#include <windows.h>
#include <psapi.h>
#include <cstdio>

#include "types.h"
#include "detours.cpp"
#include "d3dhook.cpp"

#include "dishonored.cpp"
#include "hooks.cpp"

internal bool Initialize()
{
  if (!WriteDetour((void *)0x00AA94A0, ModifyStatVariable_Hook, 0)) {
    return false;
  }
  
  if (!D3D9HookInitialize()) {
    return false;
  }
  
  return true;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  if (reason == DLL_PROCESS_ATTACH) {
    return Initialize() ? TRUE : FALSE;
  }
  
  return TRUE;
}
