/* SDL_BleepBloop - Helper Library for Basic Online Achievements
 *
 * Copyright (c) 2022-2025 Ethan Lee
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software in a
 * product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Ethan "flibitijibibo" Lee <flibitijibibo@flibitijibibo.com>
 *
 */

#ifndef SDL_BLEEPBLOOP_H
#define SDL_BLEEPBLOOP_H

#ifdef _WIN32
#define BLEEPBLOOPAPI __declspec(dllexport)
#define BLEEPBLOOPCALL __cdecl
#else
#define BLEEPBLOOPAPI
#define BLEEPBLOOPCALL
#endif

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

BLEEPBLOOPAPI bool BleepBloop_Init(int argc, char **argv);

BLEEPBLOOPAPI void BleepBloop_Quit(void);

BLEEPBLOOPAPI void BleepBloop_Update(void);

BLEEPBLOOPAPI void BleepBloop_SetAchievement(const char *id);

BLEEPBLOOPAPI void BleepBloop_UpdateAchievement(const char *id, Sint32 current, Sint32 target);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SDL_BLEEPBLOOP_H */
