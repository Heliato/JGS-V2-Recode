#pragma once

#include <cstdint>
#include <winnt.h>
#include <cstdio>
#include <consoleapi.h>
#include <libloaderapi.h>
#include <vector>
#include <iostream>
#include <Windows.h>
#include <Psapi.h>
#include "SDK.hpp"

using namespace SDK;

#define LOG(str) std::cout << "LogJGSV2: " << str << std::endl;

static int32 GSRandSeed;

#undef  PI
#define PI 					(3.1415926535897932f)
#define SMALL_NUMBER		(1.e-8f)
#define KINDA_SMALL_NUMBER	(1.e-4f)
#define BIG_NUMBER			(3.4e+38f)
#define EULERS_NUMBER       (2.71828182845904523536f)

enum EObjectFlags
{
	RF_NoFlags = 0x00000000,
	RF_Public = 0x00000001,
	RF_Standalone = 0x00000002,
	RF_MarkAsNative = 0x00000004,
	RF_Transactional = 0x00000008,
	RF_ClassDefaultObject = 0x00000010,
	RF_ArchetypeObject = 0x00000020,
	RF_Transient = 0x00000040,
	RF_MarkAsRootSet = 0x00000080,
	RF_TagGarbageTemp = 0x00000100,
	RF_NeedInitialization = 0x00000200,
	RF_NeedLoad = 0x00000400,
	RF_KeepForCooker = 0x00000800,
	RF_NeedPostLoad = 0x00001000,
	RF_NeedPostLoadSubobjects = 0x00002000,
	RF_NewerVersionExists = 0x00004000,
	RF_BeginDestroyed = 0x00008000,
	RF_FinishDestroyed = 0x00010000,
	RF_BeingRegenerated = 0x00020000,
	RF_DefaultSubObject = 0x00040000,
	RF_WasLoaded = 0x00080000,
	RF_TextExportTransient = 0x00100000,
	RF_LoadCompleted = 0x00200000,
	RF_InheritableComponentTemplate = 0x00400000,
	RF_DuplicateTransient = 0x00800000,
	RF_StrongRefOnFrame = 0x01000000,
	RF_NonPIEDuplicateTransient = 0x02000000,
	RF_WillBeLoaded = 0x08000000,
	RF_HasExternalPackage = 0x10000000,
	RF_AllocatedInSharedPage = 0x80000000,
};

struct FActorSpawnParameters
{
	FName Name;
	UObject* Template;
	UObject* Owner;
	UObject** Instigator;
	UObject* OverrideLevel;
	ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride;
	uint16	bRemoteOwned : 1;
	uint16	bNoFail : 1;
	uint16	bDeferConstruction : 1;
	uint16	bAllowDuringConstructionScript : 1;
	EObjectFlags ObjectFlags;
};

class Util
{
public:
	static inline UObject* (*ASpawnActor)(UWorld* World, UClass* Class, FTransform const* UserTransformPtr, const FActorSpawnParameters& SpawnParameters);

	static __forceinline VOID InitConsole()
	{
		FILE* fDummy;
		AllocConsole();
		freopen_s(&fDummy, "CONIN$", "r", stdin);
		freopen_s(&fDummy, "CONOUT$", "w", stdout);
	}

	static __forceinline uintptr_t BaseAddress()
	{
		return reinterpret_cast<uintptr_t>(GetModuleHandle(0));
	}

	static __forceinline uintptr_t FindPattern(const char* signature, bool bRelative = false, uint32_t offset = 0)
	{
		uintptr_t base_address = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
		static auto patternToByte = [](const char* pattern)
		{
			auto bytes = std::vector<int>{};
			const auto start = const_cast<char*>(pattern);
			const auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current)
			{
				if (*current == '?')
				{
					++current;
					if (*current == '?') ++current;
					bytes.push_back(-1);
				}
				else { bytes.push_back(strtoul(current, &current, 16)); }
			}
			return bytes;
		};

		const auto dosHeader = (PIMAGE_DOS_HEADER)base_address;
		const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)base_address + dosHeader->e_lfanew);

		const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto patternBytes = patternToByte(signature);
		const auto scanBytes = reinterpret_cast<std::uint8_t*>(base_address);

		const auto s = patternBytes.size();
		const auto d = patternBytes.data();

		for (auto i = 0ul; i < sizeOfImage - s; ++i)
		{
			bool found = true;
			for (auto j = 0ul; j < s; ++j)
			{
				if (scanBytes[i + j] != d[j] && d[j] != -1)
				{
					found = false;
					break;
				}
			}
			if (found)
			{
				uintptr_t address = reinterpret_cast<uintptr_t>(&scanBytes[i]);
				if (bRelative)
				{
					address = ((address + offset + 4) + *(int32_t*)(address + offset));
					return address;
				}
				return address;
			}
		}
		return NULL;
	}

	static BOOL MaskCompare(PVOID pBuffer, LPCSTR lpPattern, LPCSTR lpMask)
	{
		for (auto value = static_cast<PBYTE>(pBuffer); *lpMask; ++lpPattern, ++lpMask, ++value)
		{
			if (*lpMask == 'x' && *reinterpret_cast<LPCBYTE>(lpPattern) != *value)
				return false;
		}

		return true;
	}

	static PBYTE FindPattern(PVOID pBase, DWORD dwSize, LPCSTR lpPattern, LPCSTR lpMask)
	{
		dwSize -= static_cast<DWORD>(strlen(lpMask));

		for (auto i = 0UL; i < dwSize; ++i)
		{
			auto pAddress = static_cast<PBYTE>(pBase) + i;

			if (MaskCompare(pAddress, lpPattern, lpMask))
				return pAddress;
		}

		return NULL;
	}

	static PBYTE FindPattern(LPCSTR lpPattern, LPCSTR lpMask)
	{
		MODULEINFO info = { 0 };

		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info));

		return Util::FindPattern(info.lpBaseOfDll, info.SizeOfImage, lpPattern, lpMask);
	}

	/*static AActor* SpawnActor(UClass* ActorClass, FVector Location, FRotator Rotation)
	{
		FQuat Quat;
		FTransform Transform;

		auto DEG_TO_RAD = 3.14159 / 180;
		auto DIVIDE_BY_2 = DEG_TO_RAD / 2;

		auto SP = sin(Rotation.Pitch * DIVIDE_BY_2);
		auto CP = cos(Rotation.Pitch * DIVIDE_BY_2);

		auto SY = sin(Rotation.Yaw * DIVIDE_BY_2);
		auto CY = cos(Rotation.Yaw * DIVIDE_BY_2);

		auto SR = sin(Rotation.Roll * DIVIDE_BY_2);
		auto CR = cos(Rotation.Roll * DIVIDE_BY_2);

		Quat.X = CR * SP * SY - SR * CP * CY;
		Quat.Y = -CR * SP * CY - SR * CP * SY;
		Quat.Z = CR * CP * SY - SR * SP * CY;
		Quat.W = CR * CP * CY + SR * SP * SY;

		Transform.Rotation = Quat;
		Transform.Scale3D = FVector{ 1,1,1 };
		Transform.Translation = Location;

		auto Actor = Globals::GPS->STATIC_BeginSpawningActorFromClass(Globals::FortEngine->GameViewport->World, ActorClass, Transform, false, nullptr);
		Globals::GPS->STATIC_FinishSpawningActor(Actor, Transform);
		return Actor;
	}*/

	/*template <typename ActorType>
	static ActorType* SpawnActor(UClass* Class, FTransform UserTransformPtr = FTransform(), const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters())
	{
		return (ActorType*)ASpawnActor(Globals::FortEngine->GameViewport->World, Class, &UserTransformPtr, SpawnParameters);
	}*/

	template <typename ActorType>
	static ActorType* SpawnActor(UClass* Class, FVector Location, FRotator Rotation = FRotator(), FVector Scale3D = FVector(1, 1, 1), const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters())
	{
		/*const float DEG_TO_RAD = PI / (180.f);
		const float DIVIDE_BY_2 = DEG_TO_RAD / 2.f;
		float SP, SY, SR;
		float CP, CY, CR;

		SinCos(&SP, &CP, Rotation.Pitch * DIVIDE_BY_2);
		SinCos(&SY, &CY, Rotation.Yaw * DIVIDE_BY_2);
		SinCos(&SR, &CR, Rotation.Roll * DIVIDE_BY_2);

		FQuat RotationQuat{};
		RotationQuat.X = CR * SP * SY - SR * CP * CY;
		RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
		RotationQuat.Z = CR * CP * SY - SR * SP * CY;
		RotationQuat.W = CR * CP * CY + SR * SP * SY;

		FTransform UserTransformPtr{};
		UserTransformPtr.Translation = Location;
		UserTransformPtr.Rotation = RotationQuat;
		UserTransformPtr.Scale3D = Scale3D;

		return SpawnActor<ActorType>(Class, UserTransformPtr, SpawnParameters);*/

		FQuat Quat;
		FTransform Transform;

		auto DEG_TO_RAD = 3.14159 / 180;
		auto DIVIDE_BY_2 = DEG_TO_RAD / 2;

		auto SP = sin(Rotation.Pitch * DIVIDE_BY_2);
		auto CP = cos(Rotation.Pitch * DIVIDE_BY_2);

		auto SY = sin(Rotation.Yaw * DIVIDE_BY_2);
		auto CY = cos(Rotation.Yaw * DIVIDE_BY_2);

		auto SR = sin(Rotation.Roll * DIVIDE_BY_2);
		auto CR = cos(Rotation.Roll * DIVIDE_BY_2);

		Quat.X = CR * SP * SY - SR * CP * CY;
		Quat.Y = -CR * SP * CY - SR * CP * SY;
		Quat.Z = CR * CP * SY - SR * SP * CY;
		Quat.W = CR * CP * CY + SR * SP * SY;

		Transform.Rotation = Quat;
		Transform.Scale3D = Scale3D;
		Transform.Translation = Location;

		auto Actor = Globals::GPS->STATIC_BeginSpawningActorFromClass(Globals::FortEngine->GameViewport->World, Class, Transform, false, nullptr);
		Globals::GPS->STATIC_FinishSpawningActor(Actor, Transform);
		return (ActorType*)Actor;
	}

	static bool AreGuidsTheSame(FGuid guidA, FGuid guidB)
	{
		if (guidA.A == guidB.A && guidA.B == guidB.B && guidA.C == guidB.C && guidA.D == guidB.D)
			return true;
		else
			return false;
	}

	static FORCEINLINE float Sin(float Value) { return sinf(Value); }
	static FORCEINLINE float Asin(float Value) { return asinf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
	static FORCEINLINE float Sinh(float Value) { return sinhf(Value); }
	static FORCEINLINE float Cos(float Value) { return cosf(Value); }
	static FORCEINLINE float Acos(float Value) { return acosf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
	static FORCEINLINE float Tan(float Value) { return tanf(Value); }
	static FORCEINLINE float Atan(float Value) { return atanf(Value); }
	static FORCEINLINE float Sqrt(float Value) { return sqrtf(Value); }
	static FORCEINLINE float Pow(float A, float B) { return powf(A, B); }

	template< class T >
	static FORCEINLINE T Max(const T A, const T B)
	{
		return (A >= B) ? A : B;
	}

	template< class T >
	static FORCEINLINE T Clamp(const T X, const T Min, const T Max)
	{
		return X < Min ? Min : X < Max ? X : Max;
	}

	template< class T, class U >
	static FORCEINLINE T Lerp(const T& A, const T& B, const U& Alpha)
	{
		return (T)(A + Alpha * (B - A));
	}

	template< class T >
	static FORCEINLINE T Square(const T A)
	{
		return A * A;
	}

	static FORCEINLINE int32 TruncToInt(float F)
	{
		return (int32)F;
	}

	static FORCEINLINE float TruncToFloat(float F)
	{
		return (float)TruncToInt(F);
	}

	static FORCEINLINE float Fractional(float Value)
	{
		return Value - TruncToFloat(Value);
	}

	static FORCEINLINE float DistSquared(const FVector& V1, const FVector& V2)
	{
		return Util::Square(V2.X - V1.X) + Util::Square(V2.Y - V1.Y) + Util::Square(V2.Z - V1.Z);
	}

	inline static float SRand()
	{
		GSRandSeed = (GSRandSeed * 196314165) + 907633515;
		union { float f; int32 i; } Result;
		union { float f; int32 i; } Temp;
		const float SRandTemp = 1.0f;
		Temp.f = SRandTemp;
		Result.i = (Temp.i & 0xff800000) | (GSRandSeed & 0x007fffff);
		return Fractional(Result.f);
	}
};