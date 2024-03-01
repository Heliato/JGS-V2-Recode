#pragma once

#include <algorithm>
#include <time.h>
#include <vector>
#include <unordered_map>
#include <functional>
#include <string>
#include <sstream>

enum ENetMode
{
	NM_Standalone,
	NM_DedicatedServer,
	NM_ListenServer,
	NM_Client,
	NM_MAX,
};

std::vector<std::string> split(const std::string& s, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

namespace Hooks
{
	void (*TestPickup1)(__int64 a1, __int64 a2);
	void (*TestPickup2)(__int64 a1, __int64 a2, float a3, __int64 a4, char a5);
	char (*TestPickup3)(__int64 a1);

	/*void TestPickup1Hook(__int64 a1, __int64 a2)
	{
		LOG("TestPickup1Hook");

		LOG("a1: " << a1);
		LOG("a2: " << a2);

		return TestPickup1(a1, a2);
	}

	void TestPickup2Hook(__int64 a1, __int64 a2, float a3, __int64 a4, char a5)
	{
		LOG("TestPickup2Hook");

		LOG("a1: " << a1);
		LOG("a2: " << a2);
		LOG("a3: " << a3);
		LOG("a4: " << a4);
		LOG("a5: " << a5);

		return TestPickup2(a1, a2, a3, a4, a5);
	}

	char TestPickup3Hook(__int64 a1)
	{
		LOG("TestPickup3Hook");

		LOG("a1: " << a1);

		return TestPickup3(a1);
	}*/

	char sub_7FF66CA1FB60Hook(AFortGameModeAthena* a1)
	{
		return 1;
	}

	void PostGameplayEffectExecuteHook(UFortRegenHealthSet* HealthSet, __int64 a2)
	{
		LOG("PostGameplayEffectExecuteHook");

		LOG("HealthSet: " << HealthSet->GetName());

		return;
	}

	static ENetMode GetNetModeHook() {
		return ENetMode::NM_DedicatedServer;
	}

	static ENetMode InternalGetNetModeHook() {
		return ENetMode::NM_DedicatedServer;
	}

	static inline int64_t GIsServer()
	{
		return __int64(GetModuleHandleW(0)) + 0x6536b66;
	}

	static inline int64_t GIsClient()
	{
		return __int64(GetModuleHandleW(0)) + 0x6536b65;
	}

	bool bLogs = false;
	bool bIsReady = false;
	bool bHasSpawned = false;
	bool bIsInGame = false;
	bool bHasInitedTheBeacon = false;
	bool bSetupCharPartArray = false;
	bool bSetupFloorLoot = false;
	bool bEverythingSearched = false;

	LPVOID(*ProcessEvent)(void*, void*, void*);
	LPVOID ProcessEventHook(UObject* Object, UFunction* Function, LPVOID Parms)
	{
		auto FuncName = Function->GetName();

		if (FuncName.contains("Tick"))
		{
			if (GetAsyncKeyState(VK_F7) & 0x1)
			{
				bLogs = bLogs ? false : true;
			}

#ifdef CHEATS
			if (GetAsyncKeyState(VK_F6) & 0x1)
			{
				auto World = Globals::FortEngine->GameViewport->World;
				auto PlayerController = (AFortPlayerControllerAthena*)Globals::GPS->STATIC_GetPlayerController(World, 0);

				Inventory* PlayerInventory = FindInventory((AFortPlayerController*)PlayerController);

				if (PlayerInventory)
				{
					PlayerInventory->DropAllItemsFromInventory();
				}
			}

			if (GetAsyncKeyState(VK_F5) & 0x1)
			{
				auto World = Globals::FortEngine->GameViewport->World;
				auto PlayerController = (AFortPlayerControllerAthena*)Globals::GPS->STATIC_GetPlayerController(World, 0);

				Inventory* PlayerInventory = FindInventory((AFortPlayerController*)PlayerController);

				if (PlayerInventory)
				{
					PlayerInventory->RemoveAllItemsFromInventory();
					PlayerInventory->UpdateInventory();
				}
			}

			if (GetAsyncKeyState(VK_F4) & 0x1)
			{
				LOG("NetworkActors: " << Replication::NetworkActors.size());
			}

			/*if (GetAsyncKeyState(VK_F3) & 0x1)
			{

			}*/

			/*if (GetAsyncKeyState(VK_F2) & 0x1)
			{
				
			}*/

			if (GetAsyncKeyState(VK_F1) & 0x1)
			{
				
			}
#endif // CHEATS
		}

		if (bLogs)
		{
			if (!FuncName.contains("Tick") &&
				!FuncName.contains("Visual") &&
				!FuncName.contains("Clown Spinner") &&
				!FuncName.contains("CustomStateChanged") &&
				!FuncName.contains("ReceiveBeginPlay") &&
				!FuncName.contains("OnAttachToBuilding") &&
				!FuncName.contains("OnWorldReady") &&
				!FuncName.contains("K2_GetActorLocation") &&
				!FuncName.contains("ReceiveDrawHUD") &&
				!FuncName.contains("ServerUpdateCamera") &&
				!FuncName.contains("ServerMove") &&
				!FuncName.contains("ContrailCheck") &&
				!FuncName.contains("GetViewTarget") &&
				!FuncName.contains("GetAllActorsOfClass") &&
				!FuncName.contains("ClientAckGoodMove") &&
				!FuncName.contains("ReadyToEndMatch") &&
				!FuncName.contains("Check Closest Point") &&
				!FuncName.contains("ServerTriggerCombatEvent") &&
				!FuncName.contains("UpdateTime") &&
				!FuncName.contains("OnUpdateMusic") &&
				!FuncName.contains("UpdateStateEvent") &&
				!FuncName.contains("ServerTouchActiveTime") &&
				!FuncName.contains("OnCheckIfSurrounded") &&
				!FuncName.contains("ServerFireAIDirectorEventBatch") &&
				!FuncName.contains("ServerTriggerCombatEventBatch") &&
				!FuncName.contains("UserConstructionScript") &&
				!FuncName.contains("K2_OnReset") &&
				!FuncName.contains("K2_OnEndViewTarget") &&
				!FuncName.contains("K2_OnBecomeViewTarget") &&
				!FuncName.contains("ReceiveUnpossessed") &&
				!FuncName.contains("ClientGotoState") &&
				!FuncName.contains("K2_OnEndViewTarget") &&
				!FuncName.contains("K2_OnBecomeViewTarget") &&
				!FuncName.contains("ClientSetViewTarget") &&
				!FuncName.contains("ServerClientPawnLoaded") &&
				!FuncName.contains("ReceiveEndPlay") &&
				!FuncName.contains("OnPerceptionStimuliSourceEndPlay") &&
				!FuncName.contains("HandleOnHUDElementVisibilityChanged") &&
				!FuncName.contains("OnHUDElementVisibilityChanged") &&
				!FuncName.contains("HandleInteractionChanged") &&
				!FuncName.contains("BlueprintModifyCamera") &&
				!FuncName.contains("BlueprintModifyPostProcess") &&
				!FuncName.contains("ServerSetSpectatorLocation") &&
				!FuncName.contains("ServerFireAIDirectorEvent") &&
				!FuncName.contains("ServerTryActivateAbility") &&
				!FuncName.contains("ClientActivateAbilitySucceed") &&
				!FuncName.contains("K2_CommitExecute") &&
				!FuncName.contains("ServerSetSpectatorLocation") &&
				!FuncName.contains("CanJumpInternal") &&
				!FuncName.contains("K2_OnMovementModeChanged") &&
				!FuncName.contains("OnJumped") &&
				!FuncName.contains("ServerModifyStat") &&
				!FuncName.contains("OnLanded") &&
				!FuncName.contains("ReceiveHit") &&
				!FuncName.contains("K2_OnEndAbility") &&
				!FuncName.contains("OnWalkingOffLedge") &&
				!FuncName.contains("ServerEndAbility") &&
				!FuncName.contains("Execute") &&
				!FuncName.contains("NetMulticast_InvokeGameplayCueExecuted_FromSpec") &&
				!FuncName.contains("GameplayCue_Damage") &&
				!FuncName.contains("OnDamagePlayEffects") &&
				!FuncName.contains("OnMontageStarted") &&
				!FuncName.contains("OnNewDamageNumber") &&
				!FuncName.contains("BP_GetTokenizedDescriptionText") &&
				!FuncName.contains("NetMulticast_InvokeGameplayCueExecuted_WithParams") &&
				!FuncName.contains("GameplayCue_InstantDeath") &&
				!FuncName.contains("K2_GetActorRotation") &&
				!FuncName.contains("K2_DestroyActor") &&
				!FuncName.contains("OnDetachFromBuilding") &&
				!FuncName.contains("OnRep_bAlreadySearched") &&
				!FuncName.contains("OnSetSearched") &&
				!FuncName.contains("GetAircraft") &&
				!FuncName.contains("BeginSpawningActorFromClass") &&
				!FuncName.contains("BlueprintInitializeAnimation") &&
				!FuncName.contains("BlueprintUpdateAnimation") &&
				!FuncName.contains("BlueprintPostEvaluateAnimation") &&
				!FuncName.contains("FinishSpawningActor") &&
				!FuncName.contains("PawnUniqueIDSet") &&
				!FuncName.contains("OnRep_Owner") &&
				!FuncName.contains("OnRep_Pawn") &&
				!FuncName.contains("Possess") &&
				!FuncName.contains("ReceivePossessed") &&
				!FuncName.contains("ClientRestart") &&
				!FuncName.contains("SetControlRotation") &&
				!FuncName.contains("ClientRetryClientRestart") &&
				!FuncName.contains("ExecuteUbergraph_PlayerPawn_Athena_Generic") &&
				!FuncName.contains("ExecuteUbergraph_PlayerPawn_Athena") &&
				!FuncName.contains("ServerAcknowledgePossession") &&
				!FuncName.contains("IsInAircraft") &&
				!FuncName.contains("FindPlayerStart") &&
				!FuncName.contains("SpawnDefaultPawnFor") &&
				!FuncName.contains("MustSpectate") &&
				!FuncName.contains("GetDefaultPawnClassForController") &&
				!FuncName.contains("On Game Phase Change") &&
				!FuncName.contains("ClientAdjustPosition") &&
				!FuncName.contains("Movement Audio Crossfader__UpdateFunc") &&
				!FuncName.contains("Holding Audio Crossfader__UpdateFunc") &&
				!FuncName.contains("OnUpdateDirectionalLightForTimeOfDay") &&
				!FuncName.contains("OnMontageEnded") &&
				!FuncName.contains("ServerCancelAbility") &&
				!FuncName.contains("K2_ActivateAbility") &&
				!FuncName.contains("ServerHandleMissionEvent_ToggledCursorMode") &&
				!FuncName.contains("OnBlendOut_") &&
				!FuncName.contains("ClientEndAbility") &&
				!FuncName.contains("OnSafeZoneStateChange") &&
				!FuncName.contains("ClientVeryShortAdjustPosition") &&
				!FuncName.contains("OnDayPhaseChange") &&
				!FuncName.contains("On Day Phase Change") &&
				!FuncName.contains("K2_OnStartCrouch") &&
				!FuncName.contains("K2_OnEndCrouch") &&
				!FuncName.contains("On Player Won") &&
				!FuncName.contains("ClientFinishedInteractionInZone") &&
				!FuncName.contains("ClientReceiveKillNotification") &&
				!FuncName.contains("ReceiveCopyProperties") &&
				!FuncName.contains("K2_OnLogout") &&
				!FuncName.contains("ClientReceiveLocalizedMessage") &&
				!FuncName.contains("ClientCancelAbility") &&
				!FuncName.contains("ServerFinishedInteractionInZoneReport") &&
				!FuncName.contains("FallingTimeline__UpdateFunc") &&
				!FuncName.contains("BndEvt__InterceptCollision_K2Node_ComponentBoundEvent_5_ComponentBeginOverlapSignature__DelegateSignature") &&
				!FuncName.contains("ReceiveActorBeginOverlap") &&
				!FuncName.contains("Conv_StringToName") &&
				!FuncName.contains("OnRep_GamePhase") &&
				!FuncName.contains("K2_OnSetMatchState") &&
				!FuncName.contains("StartPlay") &&
				!FuncName.contains("StartMatch") &&
				!FuncName.contains("OnAircraftEnteredDropZone") &&
				!FuncName.contains("ServerShortTimeout") &&
				!FuncName.contains("UpdateStateWidgetContent") &&
				!FuncName.contains("PreConstruct") &&
				!FuncName.contains("Construct") &&
				!FuncName.contains("OnCurrentTextStyleChanged") &&
				!FuncName.contains("UpdateButtonState") &&
				!FuncName.contains("OnBangStateChanged") &&
				!FuncName.contains("OnPlayerInfoChanged") &&
				!FuncName.contains("Update") &&
				!FuncName.contains("OnBeginIntro") &&
				!FuncName.contains("HandleQuickBarChangedBP") &&
				!FuncName.contains("HandleInventoryUpdatedEvent") &&
				!FuncName.contains("OnActivated") &&
				!FuncName.contains("OnBeginOutro") &&
				!FuncName.contains("HandleActiveWidgetDeactivated") &&
				!FuncName.contains("OnDeactivated") &&
				!FuncName.contains("OnStateStarted") &&
				!FuncName.contains("SetRenderTransform") &&
				!FuncName.contains("OnAnimationFinished") &&
				!FuncName.contains("ReadyToStartMatch") &&
				!FuncName.contains("SetWidthOverride") &&
				!FuncName.contains("SetHeightOverride") &&
				!FuncName.contains("HandleMinimizeFinished") &&
				!FuncName.contains("ServerUpdateLevelVisibility") &&
				!FuncName.contains("OnDayPhaseChanged") &&
				!FuncName.contains("ServerLoadingScreenDropped") &&
				!FuncName.contains("On Game Phase Step Changed") &&
				!FuncName.contains("HandleGamePhaseStepChanged") &&
				!FuncName.contains("GamePhaseStepChanged") &&
				!FuncName.contains("SetColorAndOpacity") &&
				!FuncName.contains("OnAnimationStarted") &&
				!FuncName.contains("UpdateMessaging") &&
				!FuncName.contains("ServerSendLoadoutConfig") &&
				!FuncName.contains("CalculateBaseMagnitude") &&
				!FuncName.contains("ClientRegisterWithParty") &&
				!FuncName.contains("InitializeHUDForPlayer") &&
				!FuncName.contains("ClientSetHUD") &&
				!FuncName.contains("ClientEnableNetworkVoice") &&
				!FuncName.contains("ClientUpdateMultipleLevelsStreamingStatus") &&
				!FuncName.contains("ClientFlushLevelStreaming") &&
				!FuncName.contains("ClientOnGenericPlayerInitialization") &&
				!FuncName.contains("ClientCapBandwidth") &&
				!FuncName.contains("K2_PostLogin") &&
				!FuncName.contains("OnRep_bHasStartedPlaying") &&
				!FuncName.contains("ServerChoosePart") &&
				!FuncName.contains("SetOwner") &&
				!FuncName.contains("OnRep_QuickBar") &&
				!FuncName.contains("HandleStartingNewPlayer") &&
				!FuncName.contains("ServerUpdateMultipleLevelsVisibility") &&
				!FuncName.contains("ServerSetPartyOwner") &&
				!FuncName.contains("PlayerCanRestart") &&
				!FuncName.contains("ServerCreateCombatManager") &&
				!FuncName.contains("ServerCreateAIDirectorDataManager") &&
				!FuncName.contains("EnableSlot") &&
				!FuncName.contains("DisableSlot") &&
				!FuncName.contains("ServerSetShowHeroBackpack") &&
				!FuncName.contains("ServerSetShowHeroHeadAccessories") &&
				!FuncName.contains("ServerSetClientHasFinishedLoading") &&
				!FuncName.contains("ServerReadyToStartMatch") &&
				!FuncName.contains("Received_Notify") &&
				!FuncName.contains("Received_NotifyBegin") &&
				!FuncName.contains("AnimNotify_LeftFootStep") &&
				!FuncName.contains("AnimNotify_RightFootStep") &&
				!FuncName.contains("Completed_") &&
				!FuncName.contains("InputActionHoldStopped") &&
				!FuncName.contains("ServerCurrentMontageSetPlayRate") &&
				!FuncName.contains("AnimNotify_PlayFireFX") &&
				!FuncName.contains("ServerSetReplicatedTargetData") &&
				!FuncName.contains("Triggered_") &&
				!FuncName.contains("ActorHasTag") &&
				!FuncName.contains("RandomIntegerInRange") &&
				!FuncName.contains("GetItemDefinitionBP") &&
				!FuncName.contains("CreateTemporaryItemInstanceBP") &&
				!FuncName.contains("SetOwningControllerForTemporaryItem") &&
				!FuncName.contains("OnRep_PrimaryQuickBar") &&
				!FuncName.contains("OnRep_SecondaryQuickBar") &&
				!FuncName.contains("ServerSetupWeakSpotsOnBuildingActor") &&
				!FuncName.contains("OnStartDirectionEffect") &&
				!FuncName.contains("NetMulticast_Athena_BatchedDamageCues") &&
				!FuncName.contains("SetReplicates") &&
				!FuncName.contains("ServerCurrentMontageSetNextSectionName") &&
				!FuncName.contains("NetFadeOut") &&
				!FuncName.contains("OnFadeOut") &&
				!FuncName.contains("NetOnHitCrack") &&
				!FuncName.contains("OnHitCrack") &&
				!FuncName.contains("NetMulticast_InvokeGameplayCueAdded_WithParams") &&
				!FuncName.contains("GameplayCue") &&
				!FuncName.contains("ReceiveActorEndOverlap") &&
				!FuncName.contains("PhysicsVolumeChanged") &&
				!FuncName.contains("ServerAddItemInternal") &&
				!FuncName.contains("FortClientPlaySound") &&
				!FuncName.contains("OnCapsuleBeginOverlap") &&
				!FuncName.contains("GetPlayerController") &&
				!FuncName.contains("TossPickup") &&
				!FuncName.contains("OnRep_PrimaryPickupItemEntry") &&
				!FuncName.contains("ServerActivateSlotInternal") &&
				!FuncName.contains("EquipWeaponDefinition") &&
				!FuncName.contains("OnInitAlteration") &&
				!FuncName.contains("OnInitCosmeticAlterations") &&
				!FuncName.contains("ClientGivenTo") &&
				!FuncName.contains("K2_OnUnEquip") &&
				!FuncName.contains("OnWeaponVisibilityChanged") &&
				!FuncName.contains("OnWeaponAttached") &&
				!FuncName.contains("ClientInternalEquipWeapon") &&
				!FuncName.contains("GetItemGuid") &&
				!FuncName.contains("InternalServerSetTargeting") &&
				!FuncName.contains("ServerReleaseInventoryItemKey") &&
				!FuncName.contains("OnPawnMontageBlendingOut") &&
				!FuncName.contains("OnHitCrack") &&
				!FuncName.contains("OnHitCrack") &&
				!FuncName.contains("OnHitCrack") &&
				!FuncName.contains("OnHitCrack") &&
				!FuncName.contains("OnHitCrack") &&
				!FuncName.contains("OnHitCrack") &&
				!FuncName.contains("OnHitCrack") &&
				!FuncName.contains("OnHitCrack") &&
				!FuncName.contains("OnHitCrack") &&
				!FuncName.contains("EvaluateGraphExposedInputs_ExecuteUbergraph_Fortnite_M_Avg_Player_AnimBlueprint_AnimGraphNode_"))
			{
				LOG("FuncName: " << FuncName);
			}
		}

		if (FuncName.contains("ReadyToStartMatch"))
		{
			if (!bHasInitedTheBeacon) {
				Globals::FortEngine = UObject::FindObject<UFortEngine>("FortEngine_");
				Globals::World = Globals::FortEngine->GameViewport->World;
				Globals::PC = reinterpret_cast<AFortPlayerController*>(Globals::FortEngine->GameInstance->LocalPlayers[0]->PlayerController);
				auto BaseAddr = Util::BaseAddress();

				Beacons::InitHooks(); // Sets up the beacon and inits replication for use!

				bHasInitedTheBeacon = true;

				AFortGameModeAthena* GameMode = (AFortGameModeAthena*)Globals::World->AuthorityGameMode;
				AAthena_GameState_C* GameState = (AAthena_GameState_C*)Globals::World->GameState;

				LOG("World: " << Globals::World->GetName());

				if (Globals::World->GetName().contains("Athena_Faceoff") && GameState)
				{
					EAthenaGamePhase OldGamePhase = GameState->GamePhase;

					LOG("OldGamePhase: " << (int&)OldGamePhase);

					GameState->GamePhase = EAthenaGamePhase::None;
					GameState->OnRep_GamePhase(OldGamePhase);

					MH_CreateHook((LPVOID)(BaseAddr + 0x3FFB60), sub_7FF66CA1FB60Hook, nullptr);
					MH_EnableHook((LPVOID)(BaseAddr + 0x3FFB60));
				}

				if (GameMode)
				{
					GameMode->GameSession->MaxPlayers = 100;
					GameMode->StartMatch();
				}

				Globals::PC->CheatManager->DestroyAll(APlayerController::StaticClass());

				MH_CreateHook((LPVOID)(BaseAddr + 0x249c7c0), GetNetModeHook, nullptr);
				MH_EnableHook((LPVOID)(BaseAddr + 0x249c7c0));
				MH_CreateHook((LPVOID)(BaseAddr + 0x1ecbf80), InternalGetNetModeHook, nullptr);
				MH_EnableHook((LPVOID)(BaseAddr + 0x1ecbf80));

				//*(bool*)GIsClient() = false; // crashes with the EquipWeaponDefinition function
				//*(bool*)GIsServer() = true;

				StaticLoadObject<UBlueprintGeneratedClass>(L"/Game/Abilities/Player/Constructor/Perks/ContainmentUnit/GE_Constructor_ContainmentUnit_Applied.GE_Constructor_ContainmentUnit_Applied_C");
				LoadTables();
			}
		}

		if (FuncName.contains("HandleStartingNewPlayer") && bHasInitedTheBeacon)
		{
			auto Parameters = (AGameModeBase_HandleStartingNewPlayer_Params*)Parms;
			UWorld* World = Globals::World;

			if (World)
			{
				AFortPlayerController* PlayerController = (AFortPlayerController*)Parameters->NewPlayer;

				if (PlayerController)
				{
					AGameModeBase* GameMode = World->AuthorityGameMode;
					AController* Player = Parameters->NewPlayer;

					if (GameMode && Player)
					{
						AActor* PlayerStart = GameMode->ChoosePlayerStart(Player);

						if (PlayerStart)
						{
							FVector PlayerStartLocation = PlayerStart->K2_GetActorLocation();
							FRotator PlayerStartRotation = PlayerStart->K2_GetActorRotation();

							if (!bSetupCharPartArray)
							{
								LoadCharacterParts();
								bSetupCharPartArray = true;
							}

							/*if (!bEverythingSearched)
							{
								GPFuncs::MakeEverythingSearched();
								bEverythingSearched = true;
							}*/

							/*if (!bSetupFloorLoot)
							{
								GPFuncs::SpawnFloorLoot();
								bSetupFloorLoot = true;
							}*/

							GPFuncs::SpawnPlayer(PlayerController, PlayerStartLocation, PlayerStartRotation);
						}
					}
				}
			}
		}

		if (FuncName.contains("ServerAttemptAircraftJump"))
		{
			auto Parameters = (AFortPlayerControllerAthena_ServerAttemptAircraftJump_Params*)Parms;

			auto PlayerController = (AFortPlayerControllerAthena*)Object;
			UWorld* World = Globals::World;
			AFortGameModeAthena* GameMode = (AFortGameModeAthena*)World->AuthorityGameMode;
			AFortGameStateAthena* GameState = (AFortGameStateAthena*)World->GameState;

			if (PlayerController && World && GameMode && GameState)
			{
				if (PlayerController->IsInAircraft() && !PlayerController->Pawn)
				{
					AFortAthenaAircraft* Aircraft = GameState->GetAircraft();

					if (Aircraft)
					{
						FVector AircraftLocation = Aircraft->K2_GetActorLocation();
						FRotator ClientRotation = Parameters->ClientRotation;

						GPFuncs::SpawnPlayer(PlayerController, AircraftLocation, {}, false);
						PlayerController->SetControlRotation(ClientRotation);

						Inventory* PlayerInventory = FindInventory(PlayerController);

						if (PlayerInventory)
						{
							UFortWorldItem* ItemInstance = PlayerInventory->GetItemSlot(0, EFortQuickBars::Primary);

							if (ItemInstance) {
								PlayerController->ServerExecuteInventoryItem(ItemInstance->GetItemGuid());
							}

							PlayerInventory->RemoveAllItemsFromInventory();
							PlayerInventory->UpdateInventory();
						}
					}
				}
			}
		}

		if (FuncName.contains("AircraftExitedDropZone"))
		{
			UWorld* World = Globals::World;

			if (World && World->NetDriver)
			{
				for (int i = 0; i < World->NetDriver->ClientConnections.Num(); i++)
				{
					auto ClientConnection = World->NetDriver->ClientConnections[i];

					if (ClientConnection)
					{
						auto PlayerController = (AFortPlayerControllerAthena*)ClientConnection->PlayerController;
						if (!PlayerController) continue;
						if (PlayerController->Pawn) continue;
						if (!PlayerController->IsInAircraft()) continue;

						PlayerController->ServerAttemptAircraftJump({});
					}
				}
			}
		}

		if (FuncName.contains("ServerTryActivateAbility"))
		{
			auto Parameters = (UAbilitySystemComponent_ServerTryActivateAbility_Params*)Parms;
			auto AbilitySystemComponent = (UAbilitySystemComponent*)Object;

			if (AbilitySystemComponent)
			{
				FGameplayAbilitySpecHandle& AbilityToActivate = Parameters->AbilityToActivate;
				FPredictionKey& PredictionKey = Parameters->PredictionKey;

				Abilities::InternalServerTryActiveAbility(AbilitySystemComponent, AbilityToActivate, Parameters->InputPressed, PredictionKey, nullptr);
			}
		}
		
		if (FuncName.contains("ServerAbilityRPCBatch"))
		{
			auto Parameters = (UAbilitySystemComponent_ServerAbilityRPCBatch_Params*)Parms;
			auto AbilitySystemComponent = (UAbilitySystemComponent*)Object;

			if (AbilitySystemComponent)
			{
				auto BatchInfo = Parameters->BatchInfo;

				FGameplayAbilitySpecHandle& AbilityToActivate = BatchInfo.AbilitySpecHandle;
				FPredictionKey& PredictionKey = BatchInfo.PredictionKey;

				Abilities::InternalServerTryActiveAbility(AbilitySystemComponent, AbilityToActivate, BatchInfo.InputPressed, PredictionKey, nullptr);
			}
		}
		
		if (FuncName.contains("ServerExecuteInventoryItem"))
		{
			auto Params = (AFortPlayerController_ServerExecuteInventoryItem_Params*)Parms;
			auto PlayerController = (AFortPlayerControllerAthena*)Object;

			if (PlayerController)
			{
				if (PlayerController->IsInAircraft())
					return ProcessEvent(Object, Function, Parms);

				Inventory* PlayerInventory = FindInventory(PlayerController);
				AFortPlayerPawn* Pawn = (AFortPlayerPawn*)PlayerController->Pawn;

				if (PlayerInventory && Pawn)
				{
					UFortWorldItem* ItemInstance = PlayerInventory->GetItemInstance(Params->ItemGuid);

					if (ItemInstance)
					{
						UFortItemDefinition* ItemDefinition = ItemInstance->GetItemDefinitionBP();
						FFortItemEntry ItemEntry = ItemInstance->ItemEntry;

						if (ItemDefinition)
						{
							AFortWeapon* Weapon = Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)ItemDefinition, ItemInstance->GetItemGuid());

							if (Weapon && Weapon->IsA(AFortWeap_BuildingTool::StaticClass())) // From Reboot
							{
								AFortWeap_BuildingTool* BuildingTool = (AFortWeap_BuildingTool*)Weapon;

								if (BuildingTool)
								{
									static auto RoofPiece = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS");
									static auto FloorPiece = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor");
									static auto WallPiece = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall");
									static auto StairPiece = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W");

									if (ItemDefinition == RoofPiece)
									{
										static auto RoofMetadata = FindObjectFast<UBuildingEditModeMetadata>("/Game/Building/EditModePatterns/Roof/EMP_Roof_RoofC.EMP_Roof_RoofC");
										BuildingTool->DefaultMetadata = RoofMetadata;
									}
									else if (ItemDefinition == StairPiece)
									{
										static auto StairMetadata = FindObjectFast<UBuildingEditModeMetadata>("/Game/Building/EditModePatterns/Stair/EMP_Stair_StairW.EMP_Stair_StairW");
										BuildingTool->DefaultMetadata = StairMetadata;
									}
									else if (ItemDefinition == WallPiece)
									{
										static auto WallMetadata = FindObjectFast<UBuildingEditModeMetadata>("/Game/Building/EditModePatterns/Wall/EMP_Wall_Solid.EMP_Wall_Solid");
										BuildingTool->DefaultMetadata = WallMetadata;
									}
									else if (ItemDefinition == FloorPiece)
									{
										static auto FloorMetadata = FindObjectFast<UBuildingEditModeMetadata>("/Game/Building/EditModePatterns/Floor/EMP_Floor_Floor.EMP_Floor_Floor");
										BuildingTool->DefaultMetadata = FloorMetadata;
									}

									BuildingTool->OnRep_DefaultMetadata();
								}
							}
							else if (ItemDefinition->IsA(UFortDecoItemDefinition::StaticClass()))
							{
								UFortDecoItemDefinition* DecoItemDefinition = (UFortDecoItemDefinition*)ItemDefinition;

								Pawn->PickUpActor(nullptr, DecoItemDefinition);
								Pawn->CurrentWeapon->ItemEntryGuid = ItemInstance->GetItemGuid();
							}
						}
					}
				}
			}
		}

		if (FuncName.contains("ClientOnPawnDied"))
		{
			auto Params = (AFortPlayerControllerZone_ClientOnPawnDied_Params*)Parms;
			auto PlayerController = (AFortPlayerControllerZone*)Object;

			if (PlayerController && PlayerController->Pawn)
			{
				FFortPlayerDeathReport DeathReport = Params->DeathReport;
				Inventory* PlayerInventory = FindInventory(PlayerController);

				ABP_VictoryDrone_C* VictoryDrone = Util::SpawnActor<ABP_VictoryDrone_C>(ABP_VictoryDrone_C::StaticClass(), PlayerController->Pawn->K2_GetActorLocation());

				VictoryDrone->PlaySpawnOutAnim();

				if (PlayerInventory)
				{
					PlayerInventory->DropAllItemsFromInventory();
					Inventory::InventoryMap.erase(PlayerController);
				}
			}
		}

		if (FuncName.contains("OnCapsuleBeginOverlap"))
		{
			auto Params = (AFortPlayerPawnAthena_OnCapsuleBeginOverlap_Params*)Parms;
			auto Pawn = (AFortPlayerPawnAthena*)Object;

#ifdef AUTO_PICKUP
			if (Pawn)
			{
				auto PlayerController = (AFortPlayerControllerAthena*)Pawn->Controller;

				if (PlayerController && Params->OtherActor->IsA(AFortPickupAthena::StaticClass()))
				{
					AFortPickupAthena* Pickup = (AFortPickupAthena*)Params->OtherActor;

					if (Pickup)
					{
						UFortItemDefinition* ItemDefinition = Pickup->PrimaryPickupItemEntry.ItemDefinition;

						if (ItemDefinition)
						{
							if (ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass()) ||
								ItemDefinition->IsA(UFortTrapItemDefinition::StaticClass()) ||
								ItemDefinition->IsA(UFortResourceItemDefinition::StaticClass()))
							{
								float FlyTime = Globals::MathLib->STATIC_RandomFloatInRange(1.40f, 1.46f);

								Pawn->ServerHandlePickup((AFortPickupAthena*)Params->OtherActor, FlyTime, FVector(), true);
							}
						}
					}
				}
			}
#endif
		}

		if (FuncName.contains("OnDamageServer"))
		{
			auto Params = (ABuildingActor_OnDamageServer_Params*)Parms;
			auto Building = (ABuildingActor*)Object;

			if (Building && Params->DamageCauser && Params->InstigatedBy && Building->IsA(ABuildingSMActor::StaticClass()))
			{
				AFortPlayerControllerAthena* PlayerController = (AFortPlayerControllerAthena*)Params->InstigatedBy;
				ABuildingSMActor* BuildingActor = (ABuildingSMActor*)Building;
				AFortWeapon* Weapon = (AFortWeapon*)Params->DamageCauser;

				if (BuildingActor && Weapon && Weapon->WeaponData)
				{
					if (PlayerController && PlayerController->Pawn && !BuildingActor->bDestroyed && Weapon->WeaponData->IsA(UFortWeaponMeleeItemDefinition::StaticClass()))
					{
						Inventory* PlayerInventory = FindInventory(PlayerController);
						UFortResourceItemDefinition* ItemDefinition = nullptr;

						if (BuildingActor->ResourceType == EFortResourceType::Wood)
						{
							static auto WoodItemData = FindObjectFast<UFortResourceItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
							ItemDefinition = WoodItemData;
						}
						else if (BuildingActor->ResourceType == EFortResourceType::Stone)
						{
							static auto StoneItemData = FindObjectFast<UFortResourceItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
							ItemDefinition = StoneItemData;
						}
						else if (BuildingActor->ResourceType == EFortResourceType::Metal)
						{
							static auto MetalItemData = FindObjectFast<UFortResourceItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");
							ItemDefinition = MetalItemData;
						}

						if (PlayerInventory && ItemDefinition && BuildingActor->BuildingResourceAmountOverride.RowName.IsValid())
						{
							UCurveTable* CurveTable = FindObjectFast<UCurveTable>("/Game/Athena/Balance/DataTables/AthenaResourceRates.AthenaResourceRates");

							if (!CurveTable)
							{
								StaticLoadObject<UCurveTable>(L"/Game/Athena/Balance/DataTables/AthenaResourceRates.AthenaResourceRates");
								CurveTable = FindObjectFast<UCurveTable>("/Game/Athena/Balance/DataTables/AthenaResourceRates.AthenaResourceRates");
								LOG("AthenaResourceRates Not Found!");
							}								

							if (CurveTable)
							{
								TEnumAsByte<EEvaluateCurveTableResult> OutResult;
								FString ContextString;
								float InXY = 0.f;
								float OutXY;

								Globals::DTFunctionLibrary->STATIC_EvaluateCurveTableRow(CurveTable, BuildingActor->BuildingResourceAmountOverride.RowName, InXY, ContextString, &OutResult, &OutXY);

								int PotentialResourceCount = OutXY / (BuildingActor->GetMaxHealth() / Params->Damage);
								int ResourceCount = round(PotentialResourceCount);

								if (ResourceCount > 0)
								{
									PlayerController->ClientReportDamagedResourceBuilding(BuildingActor, BuildingActor->ResourceType, PotentialResourceCount, false, (Params->Damage == 100.f));

									FGuid ItemGuid = PlayerInventory->GetItemGuid(ItemDefinition);
									UFortWorldItem* ItemInstance = PlayerInventory->GetItemInstance(ItemGuid);
									int CountToRemove = 0;

									if (ItemInstance)
									{
										PlayerInventory->RemoveItemFromInventory(ItemInstance->GetItemGuid());

										ItemInstance->ItemEntry.Count = ItemInstance->ItemEntry.Count + ResourceCount;

										if (ItemInstance->ItemEntry.Count > ItemDefinition->MaxStackSize)
										{
											CountToRemove = ItemInstance->ItemEntry.Count - ItemDefinition->MaxStackSize;
											ItemInstance->ItemEntry.Count = ItemDefinition->MaxStackSize;
										}

										PlayerInventory->AddInventoryItem(ItemDefinition, ItemInstance->ItemEntry);
									}
									else
									{
										FFortItemEntry ItemEntry;
										ItemEntry.Count = ResourceCount;

										PlayerInventory->AddInventoryItem(ItemDefinition, ItemEntry);
									}

									if (CountToRemove > 0)
									{
										FFortItemEntry ItemEntry;
										ItemEntry.Count = CountToRemove;
										SpawnItem(ItemDefinition, ItemEntry, (AFortPawn*)PlayerController->Pawn, PlayerController->Pawn->K2_GetActorLocation());
									}
								}
							}

							PlayerInventory->UpdateInventory();
						}
					}
				}
			}
		}

		if (FuncName.contains("ServerHandlePickup"))
		{
			auto Params = (AFortPlayerPawn_ServerHandlePickup_Params*)Parms;
			AFortPlayerPawn* Pawn = (AFortPlayerPawn*)Object;
			AFortPickup* Pickup = Params->Pickup;

			if (Pickup)
			{
				if (!Pawn || Pickup->bPickedUp)
					return NULL;

				Pawn->IncomingPickups.Add(Pickup);

				FFortPickupLocationData PickupLocationData = Pickup->PickupLocationData;

				PickupLocationData.ItemOwner = Pawn;
				PickupLocationData.PickupTarget = Pawn;
				PickupLocationData.FlyTime = Params->InFlyTime;
				PickupLocationData.StartDirection == Params->InStartDirection;
				PickupLocationData.PickupGuid = Pickup->PrimaryPickupItemEntry.ItemGuid;
				Pickup->OnRep_PickupLocationData();
			}
		}

		if (FuncName.contains("ServerSpawnInventoryDrop"))
		{
			auto Params = (AFortPlayerController_ServerSpawnInventoryDrop_Params*)Parms;
			auto PlayerController = (AFortPlayerControllerAthena*)Object;

			if (PlayerController)
			{
				if (PlayerController->IsInAircraft())
					return NULL;

				AFortInventory* WorldInventory = PlayerController->WorldInventory;
				Inventory* PlayerInventory = FindInventory(PlayerController);
				AFortQuickBars* QuickBars = PlayerController->QuickBars;
				AFortPawn* Pawn = (AFortPawn*)PlayerController->Pawn;

				if (WorldInventory && PlayerInventory && QuickBars && Pawn)
				{
					UFortWorldItem* ItemInstance = PlayerInventory->GetItemInstance(Params->ItemGuid);

					if (ItemInstance)
					{
						UFortWorldItemDefinition* ItemDefinition = (UFortWorldItemDefinition*)ItemInstance->GetItemDefinitionBP();
						FFortItemEntry ItemEntry = ItemInstance->ItemEntry;
						bool bRemoveItemFromQuickbar = false;
						int ItemSlot = -1;

						if (ItemDefinition && ItemDefinition->bCanBeDropped)
						{
							FGuid ItemGuid = ItemInstance->GetItemGuid();
							ItemSlot = PlayerInventory->GetItemSlotQuickbar(ItemDefinition, EFortQuickBars::Primary);

							if (ItemEntry.Count == Params->Count)
							{
								PlayerInventory->DropItemFromInventory(ItemGuid);
								PlayerInventory->RemoveItemFromQuickbar(ItemGuid);
								PlayerInventory->RemoveItemFromInventory(ItemGuid);

								bRemoveItemFromQuickbar = true;
							}
							else if (Params->Count < ItemEntry.Count)
							{
								PlayerInventory->RemoveItemFromQuickbar(ItemGuid);
								PlayerInventory->RemoveItemFromInventory(ItemInstance->GetItemGuid());

								ItemEntry.Count = ItemEntry.Count - Params->Count;
								UFortWorldItem* NewPickupWorldItem = PlayerInventory->AddInventoryItem(ItemDefinition, ItemEntry);

								if (ItemSlot != -1)
									PlayerInventory->AddQuickBarItem(NewPickupWorldItem->GetItemGuid(), ItemSlot, EFortQuickBars::Primary);

								ItemEntry.Count = Params->Count;
								SpawnItem(ItemDefinition, ItemEntry, Pawn, Pawn->K2_GetActorLocation());
							}
						}

						if (bRemoveItemFromQuickbar && QuickBars->PrimaryQuickBar.CurrentFocusedSlot == ItemSlot)
						{
							UFortWorldItem* PickaxeInstance = PlayerInventory->GetItemSlot(0, EFortQuickBars::Primary);

							if (PickaxeInstance)
							{
								PlayerInventory->SetQuickBarSlot(0, EFortQuickBars::Primary);
							}
						}
					}

					PlayerInventory->UpdateInventory();
				}
			}
		}

		if (FuncName.contains("ServerCheat"))
		{
			auto Params = (AFortPlayerController_ServerCheat_Params*)Parms;
			auto PlayerController = (AFortPlayerControllerAthena*)Object;

#ifdef CHEATS
			if (PlayerController && Params->Msg.IsValid())
			{
				std::string Command = Params->Msg.ToString();
				std::vector<std::string> ParsedCommand = split(Command, ' ');
				AFortPawn* Pawn = (AFortPawn*)PlayerController->Pawn;

				if (!ParsedCommand.empty())
				{
					std::string Action = ParsedCommand[0];
					std::transform(Action.begin(), Action.end(), Action.begin(),
						[](unsigned char c) { return std::tolower(c); });

					FString Message = L"Unknown Command";

					if (Action == "freebuild")
					{
						PlayerController->bBuildFree = PlayerController->bBuildFree ? false : true;
						Message = PlayerController->bBuildFree ? L"FreeBuild on" : L"FreeBuild off";
					}
					else if (Action == "infiniteammo")
					{
						PlayerController->bInfiniteAmmo = PlayerController->bInfiniteAmmo ? false : true;
						Message = PlayerController->bInfiniteAmmo ? L"InfiniteAmmo on" : L"InfiniteAmmo off";
					}
					else if (Action == "godmode" && Pawn)
					{
						Pawn->bIsInvulnerable = Pawn->bIsInvulnerable ? false : true;
						Message = Pawn->bIsInvulnerable ? L"GodMode on" : L"GodMode off";
					}
					else if (Action == "giveweapon" && ParsedCommand.size() == 4 && Pawn)
					{
						std::string WeaponName = ParsedCommand[1];

						bool bIsQuantityInt = std::all_of(ParsedCommand[2].begin(), ParsedCommand[2].end(), ::isdigit);
						bool bIsLoadedAmmoInt = std::all_of(ParsedCommand[3].begin(), ParsedCommand[3].end(), ::isdigit);

						if (bIsQuantityInt && bIsLoadedAmmoInt)
						{
							int Quantity = std::stoi(ParsedCommand[2]);
							int LoadedAmmo = std::stoi(ParsedCommand[3]);

							UFortWorldItemDefinition* ItemDefinition = FindObjectFast<UFortWorldItemDefinition>("/Game/Athena/Items/Weapons/" + WeaponName + "." + WeaponName);

							if (ItemDefinition)
							{
								FFortItemEntry ItemEntry;
								ItemEntry.Count = Quantity;
								ItemEntry.LoadedAmmo = LoadedAmmo;

								SpawnItem(ItemDefinition, ItemEntry, Pawn, Pawn->K2_GetActorLocation());

								Message = L"Weapon Give!";
							}
							else
							{
								Message = L"The weapon does not exist";
							}
						}
					}
					else if (Action == "givetrap" && ParsedCommand.size() >= 3 && Pawn)
					{
						std::string TrapName = ParsedCommand[1];

						bool bIsQuantityInt = std::all_of(ParsedCommand[2].begin(), ParsedCommand[2].end(), ::isdigit);

						if (bIsQuantityInt)
						{
							int Quantity = std::stoi(ParsedCommand[2]);

							UFortWorldItemDefinition* ItemDefinition = FindObjectFast<UFortWorldItemDefinition>("/Game/Athena/Items/Traps/" + TrapName + "." + TrapName);

							if (ItemDefinition)
							{
								FFortItemEntry ItemEntry;
								ItemEntry.Count = Quantity;

								SpawnItem(ItemDefinition, ItemEntry, Pawn, Pawn->K2_GetActorLocation());

								Message = L"Trap Give!";
							}
							else
							{
								Message = L"The trap does not exist";
							}
						}
					}
					else if (Action == "spawnloot" && ParsedCommand.size() >= 2 && Pawn)
					{
						std::string TypeLoot = ParsedCommand[1];

						
					}

					PlayerController->ClientMessage(Message, FName(), 1);
				}

				LOG("Msg: " << Command);
			}
#endif // CHEATS
		}


		if (FuncName.contains("ServerCreateBuildingActor"))
		{
			auto Params = (AFortPlayerController_ServerCreateBuildingActor_Params*)Parms;
			auto PlayerController = (AFortPlayerController*)Object;

			if (PlayerController && PlayerController->Pawn)
			{
				AFortPawn* Pawn = (AFortPawn*)PlayerController->Pawn;

#ifdef CHEATS
				if (PlayerController->bBuildFree)
					return ProcessEvent(Object, Function, Parms);
#endif // CHEATS

				if (Pawn)
				{
					AFortQuickBars* QuickBars = PlayerController->QuickBars;
					Inventory* PlayerInventory = FindInventory(PlayerController);
					AFortInventory* WorldInventory = PlayerController->WorldInventory;

					if (QuickBars && PlayerInventory && WorldInventory)
					{
						int ItemSlot = QuickBars->SecondaryQuickBar.CurrentFocusedSlot;
						UFortWorldItem* ItemInstance = PlayerInventory->GetItemSlot(ItemSlot, EFortQuickBars::Secondary);
						UClass* BuildingClass = Params->BuildingClassData.BuildingClass;

						UFortResourceItemDefinition* ItemDefinition = nullptr;

						if (BuildingClass->GetName().contains("_W1_")) // Find another system
						{
							static auto WoodItemData = FindObjectFast<UFortResourceItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
							ItemDefinition = WoodItemData;
						}
						else if (BuildingClass->GetName().contains("_S1_"))
						{
							static auto StoneItemData = FindObjectFast<UFortResourceItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
							ItemDefinition = StoneItemData;
						}
						else if (BuildingClass->GetName().contains("_M1_"))
						{
							static auto MetalItemData = FindObjectFast<UFortResourceItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");
							ItemDefinition = MetalItemData;
						}

						if (ItemDefinition)
						{
							FGuid ItemGuid = PlayerInventory->GetItemGuid(ItemDefinition);
							UFortWorldItem* ItemInstance = PlayerInventory->GetItemInstance(ItemGuid);

							if (ItemInstance)
							{
								if (ItemInstance->ItemEntry.Count > 0)
									ProcessEvent(Object, Function, Parms);

								PlayerInventory->RemoveItemFromInventory(ItemInstance->GetItemGuid());

								ItemInstance->ItemEntry.Count = ItemInstance->ItemEntry.Count - 10;

								if (ItemInstance->ItemEntry.Count > 0)
								{
									PlayerInventory->AddInventoryItem(ItemDefinition, ItemInstance->ItemEntry);
								}
							}
						}

						PlayerInventory->UpdateInventory();
					}
				}
			}

			return NULL;
		}

		if (FuncName.contains("ServerSpawnDeco"))
		{
			auto Params = (AFortDecoTool_ServerSpawnDeco_Params*)Parms;
			auto DecoTool = (AFortDecoTool*)Object;

			if (DecoTool)
			{
				AFortPawn* Pawn = (AFortPawn*)DecoTool->Owner;

				if (Pawn)
				{
					AFortPlayerControllerAthena* PlayerController = (AFortPlayerControllerAthena*)Pawn->Controller;
					Inventory* PlayerInventory = FindInventory(PlayerController);

					if (PlayerController && PlayerInventory && DecoTool->WeaponData)
					{
						UFortWeaponItemDefinition* ItemDefinition = DecoTool->WeaponData;
						FGuid ItemGuid = PlayerInventory->GetItemGuid(ItemDefinition);
						UFortWorldItem* ItemInstance = PlayerInventory->GetItemInstance(ItemGuid);
						AFortInventory* WorldInventory = PlayerController->WorldInventory;

#ifdef CHEATS
						if (PlayerController->bInfiniteAmmo)
							return ProcessEvent(Object, Function, Parms);
#endif // CHEATS

						int TrapSlot = PlayerInventory->GetItemSlotQuickbar(ItemDefinition, EFortQuickBars::Secondary);

						if (ItemInstance && WorldInventory)
						{
							ProcessEvent(Object, Function, Parms);

							if (TrapSlot != -1)
								PlayerInventory->RemoveItemFromQuickbar(ItemInstance->GetItemGuid(), EFortQuickBars::Secondary);

							PlayerInventory->RemoveItemFromInventory(ItemInstance->GetItemGuid());

							ItemInstance->ItemEntry.Count = ItemInstance->ItemEntry.Count - 1;

							if (ItemInstance->ItemEntry.Count > 0)
							{
								UFortWorldItem* NewPickupWorldItem = PlayerInventory->AddInventoryItem(ItemDefinition, ItemInstance->ItemEntry);

								if (TrapSlot != -1)
									PlayerInventory->AddQuickBarItem(NewPickupWorldItem->GetItemGuid(), TrapSlot, EFortQuickBars::Secondary);
							}
							else
							{
								for (int i = 0; i < WorldInventory->Inventory.ItemInstances.Num(); i++)
								{
									UFortWorldItem* ItemInstance = WorldInventory->Inventory.ItemInstances[i];
									if (!ItemInstance) continue;

									EFortItemType ItemType = ItemInstance->GetType();
									if (ItemType != EFortItemType::Trap) continue;

									PlayerInventory->AddQuickBarItem(ItemInstance->GetItemGuid(), 4, EFortQuickBars::Secondary);
									PlayerInventory->UpdateInventory();

									return NULL;
								}

								UFortWorldItem* PickaxeInstance = PlayerInventory->GetItemSlot(0, EFortQuickBars::Primary);

								if (PickaxeInstance)
									PlayerController->ServerExecuteInventoryItem(PickaxeInstance->GetItemGuid());
							}
						}

						PlayerInventory->UpdateInventory();
					}
				}
			}

			return NULL;
		}

		if (FuncName.contains("OnBuildingActorInitialized"))
		{
			auto Params = (ABuildingActor_OnBuildingActorInitialized_Params*)Parms;
			auto Building = (ABuildingActor*)Object;

			if (Building && Building->IsA(ABuildingSMActor::StaticClass()))
			{
				ABuildingSMActor* BuildingActor = (ABuildingSMActor*)Building;
				auto PlayerController = (AFortPlayerControllerAthena*)BuildingActor->GetOwningController();

				if (PlayerController)
				{
					AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)PlayerController->PlayerState;

					if (PlayerState)
					{
						BuildingActor->bPlayerPlaced = true;
						BuildingActor->Team = PlayerState->TeamIndex;
					}
				}
			}
		}

		if (FuncName.contains("OnSpawnOutAnimEnded"))
		{
			ABP_VictoryDrone_C* VictoryDrone = (ABP_VictoryDrone_C*)Object;

			if (VictoryDrone)
				VictoryDrone->K2_DestroyActor();
		}

		if (FuncName.contains("OnDeathServer"))
		{
			AFortPawn* Pawn = (AFortPawn*)Object;

			if (Pawn)
				Pawn->K2_DestroyActor();
		}

		if (FuncName.contains("ServerAttemptInteract"))
		{
			auto PlayerController = (AFortPlayerControllerAthena*)Object;
			auto CurrentParams = (AFortPlayerController_ServerAttemptInteract_Params*)Parms;

			auto ReceivingActor = CurrentParams->ReceivingActor;
			if (!ReceivingActor)
				return NULL;

			if (ReceivingActor->IsA(ABuildingContainer::StaticClass()))
			{
				if (((ABuildingContainer*)ReceivingActor)->bAlreadySearched)
					return ProcessEvent(Object, Function, Parms);
			}

			if (ReceivingActor && ReceivingActor->Class->GetName().contains("Tiered_Short_Ammo"))
			{
				auto AmmoBox = (ABuildingContainer*)ReceivingActor;
				AmmoBox->bAlreadySearched = true;
				AmmoBox->OnRep_bAlreadySearched();

				auto Location = ReceivingActor->K2_GetActorLocation();

				auto NewFortPickup = Util::SpawnActor<AFortPickupAthena>(AFortPickupAthena::StaticClass(), Location);
				auto NewFortPickup1 = Util::SpawnActor<AFortPickupAthena>(AFortPickupAthena::StaticClass(), Location);
				auto NewFortPickup2 = Util::SpawnActor<AFortPickupAthena>(AFortPickupAthena::StaticClass(), Location);

				auto AmmoDef = (UFortAmmoItemDefinition*)Globals::Ammo[rand() % Globals::Ammo.size()];
				auto AmmoDef1 = (UFortAmmoItemDefinition*)Globals::Ammo[rand() % Globals::Ammo.size()];
				auto AmmoDef2 = (UFortAmmoItemDefinition*)Globals::Ammo[rand() % Globals::Ammo.size()];

				NewFortPickup->PrimaryPickupItemEntry.ItemDefinition = AmmoDef;
				NewFortPickup1->PrimaryPickupItemEntry.ItemDefinition = AmmoDef1;
				NewFortPickup2->PrimaryPickupItemEntry.ItemDefinition = AmmoDef2;

				NewFortPickup->PrimaryPickupItemEntry.Count = AmmoDef->DropCount * 3;
				NewFortPickup1->PrimaryPickupItemEntry.Count = AmmoDef1->DropCount * 3;
				NewFortPickup2->PrimaryPickupItemEntry.Count = AmmoDef2->DropCount * 3;

				NewFortPickup->OnRep_PrimaryPickupItemEntry();
				NewFortPickup1->OnRep_PrimaryPickupItemEntry();
				NewFortPickup2->OnRep_PrimaryPickupItemEntry();

				NewFortPickup->TossPickup(Location, nullptr, 1);
				NewFortPickup1->TossPickup(Location, nullptr, 1);
				NewFortPickup2->TossPickup(Location, nullptr, 1);
			}

			if (ReceivingActor && ReceivingActor->Class->GetName().contains("Tiered_Chest"))
			{
				auto Chest = (ABuildingContainer*)ReceivingActor;
				Chest->bAlreadySearched = true;
				Chest->OnRep_bAlreadySearched();

				auto Location = ReceivingActor->K2_GetActorLocation();

				auto NewFortPickup = Util::SpawnActor<AFortPickupAthena>(AFortPickupAthena::StaticClass(), Location);

				NewFortPickup->PrimaryPickupItemEntry.Count = 1;

				bool bEpicOrLeg = Globals::MathLib->STATIC_RandomBoolWithWeight(0.1);

				int Index = 0;

				if (bEpicOrLeg)
				{
					Index = Globals::MathLib->STATIC_RandomIntegerInRange(2, 4);
				}
				else {
					Index = Globals::MathLib->STATIC_RandomIntegerInRange(0, 2);
				}

				auto WeaponRarity = std::string(Globals::WeaponArrays.at(Index).c_str());

				if (WeaponRarity == "Common")
				{
					NewFortPickup->PrimaryPickupItemEntry.ItemDefinition = Globals::CommonWeapons[rand() % Globals::CommonWeapons.size()];
				}

				if (WeaponRarity == "UnCommon")
				{
					NewFortPickup->PrimaryPickupItemEntry.ItemDefinition = Globals::UnCommonWeapons[rand() % Globals::UnCommonWeapons.size()];
				}

				if (WeaponRarity == "Rare")
				{
					NewFortPickup->PrimaryPickupItemEntry.ItemDefinition = Globals::RareWeapons[rand() % Globals::RareWeapons.size()];
				}

				if (WeaponRarity == "Epic")
				{
					NewFortPickup->PrimaryPickupItemEntry.ItemDefinition = Globals::EpicWeapons[rand() % Globals::EpicWeapons.size()];
				}

				if (WeaponRarity == "Legendary")
				{
					NewFortPickup->PrimaryPickupItemEntry.ItemDefinition = Globals::LegendaryWeapons[rand() % Globals::LegendaryWeapons.size()];
				}

				if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition)
				{
					if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetFullName().contains("WID_Assault_Auto_"))
					{
						NewFortPickup->PrimaryPickupItemEntry.LoadedAmmo = 30;
					}

					if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetFullName().contains("WID_Assault_SemiAuto_"))
					{
						NewFortPickup->PrimaryPickupItemEntry.LoadedAmmo = 30;
					}

					if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetFullName().contains("WID_Assault_AutoHigh_"))
					{
						NewFortPickup->PrimaryPickupItemEntry.LoadedAmmo = 30;
					}

					if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetFullName().contains("WID_Assault_Surgical_"))
					{
						NewFortPickup->PrimaryPickupItemEntry.LoadedAmmo = 20;
					}

					if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetFullName().contains("WID_Shotgun_Standard_"))
					{
						NewFortPickup->PrimaryPickupItemEntry.LoadedAmmo = 5;
					}

					if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetFullName().contains("WID_Shotgun_SemiAuto_"))
					{
						NewFortPickup->PrimaryPickupItemEntry.LoadedAmmo = 8;
					}

					if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetFullName().contains("WID_Launcher_Grenade_"))
					{
						NewFortPickup->PrimaryPickupItemEntry.LoadedAmmo = 6;
					}

					if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetFullName().contains("WID_Launcher_Rocket_"))
					{
						NewFortPickup->PrimaryPickupItemEntry.LoadedAmmo = 1;
					}

					if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetFullName().contains("WID_Sniper_AMR_"))
					{
						NewFortPickup->PrimaryPickupItemEntry.LoadedAmmo = 4;
					}

					if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetFullName().contains("WID_Sniper_BoltAction_Scope_"))
					{
						NewFortPickup->PrimaryPickupItemEntry.LoadedAmmo = 1;
					}

					if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetFullName().contains("WID_Sniper_Standard_Scope_"))
					{
						NewFortPickup->PrimaryPickupItemEntry.LoadedAmmo = 10;
					}

					if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetFullName().contains("WID_Pistol_SixShooter_"))
					{
						NewFortPickup->PrimaryPickupItemEntry.LoadedAmmo = 6;
					}

					if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetFullName().contains("WID_Pistol_SemiAuto_"))
					{
						NewFortPickup->PrimaryPickupItemEntry.LoadedAmmo = 16;
					}

					if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetFullName().contains("WID_Pistol_Scavenger_"))
					{
						NewFortPickup->PrimaryPickupItemEntry.LoadedAmmo = 30;
					}

					if (NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetFullName().contains("WID_Pistol_AutoHeavy_"))
					{
						NewFortPickup->PrimaryPickupItemEntry.LoadedAmmo = 25;
					}
				}

				NewFortPickup->OnRep_PrimaryPickupItemEntry();
				NewFortPickup->TossPickup(Location, nullptr, 1);

				if (NewFortPickup && NewFortPickup->PrimaryPickupItemEntry.ItemDefinition)
				{
					auto AmmoDefintion = ((UFortWorldItemDefinition*)NewFortPickup->PrimaryPickupItemEntry.ItemDefinition)->GetAmmoWorldItemDefinition_BP();
					auto AmmoPickup = Util::SpawnActor<AFortPickupAthena>(AFortPickupAthena::StaticClass(), NewFortPickup->K2_GetActorLocation());
					AmmoPickup->PrimaryPickupItemEntry.Count = AmmoDefintion->DropCount * 1.25;
					AmmoPickup->PrimaryPickupItemEntry.ItemDefinition = AmmoDefintion;
					AmmoPickup->OnRep_PrimaryPickupItemEntry();
					AmmoPickup->TossPickup(Location, nullptr, 999);
				}

				auto NewFortPickup1 = Util::SpawnActor<AFortPickupAthena>(AFortPickupAthena::StaticClass(), Location);

				NewFortPickup1->PrimaryPickupItemEntry.Count = 1;
				NewFortPickup1->PrimaryPickupItemEntry.ItemDefinition = Globals::Consumables[rand() % Globals::Consumables.size()];
				NewFortPickup1->OnRep_PrimaryPickupItemEntry();
				NewFortPickup1->TossPickup(Location, nullptr, 1);
			}

			if (ReceivingActor && ReceivingActor->Class->GetName().contains("AthenaSupplyDrop_02"))
			{
				auto SupplyDrop = (ABuildingContainer*)ReceivingActor;

				auto Location = ReceivingActor->K2_GetActorLocation();

				auto NewFortPickup = Util::SpawnActor<AFortPickupAthena>(AFortPickupAthena::StaticClass(), Location);

				NewFortPickup->PrimaryPickupItemEntry.Count = 1;
				auto ItemDefinition = Globals::SupplyDrop[rand() % Globals::SupplyDrop.size()];
				NewFortPickup->PrimaryPickupItemEntry.ItemDefinition = ItemDefinition;
				NewFortPickup->OnRep_PrimaryPickupItemEntry();
				NewFortPickup->TossPickup(Location, nullptr, 1);

				auto AmmoDefintion = ((UFortWorldItemDefinition*)NewFortPickup->PrimaryPickupItemEntry.ItemDefinition)->GetAmmoWorldItemDefinition_BP();
				auto AmmoPickup = Util::SpawnActor<AFortPickupAthena>(AFortPickupAthena::StaticClass(), NewFortPickup->K2_GetActorLocation());
				AmmoPickup->PrimaryPickupItemEntry.Count = 30;
				AmmoPickup->OnRep_PrimaryPickupItemEntry();
				AmmoPickup->TossPickup(Location, nullptr, 999);

				auto NewFortPickup1 = Util::SpawnActor<AFortPickupAthena>(AFortPickupAthena::StaticClass(), Location);

				NewFortPickup1->PrimaryPickupItemEntry.Count = 1;
				NewFortPickup1->PrimaryPickupItemEntry.ItemDefinition = Globals::Consumables[rand() % Globals::Consumables.size()];
				NewFortPickup1->OnRep_PrimaryPickupItemEntry();
				NewFortPickup1->TossPickup(Location, nullptr, 1);

				auto NewFortPickup2 = Util::SpawnActor<AFortPickupAthena>(AFortPickupAthena::StaticClass(), Location);

				NewFortPickup2->PrimaryPickupItemEntry.Count = 1;
				NewFortPickup2->PrimaryPickupItemEntry.ItemDefinition = Globals::Consumables[rand() % Globals::Consumables.size()];
				NewFortPickup2->OnRep_PrimaryPickupItemEntry();
				NewFortPickup2->TossPickup(Location, nullptr, 1);

				auto NewFortPickup3 = Util::SpawnActor<AFortPickupAthena>(AFortPickupAthena::StaticClass(), Location);

				NewFortPickup3->PrimaryPickupItemEntry.Count = 1;
				NewFortPickup3->PrimaryPickupItemEntry.ItemDefinition = Globals::Traps[rand() % Globals::Traps.size()];
				NewFortPickup3->OnRep_PrimaryPickupItemEntry();
				NewFortPickup3->TossPickup(Location, nullptr, 1);
			}
		}

		return ProcessEvent(Object, Function, Parms);
	}

	void Init()
	{
		auto FEVFT = *reinterpret_cast<void***>(Globals::FortEngine);
		auto PEAddr = FEVFT[62];

		auto BaseAddress = (uintptr_t)GetModuleHandle(NULL);

		MH_CreateHook(reinterpret_cast<LPVOID>(PEAddr), ProcessEventHook, reinterpret_cast<LPVOID*>(&ProcessEvent));
		MH_EnableHook(reinterpret_cast<LPVOID>(PEAddr));
		

		/*MH_CreateHook((LPVOID)(BaseAddress + Offsets::PostGameplayEffectExecute), PostGameplayEffectExecuteHook, nullptr);
		MH_EnableHook((LPVOID)(BaseAddress + Offsets::PostGameplayEffectExecute));*/

		PickupInitialize = decltype(PickupInitialize)(BaseAddress + Offsets::PickupInitialize);

		MH_CreateHook((LPVOID)(BaseAddress + Offsets::PickupCombine), PickupCombineHook, (LPVOID*)(&PickupCombine));
		MH_EnableHook((LPVOID)(BaseAddress + Offsets::PickupCombine));

		//TEST PICKUP: 0x681660 (void __fastcall sub_7FF66CCA1660(__int64 a1, __int64 a2))
		//TEST PICKUP2: 0x67FDD0 (void **__fastcall sub_7FF66CC9FDD0(__int64 a1, __int64 a2, float a3, __int64 a4, char a5))
		//TEST PICKUP3: 0x681F20 (char __fastcall sub_7FF66CCA1F20(__int64 a1))

		/*MH_CreateHook((LPVOID)(BaseAddress + 0x681660), TestPickup1Hook, (LPVOID*)(&PickupCombine));
		MH_EnableHook((LPVOID)(BaseAddress + 0x681660));
		MH_CreateHook((LPVOID)(BaseAddress + 0x67FDD0), TestPickup2Hook, (LPVOID*)(&PickupCombine));
		MH_EnableHook((LPVOID)(BaseAddress + 0x67FDD0));
		MH_CreateHook((LPVOID)(BaseAddress + 0x681F20), TestPickup3Hook, (LPVOID*)(&PickupCombine));
		MH_EnableHook((LPVOID)(BaseAddress + 0x681F20));*/

		//PickupCombine = decltype(PickupCombine)(BaseAddress + Offsets::PickupCombine);
	}
}