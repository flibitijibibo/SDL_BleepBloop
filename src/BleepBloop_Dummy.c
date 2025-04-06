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

#ifdef BLEEPBLOOP_DUMMY

#include <SDL_BleepBloop.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool BleepBloop_Init(int argc, char **argv)
{
	SDL_Log("BleepBloop_Init");
	return true;
}

void BleepBloop_Quit(void)
{
	SDL_Log("BleepBloop_Quit");
}

void BleepBloop_Update(void)
{
	/* Let's _not_ spam the log every frame... */
}

void BleepBloop_SetAchievement(const char *id)
{
	SDL_Log("BleepBloop_SetAchievement: %s", id);
}

void BleepBloop_UpdateAchievement(const char *id, Sint32 current, Sint32 target)
{
	SDL_Log("BleepBloop_UpdateAchievement: %s (%d / %d)", id, current, target);
}

#endif /* BLEEPBLOOP_DUMMY */
