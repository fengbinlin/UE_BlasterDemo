// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ABlasterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void EquipWeapon(class AWeapon* WeaponToquip);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	void SetAiming(bool L_bIsAiming);
	UFUNCTION(Server,Reliable)
	void ServerSetAiming(bool L_bIsAiming);

private:
	class ABlasterCharacter* Character;
	UPROPERTY(Replicated)
	class AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bIsAiming;

public:	

	

		
};
