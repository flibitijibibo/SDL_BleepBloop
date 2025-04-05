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

#ifdef BLEEPBLOOP_EPIC

#include <SDL_BleepBloop.h>

#include <eos_sdk.h>
#include <eos_logging.h>
#include <eos_auth.h>
#include <eos_achievements.h>
#include <eos_stats.h>

#include "BleepBloop_Secrets.h"

static EOS_HPlatform eosHandle = NULL;
static EOS_EpicAccountId eosAccount = NULL;
static EOS_ProductUserId eosUser = NULL;
static uint64_t refcount = 0;

static void EOS_CALL EOSLogMessage(const EOS_LogMessage *message)
{
	SDL_Log("EOS SDK - [%s][%d]: %s", message->Category, message->Level, message->Message);
}

static void EOS_CALL OnGetAchievements(const EOS_Achievements_OnQueryDefinitionsCompleteCallbackInfo *callbackInfo)
{
	SDL_Log("EOS SDK - Got achievements");
}

static void EOS_CALL OnGetStats(const EOS_Stats_OnQueryStatsCompleteCallbackInfo *callbackInfo)
{
	SDL_Log("EOS SDK - Got stats");
}

static void EOS_CALL OnGetUser(EOS_ProductUserId userId, void* clientData)
{
	eosUser = userId;
	if (eosUser == NULL)
	{
		SDL_Log("EOS SDK - Connect failed!");
		return;
	}

	SDL_Log("EOS SDK - Query stats/achievements");

	EOS_Achievements_QueryDefinitionsOptions options;
	options.ApiVersion = EOS_ACHIEVEMENTS_QUERYDEFINITIONS_API_LATEST;
	options.LocalUserId = eosUser;
	EOS_Achievements_QueryDefinitions(
		EOS_Platform_GetAchievementsInterface(eosHandle),
		&options,
		clientData,
		OnGetAchievements
	);

	EOS_Stats_QueryStatsOptions statOptions;
	statOptions.ApiVersion = EOS_STATS_QUERYSTATS_API_LATEST;
	statOptions.LocalUserId = eosUser;
	statOptions.StartTime = EOS_STATS_TIME_UNDEFINED;
	statOptions.EndTime = EOS_STATS_TIME_UNDEFINED;
	statOptions.StatNames = NULL;
	statOptions.StatNamesCount = 0;
	statOptions.TargetUserId = eosUser;
	EOS_Stats_QueryStats(
		EOS_Platform_GetStatsInterface(eosHandle),
		&statOptions,
		clientData,
		OnGetStats
	);
}

static void EOS_CALL OnCreateUser(const EOS_Connect_CreateUserCallbackInfo *callbackInfo)
{
	OnGetUser(callbackInfo->LocalUserId, callbackInfo->ClientData);
}

static void EOS_CALL OnConnected(const EOS_Connect_LoginCallbackInfo *callbackInfo)
{
	if (callbackInfo->ResultCode == EOS_InvalidUser)
	{
		/* This is a new account, which is a totally different path */
		EOS_Connect_CreateUserOptions createOptions;
		createOptions.ApiVersion = EOS_CONNECT_CREATEUSER_API_LATEST;
		createOptions.ContinuanceToken = callbackInfo->ContinuanceToken;
		EOS_Connect_CreateUser(
			EOS_Platform_GetConnectInterface(eosHandle),
			&createOptions,
			callbackInfo->ClientData,
			OnCreateUser
		);
	}
	else
	{
		OnGetUser(callbackInfo->LocalUserId, callbackInfo->ClientData);
	}
}

static void EOS_CALL OnLoggedIn(const EOS_Auth_LoginCallbackInfo *callbackInfo)
{
	EOS_EResult result;

	eosAccount = callbackInfo->LocalUserId;
	if (eosAccount == NULL)
	{
		SDL_Log("EOS SDK - Authentication failed!");
		return;
	}

	EOS_Auth_CopyIdTokenOptions tokenOptions;
	tokenOptions.ApiVersion = EOS_AUTH_COPYIDTOKEN_API_LATEST;
	tokenOptions.AccountId = eosAccount;
	EOS_Auth_IdToken *token;
	result = EOS_Auth_CopyIdToken(
		EOS_Platform_GetAuthInterface(eosHandle),
		&tokenOptions,
		&token
	);
	if (result != EOS_Success)
	{
                SDL_Log("EOS SDK - Couldn't get IdToken!");
	}

	/* Authentication done, now chain a connection to it */
	SDL_Log("EOS SDK - Connect");

	EOS_Connect_Credentials credentials;
	credentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
	credentials.Token = token->JsonWebToken;
	credentials.Type = EOS_ECT_EPIC_ID_TOKEN;
	EOS_Connect_LoginOptions loginOptions;
	loginOptions.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
	loginOptions.Credentials = &credentials;
	loginOptions.UserLoginInfo = NULL;
	EOS_Connect_Login(
		EOS_Platform_GetConnectInterface(eosHandle),
		&loginOptions,
		callbackInfo->ClientData,
		OnConnected
	);
}

bool BleepBloop_Init(int argc, char **argv)
{
	EOS_EResult result;
	int i;

	if (refcount++ > 0)
	{
		return true;
	}

	SDL_assert(refcount == 1);

	char *token = NULL, *sandbox = NULL;
	for (i = 0; i < argc; i += 1)
	{
		if (SDL_strstr(argv[i], "-AUTH_PASSWORD=") == argv[i])
		{
			token = argv[i] + SDL_strlen("-AUTH_PASSWORD=");
		}
		else if (SDL_strstr(argv[i], "-epicsandboxid=") == argv[i])
		{
			sandbox = argv[i] + SDL_strlen("-epicsandboxid=");
		}
	}

	SDL_Log("EOS SDK - Init");

	EOS_InitializeOptions initOptions;
	initOptions.ApiVersion = EOS_INITIALIZE_API_LATEST;
	initOptions.AllocateMemoryFunction = NULL;
	initOptions.ReallocateMemoryFunction = NULL;
	initOptions.ReleaseMemoryFunction = NULL;
	initOptions.ProductName = BLEEPBLOOP_SECRET_EPIC_PRODUCT_NAME;
	initOptions.ProductVersion = BLEEPBLOOP_SECRET_EPIC_PRODUCT_VERSION;
	initOptions.Reserved = NULL;
	initOptions.SystemInitializeOptions = NULL;
	initOptions.OverrideThreadAffinity = NULL;
	result = EOS_Initialize(&initOptions);
	if (result != EOS_Success)
	{
		SDL_Log("EOS SDK - Init failed!");
		refcount -= 1;
		return false;
	}

	EOS_Logging_SetCallback(EOSLogMessage);
#if SDL_ASSERT_LEVEL > 1
	EOS_Logging_SetLogLevel(EOS_LC_ALL_CATEGORIES, EOS_LOG_VeryVerbose);
#endif

	EOS_Platform_Options options;
	SDL_zero(options);
	options.ApiVersion = EOS_PLATFORM_OPTIONS_API_LATEST;
	options.ProductId = BLEEPBLOOP_SECRET_EPIC_PRODUCT_ID;
	options.SandboxId = (sandbox != NULL) ?
		sandbox :
		BLEEPBLOOP_SECRET_EPIC_SANDBOX_IDS[0];
	options.ClientCredentials.ClientId = BLEEPBLOOP_SECRET_EPIC_CLIENT_ID;
	options.ClientCredentials.ClientSecret = BLEEPBLOOP_SECRET_EPIC_CLIENT_SECRET;
	options.DeploymentId = BLEEPBLOOP_SECRET_EPIC_DEPLOYMENT_IDS[0];
	for (i = 0; i < SDL_arraysize(BLEEPBLOOP_SECRET_EPIC_SANDBOX_IDS); i += 1)
	{
		if (SDL_strcmp(options.SandboxId, BLEEPBLOOP_SECRET_EPIC_SANDBOX_IDS[i]) == 0)
		{
			options.DeploymentId = BLEEPBLOOP_SECRET_EPIC_DEPLOYMENT_IDS[i];
		}
	}
	eosHandle = EOS_Platform_Create(&options);
	if (eosHandle == NULL)
	{
		SDL_Log("EOS SDK - Platform init failed!");
		EOS_Shutdown();
		refcount -= 1;
		return false;
	}

	EOS_Auth_Credentials credentials;
	credentials.ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;
	credentials.Id = NULL;
	credentials.Token = token;
	if (token != NULL)
	{
		SDL_Log("EOS SDK - Using exchange code!");
		credentials.Type = EOS_LCT_ExchangeCode;
	}
	else
	{
		SDL_Log("EOS SDK - No token found, trying persistent login");
		credentials.Type = EOS_LCT_PersistentAuth;
	}
	credentials.SystemAuthCredentialsOptions = NULL;
	credentials.ExternalType = EOS_ECT_EPIC;
	EOS_Auth_LoginOptions loginOptions;
	loginOptions.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
	loginOptions.Credentials = &credentials;
	loginOptions.ScopeFlags = EOS_AS_NoFlags;
	loginOptions.LoginFlags = 0;
	EOS_Auth_Login(
		EOS_Platform_GetAuthInterface(eosHandle),
		&loginOptions,
		NULL,
		OnLoggedIn
	);
	return true;
}

static void EOS_CALL OnLogout(const EOS_Auth_LogoutCallbackInfo *callbackInfo)
{
	SDL_Log("EOS SDK - Logged out");
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

	SDL_Log("EOS SDK - Shutdown");

	if (eosAccount != NULL)
	{
		EOS_Auth_LogoutOptions logoutOptions;
		logoutOptions.ApiVersion = EOS_AUTH_LOGOUT_API_LATEST;
		logoutOptions.LocalUserId = eosAccount;
		EOS_Auth_Logout(
			EOS_Platform_GetAuthInterface(eosHandle),
			&logoutOptions,
			NULL,
			OnLogout
		);
		eosAccount = NULL;
	}

	EOS_Platform_Release(eosHandle);
	EOS_Shutdown();
	eosUser = NULL;
}

void BleepBloop_Update(void)
{
	if (refcount > 0)
	{
		EOS_Platform_Tick(eosHandle);
	}
}

static void EOS_CALL OnAchievementUnlocked(const EOS_Achievements_OnUnlockAchievementsCompleteCallbackInfo *callbackInfo)
{
	SDL_Log("EOS SDK - Achievement unlocked");
}

void BleepBloop_SetAchievement(const char *id)
{
	if (eosUser != NULL)
	{
		const char *notAList[] = { id };
		EOS_Achievements_UnlockAchievementsOptions options;
		options.ApiVersion = EOS_ACHIEVEMENTS_UNLOCKACHIEVEMENTS_API_LATEST;
		options.UserId = eosUser;
		options.AchievementIds = notAList;
		options.AchievementsCount = 1;
		EOS_Achievements_UnlockAchievements(
			EOS_Platform_GetAchievementsInterface(eosHandle),
			&options,
			NULL,
			OnAchievementUnlocked
		);
	}
}

static void EOS_CALL OnStatSet(const EOS_Stats_IngestStatCompleteCallbackInfo *callbackInfo)
{
	SDL_Log("EOS SDK - Stat updated");
}

void BleepBloop_UpdateAchievement(const char *id, Sint32 current, Sint32 target)
{
	if (eosUser != NULL)
	{
		EOS_Stats_IngestData data;
		data.ApiVersion = EOS_STATS_INGESTDATA_API_LATEST;
		data.StatName = id;
		data.IngestAmount = current;
		EOS_Stats_IngestStatOptions options;
		options.ApiVersion = EOS_STATS_INGESTSTAT_API_LATEST;
		options.LocalUserId = eosUser;
		options.Stats = &data;
		options.StatsCount = 1;
		options.TargetUserId = eosUser;
		EOS_Stats_IngestStat(
			EOS_Platform_GetStatsInterface(eosHandle),
			&options,
			NULL,
			OnStatSet
		);
	}
}

#endif /* BLEEPBLOOP_EPIC */
