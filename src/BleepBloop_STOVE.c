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

#ifdef BLEEPBLOOP_STOVE

#include <SDL_BleepBloop.h>
#include <StovePCSDK.h>

#include "BleepBloop_Secrets.h"

static uint64_t refcount = 0;

static void OnError(const StovePCError error)
{
	SDL_Log("STOVE Error Code %d: %s", error.result, error.message);
}

static void OnInitComplete()
{
	SDL_Log("STOVE OnInitComplete");
}

static void OnOwnership(int size, StovePCOwnership* ownership)
{
	/* Hypothetically we could do something here, but let's be nice! */
}

static void OnSetStat(const StovePCStatValue statValue)
{
	SDL_Log("STOVE OnSetStat");
}

static void OnShutdown(const StovePCShutdown shutdown)
{
	SDL_Log("STOVE OnShutdown");
}

bool BleepBloop_Init(int argc, char **argv)
{
	StovePCConfig config;
	StovePCCallback callbacks;
	StovePCResult result;

	if (refcount++ > 0)
	{
		return true;
	}

	SDL_assert(refcount == 1);

	config.env = "live";
	config.appKey = BLEEPBLOOP_SECRET_STOVE_APP_KEY;
	config.secretKey = BLEEPBLOOP_SECRET_STOVE_APP_SECRET;
	config.gameId = BLEEPBLOOP_SECRET_STOVE_APP_ID;
	config.logLevel = STOVE_PC_LOG_LEVEL_ERROR;
	config.logPath = NULL;

	SDL_zero(callbacks);
	callbacks.OnError = OnError;
	callbacks.OnInitComplete = OnInitComplete;
	callbacks.OnOwnership = OnOwnership;
	callbacks.OnSetStat = OnSetStat;
	callbacks.OnShutdown = OnShutdown;

	result = StovePC_Init(&config, &callbacks);
	if (result != STOVE_PC_NO_ERROR)
	{
		SDL_Log("STOVE failed to initialize: %d", result);
		return false;
	}

	return true;
}

void BleepBloop_Quit(void)
{
	if (refcount > 0)
	{
		if (--refcount > 0)
		{
			return;
		}

		SDL_assert(refcount == 0);

		StovePC_UnInit();
	}
}

void BleepBloop_Update(void)
{
	if (refcount > 0)
	{
		StovePC_RunCallback();
	}
}

void BleepBloop_SetAchievement(const char *id)
{
	if (refcount > 0)
	{
		StovePC_SetStat(id, 1);
	}
}

void BleepBloop_UpdateAchievement(const char *id, Sint32 current, Sint32 target)
{
	if (refcount > 0)
	{
		/* target should be set in the admin panel */
		StovePC_SetStat(id, current);
	}
}

#endif /* BLEEPBLOOP_STOVE */
