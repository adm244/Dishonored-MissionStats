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

#ifndef _D3D_HOOK_CPP_
#define _D3D_HOOK_CPP_

#include <D3D9.h>
#include <d3dx9core.h>

// typedef IDirect3D9 * (STDCALL _Direct3DCreate9)(UINT SDKVersion);
// typedef HRESULT (STDCALL _CreateDevice)(IDirect3D9 *d3d9, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS *, IDirect3DDevice9 **);
// typedef HRESULT (STDCALL _EndScene)(IDirect3DDevice9 *device);

// internal _CreateDevice *CreateDeviceOriginal = 0;
// internal _EndScene *EndSceneOriginal = 0;

// internal HRESULT STDCALL EndScene_wrapper(IDirect3DDevice9 *device)
// {
  // return EndSceneOriginal(device);
// }

// internal HRESULT STDCALL CreateDevice_wrapper(IDirect3D9 *d3d9, UINT adapter, D3DDEVTYPE deviceType, HWND focusWindow, DWORD behaviorFlags, D3DPRESENT_PARAMETERS *params, IDirect3DDevice9 **device)
// {
  // HRESULT result = CreateDeviceOriginal(d3d9, adapter, deviceType, focusWindow, behaviorFlags, params, device);
  // if (result == D3D_OK) {
    // HRESULT (STDCALL IDirect3DDevice9:: *func)() = &IDirect3DDevice9::EndScene;
    // _EndScene *EndSceneFunc = (_EndScene *&)func;
    // if (!HotPatch(EndSceneFunc, EndScene_wrapper, (void **)&EndSceneOriginal)) {
      // return D3DERR_NOTAVAILABLE;
    // }
  // }
  
  // return result;
// }

// internal IDirect3D9 * STDCALL Direct3DCreate9_wrapper(UINT SDKVersion)
// {
  // _Direct3DCreate9 *Direct3DCreate9Func = *(_Direct3DCreate9 **)0x00F93728;
  // IDirect3D9 *d3d9 = Direct3DCreate9Func(SDKVersion);
  // if (!d3d9) {
    // return 0;
  // }
  
  // HRESULT (STDCALL IDirect3D9:: *func)(UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS *, IDirect3DDevice9 **) = &IDirect3D9::CreateDevice;
  // _CreateDevice *CreateDeviceFunc = (_CreateDevice *&)func;
  // if (!HotPatch(CreateDeviceFunc, CreateDevice_wrapper, (void **)&CreateDeviceOriginal)) {
    // return 0;
  // }
  
  // return d3d9;
// }

struct GFXManager {
  u8 unk00[0x81C];
  IDirect3DDevice9 *d3dDevice; // 0x81C
  // ...
};

internal void *callendscene_hook_ret = (void *)0x009C0716;

internal bool d3d9_initialized = false;
internal ID3DXFont *dxFont = 0;

internal void CDECL CallEndScene(GFXManager *gfx)
{
  IDirect3DDevice9 *device = gfx->d3dDevice;
  
  if (!d3d9_initialized) {
    HRESULT result = D3DXCreateFont(device, 30, 0, FW_NORMAL, 1, FALSE,
      DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
      DEFAULT_PITCH | FF_DONTCARE, "Arial", &dxFont);
    if (FAILED(result)) {
      OutputDebugStringA("Could not create a font object!");
      return;
    }
    
    d3d9_initialized = true;
  }
  
  RECT rect = {0, 0, 500, 100};
  INT result = dxFont->DrawText(0, "This is a test string!", -1, &rect,
    DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));
  if (!result) {
    OutputDebugStringA("Could not draw a text!");
    return;
  }
}

internal void NAKED CallEndScene_Hook()
{
  __asm {
    mov eax, [esp+04h]
    push eax
    call CallEndScene
    add esp, 04h
    
    push ebp
    mov ebp, esp
    mov eax, [ebp+08h]
    
    jmp [callendscene_hook_ret]
  }
}

internal bool D3D9HookInitialize()
{
  // if (!WriteDetour((void *)0x00DA4A88, Direct3DCreate9_wrapper, 1)) {
    // return false;
  // }
  
  if (!WriteDetour((void *)0x009C0710, CallEndScene_Hook, 1)) {
    return false;
  }
  
  return true;
}

#endif
