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

//NOTE(adm244): contains code to interface with Scaleform 3.3.89-ish

#ifndef _SCALEFORM_3_3_89_DISHONORED_H_
#define _SCALEFORM_3_3_89_DISHONORED_H_

struct ValueInterface;
struct GFxValue;
struct GFxMovie;
struct FunctionHandler;

//------------- Enumerables -------------//
enum SetVarType {
  SV_Normal,
  SV_Sticky,
  SV_Permanent
};

enum SetArrayType {
  SA_Int,
  SA_Double,
  SA_Float,
  SA_String,
  SA_StringW,
  SA_Value
};

enum ValueTypeControl {
  VTC_ConvertBit = 0x80,
  VTC_ManagedBit = 0x40,
  VTC_TypeMask = VTC_ConvertBit | 0x0F,
};

enum ValueType {
  VT_Undefined = 0x00,
  VT_Null = 0x01,
  VT_Boolean = 0x02,
  VT_Number = 0x03,
  VT_String = 0x04,
  VT_StringW = 0x05,
  VT_Object = 0x06,
  VT_Array = 0x07,
  VT_DisplayObject = 0x08,
  VT_ConvertBoolean = VTC_ConvertBit | VT_Boolean,
  VT_ConvertNumber = VTC_ConvertBit | VT_Number,
  VT_ConvertString = VTC_ConvertBit | VT_String,
  VT_ConvertStringW = VTC_ConvertBit | VT_StringW
};

//------------- Public functions -------------//
#define GFxMovie_CreateString(p, a, b) ((p)->vtable->CreateString(p, a, b))
#define GFxMovie_CreateStringW(p, a, b) ((p)->vtable->CreateStringW(p, a, b))
#define GFxMovie_CreateObject(p, a, b, c, d) ((p)->vtable->CreateObject(p, a, b, c, d))
#define GFxMovie_CreateArray(p, a) ((p)->vtable->CreateArray(p, a))
#define GFxMovie_CreateFunction(p, a, b, c) ((p)->vtable->CreateFunction(p, a, b, c))

#define GFxMovie_SetVariable(p, a, b, c) ((p)->vtable->SetVariable(p, a, b, c))
#define GFxMovie_GetVariable(p, a, b) ((p)->vtable->GetVariable(p, a, b))
#define GFxMovie_SetVariableArray(p, a, b, c, d, e, f) ((p)->vtable->SetVariableArray(p, a, b, c, d, e, f))
#define GFxMovie_SetVariableArraySize(p, a, b, c) ((p)->vtable->SetVariableArraySize(p, a, b, c))
#define GFxMovie_GetVariableArraySize(p, a) ((p)->vtable->GetVariableArraySize(p, a))
#define GFxMovie_GetVariableArray(p, a, b, c, d, e) ((p)->vtable->GetVariableArray(p, a, b, c, d, e))

internal bool GFxValue_IsDisplayObject(GFxValue *);
internal bool GFxValue_Invoke(GFxValue *, GFxValue *, char *, GFxValue *, int);
internal bool GFxValue_GetMember(GFxValue *, char *, GFxValue *);
internal bool GFxValue_SetMember(GFxValue *, char *, GFxValue *);
internal bool GFxValue_PushBack(GFxValue *, GFxValue *);
internal bool GFxValue_PopBack(GFxValue *, GFxValue *);

DECLARE_MEMBER_FUNCTION(void, GFxValue, SetUndefined);
DECLARE_MEMBER_FUNCTION(void, GFxValue, SetNull);
DECLARE_MEMBER_FUNCTION(void, GFxValue, SetBoolean, bool);
DECLARE_MEMBER_FUNCTION(void, GFxValue, SetDouble, r64);
DECLARE_MEMBER_FUNCTION(void, GFxValue, SetStringW, wchar_t *);

MEMBER_FUNCTION(GFxValue, SetUndefined);
MEMBER_FUNCTION(GFxValue, SetNull);
MEMBER_FUNCTION(GFxValue, SetBoolean);
MEMBER_FUNCTION(GFxValue, SetDouble);
MEMBER_FUNCTION(GFxValue, SetStringW);

//------------- Private functions -------------//
DECLARE_MEMBER_FUNCTION(bool, ValueInterface, GetMember, void *, char *, GFxValue *, bool);
DECLARE_MEMBER_FUNCTION(bool, ValueInterface, SetMember, void *, char *, GFxValue *, bool);
DECLARE_MEMBER_FUNCTION(bool, ValueInterface, Invoke, void *, GFxValue *, char *, GFxValue *, int, bool);
DECLARE_MEMBER_FUNCTION(bool, ValueInterface, PushBack, void *, GFxValue *);
DECLARE_MEMBER_FUNCTION(bool, ValueInterface, PopBack, void *, GFxValue *);

MEMBER_FUNCTION(ValueInterface, GetMember);
MEMBER_FUNCTION(ValueInterface, SetMember);
MEMBER_FUNCTION(ValueInterface, Invoke);
MEMBER_FUNCTION(ValueInterface, PushBack);
MEMBER_FUNCTION(ValueInterface, PopBack);

//------------- Virtual functions -------------//
DECLARE_VIRTUAL_FUNCTION(void, GFxMovie, CreateString, GFxValue *, char *);
DECLARE_VIRTUAL_FUNCTION(void, GFxMovie, CreateStringW, GFxValue *, wchar_t *);
DECLARE_VIRTUAL_FUNCTION(void, GFxMovie, CreateObject, GFxValue *, char *, GFxValue *, int);
DECLARE_VIRTUAL_FUNCTION(void, GFxMovie, CreateArray, GFxValue *);
DECLARE_VIRTUAL_FUNCTION(void, GFxMovie, CreateFunction, GFxValue *, FunctionHandler *, void *);

DECLARE_VIRTUAL_FUNCTION(bool, GFxMovie, SetVariable, char *, GFxValue *, SetVarType);
DECLARE_VIRTUAL_FUNCTION(bool, GFxMovie, GetVariable, GFxValue *, char *);
DECLARE_VIRTUAL_FUNCTION(bool, GFxMovie, SetVariableArray, SetArrayType, char *, int, void *, int, SetVarType);
DECLARE_VIRTUAL_FUNCTION(bool, GFxMovie, SetVariableArraySize, char *, int, SetVarType);
DECLARE_VIRTUAL_FUNCTION(uint, GFxMovie, GetVariableArraySize, char *);
DECLARE_VIRTUAL_FUNCTION(bool, GFxMovie, GetVariableArray, SetArrayType, char *, int, void *, int);

//------------- Structures -------------//
struct ValueInterface {
  void *unk00;
  // ...
};

struct GFxValue {
  ValueInterface *valueInterface;
  ValueType type;
  union {
    void *data;
    bool boolean;
    char *str;
    wchar_t *wstr;
    r64 number;
  };
};
assert_size(GFxValue, 0x10);

struct GFxMovieVTable {
  u8 unk00[0x2C-0x00];
  _GFxMovie_CreateString CreateString; // 0x2C
  _GFxMovie_CreateStringW CreateStringW; // 0x30
  _GFxMovie_CreateObject CreateObject; // 0x34
  _GFxMovie_CreateArray CreateArray; // 0x38
  _GFxMovie_CreateFunction CreateFunction; // 0x3C
  _GFxMovie_SetVariable SetVariable; // 0x40
  _GFxMovie_GetVariable GetVariable; // 0x44
  _GFxMovie_SetVariableArray SetVariableArray; // 0x48
  _GFxMovie_SetVariableArraySize SetVariableArraySize; // 0x4C
  _GFxMovie_GetVariableArraySize GetVariableArraySize; // 0x50
  _GFxMovie_GetVariableArray GetVariableArray; // 0x54
  // ...
};
assert_size(GFxMovieVTable, 0x58);

struct GFxMovie {
  GFxMovieVTable *vtable;
  // ...
};

struct FunctionHandler {
  struct Params {
    GFxValue* retValue;
    GFxMovie* movie;
    GFxValue* thisPtr;
    GFxValue* argsWithThisRef;
    GFxValue* args;
    int argsCount;
    void* userData;
  };
  
  virtual ~FunctionHandler() {}
  virtual void Call(Params *params) = 0;
};
assert_size(FunctionHandler, 0x4);

#endif
