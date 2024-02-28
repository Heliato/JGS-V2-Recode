#pragma once

namespace Misc
{
	void (*PickupDelay)(AFortPickup* Pickup);
	void (*OnReload)(AFortWeapon* Weapon, unsigned int a2);

	void PickupDelayHook(AFortPickup* Pickup)
	{
		AFortPawn* Pawn = Pickup->PickupLocationData.PickupTarget;

		if (Pawn)
		{
			auto PlayerController = (AFortPlayerControllerAthena*)Pawn->Controller;

			if (PlayerController)
			{
				AFortQuickBars* QuickBars = PlayerController->QuickBars;
				FFortItemEntry ItemEntry = Pickup->PrimaryPickupItemEntry;
				Inventory* PlayerInventory = FindInventory(PlayerController);
				UFortItemDefinition* ItemDefinition = ItemEntry.ItemDefinition;

				if (QuickBars && PlayerInventory && ItemDefinition)
				{
					bool bSuccessfullyAdded = false;
					int CountToRemove = 0;

					if (ItemDefinition->IsA(UFortWeaponRangedItemDefinition::StaticClass()))
					{
						int AvailableSlot = PlayerInventory->GetAvailableSlotQuickbar(EFortQuickBars::Primary);

						if (AvailableSlot != -1)
						{
							UFortWorldItem* NewPickupWorldItem = PlayerInventory->AddInventoryItem(ItemDefinition, ItemEntry);

							if (NewPickupWorldItem)
							{
								PlayerInventory->AddQuickBarItem(NewPickupWorldItem->GetItemGuid(), AvailableSlot, EFortQuickBars::Primary);
								bSuccessfullyAdded = true;
							}
						}
						else
						{
							int ItemSlot = PlayerInventory->GetItemSlotQuickbar(ItemDefinition, EFortQuickBars::Primary, true);

							if (ItemSlot != -1)
							{
								UFortWorldItem* ItemSlotInstance = PlayerInventory->GetItemSlot(ItemSlot, EFortQuickBars::Primary);

								if (ItemSlotInstance)
								{
									PlayerInventory->RemoveItemFromQuickbar(ItemSlotInstance->GetItemGuid(), EFortQuickBars::Primary);
									PlayerInventory->RemoveItemFromInventory(ItemSlotInstance->GetItemGuid());

									ItemEntry.Count = ItemSlotInstance->ItemEntry.Count + ItemEntry.Count;

									if (ItemEntry.Count > ItemDefinition->MaxStackSize)
									{
										CountToRemove = ItemEntry.Count - ItemDefinition->MaxStackSize;
										ItemEntry.Count = ItemDefinition->MaxStackSize;
									}

									UFortWorldItem* NewPickupWorldItem = PlayerInventory->AddInventoryItem(ItemDefinition, ItemEntry);
									PlayerInventory->AddQuickBarItem(NewPickupWorldItem->GetItemGuid(), ItemSlot, EFortQuickBars::Primary);

									bSuccessfullyAdded = true;
								}
							}
							else
							{
								FQuickBar QuickBar = ItemDefinition->IsA(UFortTrapItemDefinition::StaticClass()) ? QuickBars->SecondaryQuickBar : QuickBars->PrimaryQuickBar;

								int CurrentSlot = QuickBar.CurrentFocusedSlot;

								if (CurrentSlot > 0 && CurrentSlot < 6)
								{
									UFortWorldItem* ItemSlotInstance = PlayerInventory->GetItemSlot(CurrentSlot, EFortQuickBars::Primary);

									if (ItemSlotInstance)
									{
										PlayerController->ServerSpawnInventoryDrop(ItemSlotInstance->GetItemGuid(), ItemSlotInstance->ItemEntry.Count);

										UFortWorldItem* NewPickupWorldItem = PlayerInventory->AddInventoryItem(ItemDefinition, ItemEntry);
										PlayerInventory->AddQuickBarItem(NewPickupWorldItem->GetItemGuid(), CurrentSlot, EFortQuickBars::Primary);
										PlayerController->ServerExecuteInventoryItem(NewPickupWorldItem->GetItemGuid());

										bSuccessfullyAdded = true;
									}
								}
							}
						}
					}
					else if (ItemDefinition->IsA(UFortTrapItemDefinition::StaticClass()))
					{
						FGuid ItemGuid = PlayerInventory->GetItemGuid(ItemDefinition);
						UFortWorldItem* ItemInstance = PlayerInventory->GetItemInstance(ItemGuid);

						if (ItemInstance)
						{
							PlayerInventory->RemoveItemFromInventory(ItemInstance->GetItemGuid());

							ItemEntry.Count = ItemInstance->ItemEntry.Count + ItemEntry.Count;

							if (ItemEntry.Count > ItemDefinition->MaxStackSize)
							{
								CountToRemove = ItemEntry.Count - ItemDefinition->MaxStackSize;
								ItemEntry.Count = ItemDefinition->MaxStackSize;
							}

							UFortWorldItem* NewPickupWorldItem = PlayerInventory->AddInventoryItem(ItemDefinition, ItemEntry);

							if (NewPickupWorldItem)
							{
								PlayerInventory->AddQuickBarItem(NewPickupWorldItem->GetItemGuid(), 4, EFortQuickBars::Secondary);
								bSuccessfullyAdded = true;
							}
						}
						else
						{
							UFortWorldItem* NewPickupWorldItem = PlayerInventory->AddInventoryItem(ItemDefinition, ItemEntry);

							if (NewPickupWorldItem)
							{
								PlayerInventory->AddQuickBarItem(NewPickupWorldItem->GetItemGuid(), 4, EFortQuickBars::Secondary);
								bSuccessfullyAdded = true;
							}
						}
					}
					else if (ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass()) ||
						ItemDefinition->IsA(UFortResourceItemDefinition::StaticClass()))
					{
						FGuid ItemGuid = PlayerInventory->GetItemGuid(ItemDefinition);
						UFortWorldItem* ItemInstance = PlayerInventory->GetItemInstance(ItemGuid);

						if (ItemInstance)
						{
							PlayerInventory->RemoveItemFromInventory(ItemInstance->GetItemGuid());

							ItemEntry.Count = ItemInstance->ItemEntry.Count + ItemEntry.Count;

							if (ItemEntry.Count > ItemDefinition->MaxStackSize)
							{
								CountToRemove = ItemEntry.Count - ItemDefinition->MaxStackSize;
								ItemEntry.Count = ItemDefinition->MaxStackSize;
							}

							PlayerInventory->AddInventoryItem(ItemDefinition, ItemEntry);
							bSuccessfullyAdded = true;
						}
						else
						{
							PlayerInventory->AddInventoryItem(ItemDefinition, ItemEntry);
							bSuccessfullyAdded = true;
						}
					}

					if (bSuccessfullyAdded && CountToRemove > 0)
					{
						ItemEntry.Count = CountToRemove;
						PlayerInventory->SpawnItem(ItemDefinition, ItemEntry, Pawn->K2_GetActorLocation());
					}

					if (!bSuccessfullyAdded)
						PlayerInventory->SpawnItem(ItemDefinition, ItemEntry, Pawn->K2_GetActorLocation());

					PlayerInventory->UpdateInventory();
				}
			}
		}

		/*Pickup->bPickedUp = bSuccessfullyAdded; // Not Working
		Pickup->OnRep_bPickedUp();*/

		return PickupDelay(Pickup);
	}

	void OnReloadHook(AFortWeapon* Weapon, int Count)
	{
		if (Weapon && Count > 0)
		{
			AFortPawn* Pawn = (AFortPawn*)Weapon->Owner;

			if (Pawn)
			{
				AFortPlayerController* PlayerController = (AFortPlayerController*)Pawn->Controller;
				Inventory* PlayerInventory = FindInventory(PlayerController);

				if (PlayerController && PlayerInventory)
				{
					UFortWeaponItemDefinition* ItemDefinition = Weapon->WeaponData;
					bool bRemoveItem = false;

					if (ItemDefinition)
					{
						UFortWeaponItemDefinition* AmmoDefinition = (UFortWeaponItemDefinition*)ItemDefinition->GetAmmoWorldItemDefinition_BP();
						bool bIsItem = (ItemDefinition == AmmoDefinition);

						if (AmmoDefinition && ItemDefinition != AmmoDefinition)
							ItemDefinition = AmmoDefinition;

						FGuid ItemGuid = PlayerInventory->GetItemGuid(ItemDefinition);
						UFortWorldItem* ItemInstance = PlayerInventory->GetItemInstance(ItemGuid);
						int ItemSlot = PlayerInventory->GetItemSlotQuickbar(ItemDefinition, EFortQuickBars::Primary);

						if (ItemInstance)
						{
							if (ItemSlot != -1)
								PlayerInventory->RemoveItemFromQuickbar(ItemInstance->GetItemGuid());

							PlayerInventory->RemoveItemFromInventory(ItemInstance->GetItemGuid());

							ItemInstance->ItemEntry.Count = ItemInstance->ItemEntry.Count - Count;

							if (ItemInstance->ItemEntry.Count > 0)
							{
								UFortWorldItem* NewPickupWorldItem = PlayerInventory->AddInventoryItem(ItemDefinition, ItemInstance->ItemEntry);

								if (ItemSlot != -1)
									PlayerInventory->AddQuickBarItem(NewPickupWorldItem->GetItemGuid(), ItemSlot, EFortQuickBars::Primary);
							}
							else
							{
								bRemoveItem = (ItemSlot != -1);
							}
						}
					}

					if (bRemoveItem)
					{
						UFortWorldItem* PickaxeInstance = PlayerInventory->GetItemSlot(0, EFortQuickBars::Primary);

						if (PickaxeInstance)
							PlayerController->ServerExecuteInventoryItem(PickaxeInstance->GetItemGuid());
					}

					PlayerInventory->UpdateInventory();
				}
			}
		}

		return OnReload(Weapon, Count);
	}

	static void Init()
	{
		auto BaseAddr = Util::BaseAddress();

		MH_CreateHook((LPVOID)(BaseAddr + Offsets::PickupDelay), PickupDelayHook, (LPVOID*)(&PickupDelay));
		MH_EnableHook((LPVOID)(BaseAddr + Offsets::PickupDelay));
		MH_CreateHook((LPVOID)(BaseAddr + Offsets::OnReload), OnReloadHook, (LPVOID*)(&OnReload));
		MH_EnableHook((LPVOID)(BaseAddr + Offsets::OnReload));
	}
}