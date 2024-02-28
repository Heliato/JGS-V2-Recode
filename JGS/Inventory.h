#pragma once

#include <map>

class Inventory
{
public:
    static inline std::map<AFortPlayerController*, Inventory*> InventoryMap;

    AFortPlayerController* PC;

    void Init(AFortPlayerController* InPC)
    {
        PC = InPC;
    }

    void AddDefaultQuickBarItems()
    {
        auto QuickBars = PC->QuickBars;
        auto FortInventory = PC->WorldInventory;

        static auto PickaxeDef = FindObjectFast<UFortWeaponItemDefinition>("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
        static auto EditToolDef = FindObjectFast<UFortEditToolItemDefinition>("/Game/Items/Weapons/BuildingTools/EditTool.EditTool");
        static auto WallBuildDef = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall");
        static auto FloorBuildDef = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor");
        static auto StairBuildDef = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W");
        static auto RoofBuildDef = FindObjectFast<UFortBuildingItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS");

        auto EditToolItem = EditToolDef->CreateTemporaryItemInstanceBP(1, 0);
        auto WorldEditToolItem = (UFortWorldItem*)EditToolItem;
        WorldEditToolItem->ItemEntry.Count = 1;
        FortInventory->Inventory.ReplicatedEntries.Add(WorldEditToolItem->ItemEntry);
        FortInventory->Inventory.ItemInstances.Add(WorldEditToolItem);

        auto PickaxeItem = PickaxeDef->CreateTemporaryItemInstanceBP(1, 0);
        auto WorldPickaxeItem = reinterpret_cast<UFortWorldItem*>(PickaxeItem);
        WorldPickaxeItem->ItemEntry.Count = 1;
        FortInventory->Inventory.ReplicatedEntries.Add(WorldPickaxeItem->ItemEntry);
        FortInventory->Inventory.ItemInstances.Add(WorldPickaxeItem);
        QuickBars->ServerAddItemInternal(WorldPickaxeItem->GetItemGuid(), EFortQuickBars::Primary, 0);

        auto WallBuildItem = WallBuildDef->CreateTemporaryItemInstanceBP(1, 0);
        auto WallWorldBuildItem = reinterpret_cast<UFortWorldItem*>(WallBuildItem);
        FortInventory->Inventory.ReplicatedEntries.Add(WallWorldBuildItem->ItemEntry);
        FortInventory->Inventory.ItemInstances.Add(WallWorldBuildItem);
        QuickBars->ServerAddItemInternal(WallWorldBuildItem->GetItemGuid(), EFortQuickBars::Secondary, 0);

        auto FloorBuildItem = FloorBuildDef->CreateTemporaryItemInstanceBP(1, 0);
        auto FloorWorldBuildItem = reinterpret_cast<UFortWorldItem*>(FloorBuildItem);
        FortInventory->Inventory.ReplicatedEntries.Add(FloorWorldBuildItem->ItemEntry);
        FortInventory->Inventory.ItemInstances.Add(FloorWorldBuildItem);
        QuickBars->ServerAddItemInternal(FloorWorldBuildItem->GetItemGuid(), EFortQuickBars::Secondary, 1);

        auto StairBuildItem = StairBuildDef->CreateTemporaryItemInstanceBP(1, 0);
        auto StairWorldBuildItem = reinterpret_cast<UFortWorldItem*>(StairBuildItem);
        FortInventory->Inventory.ReplicatedEntries.Add(StairWorldBuildItem->ItemEntry);
        FortInventory->Inventory.ItemInstances.Add(StairWorldBuildItem);
        QuickBars->ServerAddItemInternal(StairWorldBuildItem->GetItemGuid(), EFortQuickBars::Secondary, 2);

        auto RoofBuildItem = RoofBuildDef->CreateTemporaryItemInstanceBP(1, 0);
        auto RoofWorldBuildItem = reinterpret_cast<UFortWorldItem*>(RoofBuildItem);
        FortInventory->Inventory.ReplicatedEntries.Add(RoofWorldBuildItem->ItemEntry);
        FortInventory->Inventory.ItemInstances.Add(RoofWorldBuildItem);
        QuickBars->ServerAddItemInternal(RoofWorldBuildItem->GetItemGuid(), EFortQuickBars::Secondary, 3);
    }

    void SetupInventory()
    {
        std::map<UFortItemDefinition*, int> ItemsToAddMap;

        auto NewQuickBars = (AFortQuickBars*)Util::SpawnActor(AFortQuickBars::StaticClass(), {}, {});
        NewQuickBars->SetOwner(PC);
        PC->QuickBars = NewQuickBars;
        PC->OnRep_QuickBar();

        auto FortInventory = PC->WorldInventory;
        auto QuickBars = PC->QuickBars;
        
        QuickBars->EnableSlot(EFortQuickBars::Primary, 0);
        QuickBars->EnableSlot(EFortQuickBars::Primary, 1);
        QuickBars->EnableSlot(EFortQuickBars::Primary, 2);
        QuickBars->EnableSlot(EFortQuickBars::Primary, 3);
        QuickBars->EnableSlot(EFortQuickBars::Primary, 4);
        QuickBars->EnableSlot(EFortQuickBars::Secondary, 0);
        QuickBars->EnableSlot(EFortQuickBars::Secondary, 1);
        QuickBars->EnableSlot(EFortQuickBars::Secondary, 2);
        QuickBars->EnableSlot(EFortQuickBars::Secondary, 3);
        QuickBars->EnableSlot(EFortQuickBars::Secondary, 4);

        AddDefaultQuickBarItems();

        for (auto Item : ItemsToAddMap)
        {
            auto ItemDef = Item.first;
            auto ItemCount = Item.second;

            if (ItemDef)
            {
                auto NewWorldItem = (UFortWorldItem*)(ItemDef->CreateTemporaryItemInstanceBP(ItemCount, 0));
                FortInventory->Inventory.ReplicatedEntries.Add(NewWorldItem->ItemEntry);
                FortInventory->Inventory.ItemInstances.Add(NewWorldItem);
            }
        }

        ItemsToAddMap.empty();
        ItemsToAddMap.clear();

        PC->bHasInitializedWorldInventory = true;
	}

	void UpdateInventory()
	{
        PC->HandleWorldInventoryLocalUpdate();

        PC->WorldInventory->HandleInventoryLocalUpdate();
        PC->WorldInventory->bRequiresLocalUpdate = true;
        PC->QuickBars->OnRep_PrimaryQuickBar();
        PC->QuickBars->OnRep_SecondaryQuickBar();
        PC->QuickBars->ForceNetUpdate();
        PC->WorldInventory->ForceNetUpdate();

        PC->WorldInventory->Inventory.MarkArrayDirty();
	}

    UFortWorldItem* GetItemInstance(FGuid Guid, bool bVerifMaxStackSize = false)
    {
        AFortInventory* WorldInventory = PC->WorldInventory;

        for (int i = 0; i < WorldInventory->Inventory.ItemInstances.Num(); i++)
        {
            UFortWorldItem* ItemInstance = WorldInventory->Inventory.ItemInstances[i];
            if (!ItemInstance) continue;

            if (Util::AreGuidsTheSame(ItemInstance->GetItemGuid(), Guid))
            {
                UFortItemDefinition* ItemDefinition = ItemInstance->GetItemDefinitionBP();

                if (ItemDefinition && ItemInstance->ItemEntry.Count >= ItemDefinition->MaxStackSize && bVerifMaxStackSize)
                    continue;

                return ItemInstance;
            } 
        }

        return nullptr;
    }

    FFortItemEntry* GetReplicatedEntry(FGuid Guid, bool bVerifMaxStackSize = false)
    {
        AFortInventory* WorldInventory = PC->WorldInventory;

        for (int i = 0; i < WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
        {
            FFortItemEntry ItemEntry = WorldInventory->Inventory.ReplicatedEntries[i];

            if (Util::AreGuidsTheSame(ItemEntry.ItemGuid, Guid))
            {
                UFortItemDefinition* ItemDefinition = ItemEntry.ItemDefinition;

                if (ItemDefinition && ItemEntry.Count >= ItemDefinition->MaxStackSize && bVerifMaxStackSize)
                    continue;

                return &ItemEntry;
            }
        }

        return nullptr;
    }

    FGuid GetItemGuid(UFortItemDefinition* ItemDefinition, bool bVerifMaxStackSize = false)
    {
        AFortInventory* WorldInventory = PC->WorldInventory;

        for (int i = 0; i < WorldInventory->Inventory.ItemInstances.Num(); i++)
        {
            UFortWorldItem* ItemInstance = WorldInventory->Inventory.ItemInstances[i];
            if (!ItemInstance) continue;

            if (ItemInstance->GetItemDefinitionBP() == ItemDefinition)
            {
                if (ItemInstance->ItemEntry.Count >= ItemDefinition->MaxStackSize && bVerifMaxStackSize)
                    continue;

                return ItemInstance->GetItemGuid();
            }
        }

        return FGuid();
    }

    int GetItemSlotQuickbar(UFortItemDefinition* ItemDefinition, EFortQuickBars FortQuickBars, bool bVerifMaxStackSize = false)
    {
        AFortInventory* WorldInventory = PC->WorldInventory;
        AFortQuickBars* QuickBars = PC->QuickBars;
        int Slot = -1;

        if (WorldInventory && QuickBars)
        {
            auto QuickBar = (FortQuickBars == EFortQuickBars::Primary) ? QuickBars->PrimaryQuickBar : QuickBars->SecondaryQuickBar;
            FGuid ItemGuid = GetItemGuid(ItemDefinition, bVerifMaxStackSize);
            UFortWorldItem* ItemInstance = GetItemInstance(ItemGuid, bVerifMaxStackSize);

            if (ItemInstance)
            {
                for (int i = 0; i < QuickBar.Slots.Num(); i++)
                {
                    FQuickBarSlot Slots = QuickBar.Slots[i];
                    if (i >= 6) break;
                    if (!Slots.Items.IsValidIndex(0)) continue;

                    if (Util::AreGuidsTheSame(ItemGuid, Slots.Items[0]))
                    {
                        if (ItemInstance->ItemEntry.Count >= ItemDefinition->MaxStackSize && bVerifMaxStackSize)
                            continue;

                        Slot = i;
                        break;
                    }
                }
            }
        }

        return Slot;
    }

    int GetAvailableSlotQuickbar(EFortQuickBars FortQuickBars)
    {
        AFortQuickBars* QuickBars = PC->QuickBars;
        int Slot = -1;

        if (QuickBars)
        {
            auto QuickBar = (FortQuickBars == EFortQuickBars::Primary) ? QuickBars->PrimaryQuickBar : QuickBars->SecondaryQuickBar;

            for (int i = 0; i < QuickBar.Slots.Num(); i++)
            {
                FQuickBarSlot Slots = QuickBar.Slots[i];
                if (i >= 6) break;
                if (Slots.Items.IsValidIndex(0)) continue;

                Slot = i;
                break;
            }
        }

        return Slot;
    }

    UFortWorldItem* GetItemSlot(int Slot, EFortQuickBars FortQuickBars)
    {
        AFortQuickBars* QuickBars = PC->QuickBars;

        if (QuickBars)
        {
            auto QuickBar = (FortQuickBars == EFortQuickBars::Primary) ? QuickBars->PrimaryQuickBar : QuickBars->SecondaryQuickBar;
            FQuickBarSlot Slots = QuickBar.Slots[Slot];

            if (Slot < 6 && Slots.Items.IsValidIndex(0))
                return GetItemInstance(Slots.Items[0]);
        }

        return nullptr;
    }

    AFortWeapon* GetWeapon(FGuid Guid)
    {
        UFortWorldItem* ItemInstance = GetItemInstance(Guid);
        AFortPawn* Pawn = (AFortPawn*)PC->Pawn;

        if (ItemInstance && Pawn)
        {
            for (int i = 0; i < Pawn->CurrentWeaponList.Num(); i++)
            {
                AFortWeapon* Weapon = Pawn->CurrentWeaponList[i];
                if (!Weapon) continue;

                if (Util::AreGuidsTheSame(Weapon->ItemEntryGuid, ItemInstance->GetItemGuid()))
                    return Weapon;
            }
        }

        return nullptr;
    }

    bool RemoveItemFromInventory(FGuid Guid)
    {
        AFortInventory* WorldInventory = PC->WorldInventory;

        for (int i = 0; i < WorldInventory->Inventory.ItemInstances.Num(); i++)
        {
            UFortWorldItem* ItemInstance = WorldInventory->Inventory.ItemInstances[i];
            if (!ItemInstance) continue;

            if (Util::AreGuidsTheSame(ItemInstance->GetItemGuid(), Guid))
            {
                WorldInventory->Inventory.ItemInstances.Remove(i);

                for (int j = 0; j < WorldInventory->Inventory.ReplicatedEntries.Num(); j++)
                {
                    FFortItemEntry ItemEntry = WorldInventory->Inventory.ReplicatedEntries[j];

                    if (Util::AreGuidsTheSame(ItemEntry.ItemGuid, Guid))
                    {
                        WorldInventory->Inventory.ReplicatedEntries.Remove(j);

                        return true;
                    }
                }

                return true;
            }
        }

        return false;
    }

    bool RemoveItemFromQuickbar(FGuid Guid, EFortQuickBars FortQuickBars = EFortQuickBars::Primary)
    {
        AFortQuickBars* QuickBars = PC->QuickBars;

        if (QuickBars)
        {
            auto QuickBar = (FortQuickBars == EFortQuickBars::Primary) ? QuickBars->PrimaryQuickBar : QuickBars->SecondaryQuickBar;

            for (int i = 0; i < QuickBar.Slots.Num(); i++)
            {
                FQuickBarSlot Slots = QuickBar.Slots[i];
                if (i >= 6) break;

                if (Slots.Items.IsValidIndex(0))
                {
                    if (Util::AreGuidsTheSame(Slots.Items[0], Guid))
                    {
                        if (i >= 0)
                        {
                            QuickBars->EmptySlot(FortQuickBars, i);
                            //QuickBars->ServerRemoveItemInternal(Guid, true, true); // works without
                        }

                        return true;
                    }
                }
            }
        }

        return false;
    }

    UFortWorldItem* AddInventoryItem(UFortItemDefinition* ItemDefinition, FFortItemEntry ItemEntry)
    {
        AFortInventory* WorldInventory = PC->WorldInventory;

        UFortWorldItem* NewPickupWorldItem = (UFortWorldItem*)ItemDefinition->CreateTemporaryItemInstanceBP(ItemEntry.Count, 1);
        NewPickupWorldItem->ItemEntry = ItemEntry;
        NewPickupWorldItem->ItemEntry.Count = ItemEntry.Count;
        NewPickupWorldItem->SetOwningControllerForTemporaryItem(PC);

        if (WorldInventory)
        {
            WorldInventory->Inventory.ItemInstances.Add(NewPickupWorldItem);
            WorldInventory->Inventory.ReplicatedEntries.Add(NewPickupWorldItem->ItemEntry);
        }

        return NewPickupWorldItem;
    }

    bool AddQuickBarItem(FGuid Guid, int Slot, EFortQuickBars FortQuickBars)
    {
        UFortWorldItem* ItemInstance = GetItemInstance(Guid);
        AFortQuickBars* QuickBars = PC->QuickBars;

        if (ItemInstance && QuickBars)
        {
            QuickBars->ServerAddItemInternal(ItemInstance->GetItemGuid(), FortQuickBars, Slot);

            return true;
        }

        return false;
    }

    bool ModifyCountItem(FGuid Guid, int Count)
    {
        UFortWorldItem* ItemInstance = GetItemInstance(Guid);
        FFortItemEntry ReplicatedEntry = *GetReplicatedEntry(Guid);

        if (ItemInstance)
        {
            ItemInstance->ItemEntry.Count = Count;
            ReplicatedEntry.Count = Count;
        }

        return false;
    }

    void SetQuickBarSlot(int Slot, EFortQuickBars FortQuickBars)
    {
        AFortQuickBars* QuickBars = PC->QuickBars;
        AFortPawn* Pawn = (AFortPawn*)PC->Pawn;

        if (QuickBars && Pawn)
        {
            auto QuickBar = (FortQuickBars == EFortQuickBars::Primary) ? QuickBars->PrimaryQuickBar : QuickBars->SecondaryQuickBar;
            UFortWorldItem* PrevItemInstance = GetItemSlot(QuickBar.CurrentFocusedSlot, FortQuickBars);
            UFortWorldItem* NewItemInstance = GetItemSlot(Slot, FortQuickBars);

            if (PrevItemInstance && NewItemInstance)
            {
                AFortWeapon* PrevWeapon = GetWeapon(PrevItemInstance->GetItemGuid());
                AFortWeapon* NewWeapon = GetWeapon(NewItemInstance->GetItemGuid());

                if (PrevWeapon && NewWeapon)
                {
                    /*NewWeapon->OnSetTargeting(true);
                    Pawn->OnWeaponEquipped(NewWeapon, PrevWeapon);
                    Pawn->ClientInternalEquipWeapon(NewWeapon);*/
                    PC->ServerExecuteInventoryItem(NewItemInstance->GetItemGuid());
                    QuickBar.CurrentFocusedSlot = Slot;
                    QuickBars->OnRep_PrimaryQuickBar();
                }
            }
        }
    }

    AFortPickupAthena* SpawnItem(UFortItemDefinition* ItemDefinition, FFortItemEntry ItemEntry, FVector Location)
    {
        AFortPickupAthena* FortPickup = (AFortPickupAthena*)(Util::SpawnActor(AFortPickupAthena::StaticClass(), Location, FRotator()));

        FortPickup->PrimaryPickupItemEntry.ItemDefinition = ItemDefinition;
        FortPickup->PrimaryPickupItemEntry.Count = ItemEntry.Count;
        FortPickup->OnRep_PrimaryPickupItemEntry();

        FortPickup->TossPickup(Location, nullptr, ItemDefinition->MaxStackSize);

        return FortPickup;
    }

    AFortPickupAthena* DropItemFromInventory(FGuid Guid)
    {
        UFortWorldItem* ItemInstance = GetItemInstance(Guid);
        APawn* Pawn = PC->Pawn;

        if (ItemInstance && Pawn)
        {
            UFortItemDefinition* ItemDefinition = ItemInstance->GetItemDefinitionBP();

            if (ItemDefinition)
            {
                AFortPickupAthena* FortPickup = SpawnItem(ItemDefinition, ItemInstance->ItemEntry, Pawn->K2_GetActorLocation());

                return FortPickup;
            }
        }

        return nullptr;
    }

    void RemoveAllItemsFromInventory(bool bRemoveCantBeDropped = false)
    {
        AFortInventory* WorldInventory = PC->WorldInventory;

        if (WorldInventory)
        {
            std::vector<FGuid> GuidToRemove;

            for (int i = 0; i < WorldInventory->Inventory.ItemInstances.Num(); i++)
            {
                UFortWorldItem* ItemInstance = WorldInventory->Inventory.ItemInstances[i];
                if (!ItemInstance) continue;

                FFortItemEntry ItemEntry = ItemInstance->ItemEntry;

                UFortWorldItemDefinition* ItemDefinition = (UFortWorldItemDefinition*)ItemInstance->GetItemDefinitionBP();
                if (!ItemDefinition) continue;

                if (!ItemDefinition->bCanBeDropped && !bRemoveCantBeDropped)
                    continue;

                LOG("[" << i << "] RemoveItem: " << ItemDefinition->GetName());

                GuidToRemove.push_back(ItemEntry.ItemGuid);
            }

            for (FGuid& Guid : GuidToRemove)
            {
                RemoveItemFromQuickbar(Guid);
                RemoveItemFromInventory(Guid);
            }
        }
    }

    void DropAllItemsFromInventory(bool bRemoveCantBeDropped = false)
    {
        AFortInventory* WorldInventory = PC->WorldInventory;

        if (WorldInventory)
        {
            for (int i = 0; i < WorldInventory->Inventory.ItemInstances.Num(); i++)
            {
                UFortWorldItem* ItemInstance = WorldInventory->Inventory.ItemInstances[i];
                if (!ItemInstance) continue;

                FFortItemEntry ItemEntry = ItemInstance->ItemEntry;

                UFortWorldItemDefinition* ItemDefinition = (UFortWorldItemDefinition*)ItemInstance->GetItemDefinitionBP();
                if (!ItemDefinition) continue;

                if (!ItemDefinition->bCanBeDropped)
                    continue;

                LOG("[" << i << "] DropItemFromInventory: " << ItemDefinition->GetName());

                DropItemFromInventory(ItemInstance->GetItemGuid());
            }

            RemoveAllItemsFromInventory(bRemoveCantBeDropped);
            UpdateInventory();
        }
    }
};

static Inventory* FindInventory(AFortPlayerController* PC)
{
    for (auto Inv : Inventory::InventoryMap)
    {
        if (Inv.first == PC)
            return Inv.second;
    }

    return nullptr;
}

static Inventory* CreateInventoryForPlayerController(AFortPlayerController* PC)
{
    Inventory* NewInv = new Inventory;
    NewInv->Init(PC);
    Inventory::InventoryMap.insert_or_assign(PC, NewInv);
    return NewInv;
}