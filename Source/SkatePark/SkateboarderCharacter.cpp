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
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->GroundFriction = 0.1f;
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
}

void ASkateboarderCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SkateboardMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SkateboardSocketName);
}

void ASkateboarderCharacter::CalculateSlope()
{
	const FVector SkateboardSocketLocation = GetMesh()->GetSocketTransform(SkateboardSocketName, RTS_World).GetLocation();
	const FVector ForwardSlopeDetection = SkateboardSocketLocation + SlopeDetectionDistance * GetActorForwardVector();
	const FVector BehindSlopeDetection = SkateboardSocketLocation - SlopeDetectionDistance * GetActorForwardVector();
	const FVector DeltaHeight = FVector(0, 0, 50);
	
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

	CurrentSlope = FMath::RadiansToDegrees(FMath::Asin(Cat / Hip));
}

void ASkateboarderCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CalculateSlope();
	float SlopeImpulse = -CurrentSlope * SlopeGravityIntensity * DeltaSeconds;
	AddMovementInput(GetActorForwardVector(), SlopeImpulse);
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

void ASkateboarderCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (UPawnMovementComponent* MovementComponent = GetMovementComponent())
	{
		FVector Velocity = MovementComponent->Velocity;

		//Movement Input
		if (MovementVector.Y != 0)
		{
			if (MovementVector.Y > 0.f || Velocity.Dot(GetActorForwardVector()) > 0.f)
			{
				AddMovementInput(GetActorForwardVector(), MovementVector.Y);
			}
		}
		
		//Rotation Input
		if (MovementVector.X != 0)
		{
			AddActorLocalRotation(FRotator(0, MovementVector.X, 0));

			Velocity = Velocity.RotateAngleAxis(MovementVector.X, FVector::UpVector);
			MovementComponent->Velocity = Velocity;
			MovementComponent->UpdateComponentVelocity();
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
