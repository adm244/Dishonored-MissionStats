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

#ifndef _DISHONORED_H_
#define _DISHONORED_H_

struct DishonoredEngine;
struct DishonoredPlayerPawn;
struct DishonoredPlayerController;
struct DisGlobalUIManager;
struct DisGFxMoviePlayerHUD;
struct DisGFxMoviePlayerPowerWheel;
struct DisGFxMoviePlayerJournal;
struct DisGFxMoviePlayerNote;
struct DisGFxMoviePlayerPauseMenu;
struct DisGFxMoviePlayerMissionStats;
struct DisStoryFlagSet;
struct DisSpecialActionFlagSet;
struct DisDarknessManager;
struct ArkProfileSettings;

//------------- Static pointers -------------//
internal DishonoredEngine **dishonoredEngine = (DishonoredEngine **)(0x0143B20C);
internal DishonoredPlayerController **playerController = (DishonoredPlayerController **)(0x01452DD4);
internal DishonoredPlayerPawn **playerPawn = (DishonoredPlayerPawn **)(0x01452DE8);

//------------- Functions -------------//
typedef DisGlobalUIManager * (CDECL _GetGlobalUIManager)();
typedef DisGFxMoviePlayerHUD * (STDCALL _GetHUD)();
typedef bool (CDECL _IsDLC06)();
typedef bool (CDECL _IsDLC07)();
typedef bool (THISCALL *_DishonoredPlayerPawn_HasCompleteAction)(DishonoredPlayerPawn *, DisStoryFlagSet *, DisSpecialActionFlagSet *);
typedef ArkProfileSettings * (THISCALL *_DishonoredPlayerController_GetProfileSettings)(DishonoredPlayerController *);

internal _GetGlobalUIManager *GetGlobalUIManager = (_GetGlobalUIManager *)(0x00BBF730);
internal _GetHUD *GetHUD = (_GetHUD *)0x00BBF760;
internal _IsDLC06 *IsDLC06 = (_IsDLC06 *)(0x00BBF1D0);
internal _IsDLC07 *IsDLC07 = (_IsDLC07 *)(0x00BBF270);
internal _DishonoredPlayerPawn_HasCompleteAction DishonoredPlayerPawn_HasCompleteAction = (_DishonoredPlayerPawn_HasCompleteAction)(0x00AB1100);

//------------- Structures -------------//
#pragma pack(4)

struct DishonoredEngineUnk01 {
  UArray unk00;
  UArray unk0C;
  u32 unk18;
  u32 unk1C;
  u32 unk20;
  u32 unk24;
  UArray unk28;
  u32 unk34;
  u32 unk38;
  u32 unk3C;
  u32 unk40;
  u32 unk44;
  UArray unk48;
  UArray unk54;
  u32 unk60;
  u32 unk64;
  UArray unk68;
  UString levelName;
  u32 missionNumber;
  // ???
};

struct DishonoredEngine {
  void *vtable;
  u8 unk04[0x7BC-0x04];
  DishonoredEngineUnk01 *unk01; // 0x7BC
  u8 unk7C0[0x804-0x7C0];
};
assert_size(DishonoredEngine, 0x804);

struct DishonoredPlayerPawn {
  u8 unk0[0xA94-0x0];
  DisDarknessManager *darknessManager; // 0xA94
  u8 unkA98[0xB7C-0xA98];
  UArray missionStats; // 0xB7C
  u8 unkB88[0x1250-0xB88];
};
assert_size(DishonoredPlayerPawn, 0x1250);

struct ArkProfileSettings {
  void *vtable;
  // ...
};

struct UIDataStore_OnlinePlayerData {
  void *vtable;
  // ...
};

struct DishonoredPlayerControllerVTable {
  u8 unk00[0x590-0x0];
  _DishonoredPlayerController_GetProfileSettings GetProfileSettings; // 0x590
  // ...
};

struct DishonoredPlayerController {
  DishonoredPlayerControllerVTable *vtable;
  u8 unk04[0x49C-0x4];
  UIDataStore_OnlinePlayerData *onlinePlayerData; // 0x49C
  // ...
};

struct DisGlobalUIManager {
  void *vtable;
  u8 unk04[0x2DC-0x4];
  DisGFxMoviePlayerHUD *hud; // 0x2DC
  DisGFxMoviePlayerPowerWheel *powerWheel; // 0x2E0
  DisGFxMoviePlayerJournal *journal; // 0x2E4
  DisGFxMoviePlayerNote *note; // 0x2E8
  DisGFxMoviePlayerPauseMenu *pauseMenu; // 0x2EC
  void *unk2F0;
  DisGFxMoviePlayerMissionStats *missionStats; // 0x2F4
  // ...
};
assert_size(DisGlobalUIManager, 0x2F8);

struct DishonoredGameInfo {
  void *vtable;
  u8 unk04[0x41C-0x4];
  DisGlobalUIManager *globalUIManager; // 0x41C
  u8 unk420[0x14E0-0x420];
  UString missionNames[21]; // 0x14E0
  // ...
};
assert_size(DishonoredGameInfo, 0x15DC);

#pragma pack(pop)

#endif
