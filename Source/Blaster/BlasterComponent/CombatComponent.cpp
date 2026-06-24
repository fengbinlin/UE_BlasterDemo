// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}
void UCombatComponent::SetAiming(bool L_bIsAiming)
{
	bIsAiming = L_bIsAiming;
}
void UCombatComponent::ServerSetAiming_Implementation(bool L_bIsAiming)
{
	bIsAiming = L_bIsAiming;
}
void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UCombatComponent::EquipWeapon(AWeapon* WeaponToquip)
{
	if (Character && WeaponToquip) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("EquipWeapon")));
		}
		EquippedWeapon = WeaponToquip;
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName("RightHandSocket");
		if (HandSocket) {
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());

		}
		EquippedWeapon->SetOwner(Character);

	}
}



