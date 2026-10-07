#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ForestRoadTestCamera.generated.h"

class USpringArmComponent;
class UCameraComponent;
class APawn;
class APlayerController;

/** Reversible PIE-only observer. F6: view toggle; F7: next NPC; F9: navigation overlay. */
UCLASS(Blueprintable)
class DEMO3_UE58_API AForestRoadTestCamera : public AActor
{
    GENERATED_BODY()
public:
    AForestRoadTestCamera();
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC Test")
    bool bEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC Test")
    bool bAutoFollowOnPlay = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC Test")
    bool bShowRoadOverlay = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC Test", meta=(ClampMin="200", ClampMax="2000"))
    float FollowDistance = 280.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC Test")
    float FollowPitch = -8.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC Test")
    float ShoulderHeight = 55.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC Test")
    float ShoulderOffset = 65.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC Test", meta=(ClampMin="40", ClampMax="100"))
    float FollowFOV = 70.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC Test")
    TObjectPtr<APawn> TargetNPC;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="NPC Test")
    TObjectPtr<USpringArmComponent> SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="NPC Test")
    TObjectPtr<UCameraComponent> Camera;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    TWeakObjectPtr<APlayerController> Observer;
    bool bFollowing = false;
    bool bSuppressedPlayerInput = false;
    bool bInitialViewChosen = false;
    double NextOverlayTime = 0.;
    void SelectNextNPC();
    void SetFollowing(bool bFollow);
    void RestorePlayerInput();
};
