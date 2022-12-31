// Copy righted to RAED ABBAS 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class MGS_ONLINE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray <FLifetimeProperty> &OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "MGS|Combat")
	void AimItem(bool bNewAim);
	UFUNCTION(BlueprintCallable, Category = "MGS|Combat")
	void AimOffset(float NewAimYaw, float NewAimRoll);

private:
	UFUNCTION(Server, Reliable)
	void ServerAim(bool bIsAimNew);
	UFUNCTION(Server, Reliable)
	void ServerAimOffset(float AimYawNew, float AimRollNew);

public:
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "MGS|Combat")
	bool bIsAim = false;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "MGS|Combat")
	float AimYaw = 0.0f;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "MGS|Combat")
	float AimRoll = 0.0f;
};