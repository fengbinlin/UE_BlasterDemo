// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterComponent/CombatComponent.h"

// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;


	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon,COND_OwnerOnly);

}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat) {
		Combat->Character = this;
	}
}

// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
	
{
	Super::BeginPlay();
}

// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward",this,&ThisClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ThisClass::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ThisClass::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ThisClass::LookUp);
	PlayerInputComponent->BindAction("Jump",IE_Pressed, this, &ThisClass::Jump);
	PlayerInputComponent->BindAction("Equip",IE_Pressed,this, &ABlasterCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);
}



void ABlasterCharacter::MoveForward(float Value) {
	const FRotator YawRotation(0.f,Controller->GetControlRotation().Yaw, 0.f);
	const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
	AddMovementInput(Direction, Value);

}

void ABlasterCharacter::MoveRight(float Value) {
	const FRotator PitchRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector Direction(FRotationMatrix(PitchRotation).GetUnitAxis(EAxis::Y));
	AddMovementInput(Direction, Value);
}

void ABlasterCharacter::Turn(float Value) {
	AddControllerYawInput(Value);
}

void ABlasterCharacter::LookUp(float Value) {
	AddControllerPitchInput(Value);
}

void ABlasterCharacter::CrouchButtonPressed() {
	if (bIsCrouched) {
		UnCrouch();
	}
	else {
		Crouch();
	}

}

void ABlasterCharacter::AimButtonPressed()
{
	if (Combat) {
		if (HasAuthority()) {
			Combat->SetAiming(true);
		}
		else {
			Combat->ServerSetAiming(true);
		}
		
	}
}
void ABlasterCharacter::AimButtonReleased()
{
	if (Combat) {
		if (HasAuthority()) {
			Combat->SetAiming(false);
		}
		else {
			Combat->ServerSetAiming(false);
		}

	}
}

void ABlasterCharacter::EquipButtonPressed()
{
	if (Combat) {
		if (HasAuthority()) {
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else {
			ServerEquipButtonPressed();
		}
	}
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}

	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}

}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat) {
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	//官方教程有BUG
	//if (OverlappingWeapon) {
	//	OverlappingWeapon->ShowPickupWidget(false);
	//}
	//OverlappingWeapon = Weapon;
	//if (IsLocallyControlled()) {D
	//	if (OverlappingWeapon) {
	//		OverlappingWeapon->ShowPickupWidget(true);
	//	}
	//}

	if (Weapon) {
		OverlappingWeapon = Weapon;
		if (IsLocallyControlled()) {
			if (OverlappingWeapon) {

				OverlappingWeapon->ShowPickupWidget(true);
			}
		}
	}
	else {
		if (IsLocallyControlled()) {
			if (OverlappingWeapon) {
				OverlappingWeapon->ShowPickupWidget(false);
			}
		}

		OverlappingWeapon = Weapon;
	}

}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (Combat && Combat->bIsAiming);
}


