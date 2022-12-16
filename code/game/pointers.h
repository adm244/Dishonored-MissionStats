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

#ifndef _POINTERS_H_
#define _POINTERS_H_

//------------- Macro -------------//
// internal void (* ptr_<name>_addr) = (void *)(<addr>)
#define DEFINE_STATIC_POINTER(name, addr) internal void (* ptr_##name##_addr) = (void *)(addr)
// internal <type> (*const <name>) = (<type> *)ptr_<name>_addr;
#define STATIC_POINTER(type, name) internal type (*const name) = (type *)ptr_##name##_addr
// internal <type> (*const *const <name>) = (<type> **)ptr_<name>_addr
#define STATIC_POINTER_POINTER(type, name) internal type (*const *const name) = (type **)ptr_##name##_addr

// internal void (* ptr_<obj>_<func>_addr) = (void *)(<addr>)
#define DEFINE_MEMBER_FUNCTION(obj, func, addr) internal void (* ptr_##obj##_##func##_addr) = (void *)(addr)
// typedef <ret> (THISCALL * _<obj>_<func>)(obj *, ...)
#define DECLARE_MEMBER_FUNCTION(ret, obj, func, ...) typedef ret (THISCALL * _##obj##_##func)(obj *, __VA_ARGS__)
// internal _<obj>_<func> <obj>_<func> = (_<obj>_<func>)(ptr_<obj>_<func>_addr)
#define MEMBER_FUNCTION(obj, func) internal _##obj##_##func obj##_##func = (_##obj##_##func)(ptr_##obj##_##func##_addr)

// typedef <ret> (THISCALL * _<obj>_<func>)(obj *, ...)
#define DECLARE_VIRTUAL_FUNCTION(ret, obj, func, ...) DECLARE_MEMBER_FUNCTION(ret, obj, func, __VA_ARGS__)
// _<obj>_<func> <func>
#define VIRTUAL_FUNCTION(obj, func) _##obj##_##func func

// internal void (* ptr_<func>_addr) = (void *)(<addr>)
#define DEFINE_CDECL_FUNCTION(func, addr) internal void (* ptr_##func##_addr) = (void *)(addr)
// typedef <ret> (CDECL * _<func>)(...)
#define DECLARE_CDECL_FUNCTION(ret, func, ...) typedef ret (CDECL * _##func)(__VA_ARGS__)
// internal _<func> <func> = (_<func>)(ptr_<func>_addr)
#define CDECL_FUNCTION(func) internal _##func func = (_##func)(ptr_##func##_addr)

// internal void (* ptr_<func>_addr) = (void *)(<addr>)
#define DEFINE_STDCALL_FUNCTION(func, addr) DEFINE_CDECL_FUNCTION(func, addr)
// typedef <ret> (STDCALL * _<func>)(...)
#define DECLARE_STDCALL_FUNCTION(ret, func, ...) typedef ret (STDCALL * _##func)(__VA_ARGS__)
// internal _<func> <func> = (_<func>)(ptr_<func>_addr)
#define STDCALL_FUNCTION(func) CDECL_FUNCTION(func)

//------------- Unreal.h -------------//
DEFINE_STATIC_POINTER(world, 0x01449888);

DEFINE_CDECL_FUNCTION(NewObject, 0x004955a0);
DEFINE_MEMBER_FUNCTION(UClass, GetDefaultObject, 0x00497130);
DEFINE_MEMBER_FUNCTION(World, GetGameInfo, 0x00781020);

//------------- Dishonored.h -------------//
DEFINE_STATIC_POINTER(dishonoredEngine, 0x0144721c);
DEFINE_STATIC_POINTER(playerController, 0x0145f614);
DEFINE_STATIC_POINTER(playerPawn, 0x0145f628);

DEFINE_CDECL_FUNCTION(GetGlobalUIManager, 0x00bbf700);
DEFINE_CDECL_FUNCTION(IsDLC06, 0x00bbf110);
DEFINE_CDECL_FUNCTION(IsDLC07, 0x00bbf1b0);
DEFINE_STDCALL_FUNCTION(GetHUD, 0x00bbf730);
DEFINE_MEMBER_FUNCTION(DishonoredPlayerPawn, HasCompleteAction, 0x00ab1370);

//------------- MissionStats.h -------------//
DEFINE_MEMBER_FUNCTION(DisStoryFlagSet, GetStoryFlagSet, 0x00c049f0);
DEFINE_MEMBER_FUNCTION(DisDarknessManager, GetChaosTresholdIndex, 0x00c487d0);
DEFINE_MEMBER_FUNCTION(DishonoredPlayerPawn, GetStatsValue, 0x00aa9700);
DEFINE_MEMBER_FUNCTION(ArkProfileSettings, SetMissionStats, 0x009376d0);

//------------- MoviePlayer.h -------------//
DEFINE_MEMBER_FUNCTION(DisGFxMoviePlayerHUD, ShowLocationDiscovery, 0x00b95160);
DEFINE_MEMBER_FUNCTION(DisGFxMoviePlayerHUD, ShowGameMessage, 0x00bb0140);
DEFINE_MEMBER_FUNCTION(DisGFxMoviePlayerMissionStats, Show, 0x00bd3b20);
//DEFINE_MEMBER_FUNCTION(Unk20, ShowNote, 0x00bcc390);

//------------- Scaleform3.h -------------//
DEFINE_MEMBER_FUNCTION(GFxValue, SetUndefined, 0x0097aac0);
DEFINE_MEMBER_FUNCTION(GFxValue, SetNull, 0x0097aaf0);
DEFINE_MEMBER_FUNCTION(GFxValue, SetBoolean, 0x0097ab20);
DEFINE_MEMBER_FUNCTION(GFxValue, SetDouble, 0x0097ab70);
DEFINE_MEMBER_FUNCTION(GFxValue, SetStringW, 0x0097abb0);

DEFINE_MEMBER_FUNCTION(ValueInterface, GetMember, 0x00da9470);
DEFINE_MEMBER_FUNCTION(ValueInterface, SetMember, 0x00da6290);
DEFINE_MEMBER_FUNCTION(ValueInterface, Invoke, 0x00da8bd0);
DEFINE_MEMBER_FUNCTION(ValueInterface, PushBack, 0x00da63c0);
DEFINE_MEMBER_FUNCTION(ValueInterface, PopBack, 0x00da7260);

//------------- MissionStatsButton.cpp -------------//
DEFINE_STATIC_POINTER(detour_showpausemenu, 0x00bccd80);
DEFINE_STATIC_POINTER(detour_setmissionstats, 0x00bd3ca0);
DEFINE_STATIC_POINTER(detour_shouldskipmustdoflag, 0x00bd43b1);
DEFINE_STATIC_POINTER(detour_distweaks_missionstats_ctor, 0x00a0d930);
DEFINE_STATIC_POINTER(detour_distweaks_missionstats_dtor, 0x00a09ce0);

DEFINE_STATIC_POINTER(hook_showpausemenu_ret, 0x00bccd85);
DEFINE_STATIC_POINTER(hook_setmissionstats_ret, 0x00bd3ca6);
DEFINE_STATIC_POINTER(hook_shouldskipmustdoflag_ret, 0x00bd43b7);
DEFINE_STATIC_POINTER(hook_distweaks_missionstats_ctor_ret, 0x00a0d937);
DEFINE_STATIC_POINTER(hook_distweaks_missionstats_dtor_ret, 0x00a09ce5);

//------------- MissionStatsGuard.cpp -------------//
DEFINE_STATIC_POINTER(detour_modifystatvariable, 0x00aa97e0);
DEFINE_STATIC_POINTER(hook_modifystatvariable_ret, 0x00aa97e5);

#endif
