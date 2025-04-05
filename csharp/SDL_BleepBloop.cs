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

using System.Runtime.InteropServices;

public static class SDL_BleepBloop
{
	private const string nativeLibName = "SDL_BleepBloop";

	[DllImport(nativeLibName, CallingConvention = CallingConvention.Cdecl)]
	public static extern bool BleepBloop_Init(int argc, IntPtr argv);

	[DllImport(nativeLibName, CallingConvention = CallingConvention.Cdecl)]
	public static extern void BleepBloop_Quit();

	[DllImport(nativeLibName, CallingConvention = CallingConvention.Cdecl)]
	public static extern void BleepBloop_Update();

	[DllImport(nativeLibName, CallingConvention = CallingConvention.Cdecl)]
	public static extern void BleepBloop_SetAchievement(
		[MarshalAs(UnmanagedType.LPStr)] string id
	);

	[DllImport(nativeLibName, CallingConvention = CallingConvention.Cdecl)]
	public static extern void BleepBloop_UpdateAchievement(
		[MarshalAs(UnmanagedType.LPStr)] string id,
		int current,
		int target
	);
}
