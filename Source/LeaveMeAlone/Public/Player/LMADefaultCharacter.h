// LeaveMeAlone Game by Netologiya. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LMADefaultCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ULMAHealthComponent;
class UAnimMontage;

UCLASS()
class LEAVEMEALONE_API ALMADefaultCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALMADefaultCharacter();

	UFUNCTION()
	ULMAHealthComponent* GetHealthComponent() const { return HealthComponent; }  

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY()
	UDecalComponent* CurrentCursor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components|Health")
	ULMAHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

	UFUNCTION(BlueprintCallable)
	bool GetSprint() { return isSprint; };

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	float YRotation = -75.0f;

	float ArmLength = 1400.0f;

	float FOV = 55.0f;

	

	void MoveForward(float Value);
	void MoveRight(float Value);
	
	//homework 5 
	const float MaxLengthArm = 1400.0f; // максимальное расстояние камеры от персонажа
	const float MinLengthArm = 500.0f;	// минимальное расстояние камеры от персонажа
	const float LengthChange = 20.0f;	// шаг изменения расстояния до персонажа

	void ZoomIn();
	void ZoomOut();

	void OnDeath();
	void OnHealthChanged(float NewHealth);

	void RotationPlayerOnCursor();

	//homework 6
	bool isSprint = false;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina", meta = (ClampMin = "0", ClampMax = "100"))
	float Stamina = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float MinusStamina = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float PlusStamina = 1.0f;


	void Sprint();
	void StopSprint();

	void DecreaseStamina();
	void IncreseStamina();

};