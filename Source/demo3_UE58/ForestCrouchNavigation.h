#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Navigation/NavLinkProxy.h"
#include "NavAreas/NavArea.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "Animation/AnimInstance.h"
#include "ForestCrouchNavigation.generated.h"
class USplineComponent;
class UNavModifierComponent;
class ACharacter;
class AForestCrouchPassage;

UCLASS()
class DEMO3_UE58_API UForestRoadArea : public UNavArea
{ GENERATED_BODY() public: UForestRoadArea(); };
UCLASS()
class DEMO3_UE58_API UForestRoadQueryFilter : public UNavigationQueryFilter
{ GENERATED_BODY() public: UForestRoadQueryFilter(); };

UCLASS(Blueprintable)
class DEMO3_UE58_API AForestRoadPreference : public AActor
{
 GENERATED_BODY()
public:
 AForestRoadPreference();
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Road") FVector Extent=FVector(150,180,150);
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Road") TObjectPtr<UNavModifierComponent> Modifier;
 virtual void OnConstruction(const FTransform& Transform) override;
};

UCLASS(ClassGroup=(NPC),meta=(BlueprintSpawnableComponent))
class DEMO3_UE58_API UForestAutoCrouchComponent : public UActorComponent
{
 GENERATED_BODY()
public:
 UForestAutoCrouchComponent();
 virtual void TickComponent(float DT,ELevelTick Type,FActorComponentTickFunction* Fn) override;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Crouch") float LookAhead=90;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Crouch") float StandDelay=0.6f;
 UFUNCTION(BlueprintCallable,Category="Crouch") void SetManualCrouch(bool bEnabled);
 UFUNCTION(BlueprintPure,Category="Crouch") bool CanStand() const;
 UFUNCTION(BlueprintPure,Category="Crouch") FString GetCrouchStatus() const;
 bool IsInPassage() const { return Passage.IsValid(); }
 void QueuePassage(AForestCrouchPassage* Link);
 void StartPassage(const TArray<FVector>& Points);
 void CancelPassage();
private:
 TWeakObjectPtr<AForestCrouchPassage> Passage;
 TArray<FVector> PassagePoints;
 int32 PointIndex=0;
 bool bManual=false,bQueued=false,bAutomatic=false;
 double ClearSince=0,ProgressAt=0;
 FVector LastProgress=FVector::ZeroVector;
 bool CapsuleClear(float HalfHeight,const FVector& Direction,float Distance) const;
};

/** The editable spline contains floor positions from entrance to exit, in either direction. */
UCLASS(Blueprintable)
class DEMO3_UE58_API AForestCrouchPassage : public ANavLinkProxy
{
 GENERATED_BODY()
public:
 AForestCrouchPassage();
 UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Crouch Passage") TObjectPtr<USplineComponent> PassagePath;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Crouch Passage") bool bEnabled=true;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Crouch Passage") float CrouchSpeed=100;
 UFUNCTION(BlueprintCallable,CallInEditor,Category="Crouch Passage") void RefreshLink();
 void Release(ACharacter* Character,bool bSuccess);
 virtual void OnConstruction(const FTransform& Transform) override;
 virtual void Tick(float DT) override;
protected:
 virtual void BeginPlay() override;
private:
 struct FWaitingAgent { TWeakObjectPtr<ACharacter> Character; FVector Exit; };
 TArray<FWaitingAgent> Waiting;
 TWeakObjectPtr<ACharacter> Active;
 UFUNCTION() void LinkReached(AActor* Agent,const FVector& Exit);
 void ActivateNext();
};

/** Post-process instance bends the existing walking pose into a crouched stance. */
UCLASS(Blueprintable)
class DEMO3_UE58_API UForestCrouchPoseInstance : public UAnimInstance
{
 GENERATED_BODY()
public:
 UPROPERTY(BlueprintReadOnly,Category="Crouch") float CrouchAlpha=0;
 virtual void NativeUpdateAnimation(float DT) override;
};
