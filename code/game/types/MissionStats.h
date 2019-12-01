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

#ifndef _MISSIONSTATS_H_
#define _MISSIONSTATS_H_

struct DisStoryFlagSet;
struct DisSpecialActionFlagSet;
struct DisDarknessManager;
struct StoryFlagSet;

//------------- Enumerables -------------//
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

//------------- Functions -------------//
typedef StoryFlagSet * (THISCALL *_DisStoryFlagSet_GetStoryFlagSet)(DisStoryFlagSet *, DisSpecialActionFlagSet *);
typedef int (THISCALL *_DisDarknessManager_GetChaosTresholdIndex)(DisDarknessManager *);
typedef int * (THISCALL *_DishonoredPlayerPawn_GetStatsValue)(DishonoredPlayerPawn *, int, float *, int *);
typedef void (THISCALL *_ArkProfileSettings_SetMissionStats)(ArkProfileSettings *, int, int, u16, r32 *, int);

internal _DisStoryFlagSet_GetStoryFlagSet DisStoryFlagSet_GetStoryFlagSet = (_DisStoryFlagSet_GetStoryFlagSet)(0x00C04640);
internal _DisDarknessManager_GetChaosTresholdIndex DisDarknessManager_GetChaosTresholdIndex = (_DisDarknessManager_GetChaosTresholdIndex)(0x00C47FF0);
internal _DishonoredPlayerPawn_GetStatsValue DishonoredPlayerPawn_GetStatsValue = (_DishonoredPlayerPawn_GetStatsValue)(0x00AA93C0);
internal _ArkProfileSettings_SetMissionStats ArkProfileSettings_SetMissionStats = (_ArkProfileSettings_SetMissionStats)(0x00938210);

//------------- Structures -------------//
#pragma pack(4)

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
  UArray statsValues; // 0xA0
  UArray statsMaxValues; // 0xAC
  UString bgImageName; // 0xB8
};
assert_size(DisTweaks_MissionStats, 0xC4);

struct DisStoryFlagSet {
  void *vtable;
  u8 unk04[0x38-0x4];
  UArray flagSets; // 0x38
  u32 unk44;
  u32 unk48;
  u32 unk4C;
};
assert_size(DisStoryFlagSet, 0x50);

struct DisSpecialActionFlagSet {
  u32 unk00;
  u32 unk04;
  u32 unk08;
  u32 unk0C;
  u32 unk10;
};
assert_size(DisSpecialActionFlagSet, 0x14);

struct StoryFlagSet {
  u32 unk00;
  u32 unk04;
  DisSpecialActionFlagSet flagSet; // 0x8
};
assert_size(StoryFlagSet, 0x1C);

struct DisSpecialAction {
  UString name;
  DisStoryFlagSet *storyFlagSet; // 0xC
  DisSpecialActionFlagSet flagSet; // 0x10
};
assert_size(DisSpecialAction, 0x24);

struct StatValueTemplate {
  UString statName;
  u8 type1; // 0xC
  u32 checkType1; // 0x10
  u8 type2; // 0x14
  
  //TODO(adm244): are we sure about this one?
  u32 checkType2; // 0x18
};
assert_size(StatValueTemplate, 0x1C);

struct MissionStatEntry {
  u8 type;
  r32 value;
  u32 maxValue;
};
assert_size(MissionStatEntry, 0xC);

struct DisDarknessManager {
  UObject object;
  u32 chaosLevel; // 0x38
  u32 unk3C;
  u32 unk40;
  u32 unk44;
};
assert_size(DisDarknessManager, 0x48);

#pragma pack(pop)

#endif
