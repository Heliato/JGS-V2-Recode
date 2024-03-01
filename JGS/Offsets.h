#pragma once

namespace Offsets
{
	//Replication
	uintptr_t IsAdaptiveNetUpdateFrequencyEnabled = 0x222F1D0;
	uintptr_t ClientHasInitializedLevelFor = 0x22243C0;
	uintptr_t SendClientAdjustment = 0x232BAD0;
	uintptr_t GetPlayerViewPoint = 0x2320D00;
	uintptr_t CallPreReplication = 0x1EB94E0;
	uintptr_t RemoveNetworkActor = 0x24A1DD0;
	uintptr_t ActorChannelClose = 0x20A8450;
	uintptr_t GetWorldSettings = 0x249AE00;
	uintptr_t IsNetRelevantFor = 0x1ECC700;
	uintptr_t SetChannelActor = 0x20C3390;
	uintptr_t AddNetworkActor = 0x2493BC0;
	uintptr_t ReplicateActor = 0x20BEA90;
	uintptr_t GetMaxTickRate = 0x24543E0;
	uintptr_t GetNetPriority = 0x23206E0;
	uintptr_t CreateChannel = 0x22247F0;
	uintptr_t TickFlush = 0x223DB40;

	//Beacons
	uintptr_t InitHost = 0x391E000;
	uintptr_t InitListen = 0x391e3e0;
	uintptr_t SetWorld = 0x223C500;
	uintptr_t LocalSpawnPlayActor = 0x21D83B0;
	uintptr_t KickPatch = 0x6DDBD0;
	uintptr_t CollectGarbage = 0x137DBC0;

	//Pickup
	uintptr_t PickupDelay = 0x6685D0;
	uintptr_t PickupCombine = 0x681C30;
	uintptr_t PickupCombine2 = 0x681C67;
	uintptr_t PickupInitialize = 0x67FCE0;
	
	//Abilities
	uintptr_t InternalTryActivateAbility = 0x3D51D30;
	uintptr_t GiveAbility = 0x3D50A60;

	//Gameplay
	uintptr_t PostGameplayEffectExecute = 0x443FF0;

	//Misc
	uintptr_t OnReload = 0x9239C0;
}