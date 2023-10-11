// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Animation\CachedAnimData.h>
#include "CoreMinimal.h"
#include "MyAnimInstance.generated.h"


/**
 * 
 */

UENUM(BlueprintType)
enum class LOCOMOTION_STATE : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Walk UMETA(DisplayName = "Walk"),
	Jog UMETA(DisplayName = "Jog")
};


class AMyCharacter;
class UCharacterMovementComponent;
class USkeletalMeshComponent;
UCLASS()
class PROJECTSA_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativePostEvaluateAnimation() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Functions")
	void SetReference();
	UFUNCTION(BlueprintCallable, Category = "Functions")
	void SetEssentialMovementData();
	UFUNCTION(BlueprintCallable, Category = "Functions")
	bool IsMovementWithinThresholds(float minCurrentSpeed, float minMaxSpeed, float minInputAcceleration);
	UFUNCTION(BlueprintCallable, Category = "Functions")
	void DetermineLocomotionState();
	UFUNCTION(BlueprintCallable, Category = "Functions")
	void TrackLocomotionState(LOCOMOTION_STATE Stat);
	UFUNCTION(BlueprintCallable)
	void UpdateLocomotionValues();
	UFUNCTION(BlueprintCallable, Category = "Functions")
	void UpdateOnWalkEntry();
	UFUNCTION(BlueprintCallable, Category = "Functions")
	void UpdateOnJogEntry();
	UFUNCTION(BlueprintCallable, Category = "Functions")
	void UpdateCharacterRotation();
	UFUNCTION(BlueprintCallable, Category = "Functions")
	void ResetTargetRotations();
	UFUNCTION(BlueprintCallable, Category = "Functions")
	void ResetTransitions();
	UFUNCTION(BlueprintCallable, Category = "Functions")
	FRotator GetTargetRotation();
	UFUNCTION()
	void PrintEnumToString(LOCOMOTION_STATE state);


	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set CachedAnimStateData members"), Category = "StateData")
	static FCachedAnimStateData SetCachedAnimStateDataValues(FCachedAnimStateData structure, const FName state_machine_name, const FName state_name);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Make CachedAnimStateData"), Category = "StateData")
	static FCachedAnimStateData MakeCachedAnimStateDataValues(const FName state_machine_name, const FName state_name);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	AMyCharacter* Character;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	UCharacterMovementComponent* CharacterMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* CharaterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EssentialData", meta = (AllowPrivateAccess = "true"))
	FVector Velocity;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EssentialData", meta = (AllowPrivateAccess = "true"))
	float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EssentialData", meta = (AllowPrivateAccess = "true"))
	float MaxSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EssentialData", meta = (AllowPrivateAccess = "true"))
	FVector InputVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EssentialData", meta = (AllowPrivateAccess = "true"))
	bool IsFalling;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EssentialData", meta = (AllowPrivateAccess = "true"))
	float DeltaTimeX;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	LOCOMOTION_STATE LocomotionState;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "String", meta = (AllowPrivateAccess = "true"))
	FString StateString;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float PlayRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (AllowPrivateAccess = "true"))
	FRotator StartRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (AllowPrivateAccess = "true"))
	FRotator PrimaryTargetRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (AllowPrivateAccess = "true"))
	FRotator SecondaryTargetRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float WalkStartAngle;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float JogStartAngle;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	bool PlayWalkStart;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	bool PlayJogStart;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bTrueHasExcuteOnce;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bFalseHasExcuteOnce;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (AllowPrivateAccess = "true"))
	bool DoInputVectorRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "StateData", meta = (AllowPrivateAccess = "true"))
	FCachedAnimStateData WalkStateData;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "StateData", meta = (AllowPrivateAccess = "true"))
	FCachedAnimStateData JogStateData;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "StateData", meta = (AllowPrivateAccess = "true"))
	FCachedAnimStateData LocomotionStateData;
};
