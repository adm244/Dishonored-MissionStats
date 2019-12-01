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

//------------- Macros -------------//
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
DEFINE_STATIC_POINTER(world, 0x0143D878);

DEFINE_CDECL_FUNCTION(NewObject, 0x00494C80);
DEFINE_MEMBER_FUNCTION(UClass, GetDefaultObject, 0x004967F0);
DEFINE_MEMBER_FUNCTION(World, GetGameInfo, 0x007816E0);

//------------- Dishonored.h -------------//
DEFINE_STATIC_POINTER(dishonoredEngine, 0x0143B20C);
DEFINE_STATIC_POINTER(playerController, 0x01452DD4);
DEFINE_STATIC_POINTER(playerPawn, 0x01452DE8);

DEFINE_CDECL_FUNCTION(GetGlobalUIManager, 0x00BBF730);
DEFINE_CDECL_FUNCTION(IsDLC06, 0x00BBF1D0);
DEFINE_CDECL_FUNCTION(IsDLC07, 0x00BBF270);
DEFINE_STDCALL_FUNCTION(GetHUD, 0x00BBF760);
DEFINE_MEMBER_FUNCTION(DishonoredPlayerPawn, HasCompleteAction, 0x00AB1100);

//------------- MissionStats.h -------------//
DEFINE_MEMBER_FUNCTION(DisStoryFlagSet, GetStoryFlagSet, 0x00C04640);
DEFINE_MEMBER_FUNCTION(DisDarknessManager, GetChaosTresholdIndex, 0x00C47FF0);
DEFINE_MEMBER_FUNCTION(DishonoredPlayerPawn, GetStatsValue, 0x00AA93C0);
DEFINE_MEMBER_FUNCTION(ArkProfileSettings, SetMissionStats, 0x00938210);

//------------- MoviePlayer.h -------------//
DEFINE_MEMBER_FUNCTION(DisGFxMoviePlayerHUD, ShowLocationDiscovery, 0x00B94D60);
DEFINE_MEMBER_FUNCTION(DisGFxMoviePlayerHUD, ShowGameMessage, 0x00BAFFA0);
DEFINE_MEMBER_FUNCTION(DisGFxMoviePlayerMissionStats, Show, 0x00BD3A60);
//DEFINE_MEMBER_FUNCTION(Unk20, ShowNote, 0x00BCC2B0);

//------------- Scaleform3.h -------------//
DEFINE_MEMBER_FUNCTION(GFxValue, SetUndefined, 0x0097B650);
DEFINE_MEMBER_FUNCTION(GFxValue, SetNull, 0x0097B680);
DEFINE_MEMBER_FUNCTION(GFxValue, SetBoolean, 0x0097B6B0);
DEFINE_MEMBER_FUNCTION(GFxValue, SetDouble, 0x0097B700);
DEFINE_MEMBER_FUNCTION(GFxValue, SetStringW, 0x0097B740);

DEFINE_MEMBER_FUNCTION(ValueInterface, GetMember, 0x00DA8AD0);
DEFINE_MEMBER_FUNCTION(ValueInterface, SetMember, 0x00DA58F0);
DEFINE_MEMBER_FUNCTION(ValueInterface, Invoke, 0x00DA8230);
DEFINE_MEMBER_FUNCTION(ValueInterface, PushBack, 0x00DA5A20);
DEFINE_MEMBER_FUNCTION(ValueInterface, PopBack, 0x00DA68C0);

//------------- MissionStatsButton.cpp -------------//
DEFINE_STATIC_POINTER(detour_showpausemenu, 0x00BCCCA0);
DEFINE_STATIC_POINTER(detour_distweaks_missionstats_ctor, 0x00A0DD50);

DEFINE_STATIC_POINTER(hook_showpausemenu_ret, 0x00BCCCA5);
DEFINE_STATIC_POINTER(hook_distweaks_missionstats_ctor_ret, 0x00A0DD57);

//------------- MissionStatsGuard.cpp -------------//
DEFINE_STATIC_POINTER(detour_modifystatvariable, 0x00AA94A0);
DEFINE_STATIC_POINTER(hook_modifystatvariable_ret, 0x00AA94A5);

#endif
