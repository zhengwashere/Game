// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/BVTypes.h"
#include "BVPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UBVStealthComponent;
class UBVInteractionComponent;
class UBVTakedownComponent;
class UBVEquipmentInventory;
class UInputMappingContext;
class UInputAction;
class UInputComponent;
class ABVBodyActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBVOnSpeedChanged, EBVMovementSpeed, NewSpeed, EBVNoiseLevel, Noise);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBVOnPostureChanged, EBVPosture, NewPosture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBVOnCoverChanged, EBVCoverStance, NewStance);

/**
 * The player's infiltrator avatar: VEIL.
 *
 * Owns the full stealth movement suite (walk/jog/sprint/crouch/prone/silent),
 * cover and corner peeking, wall hugging, leaning, vaulting, ledge hangs,
 * takedowns and body carrying. Movement state is broadcast through delegates so
 * the animation blueprint and HUD stay in sync without polling.
 */
UCLASS()
class BLACKVEIL_API ABVPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABVPlayerCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// ------------------------------------------------------------------
	// Component access
	// ------------------------------------------------------------------
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player")
	UBVStealthComponent* GetStealthComponent() const { return StealthComponent; }

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player")
	UBVInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player")
	UBVTakedownComponent* GetTakedownComponent() const { return TakedownComponent; }

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player")
	UBVEquipmentInventory* GetEquipmentInventory() const { return EquipmentInventory; }

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player")
	UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// ------------------------------------------------------------------
	// Movement state
	// ------------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player|Movement")
	void SetMovementSpeed(EBVMovementSpeed NewSpeed);

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player|Movement")
	EBVMovementSpeed GetMovementSpeed() const { return CurrentSpeed; }

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player|Movement")
	EBVPosture GetPosture() const { return CurrentPosture; }

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player|Movement")
	float GetSpeedForTier(EBVMovementSpeed Tier) const;

	/** Current horizontal speed in cm/s, used by animation and IK. */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player|Movement")
	float GetCurrentPlanarSpeed() const;

	// ------------------------------------------------------------------
	// Posture toggles (bound to input)
	// ------------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player|Movement")
	void ToggleCrouch();

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player|Movement")
	void ToggleProne();

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player|Movement")
	void StartSprint();

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player|Movement")
	void StopSprint();

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player|Movement")
	void SetSilentMove(bool bEnabled);

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player|Movement")
	bool IsSilentMoveActive() const { return bSilentMove; }

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player|Movement")
	bool IsSprinting() const { return bSprinting; }

	// ------------------------------------------------------------------
	// Cover / wall hug / lean / peek
	// ------------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player|Cover")
	bool TryEnterCover();

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player|Cover")
	void ExitCover();

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player|Cover")
	bool IsInCover() const { return bInCover; }

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player|Cover")
	EBVCoverStance GetCoverStance() const { return CoverStance; }

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player|Cover")
	bool TryToggleWallHug();

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player|Cover")
	bool IsWallHugging() const { return bWallHugging; }

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player|Cover")
	void SetLeaning(bool bLeanLeft);

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player|Cover")
	void StopLeaning();

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player|Cover")
	bool IsLeaning() const { return bLeaning; }

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player|Cover")
	float GetLeanAmount() const { return LeanAmount; }



	// ------------------------------------------------------------------
	// Traversal: vault, climb, ledge hang
	// ------------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player|Traversal")
	bool TryVaultOrClimb();

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player|Traversal")
	bool TryLedgeGrab();

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player|Traversal")
	void ReleaseLedge();

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player|Traversal")
	bool IsHangingFromLedge() const { return bLedgeHanging; }

	// ------------------------------------------------------------------
	// Health / status
	// ------------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player")
	void ApplyDamageToPlayer(float Amount);

	UFUNCTION(BlueprintCallable, Category = "BlackVeil|Player")
	void OnIncapacitated();

	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player")
	float GetHealthPercent() const { return MaxHealth > 0.f ? Health / MaxHealth : 0.f; }

	/** Surface under the player's feet, resolved from the physical material. */
	UFUNCTION(BlueprintPure, Category = "BlackVeil|Player")
	EBVSurfaceType GetCurrentSurface() const { return CurrentSurface; }

	// ------------------------------------------------------------------
	// Delegates for animation / HUD
	// ------------------------------------------------------------------
	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Player")
	FBVOnSpeedChanged OnSpeedChanged;

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Player")
	FBVOnPostureChanged OnPostureChanged;

	UPROPERTY(BlueprintAssignable, Category = "BlackVeil|Player")
	FBVOnCoverChanged OnCoverChanged;

protected:
	// ---- Components ----
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components")
	TObjectPtr<UBVStealthComponent> StealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components")
	TObjectPtr<UBVInteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components")
	TObjectPtr<UBVTakedownComponent> TakedownComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BlackVeil|Components")
	TObjectPtr<UBVEquipmentInventory> EquipmentInventory;

	// ---- Tunable movement speeds (cm/s) ----
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Tuning") float ProneSpeed = 80.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Tuning") float CrouchSpeed = 170.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Tuning") float SilentSpeed = 140.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Tuning") float WalkSpeed = 210.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Tuning") float JogSpeed = 375.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Tuning") float SprintSpeed = 620.f;

	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Tuning") float MaxHealth = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Tuning") float CoverTraceDistance = 120.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Tuning") float VaultTraceForward = 120.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Tuning") float VaultTraceHeight = 90.f;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Tuning") float LedgeTraceHeight = 220.f;

	// ---- Runtime state ----
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|State") EBVMovementSpeed CurrentSpeed = EBVMovementSpeed::Walk;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|State") EBVPosture CurrentPosture = EBVPosture::Standing;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|State") bool bSprinting = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|State") bool bSilentMove = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|State") bool bInCover = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|State") bool bWallHugging = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|State") bool bLeaning = false;

	// ---- Input assets (assigned in the Blueprint derived class) ----
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputMappingContext> DefaultMappingContext;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> SprintAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> CrouchAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> ProneAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> JumpVaultAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> InteractAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> FireAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> AimAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> TakedownAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> LeanLeftAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> LeanRightAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> WallHugAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> SilentMoveAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> GadgetSlot1Action;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> GadgetSlot2Action;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> GadgetSlot3Action;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> GadgetSlot4Action;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> SwitchWeaponAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> ReloadAction;
	UPROPERTY(EditDefaultsOnly, Category = "BlackVeil|Input") TObjectPtr<UInputAction> PauseAction;

	// ---- Input handlers ----
	void Input_Move(const struct FInputActionValue& Value);
	void Input_Look(const struct FInputActionValue& Value);
	void Input_SprintStart(const struct FInputActionValue& Value);
	void Input_SprintStop(const struct FInputActionValue& Value);
	void Input_Crouch(const struct FInputActionValue& Value);
	void Input_Prone(const struct FInputActionValue& Value);
	void Input_JumpVault(const struct FInputActionValue& Value);
	void Input_InteractStart(const struct FInputActionValue& Value);
	void Input_InteractStop(const struct FInputActionValue& Value);
	void Input_FireStart(const struct FInputActionValue& Value);
	void Input_FireStop(const struct FInputActionValue& Value);
	void Input_AimStart(const struct FInputActionValue& Value);
	void Input_AimStop(const struct FInputActionValue& Value);
	void Input_Takedown(const struct FInputActionValue& Value);
	void Input_LeanLeft(const struct FInputActionValue& Value);
	void Input_LeanRight(const struct FInputActionValue& Value);
	void Input_WallHug(const struct FInputActionValue& Value);
	void Input_SilentMoveToggle(const struct FInputActionValue& Value);
	void Input_GadgetSlot1(const struct FInputActionValue& Value);
	void Input_GadgetSlot2(const struct FInputActionValue& Value);
	void Input_GadgetSlot3(const struct FInputActionValue& Value);
	void Input_GadgetSlot4(const struct FInputActionValue& Value);
	void Input_SwitchWeapon(const struct FInputActionValue& Value);
	void Input_Reload(const struct FInputActionValue& Value);
	void Input_Pause(const struct FInputActionValue& Value);

	/** Resolves the ground physical material into an EBVSurfaceType. */
	EBVSurfaceType ResolveCurrentSurface() const;

	/** Pushes the current posture/speed to the stealth component each tick. */
	void UpdateStealthInputs();

	/** Camera-relative move input cached for animation. */
	FVector2D LastMoveInput = FVector2D::ZeroVector;
	bool bIsAiming = false;
};

	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|State") bool bLedgeHanging = false;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|State") float LeanAmount = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|State") float Health = 100.f;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|State") EBVCoverStance CoverStance = EBVCoverStance::None;
	UPROPERTY(BlueprintReadOnly, Category = "BlackVeil|State") EBVSurfaceType CurrentSurface = EBVSurfaceType::Concrete;
