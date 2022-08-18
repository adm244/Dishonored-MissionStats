# Dishonored MissionStats
A native mod for **Dishonored** that adds "**Statistics**" button in pause menu, so it is possible to check statistics **mid-mission**.

![Screenshot](https://staticdelivery.nexusmods.com/mods/802/images/17/17-1575764585-1536986851.png)

Additionally, it monitors whether certain conditions are met, such as:

* alarm is raised (only once);
* body is found (only once);
* player is detected (only once);
* hostile or civilian is killed (only once);
* coins picked up;

and displays an in-game message.

![Screenshot](https://staticdelivery.nexusmods.com/mods/802/images/17/17-1575764636-1730046211.png)

Each of these messages can be turned on and off in a config file, as well as statistics button itself.

## Translations
This mod supports multiple languages by loading text from a config file in UTF-8 encoding.

![Screenshot](https://staticdelivery.nexusmods.com/mods/802/images/19/19-1576647033-548186625.png)

## Compilation
Compilation is done as a **single unit**, just compile **main.cpp** and it should work.
For more details see `tools/build.bat` file.

## Installation
Copy "dinput8.dll" and "NativeMods" folder into "Dishonored\Binaries\Win32".

## Supported versions
Works on the latest version that is prepared for DLC **The Knife of Dunwall** and DLC **The Brigmore Witches**.
These DLC's are **NOT** required though.

## Notes
* If you're using GOG version do not launch game through Galaxy client (it's a known issue)
* DLL injector is **NOT** included in this repository
* No fool-proof checks, make sure you know what you're doing

## License
This is free and unencumbered software released into the **public domain**.

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
