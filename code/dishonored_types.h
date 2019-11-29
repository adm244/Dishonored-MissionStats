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

#ifndef _DISHONORED_TYPES_H_
#define _DISHONORED_TYPES_H_

#include "scaleform3.h"

enum MissionStatTypes {
  MissionStat_AlarmsRung = 0x03,
  MissionStat_BodiesFound = 0x04,
  MissionStat_DetectedTimes = 0x0A,
  MissionStat_OverallChaos = 0x1B,
  MissionStat_HostilesKilled = 0x1C,
  MissionStat_CiviliansKilled = 0x1E,
  MissionStat_CoinsFound = 0x21,
  MissionStat_RunesFound = 0x22,
  MissionStat_BoneCharmsFound = 0x23,
  MissionStat_OutsiderShrinesFound = 0x24,
  MissionStat_SokolovPaintingsFound = 0x25,
};

enum DisGFxMoviePlayerPauseMenu_Modes {
  PauseMenuMode_GameOver = 2,
};

#pragma pack(4)

struct UArray {
  void *data;
  u32 length;
  u32 capacity;
};
assert_size(UArray, 0xC);

struct UString {
  wchar_t *text;
  u32 length;
  u32 capacity;
};
assert_size(UString, 0xC);

struct UObject;

struct UClass {
  void *vtable;
  u8 unk04[0x24-0x4];
  UObject *package; // 0x24
  u8 unk28[0x44-0x28];
  UClass *classWithin; // 0x44
  u8 unk48[0x130-0x48];
  UString name; // 0x130
  UObject *defaultObject; // 0x13C
  u8 unk140[0x1B4-0x140];
};
assert_size(UClass, 0x1B4);

struct UObject {
  void *vtable;
  u32 unk04;
  u32 flags; // 0x8
  u32 unk0C;
  u32 unk10;
  u32 unk14;
  u32 unk18;
  u32 unk1C;
  u32 unk20;
  u32 unk24;
  u32 uniqueId; // 0x28
  u32 unk2C;
  UClass *classPtr; // 0x30
  u32 unk34;
};
assert_size(UObject, 0x38);

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

struct MissionStatEntry {
  u8 type;
  r32 value;
  u32 maxValue;
};
assert_size(MissionStatEntry, 0xC);

struct DishonoredPlayerPawn {
  u8 unk0[0xB7C-0x0];
  UArray missionStats; // 0xB7C
  // ...
};

struct DisGFxMoviePlayer;

struct DisGFxMovie {
  UString name;
  u8 unk0C[0x34-0x0C];
  GFxMovie *movie; // 0x34
  u32 unk38;
  u32 unk3C;
  u32 unk40;
  u32 unk44;
  u32 flags; // 0x48
  u8 unk4C[0x5C-0x4C];
  DisGFxMoviePlayer *disMoviePlayer; // 0x5C
  // ...
};

typedef bool (THISCALL *_DisGFxMoviePlayer_Start)(DisGFxMoviePlayer *, void *);

struct DisGFxMoviePlayerVTable {
  u8 unk00[0x124-0x0];
  void *Preload; // 0x124
  _DisGFxMoviePlayer_Start Start; // 0x128
  void *Advance; // 0x12C
  void *unk130;
  void *unk134;
  void *Close; // 0x138
  // ...
};

struct DisGFxMoviePlayer {
  DisGFxMoviePlayerVTable *vtable;
  u8 unk04[0x38-0x04];
  DisGFxMovie *disMovie; // 0x38
  u8 unk3C[0xC4-0x3C];
  u32 flags; // 0xC4
  u8 unkC8[0x180-0xC8];
  UString name; // 0x180
  // ...
};

struct DisGFxMoviePlayerPauseMenu {
  DisGFxMoviePlayer base;
  u8 unk3C[0x1F8-sizeof(DisGFxMoviePlayer)];
  u8 mode; // 0x1F8
  // ...
};
assert_size(DisGFxMoviePlayerPauseMenu, 0x1FC);

struct DisGFxMoviePlayerHUD {
  DisGFxMoviePlayer base;
  // ...
};

struct DisGFxMoviePlayerPowerWheel {
  DisGFxMoviePlayer base;
  // ...
};

struct DisGFxMoviePlayerJournal {
  DisGFxMoviePlayer base;
  // ...
};

struct DisGFxMoviePlayerNote {
  DisGFxMoviePlayer base;
  // ...
};

struct DisGFxMoviePlayerMissionStats {
  DisGFxMoviePlayer base;
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

struct WorldInfo {
  void *vtable;
  u8 unk04[0x410-0x4];
  DishonoredGameInfo *gameInfo; // 0x410
  // ...
};
assert_size(WorldInfo, 0x414);

struct World {
  void *vtable;
  u8 unk04[0x2C0-0x4];
  WorldInfo *worldInfo; // 0x2C0
  // ...
};
assert_size(World, 0x2C4);

struct DisTweaksBase {
  UObject object;
  u32 unk38;
  UArray unk3C;
  u32 unk48;
  u32 unk4C;
  u32 unk50;
  UArray unk54;
  u32 unk60;
  u32 unk64;
  u32 unk68;
  u32 unk6C;
  u32 unk70;
  UArray unk74;
  u32 unk80;
  u32 unk84;
  u32 unk88;
};
assert_size(DisTweaksBase, 0x8C);

struct DisTweaks_MissionStats {
  DisTweaksBase base;
  u32 missionNumber; // 0x8C
  u32 dlcNumber; // 0x90
  UArray specialActions; // 0x94
  UArray statValues; // 0xA0
  UArray statsMaxValues; // 0xAC
  UString bgImageName; // 0xB8
};
assert_size(DisTweaks_MissionStats, 0xC4);

/*struct UIObject {
  UString name;
  u32 unk0C[15];
  u32 flags; // 0x48
  u32 unk4C[4];
  void *unk20; // 0x5C
  // ...
};

struct Unk20_1 {
  void *vtable;
  u32 unk04;
  u32 unk08;
  u32 unk0C;
  UArray uiObjects; // 0x10
  // ...
};

struct NoteContents {
  void *vtable;
  UString title;
  UString text;
};
assert_size(NoteContents, 0x1C);*/

#pragma pack(pop)

internal DishonoredEngine **dishonoredEngine = (DishonoredEngine **)0x0143B20C;
internal World **world = (World **)(0x0143D878);

typedef DishonoredGameInfo * (THISCALL *_World_GetGameInfo)(World *);
typedef DisGlobalUIManager * (CDECL _GetGlobalUIManager)();
typedef DisGFxMoviePlayerHUD * (STDCALL _GetHUD)();
typedef void (THISCALL *_DisGFxMoviePlayerHUD_ShowLocationDiscovery)(DisGFxMoviePlayerHUD *, UString *, int);
typedef void (THISCALL *_DisGFxMoviePlayerHUD_ShowGameMessage)(DisGFxMoviePlayerHUD *, UString *, r32);
//typedef void (THISCALL *_Unk20_ShowNote)(void *unk20, NoteContents *contents, int unk03);
typedef int (THISCALL *_DisGFxMoviePlayerMissionStats_Show)(DisGFxMoviePlayerMissionStats *, DisTweaks_MissionStats *, int);
typedef UObject * (THISCALL *_UClass_GetDefaultObject)(UClass *, int);
typedef UObject * (CDECL _NewObject)(UClass *classPtr, UObject *package, int uniqueId, int, int, int, UObject *defaultObject, int, int, int);

internal _World_GetGameInfo World_GetGameInfo = (_World_GetGameInfo)(0x007816E0);
internal _GetGlobalUIManager *GetGlobalUIManager = (_GetGlobalUIManager *)(0x00BBF730);
internal _GetHUD *GetHUD = (_GetHUD *)0x00BBF760;
internal _DisGFxMoviePlayerHUD_ShowLocationDiscovery DisGFxMoviePlayerHUD_ShowLocationDiscovery = (_DisGFxMoviePlayerHUD_ShowLocationDiscovery)0x00B94D60;
internal _DisGFxMoviePlayerHUD_ShowGameMessage DisGFxMoviePlayerHUD_ShowGameMessage = (_DisGFxMoviePlayerHUD_ShowGameMessage)0x00BAFFA0;
//internal _Unk20_ShowNote Unk20_ShowNote = (_Unk20_ShowNote)0x00BCC2B0;
internal _DisGFxMoviePlayerMissionStats_Show DisGFxMoviePlayerMissionStats_Show = (_DisGFxMoviePlayerMissionStats_Show)(0x00BD3A60);
internal _NewObject *NewObject = (_NewObject *)(0x00494C80);
internal _UClass_GetDefaultObject UClass_GetDefaultObject = (_UClass_GetDefaultObject)(0x004967F0);

#endif
