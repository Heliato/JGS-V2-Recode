#pragma once

namespace GPFuncs
{
	static void SpawnFloorLoot()
	{
		TArray<AActor*> OutActors;
		Globals::GPS->STATIC_GetAllActorsOfClass(Globals::World, UObject::FindClass("BlueprintGeneratedClass Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C"), &OutActors);

		for (int i = 0; i < OutActors.Num(); i++)
		{
			bool bShouldSpawn = Globals::MathLib->STATIC_RandomBoolWithWeight(0.6f);
			bool bIsConsumable = !Globals::MathLib->STATIC_RandomBoolWithWeight(0.2f);

			auto Actor = OutActors[i];

			if (Actor && bShouldSpawn)
			{
				auto SpawnLoc = Actor->K2_GetActorLocation();

				auto NewFortPickup = reinterpret_cast<AFortPickupAthena*>(Util::SpawnActor(AFortPickupAthena::StaticClass(), SpawnLoc, FRotator()));

				NewFortPickup->PrimaryPickupItemEntry.Count = 1;
				if (bIsConsumable)
				{
					bool bEpicOrLeg = Globals::MathLib->STATIC_RandomBoolWithWeight(0.07);

					int Index = 0;

					if (bEpicOrLeg)
					{
						Index = Globals::MathLib->STATIC_RandomIntegerInRange(3, 4);
					}
					else {
						Index = Globals::MathLib->STATIC_RandomIntegerInRange(0, 2);
					}

					LOG("Index: " << Index);

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
				}
				else {
					NewFortPickup->PrimaryPickupItemEntry.ItemDefinition = Globals::Consumables[rand() % Globals::Consumables.size()];
				}
				NewFortPickup->OnRep_PrimaryPickupItemEntry();
				NewFortPickup->TossPickup(SpawnLoc, nullptr, 1);

				if (bIsConsumable && NewFortPickup->PrimaryPickupItemEntry.ItemDefinition)
				{
					auto AmmoDefintion = ((UFortWorldItemDefinition*)NewFortPickup->PrimaryPickupItemEntry.ItemDefinition)->GetAmmoWorldItemDefinition_BP();
					auto AmmoPickup = reinterpret_cast<AFortPickupAthena*>(Util::SpawnActor(AFortPickupAthena::StaticClass(), NewFortPickup->K2_GetActorLocation(), {}));
					AmmoPickup->PrimaryPickupItemEntry.Count = AmmoDefintion->DropCount * 1.25;
					AmmoPickup->PrimaryPickupItemEntry.ItemDefinition = AmmoDefintion;
					AmmoPickup->OnRep_PrimaryPickupItemEntry();
					AmmoPickup->TossPickup(SpawnLoc, nullptr, 999);
				}

				Actor->K2_DestroyActor();
				//LOG("Spawned Pickup: " << NewFortPickup->GetName() << " With weapon definition: " << NewFortPickup->PrimaryPickupItemEntry.ItemDefinition->GetName());
			}
		}
	}

	int totalSearchedActors = 0;

	static void MakeEverythingSearched()
	{
		TArray<AActor*> OutActors;
		Globals::GPS->STATIC_GetAllActorsOfClass(Globals::World, ABuildingContainer::StaticClass(), &OutActors);

		for (int32_t i = 0; i < OutActors.Num(); i++)
		{
			auto Actor = (ABuildingContainer*)OutActors[i];

			if (Actor->GetFullName().contains("Tiered_Chest") || 
				Actor->GetFullName().contains("Tiered_Short_Ammo")) continue;

			totalSearchedActors++;

			Actor->bAlreadySearched = true;
			Actor->OnRep_bAlreadySearched();
		}
	}

	AFortPawn* SpawnPlayer(AFortPlayerController* PlayerController, FVector& Location, FRotator Rotation, bool NewPlayer = true)
	{
		auto Pawn = (APlayerPawn_Athena_C*)(Util::SpawnActor(APlayerPawn_Athena_C::StaticClass(), Location, Rotation));
		UWorld* World = Globals::World;

		Pawn->bCanBeDamaged = (NewPlayer == true && !World->GetName().contains("Athena_Faceoff")) ? false : true;
		Pawn->Owner = PlayerController;
		Pawn->OnRep_Owner();

		PlayerController->Pawn = Pawn;
		PlayerController->OnRep_Pawn();
		PlayerController->Possess(Pawn);

		Pawn->SetMaxHealth(100);
		Pawn->SetHealth(100);
		auto HealthSet = Pawn->HealthSet;
		HealthSet->CurrentShield.Minimum = 0;
		HealthSet->CurrentShield.Maximum = 100;
		HealthSet->CurrentShield.BaseValue = 0;
		HealthSet->Shield.Minimum = 0;
		HealthSet->Shield.Maximum = 100;
		HealthSet->Shield.BaseValue = 100;
		HealthSet->OnRep_Shield();
		HealthSet->OnRep_CurrentShield();

		Pawn->HealthRegenDelayGameplayEffect = nullptr;
		Pawn->ShieldRegenDelayGameplayEffect = nullptr;
		Pawn->ShieldRegenGameplayEffect = nullptr;
		Pawn->HealthRegenGameplayEffect = nullptr;

		if (NewPlayer)
		{
			auto RandomParts = Globals::CharacterParts[rand() % Globals::CharacterParts.size()];

			if (RandomParts.HeadPart)
				Pawn->ServerChoosePart(EFortCustomPartType::Head, RandomParts.HeadPart);

			if (RandomParts.BodyPart)
				Pawn->ServerChoosePart(EFortCustomPartType::Body, RandomParts.BodyPart);

			if (RandomParts.HatPart)
				Pawn->ServerChoosePart(EFortCustomPartType::Hat, RandomParts.HatPart);

			auto PlayerState = (AFortPlayerStateAthena*)PlayerController->PlayerState;
			PlayerState->bHasFinishedLoading = true;
			PlayerState->bHasStartedPlaying = true;
			PlayerState->OnRep_bHasStartedPlaying();
			PlayerState->OnRep_CharacterParts();

			UFortCheatManager* CheatManager = (UFortCheatManager*)(Globals::GPS->STATIC_SpawnObject(UFortCheatManager::StaticClass(), PlayerController));
			PlayerController->CheatManager = CheatManager;
			CheatManager->BackpackSetSize(5);

			static UFortAbilitySet* GlobalAbility = FindObjectFast<UFortAbilitySet>("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer");

			Abilities::GrantGameplayAbility(GlobalAbility, Pawn);

			auto NewInv = CreateInventoryForPlayerController(PlayerController);
			NewInv->SetupInventory();
			NewInv->UpdateInventory();

			PlayerController->bIsDisconnecting = false;
			PlayerController->bHasClientFinishedLoading = true;
			PlayerController->bHasServerFinishedLoading = true;
			PlayerController->bHasInitiallySpawned = true;
		}

		return Pawn;
	}
}