// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SkateboarderCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class SKATEPARK_API ASkateboarderCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float RotationSpeed = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SkateboardMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Defaults, meta = (AllowPrivateAccess = "true"))
	FName SkateboardLeftFootSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Defaults, meta = (AllowPrivateAccess = "true"))
	FName SkateboardRightFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Defaults, meta = (AllowPrivateAccess = "true"))
	float SlopeGravityIntensity = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Defaults, meta = (AllowPrivateAccess = "true"))
	float SlopeDetectionDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Defaults, meta = (AllowPrivateAccess = "true"))
	float MaxSlopeAngle = 60.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrentSlope;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bPreparingJump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Defaults, meta = (AllowPrivateAccess = "true"))
	float GroundDrag = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Defaults, meta = (AllowPrivateAccess = "true"))
	float MaxMovement = 100.f;
	
public:
	// Sets default values for this pawn's properties
	ASkateboarderCharacter();

	UFUNCTION(BlueprintPure)
	FVector GetLeftFootSocketLocation() const;

	UFUNCTION(BlueprintPure)
	FVector GetRightFootSocketLocation() const;

protected:

	virtual void OnConstruction(const FTransform& Transform) override;
	
	void CalculateSlope();

	void WallCheck();

	virtual void Tick(float DeltaSeconds) override;
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void JumpPressed();

	void JumpReleased();

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	FVector GetAdjustedLocation(const FTransform& Transform) const;
	void AddMovement(float Amount);
	void Brake(float Amount);
	void RotateActorAroundUpVector(float Angle);
	float Inertia;
	
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
