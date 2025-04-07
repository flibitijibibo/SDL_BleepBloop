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

#ifdef BLEEPBLOOP_XBL

#include <SDL_BleepBloop.h>

#include <xsapi-c/services_c.h>

#include "BleepBloop_Secrets.h"

static XblContextHandle xblHandle = NULL;
static uint64_t xboxUserId = 0;
static uint64_t refcount = 0;

static void AsyncCompleted(XAsyncBlock* async)
{
	delete async;
}

static XAsyncBlock* CreateAsync()
{
	XAsyncBlock* async = new XAsyncBlock;
	bool gotQueue = SDL_GetGDKTaskQueue(&async->queue);
	SDL_assert(gotQueue);
	async->context = NULL;
	async->callback = AsyncCompleted;
	return async;
}

bool BleepBloop_Init(int argc, char **argv)
{
	HRESULT hr;
	XblInitArgs initArgs;
	XblUserHandle xblUser;

	(void)argc;
	(void)argv;

	if (refcount++ > 0)
	{
		return true;
	}

	SDL_assert(refcount == 1);

	SDL_SetHintWithPriority(
		"SDL_GDK_SERVICE_CONFIGURATION_ID",
		BLEEPBLOOP_SECRET_XBOX_SCID,
		SDL_HINT_OVERRIDE
	);

	if (!SDL_GetGDKDefaultUser(&xblUser))
	{
		refcount -= 1;
		return false;
	}

	hr = XUserGetId(xblUser, &xboxUserId);
	if (FAILED(hr))
	{
		refcount -= 1;
		return false;
	}

	if (!SDL_GetGDKTaskQueue(&initArgs.queue))
	{
		refcount -= 1;
		return false;
	}
	initArgs.scid = BLEEPBLOOP_SECRET_XBOX_SCID;
	hr = XblInitialize(&initArgs);
	if (FAILED(hr))
	{
		refcount -= 1;
		return false;
	}

	hr = XblContextCreateHandle(xblUser, &xblHandle);
	if (FAILED(hr))
	{
		XblCleanupAsync(CreateAsync());
		refcount -= 1;
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
	}

	SDL_assert(refcount == 0);

	XblContextCloseHandle(xblHandle);
	XblCleanupAsync(CreateAsync());
}

void BleepBloop_Update(void)
{
	/* Nothing to do, XBL handles this for us */
}

void BleepBloop_SetAchievement(const char *id)
{
	/* XBL doesn't have this, just use a boolean stat instead */
	BleepBloop_UpdateAchievement(id, 1, 1);
}

void BleepBloop_UpdateAchievement(const char *id, Sint32 current, Sint32 target)
{
	if (refcount > 0)
	{
		int i;
		for (i = 0; i < SDL_arraysize(BLEEPBLOOP_SECRET_ACHIEVEMENT_NAMES); i += 1)
		{
			if (SDL_strcmp(id, BLEEPBLOOP_SECRET_ACHIEVEMENT_NAMES[i]) == 0)
			{
				break;
			}
		}
		if (i < SDL_arraysize(BLEEPBLOOP_SECRET_ACHIEVEMENT_NAMES))
		{
			char idString[4]; /* Arbitrarily 3 digits plus null terminator */
			SDL_snprintf(idString, sizeof(idString), "%d", i + 1);
			XblAchievementsUpdateAchievementAsync(
				xblHandle,
				xboxUserId,
				idString,
				(uint32_t) SDL_floorf(((float) current / (float) target) * 100.0f),
				CreateAsync()
			);
		}
	}
}

#endif /* BLEEPBLOOP_XBL */
