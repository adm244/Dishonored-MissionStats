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

#ifndef _SCALEFORM_3_3_89_DISHONORED_CPP_
#define _SCALEFORM_3_3_89_DISHONORED_CPP_

#include "Scaleform3.h"

internal void GFxMovie_CreateFunction(GFxMovie *movie, GFxValue *returnValue, FunctionHandler *func, void *userData)
{
  func->refCount = 0;
  movie->vtable->CreateFunction(movie, returnValue, func, userData);
}

internal bool GFxValue_IsDisplayObject(GFxValue *value)
{
  return (value->type & VT_DisplayObject) ? true : false;
}

//FIX(adm244): swap result and name args
internal bool GFxValue_Invoke(GFxValue *ptr, GFxValue *result, char *name, GFxValue *args, int argsCount)
{
  bool isDisplayObject = GFxValue_IsDisplayObject(ptr);
  return ValueInterface_Invoke(ptr->valueInterface, ptr->data, result, name, args, argsCount, isDisplayObject);
}

internal bool GFxValue_GetMember(GFxValue *ptr, char *name, GFxValue *dest)
{
  bool isDisplayObject = GFxValue_IsDisplayObject(ptr);
  return ValueInterface_GetMember(ptr->valueInterface, ptr->data, name, dest, isDisplayObject);
}

internal bool GFxValue_SetMember(GFxValue *ptr, char *name, GFxValue *src)
{
  bool isDisplayObject = GFxValue_IsDisplayObject(ptr);
  return ValueInterface_SetMember(ptr->valueInterface, ptr->data, name, src, isDisplayObject);
}

internal bool GFxValue_PushBack(GFxValue *ptr, GFxValue *value)
{
  return ValueInterface_PushBack(ptr->valueInterface, ptr->data, value);
}

internal bool GFxValue_PopBack(GFxValue *ptr, GFxValue *value)
{
  return ValueInterface_PopBack(ptr->valueInterface, ptr->data, value);
}

#endif
