// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectSA/AnimInstance/MyAnimInstance.h"
#include <ProjectSA\Character\MyCharacter.h>
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet\KismetMathLibrary.h>

void UMyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SetReference();
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	DeltaTimeX = DeltaSeconds;
	if (Character != nullptr)
	{
		SetEssentialMovementData();

		DetermineLocomotionState();

		TrackLocomotionState(LOCOMOTION_STATE::Idle);
		TrackLocomotionState(LOCOMOTION_STATE::Walk);
		TrackLocomotionState(LOCOMOTION_STATE::Jog);
	}
}


void UMyAnimInstance::SetReference()
{
	if (AMyCharacter* CharacterRef = Cast<AMyCharacter>(GetOwningActor()))
	{
		Character = CharacterRef;
		CharacterMovement = Character->GetCharacterMovement();
		CharaterMesh = Character->GetMesh();
	}
}

void UMyAnimInstance::SetEssentialMovementData()
{
	Velocity = Character->GetVelocity();
	Speed = Velocity.Length();

	MaxSpeed = CharacterMovement->GetMaxSpeed();
	InputVector = UKismetMathLibrary::ClampVectorSize(CharacterMovement->GetLastInputVector(), 0.f, 1.f);

	IsFalling = CharacterMovement->IsFalling();
}

bool UMyAnimInstance::IsMovementWithinThresholds(float minCurrentSpeed, float minMaxSpeed, float minInputAcceleration)
{
	return (minCurrentSpeed <= Speed &&
			minMaxSpeed <= MaxSpeed &&
			minInputAcceleration <= InputVector.Length());
}

void UMyAnimInstance::DetermineLocomotionState()
{
	// 떨어지지 않는 상태라면 속도를 찾아야 함
	if (IsFalling == false)
	{
		// 두 벡터의 내적 계산
		// 벡터를 먼저 정규화 한 뒤 내적 계산 함
		if (FVector::DotProduct(Velocity.GetSafeNormal(), CharacterMovement->GetCurrentAcceleration().GetSafeNormal()) < -0.5f)
		{
			LocomotionState = LOCOMOTION_STATE::Idle;
		}
		else
		{
			if (IsMovementWithinThresholds(1.0f, 300.f, 0.5f))
			{
				LocomotionState = LOCOMOTION_STATE::Jog;
			}
			else if (IsMovementWithinThresholds(1.0f, 0.f, 0.01f))
			{
				LocomotionState = LOCOMOTION_STATE::Walk;
			}
			else
			{
				LocomotionState = LOCOMOTION_STATE::Idle;
			}
		}
	}
	PrintEnumToString(LocomotionState);
}

void UMyAnimInstance::TrackLocomotionState(LOCOMOTION_STATE State)
{
	if (LocomotionState == State)
	{
		if (bTrueHasExcuteOnce == false)
		{
			bTrueHasExcuteOnce = true;
			bFalseHasExcuteOnce = false;
			//TODO OnEntry
			switch (LocomotionState)
			{
			case LOCOMOTION_STATE::Walk:
				UpdateOnWalkEntry();
				break;
			case LOCOMOTION_STATE::Jog:
				UpdateOnJogEntry();
				break;
			}
			
		}
		else
		{
			//TODO WhileTure
		}
	}
	else
	{
		if (bFalseHasExcuteOnce == false)
		{
			bFalseHasExcuteOnce = true;
			bTrueHasExcuteOnce = false;
			// OnExit
		}
		else
		{
			//TODO WhileFalse
		}
	}
	UpdateLocomotionValues();
}

void UMyAnimInstance::UpdateLocomotionValues()
{
	PlayRate = UKismetMathLibrary::SafeDivide(Speed, FMath::Clamp(GetCurveValue(TEXT("MoveData_Speed")), 50.f, 1000.f));
}

void UMyAnimInstance::UpdateOnWalkEntry()
{
	if (Speed < 50.f)
	{
		StartRotation = Character->GetActorRotation();

		// x벡터를 기준으로 한 회전값
		PrimaryTargetRotation = FRotationMatrix::MakeFromX(InputVector).Rotator();
		SecondaryTargetRotation = PrimaryTargetRotation;
		WalkStartAngle = (PrimaryTargetRotation - StartRotation).Yaw;
		PlayWalkStart = true;
	}
	else
	{
		ResetTargetRotations();
	}
}

void UMyAnimInstance::UpdateOnJogEntry()
{
	if (Speed < 200.f)
	{
		StartRotation = Character->GetActorRotation();

		PrimaryTargetRotation = FRotationMatrix::MakeFromX(InputVector).Rotator();
		SecondaryTargetRotation = PrimaryTargetRotation;
		JogStartAngle = (PrimaryTargetRotation - StartRotation).Yaw;
		PlayJogStart = true;
	}
	else
	{
		ResetTargetRotations();
	}
}

void UMyAnimInstance::UpdateCharacterRotation()
{
	if (!Character->HasAnyRootMotion())
	{

	}
}

void UMyAnimInstance::ResetTargetRotations()
{
	PrimaryTargetRotation = Character->GetActorRotation();
	SecondaryTargetRotation = PrimaryTargetRotation;
}

void UMyAnimInstance::PrintEnumToString(LOCOMOTION_STATE state)
{
	const UEnum* CharStateEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("LOCOMOTION_STATE"), true);
	if (CharStateEnum)
	{
		if (StateString == CharStateEnum->GetNameStringByValue((int64)state)) return;
		StateString = CharStateEnum->GetNameStringByValue((int64)state);
	}
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, StateString);
}
