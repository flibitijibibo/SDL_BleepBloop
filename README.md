This is **SDL_BleepBloop**, a helper library to handle basic achievements for various online platforms.

![I have so many achievables!](/doc/bleepbloop.png)
<p align="center"><sup><sub><a href="https://www.youtube.com/watch?v=mdZC5LJswFE"><i>Image Source</i></a></sub></sup></p>

License
-------
SDL_BleepBloop is released under the zlib license. See [`LICENSE`](/LICENSE) for details.

About
-----
Achievements are generally very simple - _programming_ achievements is generally not simple, especially if it's the only real "online" feature you have.

SDL_BleepBloop handles exactly three things:

- Logging in
- Logging out
- Updating achievements

If that's all you need, this library is for you!

The following backends are supported:

- Steam (`-DBLEEPBLOOP_STEAM`)
- Epic Online Services (`-DBLEEPBLOOP_EPIC`, EGS only)
- Xbox Live (`-DBLEEPBLOOP_XBL`)
- PlayStation Network (`-DBLEEPBLOOP_PSN`, requires NDA access)

Dependencies
------------
Aside from the online service being targeted, SDL_BleepBloop solely depends on SDL 3.2.0 or newer.

SDL_BleepBloop never explicitly uses the C runtime; however, some backends may be C++ only and therefore require the C++ runtime libraries.

Found an issue?
---------------
Issues and patches can be reported via GitHub:

https://github.com/flibitijibibo/SDL_BleepBloop/issues
