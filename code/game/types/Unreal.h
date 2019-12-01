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

#ifndef _UNREAL_H_
#define _UNREAL_H_

struct UClass;
struct UObject;
struct World;
struct DishonoredGameInfo;

//------------- Static pointers -------------//
internal World **world = (World **)(0x0143D878);

//------------- Functions -------------//
typedef UObject * (CDECL _NewObject)(UClass *classPtr, UObject *package, int uniqueId, int, int, int, UObject *defaultObject, int, int, int);
typedef UObject * (THISCALL *_UClass_GetDefaultObject)(UClass *, int);
typedef DishonoredGameInfo * (THISCALL *_World_GetGameInfo)(World *);

internal _NewObject *NewObject = (_NewObject *)(0x00494C80);
internal _UClass_GetDefaultObject UClass_GetDefaultObject = (_UClass_GetDefaultObject)(0x004967F0);
internal _World_GetGameInfo World_GetGameInfo = (_World_GetGameInfo)(0x007816E0);

//------------- Structures -------------//
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

#pragma pack(pop)

#endif
