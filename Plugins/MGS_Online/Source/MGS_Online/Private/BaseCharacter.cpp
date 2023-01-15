// Copy righted to RAED ABBAS 2023


#include "BaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseCharacter, bIsAim);
	DOREPLIFETIME(ABaseCharacter, AimYaw);
	DOREPLIFETIME(ABaseCharacter, AimRoll);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	AimItem(false);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::AimItem(bool bNewAim)
{
	ServerAim(bNewAim);
}
void ABaseCharacter::ServerAim_Implementation(bool bIsAimNew)
{
	bIsAim = bIsAimNew;
	if (bIsAim == false)
	{
		AimYaw = 0;
		AimRoll = 0;
	}
}

void ABaseCharacter::AimOffset(float NewAimYaw, float NewAimRoll)
{
	ServerAimOffset(NewAimYaw, NewAimRoll);
}
void ABaseCharacter::ServerAimOffset_Implementation(float AimYawNew, float AimRollNew)
{
	AimYaw = AimYaw + AimYawNew;
	AimRoll = AimRoll + AimRollNew;
}
