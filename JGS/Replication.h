#pragma once

enum EReqLevelBlock
{
	/** Block load AlwaysLoaded levels. Otherwise Async load. */
	BlockAlwaysLoadedLevelsOnly,
	/** Block all loads */
	AlwaysBlock,
	/** Never block loads */
	NeverBlock,
};

namespace Replication
{
	inline void (*SendClientAdjustment)(APlayerController* PlayerController);
	inline float (*GetMaxTickRate)(UFortEngine* FortEngine, float DeltaTime, bool bAllowFrameRateSmoothing);
	inline bool (*IsAdaptiveNetUpdateFrequencyEnabled)(UNetDriver* NetDriver);
	inline AWorldSettings* (*GetWorldSettings)(UWorld* World, const bool bCheckStreamingPersistent, const bool bChecked);
	inline UChannel* (*CreateChannel)(UNetConnection* Connection, EChannelType ChType, bool bOpenedLocally, int32 ChannelIndex);
	inline bool (*IsNetRelevantFor)(AActor* Actor, AActor* RealViewer, AActor* ViewTarget, const FVector& SrcLocation);
	inline void (*SetChannelActor)(UActorChannel* ActorChannel, AActor* InActor);
	inline void (*CallPreReplication)(AActor* Actor, UNetDriver* NetDriver);
	inline int64 (*ReplicateActor)(UActorChannel* ActorChannel);
	inline void (*RemoveNetworkActor)(UWorld* World, AActor* Actor);
	inline void (*AddNetworkActor)(UWorld* World, AActor* Actor);
	inline float (*GetNetPriority)(APlayerController* PlayerController, const FVector& ViewPos, const FVector& ViewDir, AActor* Viewer, AActor* ViewTarget, UActorChannel* InChannel, float Time, bool bLowBandwidth);
	inline void (*GetPlayerViewPoint)(APlayerController* PlayerController, FVector& out_Location, FRotator& out_Rotation);
	inline static void (*ActorChannelClose)(UActorChannel* Channel);
	inline bool (*ClientHasInitializedLevelFor)(UNetConnection* NetConnection, const AActor* TestActor);
	//inline bool (*IsLevelInitializedForActor)(UNetDriver* NetDriver, const AActor* InActor, const UNetConnection* InConnection);

	inline bool bFirstBuildConsiderList = false;

	inline std::vector<FNetworkObjectInfo*> NetworkActors;
	inline std::vector<AActor*> ActorsReplicateOneTime;

	UActorChannel* FindChannel(AActor* Actor, UNetConnection* Connection)
	{
		if (!Actor || !Connection) return nullptr;

		for (int i = 0; i < Connection->OpenChannels.Num(); i++)
		{
			auto Channel = Connection->OpenChannels[i];

			if (Channel && Channel->Class)
			{
				if (Channel->Class == UActorChannel::StaticClass())
				{
					if (((UActorChannel*)Channel)->Actor == Actor)
						return ((UActorChannel*)Channel);
				}
			}
		}

		return nullptr;
	}

	void AddNetworkActorHook(UWorld* World, AActor* Actor)
	{
		if (Actor == nullptr || World == nullptr)
			return AddNetworkActor(World, Actor);

		if (!World->NetDriver)
			return AddNetworkActor(World, Actor);

		if (World->NetDriver->ClientConnections.Num() == 0)
			return AddNetworkActor(World, Actor);

		if (Actor->IsPendingKill())
			return AddNetworkActor(World, Actor);

		for (auto It = NetworkActors.begin(); It != NetworkActors.end(); ++It)
		{
			if ((*It)->Actor == Actor)
			{
				delete* It;
				NetworkActors.erase(It);
				break;
			}
		}

		FNetworkObjectInfo* ActorInfo = new FNetworkObjectInfo();
		ActorInfo->Actor = Actor;
		NetworkActors.push_back(ActorInfo);

		return AddNetworkActor(World, Actor);
	}

	void RemoveNetworkActorHook(UWorld* World, AActor* Actor, bool bDestroyActor = true)
	{
		if (Actor == nullptr || World == nullptr)
			return RemoveNetworkActor(World, Actor);

		if (!World->NetDriver)
			return RemoveNetworkActor(World, Actor);

		if (World->NetDriver->ClientConnections.Num() == 0)
			return RemoveNetworkActor(World, Actor);

		if (bDestroyActor)
		{
			Actor->bActorIsBeingDestroyed = true;
			Actor->bAutoDestroyWhenFinished = true;
			Actor->K2_DestroyActor();
			Actor->ReceiveDestroyed();
			Actor->ForceNetUpdate();

			for (int i = 0; i < World->NetDriver->ClientConnections.Num(); i++)
			{
				UNetConnection* Connection = World->NetDriver->ClientConnections[i];
				if (!Connection) continue;

				auto Channel = FindChannel(Actor, Connection);
				if (Channel) ActorChannelClose(Channel);
			}
		}

		for (auto it = NetworkActors.begin(); it != NetworkActors.end(); )
		{
			if ((*it)->Actor == Actor)
			{
				delete* it;
				it = NetworkActors.erase(it);
			}
			else ++it;
		}

		return RemoveNetworkActor(World, Actor);
	}

	UWorld* GetWorld(AActor* Actor)
	{
		if (ULevel* Level = Actor->GetLevel())
		{
			return Level->OwningWorld;
		}

		return nullptr;
	}

	UActorChannel* ReplicateToClient(AActor* InActor, UNetConnection* InConnection)
	{
		if (!InActor || !InConnection) return nullptr;

		if (InActor->IsA(APlayerController::StaticClass()) && InActor != InConnection->PlayerController)
			return nullptr;

		auto Channel = (UActorChannel*)(CreateChannel(InConnection, EChannelType::CHTYPE_Actor, true, -1));

		if (Channel) {
			SetChannelActor(Channel, InActor);
			Channel->Connection = InConnection;

			return Channel;
		}

		return nullptr;
	}
	
	bool IsActorRelevantToConnection(AActor* Actor, UNetConnection* NetConnection)
	{
		if (NetConnection)
		{
			AActor* ViewTarget = NetConnection->ViewTarget;

			if (ViewTarget)
				return IsNetRelevantFor(Actor, ViewTarget, ViewTarget, ViewTarget->K2_GetActorLocation());
		}

		return false;
	}

	bool IsLevelInitializedForActor(UNetDriver* NetDriver, AActor* InActor, UNetConnection* InConnection)
	{
		//const bool bCorrectWorld = (InConnection->ClientWorldPackageName == GetWorldPackage()->GetFName() && InConnection->ClientHasInitializedLevelFor(InActor));
		const bool bCorrectWorld = true && ClientHasInitializedLevelFor(InConnection, InActor);
		const bool bIsConnectionPC = (InActor == InConnection->PlayerController);

		return bCorrectWorld || bIsConnectionPC;
	}

	int32 ServerReplicateActors_PrepConnections(UNetDriver* NetDriver, const float DeltaSeconds)
	{
		int32 NumClientsToTick = NetDriver->ClientConnections.Num();
		bool bFoundReadyConnection = false;

		for (int32 ConnIdx = 0; ConnIdx < NetDriver->ClientConnections.Num(); ConnIdx++)
		{
			UNetConnection* Connection = NetDriver->ClientConnections[ConnIdx];
			if (!Connection) continue;

			AActor* OwningActor = Connection->OwningActor;
			if (OwningActor != NULL && (Connection->Driver->Time - Connection->LastReceiveTime < 1.5f))
			{
				bFoundReadyConnection = true;

				AActor* DesiredViewTarget = OwningActor;
				if (Connection->PlayerController)
				{
					if (AActor* ViewTarget = Connection->PlayerController->GetViewTarget())
					{
						if (GetWorld(ViewTarget))
							DesiredViewTarget = ViewTarget;
					}
				}

				Connection->ViewTarget = DesiredViewTarget;

				for (int32 ChildIdx = 0; ChildIdx < Connection->Children.Num(); ChildIdx++)
				{
					UNetConnection* Child = Connection->Children[ChildIdx];
					APlayerController* ChildPlayerController = Child->PlayerController;

					if (ChildPlayerController != NULL)
					{
						Child->ViewTarget = ChildPlayerController->GetViewTarget();
					}
					else
					{
						Child->ViewTarget = NULL;
					}
				}
			}
			else
			{
				Connection->ViewTarget = NULL;
				for (int32 ChildIdx = 0; ChildIdx < Connection->Children.Num(); ChildIdx++)
					Connection->Children[ChildIdx]->ViewTarget = NULL;
			}
		}

		return bFoundReadyConnection ? NumClientsToTick : 0;
	}

	void ServerReplicateActors_BuildConsiderList(UNetDriver* NetDriver, const float ServerTickTime)
	{
		int32 NumInitiallyDormant = 0;
		const bool bUseAdapativeNetFrequency = IsAdaptiveNetUpdateFrequencyEnabled(NetDriver);

		UGameplayStatics* GameplayStatics = Globals::GPS;

		TArray<AActor*> Actors;
		Globals::GPS->STATIC_GetAllActorsOfClass(NetDriver->World, AActor::StaticClass(), &Actors);

		for (int i = 0; i < Actors.Num(); i++)
		{
			AActor* Actor = Actors[i];
			if (!Actor) continue;

			FNetworkObjectInfo* ActorInfo = new FNetworkObjectInfo();
			ActorInfo->Actor = Actor;

			if (Actor->NetDriverName != NetDriver->NetDriverName || !Actor->bActorInitialized)
				continue;

			if ((Actor->bActorIsBeingDestroyed || Actor->IsPendingKill()))
				continue;

			if (Actor->RemoteRole == ENetRole::ROLE_None)
				continue;

			if (Actor->GetLevel()->HasVisibilityChangeRequestPending())
				continue;

			if (Actor->NetDormancy == ENetDormancy::DORM_Initial && Actor->bNetStartup)
				continue;

			if (ActorInfo->LastNetReplicateTime == 0)
			{
				ActorInfo->LastNetReplicateTime = GameplayStatics->STATIC_GetTimeSeconds(NetDriver->World);
				ActorInfo->OptimalNetUpdateDelta = 1.0f / Actor->NetUpdateFrequency;
			}

			const float ScaleDownStartTime = 2.0f;
			const float ScaleDownTimeRange = 5.0f;

			const float LastReplicateDelta = GameplayStatics->STATIC_GetTimeSeconds(NetDriver->World) - ActorInfo->LastNetReplicateTime;

			if (LastReplicateDelta > ScaleDownStartTime)
			{
				if (Actor->MinNetUpdateFrequency == 0.0f)
					Actor->MinNetUpdateFrequency = 2.0f;

				const float MinOptimalDelta = 1.0f / Actor->NetUpdateFrequency;
				const float MaxOptimalDelta = Util::Max(1.0f / Actor->MinNetUpdateFrequency, MinOptimalDelta);
				const float Alpha = Util::Clamp((LastReplicateDelta - ScaleDownStartTime) / ScaleDownTimeRange, 0.0f, 1.0f);

				ActorInfo->OptimalNetUpdateDelta = Util::Lerp(MinOptimalDelta, MaxOptimalDelta, Alpha);
			}

			if (!ActorInfo->bPendingNetUpdate)
			{
				const float NextUpdateDelta = bUseAdapativeNetFrequency ? ActorInfo->OptimalNetUpdateDelta : 1.0f / Actor->NetUpdateFrequency;

				ActorInfo->NextUpdateTime = GameplayStatics->STATIC_GetTimeSeconds(NetDriver->World) + Util::SRand() * ServerTickTime + NextUpdateDelta;
				ActorInfo->LastNetUpdateTime = NetDriver->Time;
			}

			ActorInfo->bPendingNetUpdate = false;
			NetworkActors.push_back(ActorInfo);
			CallPreReplication(Actor, NetDriver);
		}

		bFirstBuildConsiderList = true;

		LOG("ServerReplicateActors_BuildConsiderList, bFirstBuildConsiderList: " << bFirstBuildConsiderList);
	}

	int32 ServerReplicateActors(UNetDriver* NetDriver, float DeltaSeconds)
	{
		if (NetDriver->ClientConnections.Num() == 0)
			return 0;

		if (!NetDriver->World)
			return 0;

		NetDriver->ReplicationFrame++;

		int32 Updated = 0;

		const int32 NumClientsToTick = ServerReplicateActors_PrepConnections(NetDriver, DeltaSeconds);

		if (NumClientsToTick == 0)
			return 0;

		AWorldSettings* WorldSettings = GetWorldSettings(NetDriver->World, false, true);

		bool bCPUSaturated = false; // Use Use priority actor usage only if the CPU is saturated
		float ServerTickTime = GetMaxTickRate(Globals::FortEngine, DeltaSeconds, true);
		if (ServerTickTime == 0.f)
		{
			ServerTickTime = DeltaSeconds;
		}
		else
		{
			ServerTickTime = 1.f / ServerTickTime;
			bCPUSaturated = DeltaSeconds > 1.2f * ServerTickTime;
		}

		if (!bFirstBuildConsiderList)
			ServerReplicateActors_BuildConsiderList(NetDriver, ServerTickTime);

		for (int32 i = 0; i < NetDriver->ClientConnections.Num(); i++)
		{
			UNetConnection* Connection = NetDriver->ClientConnections[i];
			if (!Connection) continue;

			if (i >= NumClientsToTick)
			{
				if (bCPUSaturated)
				{
					for (int32 ConsiderIdx = 0; ConsiderIdx < NetworkActors.size(); ConsiderIdx++)
					{
						AActor* Actor = NetworkActors[ConsiderIdx]->Actor;
						if (Actor != NULL && !NetworkActors[ConsiderIdx]->bPendingNetUpdate)
						{
							UActorChannel* Channel = FindChannel(NetworkActors[ConsiderIdx]->WeakActor.Get(), Connection);

							if (Channel != NULL && Channel->LastUpdateTime < NetworkActors[ConsiderIdx]->LastNetUpdateTime)
								NetworkActors[ConsiderIdx]->bPendingNetUpdate = true;
						}
					}

					// Connection->TimeSensitive = false;
				}
			}
			else if (Connection->ViewTarget)
			{
				const bool bUseAdapativeNetFrequency = IsAdaptiveNetUpdateFrequencyEnabled(NetDriver);

				if (Connection->PlayerController)
					SendClientAdjustment(Connection->PlayerController);

				for (int32 ChildIdx = 0; ChildIdx < Connection->Children.Num(); ChildIdx++) // Use for split screen
				{
					if (Connection->Children[ChildIdx]->PlayerController != NULL)
						SendClientAdjustment(Connection->Children[ChildIdx]->PlayerController);
				}

				UGameplayStatics* GameplayStatics = Globals::GPS;

				NetDriver->NetTag++;
				Connection->TickCount++;

				for (int32 j = 0; j < Connection->SentTemporaries.Num(); j++)
					Connection->SentTemporaries[j]->NetTag = NetDriver->NetTag;

				for (auto& ActorInfo : NetworkActors)
				{
					if (!ActorInfo) continue;

					AActor* Actor = ActorInfo->Actor;
					if (!Actor || !Actor->bActorInitialized) continue;

					if ((Actor->bActorIsBeingDestroyed || Actor->IsPendingKill()))
					{
						RemoveNetworkActorHook(NetDriver->World, Actor);
						continue;
					}

					if (Actor->RemoteRole == ENetRole::ROLE_None)
					{
						RemoveNetworkActorHook(NetDriver->World, Actor);
						continue;
					}

					if (Actor->GetLevel()->HasVisibilityChangeRequestPending())
						continue;

					if (Actor->NetDormancy == ENetDormancy::DORM_Initial && Actor->bNetStartup)
					{
						RemoveNetworkActorHook(NetDriver->World, Actor);
						continue;
					}

					if (bCPUSaturated) // Actors may be bugged
					{
						if (ActorInfo->LastNetReplicateTime == 0)
						{
							ActorInfo->LastNetReplicateTime = GameplayStatics->STATIC_GetTimeSeconds(NetDriver->World);
							ActorInfo->OptimalNetUpdateDelta = 1.0f / Actor->NetUpdateFrequency;
						}

						const float ScaleDownStartTime = 2.0f;
						const float ScaleDownTimeRange = 5.0f;

						const float LastReplicateDelta = GameplayStatics->STATIC_GetTimeSeconds(NetDriver->World) - ActorInfo->LastNetReplicateTime;

						if (LastReplicateDelta > ScaleDownStartTime)
						{
							if (Actor->MinNetUpdateFrequency == 0.0f)
								Actor->MinNetUpdateFrequency = 2.0f;

							const float MinOptimalDelta = 1.0f / Actor->NetUpdateFrequency;
							const float MaxOptimalDelta = Util::Max(1.0f / Actor->MinNetUpdateFrequency, MinOptimalDelta);
							const float Alpha = Util::Clamp((LastReplicateDelta - ScaleDownStartTime) / ScaleDownTimeRange, 0.0f, 1.0f);

							ActorInfo->OptimalNetUpdateDelta = Util::Lerp(MinOptimalDelta, MaxOptimalDelta, Alpha);
						}

						if (!ActorInfo->bPendingNetUpdate)
						{
							const float NextUpdateDelta = bUseAdapativeNetFrequency ? ActorInfo->OptimalNetUpdateDelta : 1.0f / Actor->NetUpdateFrequency;

							ActorInfo->NextUpdateTime = GameplayStatics->STATIC_GetTimeSeconds(NetDriver->World) + Util::SRand() * ServerTickTime + NextUpdateDelta;
							ActorInfo->LastNetUpdateTime = NetDriver->Time;
						}

						ActorInfo->bPendingNetUpdate = false;
					}
					
					CallPreReplication(Actor, NetDriver);

					UActorChannel* Channel = FindChannel(Actor, Connection);

					if (Actor->NetTag != NetDriver->NetTag)
						Actor->NetTag = NetDriver->NetTag;

					if (!Channel)
					{
						if (!IsLevelInitializedForActor(NetDriver, Actor, Connection))
							continue;
					}

					if (!Channel) Channel = ReplicateToClient(Actor, Connection);
					if (Channel) ReplicateActor(Channel);

					if (bCPUSaturated) // Actors may be bugged
					{
						if (Channel != NULL && NetDriver->Time - Channel->RelevantTime <= 1.f)
						{
							ActorInfo->bPendingNetUpdate = true;
						}
						else if (IsActorRelevantToConnection(Actor, Connection))
						{
							ActorInfo->bPendingNetUpdate = true;
							if (Channel != NULL)
							{
								Channel->RelevantTime = NetDriver->Time + 0.5f * Util::SRand();
							}
						}
					}
				}
			}
		}

		return Updated;
	}

	void InitOffsets()
	{
		auto BaseAddress = (uintptr_t)GetModuleHandle(NULL);

		MH_CreateHook((LPVOID)(BaseAddress + Offsets::AddNetworkActor), AddNetworkActorHook, (LPVOID*)(&AddNetworkActor));
		MH_EnableHook((LPVOID)(BaseAddress + Offsets::AddNetworkActor));
		MH_CreateHook((LPVOID)(BaseAddress + Offsets::RemoveNetworkActor), RemoveNetworkActorHook, (LPVOID*)(&RemoveNetworkActor));
		MH_EnableHook((LPVOID)(BaseAddress + Offsets::RemoveNetworkActor));

		SendClientAdjustment = decltype(SendClientAdjustment)(BaseAddress + Offsets::SendClientAdjustment);
		GetMaxTickRate = decltype(GetMaxTickRate)(BaseAddress + Offsets::GetMaxTickRate);
		IsAdaptiveNetUpdateFrequencyEnabled = decltype(IsAdaptiveNetUpdateFrequencyEnabled)(BaseAddress + Offsets::IsAdaptiveNetUpdateFrequencyEnabled);
		GetWorldSettings = decltype(GetWorldSettings)(BaseAddress + Offsets::GetWorldSettings);
		CreateChannel = decltype(CreateChannel)(BaseAddress + Offsets::CreateChannel);
		IsNetRelevantFor = decltype(IsNetRelevantFor)(BaseAddress + Offsets::IsNetRelevantFor);
		SetChannelActor = decltype(SetChannelActor)(BaseAddress + Offsets::SetChannelActor);
		CallPreReplication = decltype(CallPreReplication)(BaseAddress + Offsets::CallPreReplication);
		ReplicateActor = decltype(ReplicateActor)(BaseAddress + Offsets::ReplicateActor);
		GetNetPriority = decltype(GetNetPriority)(BaseAddress + Offsets::GetNetPriority);
		GetPlayerViewPoint = decltype(GetPlayerViewPoint)(BaseAddress + Offsets::GetPlayerViewPoint);
		ActorChannelClose = decltype(ActorChannelClose)(BaseAddress + Offsets::ActorChannelClose);
		ClientHasInitializedLevelFor = decltype(ClientHasInitializedLevelFor)(BaseAddress + Offsets::ClientHasInitializedLevelFor);

		if (Globals::World->NetDriver)
		{
			//IsLevelInitializedForActor = decltype(IsLevelInitializedForActor)(Globals::World->NetDriver->Vtable[0x69]);
		}
	}
}