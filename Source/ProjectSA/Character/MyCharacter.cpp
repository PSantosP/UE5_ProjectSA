// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectSA/Character/MyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"



// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	//static ConstructorHelpers::FObjectFinder<USkeletalMesh>
	//	CharacterMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));

	//if (CharacterMesh.Succeeded())
	//{
	//	PlayerMesh = GetMesh();
	//	PlayerMesh->SetSkeletalMesh(CharacterMesh.Object);
	//}

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	

	CharacterMesh = GetMesh();

	// 캐릭터 메쉬 초기화
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
	if (SM.Succeeded())
	{
		CharacterMesh->SetSkeletalMesh(SM.Object);
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint> ABP(TEXT("/Script/Engine.AnimBlueprint'/Game/EchoLocomotion/ABP_MyAnimInstance.ABP_MyAnimInstance'"));
	if (ABP.Succeeded())
	{
		CharacterMesh->SetAnimClass(ABP.Object->GeneratedClass);
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> 
		InputMappingRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/EchoLocomotion/Input/InputMappingContext.InputMappingContext'"));
	if (InputMappingRef.Succeeded())
	{
		InputMappingContext = InputMappingRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> 
		InputMoveForward(TEXT("/Script/EnhancedInput.InputAction'/Game/EchoLocomotion/Input/IA_Move.IA_Move'"));
	if (InputMoveForward.Succeeded())
	{
		Move = InputMoveForward.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> 
		InputLookAround(TEXT("/Script/EnhancedInput.InputAction'/Game/EchoLocomotion/Input/IA_LookAround.IA_LookAround'"));
	if (InputLookAround.Succeeded())
	{
		LookAround = InputLookAround.Object;
	}


	static ConstructorHelpers::FObjectFinder<UInputAction>
		InputJogOverride(TEXT("/Script/EnhancedInput.InputAction'/Game/EchoLocomotion/Input/IA_JogOverride.IA_JogOverride'"));
	if (InputJogOverride.Succeeded())
	{
		JogOverride = InputJogOverride.Object;
	}

	// SpringArm 초기화
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(CharacterMesh);
	CameraBoom->AddRelativeLocation(FVector(0.f, 0.f, 107.f));
	CameraBoom->AddRelativeRotation(FRotator(0.f, 90.f, 0.f));
	CameraBoom->TargetArmLength = 310.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;
	

	// 캡슐 사이즈 초기화
	// GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);
	// GetCapsuleComponent()->SetCapsuleRadius(42.f);
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);


	// 캐릭터 movement 초기화
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->GroundFriction = 4.f;
	GetCharacterMovement()->MaxWalkSpeed = 147.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 0.f;
	GetCharacterMovement()->AirControl = 0.f;
	GetCharacterMovement()->AirControlBoostMultiplier = 0.f;
	GetCharacterMovement()->AirControlBoostVelocityThreshold = 0.f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 무조건 BeginPlay에서 해줘야 함
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(InputMappingContext, 0);
		}
	}


}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		/*EnhancedInputComponent->BindAction(MoveForward, ETriggerEvent::Started, this, &AMyCharacter::HandleForwardInput);*/
		EnhancedInputComponent->BindAction(Move, ETriggerEvent::Triggered, this, &AMyCharacter::HandleMoveInput);
		/*EnhancedInputComponent->BindAction(MoveForward, ETriggerEvent::Completed, this, &AMyCharacter::HandleForwardInput);*/

		/*EnhancedInputComponent->BindAction(LookUp, ETriggerEvent::Started, this, &AMyCharacter::HandleLookUpInput);*/
		EnhancedInputComponent->BindAction(LookAround, ETriggerEvent::Triggered, this, &AMyCharacter::HandleLookAroundInput);
		/*EnhancedInputComponent->BindAction(LookUp, ETriggerEvent::Completed, this, &AMyCharacter::HandleLookUpInput);*/
		EnhancedInputComponent->BindAction(JogOverride, ETriggerEvent::Triggered, this, &AMyCharacter::HandleJogOverrideStartInput);
		EnhancedInputComponent->BindAction(JogOverride, ETriggerEvent::Completed, this, &AMyCharacter::HandleJogOverrideEndInput);
	}
}


void AMyCharacter::HandleMoveInput(const FInputActionValue& InputValue)
{
	FVector2D Movementvector = InputValue.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const FRotator CharacterRotator = Controller->GetControlRotation();
		const FRotator CharacterYawRoatation(0.f, CharacterRotator.Yaw, 0.f);

		const FVector ForwardVector = FRotationMatrix(CharacterYawRoatation).GetUnitAxis(EAxis::X);

		const FVector RightVector = FRotationMatrix(CharacterYawRoatation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardVector, Movementvector.Y);
		AddMovementInput(RightVector, Movementvector.X);
	}
}

void AMyCharacter::HandleLookAroundInput(const FInputActionValue& InputValue)
{
	FVector2D LookAxisVector = InputValue.Get<FVector2D>();
	// 좌우
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);			// 상하
		AddControllerPitchInput(LookAxisVector.Y);			// 좌우
		// pitch yaw roll -> x, y, z
	}
}

void AMyCharacter::HandleJogOverrideStartInput(const FInputActionValue& InputValue)
{
	GetCharacterMovement()->MaxWalkSpeed = 427.f;
}

void AMyCharacter::HandleJogOverrideEndInput(const FInputActionValue& InputValue)
{
	GetCharacterMovement()->MaxWalkSpeed = 147.f;
}


FVector AMyCharacter::GetMovementSpped()
{
	FVector SpeedValue(IAMoveForwardAction, IAMoveRightAction, 0.f);
	
	// 벡터의 길이를 호출해서 절대값으로 변환
	if (abs(SpeedValue.Length()) < abs(MakeLiteralFloat(0.586f)))
	{
		return UKismetMathLibrary::ClampVectorSize(SpeedValue, 0.207f, 0.332f);
	}
	else
	{
		return SpeedValue;
	}
}

