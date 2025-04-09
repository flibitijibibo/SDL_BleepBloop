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

#ifdef BLEEPBLOOP_STEAM

#include <SDL_BleepBloop.h>

/* Steamworks interface versions */

#define BLEEPBLOOP_STEAMCLIENT "SteamClient017"
#define BLEEPBLOOP_STEAMUSERSTATS "STEAMUSERSTATS_INTERFACE_VERSION011"

/* Shared object file name */

#if defined(_WIN32)
#define STEAM_LIBRARY "steam_api.dll"
#elif defined(__APPLE__)
#define STEAM_LIBRARY "libsteam_api.dylib"
#elif defined(__linux__)
#define STEAM_LIBRARY "libsteam_api.so"
#else
#error STEAM_LIBRARY: Unrecognized platform!
#endif

/* DLL, Entry Points */

static SDL_SharedObject *steam_api = NULL;
static uint64_t refcount = 0;

struct ISteamClient;
struct ISteamUserStats;
static struct ISteamUserStats* steamUserStats = NULL;

static bool (*SteamAPI_Init)(void);
static void (*SteamAPI_Shutdown)(void);
static void (*SteamAPI_RunCallbacks)(void);
static bool (*SteamAPI_ISteamUserStats_StoreStats)(struct ISteamUserStats*);
static bool (*SteamAPI_ISteamUserStats_SetAchievement)(struct ISteamUserStats*, const char*);
static bool (*SteamAPI_ISteamUserStats_SetStatInt32)(struct ISteamUserStats*, const char*, Sint32);

bool BleepBloop_Init(int argc, char **argv)
{
	if (refcount++ > 0)
	{
		return true;
	}

	SDL_assert(refcount == 1);

	steam_api = SDL_LoadObject(STEAM_LIBRARY);
	if (steam_api == NULL)
	{
		refcount -= 1;
		return false;
	}

	SteamAPI_Init = (bool (*)(void)) SDL_LoadFunction(steam_api, "SteamAPI_Init");
	SteamAPI_Shutdown = (void (*)(void)) SDL_LoadFunction(steam_api, "SteamAPI_Shutdown");
	SteamAPI_RunCallbacks = (void (*)(void)) SDL_LoadFunction(steam_api, "SteamAPI_RunCallbacks");
	SteamAPI_ISteamUserStats_StoreStats = (bool (*)(struct ISteamUserStats*)) SDL_LoadFunction(steam_api, "SteamAPI_ISteamUserStats_StoreStats");
	SteamAPI_ISteamUserStats_SetAchievement = (bool (*)(struct ISteamUserStats*, const char*)) SDL_LoadFunction(steam_api, "SteamAPI_ISteamUserStats_SetAchievement");
	SteamAPI_ISteamUserStats_SetStatInt32 = (bool (*)(struct ISteamUserStats*, const char*, Sint32)) SDL_LoadFunction(steam_api, "SteamAPI_ISteamUserStats_SetStatInt32");

	struct ISteamClient* (*SteamInternal_CreateInterface)(
		const char*
	) = (struct ISteamClient* (*)(
		const char*
	)) SDL_LoadFunction(steam_api, "SteamInternal_CreateInterface");
	Sint32 (*SteamAPI_GetHSteamUser)(void) = (Sint32 (*)(void)) SDL_LoadFunction(steam_api, "SteamAPI_GetHSteamUser");
	Sint32 (*SteamAPI_GetHSteamPipe)(void) = (Sint32 (*)(void)) SDL_LoadFunction(steam_api, "SteamAPI_GetHSteamUser");
	struct ISteamUserStats* (*SteamAPI_ISteamClient_GetISteamUserStats)(
		struct ISteamClient*,
		int32_t,
		int32_t,
		const char*
	) = (struct ISteamUserStats* (*)(
		struct ISteamClient*,
		int32_t,
		int32_t,
		const char*
	)) SDL_LoadFunction(steam_api, "SteamAPI_ISteamClient_GetISteamUserStats");
	bool (*SteamAPI_ISteamUserStats_RequestCurrentStats)(
		struct ISteamUserStats*
	) = (bool (*)(
		struct ISteamUserStats*
	)) SDL_LoadFunction(steam_api, "SteamAPI_ISteamUserStats_RequestCurrentStats");

	if (	SteamAPI_Init == NULL ||
		SteamAPI_Shutdown == NULL ||
		SteamAPI_RunCallbacks == NULL ||
		SteamAPI_ISteamUserStats_StoreStats == NULL ||
		SteamAPI_ISteamUserStats_SetAchievement == NULL ||
		SteamAPI_ISteamUserStats_SetStatInt32 == NULL ||
		SteamInternal_CreateInterface == NULL ||
		SteamAPI_GetHSteamUser == NULL ||
		SteamAPI_GetHSteamPipe == NULL ||
		SteamAPI_ISteamClient_GetISteamUserStats == NULL ||
		SteamAPI_ISteamUserStats_RequestCurrentStats == NULL	)
	{
		SDL_UnloadObject(steam_api);
		refcount -= 1;
		return false;
	}

	if (!SteamAPI_Init())
	{
		SDL_UnloadObject(steam_api);
		refcount -= 1;
		return false;
	}

	struct ISteamClient *steamClient = SteamInternal_CreateInterface(BLEEPBLOOP_STEAMCLIENT);
	Sint32 steamUser = SteamAPI_GetHSteamUser();
	Sint32 steamPipe = SteamAPI_GetHSteamPipe();
	if (steamClient == 0 || steamUser == 0 || steamPipe == 0)
	{
		SteamAPI_Shutdown();
		SDL_UnloadObject(steam_api);
		refcount -= 1;
		return false;
	}

	steamUserStats = SteamAPI_ISteamClient_GetISteamUserStats(
		steamClient,
		steamUser,
		steamPipe,
		BLEEPBLOOP_STEAMUSERSTATS
	);
	if (steamUserStats == NULL)
	{
		SteamAPI_Shutdown();
		SDL_UnloadObject(steam_api);
		refcount -= 1;
		return false;
	}
	SteamAPI_ISteamUserStats_RequestCurrentStats(steamUserStats);

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

		SteamAPI_Shutdown();
		SDL_UnloadObject(steam_api);
	}
}

void BleepBloop_Update(void)
{
	if (refcount > 0)
	{
		SteamAPI_RunCallbacks();
	}
}

void BleepBloop_SetAchievement(const char *id)
{
	if (refcount > 0)
	{
		SteamAPI_ISteamUserStats_SetAchievement(steamUserStats, id);
		SteamAPI_ISteamUserStats_StoreStats(steamUserStats);
	}
}

void BleepBloop_UpdateAchievement(const char *id, Sint32 current, Sint32 target)
{
	if (refcount > 0)
	{
		/* `target` should be configured in the Steamworks App Admin site */
		SteamAPI_ISteamUserStats_SetStatInt32(steamUserStats, id, current);
		SteamAPI_ISteamUserStats_StoreStats(steamUserStats);
	}
}

#endif /* BLEEPBLOOP_STEAM */
