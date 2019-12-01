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

#ifndef _MOVIEPLAYER_H_
#define _MOVIEPLAYER_H_

struct DisGFxMoviePlayer;
struct DisGFxMoviePlayerHUD;
struct DisGFxMoviePlayerMissionStats;
struct DisTweaks_MissionStats;

//------------- Enumerables -------------//
enum DisGFxMoviePlayerPauseMenu_Modes {
  PauseMenuMode_GameOver = 2,
};

//------------- Functions -------------//
DECLARE_MEMBER_FUNCTION(void, DisGFxMoviePlayerHUD, ShowLocationDiscovery, UString *, int);
DECLARE_MEMBER_FUNCTION(void, DisGFxMoviePlayerHUD, ShowGameMessage, UString *, r32);
DECLARE_MEMBER_FUNCTION(int, DisGFxMoviePlayerMissionStats, Show, DisTweaks_MissionStats *, int);
//DECLARE_MEMBER_FUNCTION(void, Unk20, ShowNote, NoteContents *, int);

MEMBER_FUNCTION(DisGFxMoviePlayerHUD, ShowLocationDiscovery);
MEMBER_FUNCTION(DisGFxMoviePlayerHUD, ShowGameMessage);
MEMBER_FUNCTION(DisGFxMoviePlayerMissionStats, Show);
//MEMBER_FUNCTION(Unk20, ShowNote);

//------------- Virtual functions -------------//
#define DisGFxMoviePlayer_Start(p, a) ((p)->vtable->Start(p, a))
DECLARE_VIRTUAL_FUNCTION(bool, DisGFxMoviePlayer, Start, void *);

//------------- Structures -------------//
#pragma pack(4)

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

struct DisGFxMoviePlayerVTable {
  u8 unk00[0x124-0x0];
  void *Preload; // 0x124
  VIRTUAL_FUNCTION(DisGFxMoviePlayer, Start); // 0x128
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

#pragma pack(pop)

#endif
