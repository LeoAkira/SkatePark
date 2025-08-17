// Fill out your copyright notice in the Description page of Project Settings.


#include "SkateboarderCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// Sets default values
ASkateboarderCharacter::ASkateboarderCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->GroundFriction = 1000.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 0.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 0.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	SkateboardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkateboardMesh"));
	SkateboardMesh->SetupAttachment(GetRootComponent());
}

FVector ASkateboarderCharacter::GetLeftFootSocketLocation() const
{
	return GetAdjustedLocation(SkateboardMesh->GetSocketTransform(SkateboardLeftFootSocketName, RTS_Component));
}

FVector ASkateboarderCharacter::GetRightFootSocketLocation() const
{
	return GetAdjustedLocation(SkateboardMesh->GetSocketTransform(SkateboardRightFootSocketName, RTS_Component));
}

void ASkateboarderCharacter::OnConstruction(const FTransform& Transform)
{
	GetMesh()->AttachToComponent(SkateboardMesh, FAttachmentTransformRules::KeepRelativeTransform);
	Super::OnConstruction(Transform);
}

void ASkateboarderCharacter::CalculateSlope()
{
	const FVector SkateboardSocketLocation = SkateboardMesh->GetComponentLocation();
	const FVector ForwardSlopeDetection = SkateboardSocketLocation + SlopeDetectionDistance * GetActorForwardVector();
	const FVector BehindSlopeDetection = SkateboardSocketLocation - SlopeDetectionDistance * GetActorForwardVector();
	const FVector DeltaHeight = GetActorUpVector() * 200;
	
	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, ForwardSlopeDetection + DeltaHeight, ForwardSlopeDetection - DeltaHeight, ECC_WorldStatic))
	{
		CurrentSlope = 0;
		return;
	}
	const FVector ForwardSlopeLocation = Hit.Location;

	if (!GetWorld()->LineTraceSingleByChannel(Hit, BehindSlopeDetection + DeltaHeight, BehindSlopeDetection - DeltaHeight, ECC_WorldStatic))
	{
		CurrentSlope = 0;
		return;
	}
	const FVector BehindSlopeLocation = Hit.Location;
	const float Cat = ForwardSlopeLocation.Z - BehindSlopeLocation.Z;
	const float Hip = (BehindSlopeLocation - ForwardSlopeLocation).Length();

	CurrentSlope = Cat / Hip;
}

void ASkateboarderCharacter::WallCheck()
{
	const FVector HighStartVector = GetActorLocation() + GetActorForwardVector() * 50 + GetActorUpVector() * 50;
	const FVector LowStartVector = GetActorLocation() + GetActorForwardVector() * 50 - GetActorUpVector() * 50;
	const FVector DistTest = GetActorForwardVector() * 10;

	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, HighStartVector, HighStartVector + DistTest, ECC_WorldStatic))
	{
		if (!GetWorld()->LineTraceSingleByChannel(Hit, LowStartVector, LowStartVector + DistTest, ECC_WorldStatic))
		{
			return;
		}
	}
	const float Dot = GetActorForwardVector().Dot(Hit.ImpactNormal);
	Inertia *= -Dot * 0.5f;
	
	const FVector MirrorVector = GetActorForwardVector().MirrorByVector(Hit.ImpactNormal);
	SetActorRotation(MirrorVector.Rotation());
	
}

void ASkateboarderCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	WallCheck();
	CalculateSlope();
	FRotator ActorRotation = GetActorRotation();
	float SlopeAngle = FMath::RadiansToDegrees(FMath::Asin(CurrentSlope));
	ActorRotation.Pitch = FMath::Min(SlopeAngle, MaxSlopeAngle);
	ActorRotation.Roll = 0;
	SetActorRotation(ActorRotation);
	
	AddMovement(-CurrentSlope * SlopeGravityIntensity * DeltaSeconds);

	AddMovementInput(GetActorForwardVector(), Inertia * DeltaSeconds);
	
	if (GetMovementComponent()->IsMovingOnGround())
	{
		Brake(GroundDrag);
	}
}

// Input

void ASkateboarderCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ASkateboarderCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASkateboarderCharacter::JumpPressed);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ASkateboarderCharacter::JumpReleased);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASkateboarderCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASkateboarderCharacter::Look);
	}
}

FVector ASkateboarderCharacter::GetAdjustedLocation(const FTransform& Transform) const
{
	FVector Location = Transform.GetLocation();
	Location *= Transform.GetScale3D();
	return Transform.Rotator().RotateVector(Location);
}

void ASkateboarderCharacter::AddMovement(float Amount)
{
	Inertia += Amount;
	if (Inertia < -0.01f)
	{
		Inertia = -Inertia;
		RotateActorAroundUpVector(180);
	}
	Inertia = FMath::Clamp(Inertia, -2.f, MaxMovement);
}

void ASkateboarderCharacter::Brake(float Amount)
{
	Inertia -= Amount;
	if (Inertia < 0)
	{
		Inertia = 0;
		GetMovementComponent()->StopMovementImmediately();
	}
}

void ASkateboarderCharacter::RotateActorAroundUpVector(const float Angle)
{
	FVector ActorForward = GetActorForwardVector();
	ActorForward = ActorForward.RotateAngleAxis(Angle, GetActorUpVector());
	SetActorRotation(ActorForward.Rotation());
}

void ASkateboarderCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (GetMovementComponent()->IsMovingOnGround())
	{
		if (MovementVector.Y != 0)
		{
			if (MovementVector.Y > 0)
			{
				AddMovement(MovementVector.Y);
			}
			else
			{
				Brake(-MovementVector.Y);
			}
		}
		if (MovementVector.X != 0)
		{
			RotateActorAroundUpVector(RotationSpeed * MovementVector.X);
		}
	}
}

void ASkateboarderCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ASkateboarderCharacter::JumpPressed()
{
	bPreparingJump = true;
}

void ASkateboarderCharacter::JumpReleased()
{
	bPreparingJump = false;
	Jump();
}
