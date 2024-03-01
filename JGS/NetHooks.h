#pragma once

#include "Inventory.h"
#include <fstream>

namespace Beacons
{
	inline bool (*InitHost)(AOnlineBeaconHost*);
	inline char (*InitListen)(UNetDriver*, void*, FURL&, bool, FString&);
	inline void (*SetWorld)(UNetDriver*, UWorld*);
	inline void (*TickFlush)(UNetDriver*, float DeltaSeconds);

	void TickFlushHook(UNetDriver* NetDriver, float DeltaSeconds)
	{
		if (NetDriver)
		{
			if (NetDriver->IsA(UIpNetDriver::StaticClass()) &&
				NetDriver->ClientConnections.Num() > 0 &&
				!NetDriver->ClientConnections[0]->InternalAck)
			{
				Replication::ServerReplicateActors(NetDriver, DeltaSeconds);
			}
		}

		return TickFlush(NetDriver, DeltaSeconds);
	}

	int64_t KickPatch()
	{
		return 0;
	}

	bool LocalSpawnPlayActorHook(ULocalPlayer* Player, const FString& URL, FString& OutError, UWorld* World)
	{
		return true;
	}

	void CollectGarbageHook() 
	{ 
		return; 
	}

	void InitHooks()
	{
		auto BaseAddr = Util::BaseAddress();
		InitHost = decltype(InitHost)(BaseAddr + Offsets::InitHost);
		InitListen = decltype(InitListen)(BaseAddr + Offsets::InitListen);
		SetWorld = decltype(SetWorld)(BaseAddr + Offsets::SetWorld);

		UWorld* World = Globals::World;

		AOnlineBeaconHost* Beacon = Util::SpawnActor<AOnlineBeaconHost>(AOnlineBeaconHost::StaticClass(), FVector());
        Beacon->ListenPort = 7777 - 1;

        if (World->NetDriver)
        {
			LOG("Beacon already create!");
            return;
        }

        if (InitHost(Beacon))
        {
            World->NetDriver = Beacon->NetDriver;
			LOG("Beacon created successful!");
        }
        else
        {
			LOG("Failed to create Beacon!");
            return;
        }

        if (World->NetDriver)
        {
            World->NetDriver->World = World;
			World->NetDriver->NetDriverName = Globals::StringLib->STATIC_Conv_StringToName(L"GameNetDriver");

            FString Error;
            auto URL = FURL();
            URL.Port = 7777;

            InitListen(World->NetDriver, World, URL, true, Error);

            SetWorld(World->NetDriver, World);

            World->LevelCollections[0].NetDriver = World->NetDriver;
            World->LevelCollections[1].NetDriver = World->NetDriver;

            LOG("Beacon listen: " << URL.Port);
        }
        else
        {
            LOG("Failed to listen!");
        }

		MH_CreateHook((LPVOID)(BaseAddr + Offsets::TickFlush), TickFlushHook, (LPVOID*)(&TickFlush));
		MH_EnableHook((LPVOID)(BaseAddr + Offsets::TickFlush));
		MH_CreateHook((LPVOID)(BaseAddr + Offsets::LocalSpawnPlayActor), LocalSpawnPlayActorHook, nullptr);
		MH_EnableHook((LPVOID)(BaseAddr + Offsets::LocalSpawnPlayActor));
		MH_CreateHook((LPVOID)(BaseAddr + Offsets::KickPatch), KickPatch, nullptr);
		MH_EnableHook((LPVOID)(BaseAddr + Offsets::KickPatch));
		MH_CreateHook((LPVOID)(BaseAddr + Offsets::CollectGarbage), CollectGarbageHook, nullptr);
		MH_EnableHook((LPVOID)(BaseAddr + Offsets::CollectGarbage));

		Misc::Init();
	}
}