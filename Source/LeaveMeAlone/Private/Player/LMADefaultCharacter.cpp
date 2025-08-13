// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "Player/LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "Components/LMAHealthComponent.h"
#include "Components/LMAWeaponComponent.h"

// Sets default values
ALMADefaultCharacter::ALMADefaultCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true); // данное условие не позволит нашей камере поворачиваться в момент поворота
														// персонажа
	SpringArmComponent->TargetArmLength = ArmLength;
	// структура FRotator хранит аргументы в следующей последовательности : Pitch,Yaw, Roll.Так как нам необходимо определить значения по
	// оси Y,мы устанавливаем Pitch аргумент
	SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;


	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetFieldOfView(FOV);
	CameraComponent->bUsePawnControlRotation = false; // данное условие запрещаем камере вращаться относительно SpringArmComponent.

	// запретим нашему персонажу поворачиваться в сторону камеры
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");
	WeaponComponent = CreateDefaultSubobject<ULMAWeaponComponent>("Weapon");
	
}

// Called when the game starts or when spawned
void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}
	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
	OnHealthChanged(HealthComponent->GetHealth());
	HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);
}

// Called every frame
void ALMADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor)
		{
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}

	if (!(HealthComponent->IsDead()))
	{
		RotationPlayerOnCursor();
	}


	WalkOrSprint();


}

// Called to bind functionality to input
void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);

	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &ALMADefaultCharacter::ZoomIn);
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &ALMADefaultCharacter::ZoomOut);


	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ALMADefaultCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ALMADefaultCharacter::StopSprint);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &ULMAWeaponComponent::FireOff);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::Reload);

}

void ALMADefaultCharacter::MoveForward(float Value)
{
	CheckSprintActivity(Value);
	AddMovementInput(GetActorForwardVector(), Value);
}

void ALMADefaultCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void ALMADefaultCharacter::ZoomIn()
{
	float CurrLength = SpringArmComponent->TargetArmLength;
	SpringArmComponent->TargetArmLength -= (CurrLength > MinLengthArm) ? LengthChange : 0;
}

void ALMADefaultCharacter::ZoomOut()
{
	float CurrLength = SpringArmComponent->TargetArmLength;
	SpringArmComponent->TargetArmLength += (CurrLength < MaxLengthArm) ? LengthChange : 0;
}


void ALMADefaultCharacter::OnDeath() 
{
	CurrentCursor->DestroyRenderState_Concurrent();

	PlayAnimMontage(DeathMontage);

	GetCharacterMovement()->DisableMovement();

	SetLifeSpan(5.0f);

	if (Controller)
	{
		Controller->ChangeState(NAME_Spectating);
	}
}

void ALMADefaultCharacter::OnHealthChanged(float NewHealth) 
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health = %f"), NewHealth));
}

void ALMADefaultCharacter::RotationPlayerOnCursor() 
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor)
		{
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}
}

void ALMADefaultCharacter::Sprint()
{
	isShift = true;
}

void ALMADefaultCharacter::StopSprint() 
{
	isShift = false;
}

void ALMADefaultCharacter::WalkOrSprint()
{
	
	if (!isSprintActive)
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkVelocity;
	}
	if (Stamina == StaminaMin)
	{
		isStartCooldown = true;
	}

	if (isStartCooldown)
	{
		Stamina = FMath::Clamp(Stamina + StaminaCost, StaminaMin, StaminaCooldown);
		if (Stamina == StaminaCooldown)
		{
			isStartCooldown = false;
		}
	}
	else if (!isStaminaFull && !isSprintActive)
	{
		Stamina = FMath::Clamp(Stamina + StaminaCost, StaminaMin, StaminaMax);
		if (Stamina == StaminaMax)
		{
			isStaminaFull = true;
		}
		OnStaminaChanged();
	}
}

void ALMADefaultCharacter::CheckSprintActivity(const float& Value)
{
	isSprintActive = false;
	if (isShift && Value > 0 && !isStartCooldown)
	{
		isSprintActive = true;
		Stamina = FMath::Clamp(Stamina - StaminaCost, StaminaMin, StaminaMax);
		OnStaminaChanged();
		GetCharacterMovement()->MaxWalkSpeed = SprintVelocity;
		isStaminaFull = false;
	}
}

void ALMADefaultCharacter::OnStaminaChanged()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Stamina = %f"), Stamina));
}