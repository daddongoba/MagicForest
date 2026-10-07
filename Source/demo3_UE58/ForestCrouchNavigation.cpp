#include "ForestCrouchNavigation.h"
#include "ForestRoadAIController.h"
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "NavModifierComponent.h"
#include "NavLinkCustomComponent.h"
#include "NavAreas/NavArea_Default.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
DEFINE_LOG_CATEGORY_STATIC(LogForestCrouch,Log,All);

UForestRoadArea::UForestRoadArea() { DefaultCost=1;DrawColor=FColor(230,220,150); }
UForestRoadQueryFilter::UForestRoadQueryFilter()
{ AddTravelCostOverride(UNavArea_Default::StaticClass(),4);AddTravelCostOverride(UForestRoadArea::StaticClass(),1); }
AForestRoadPreference::AForestRoadPreference()
{
 RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
 Modifier=CreateDefaultSubobject<UNavModifierComponent>(TEXT("RoadPreference"));
 Modifier->SetAreaClass(UForestRoadArea::StaticClass());Modifier->FailsafeExtent=Extent;
}
void AForestRoadPreference::OnConstruction(const FTransform& Transform)
{ Super::OnConstruction(Transform);Modifier->FailsafeExtent=Extent;Modifier->CalcAndCacheBounds(); }

UForestAutoCrouchComponent::UForestAutoCrouchComponent()
{ PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.TickGroup=TG_PrePhysics; }
bool UForestAutoCrouchComponent::CapsuleClear(float HalfHeight,const FVector& Direction,float Distance) const
{
 const auto* C=Cast<ACharacter>(GetOwner());if(!C)return false;
 const auto* Capsule=C->GetCapsuleComponent();
 const float Radius=Capsule->GetScaledCapsuleRadius();
 const FVector Feet=C->GetActorLocation()-FVector(0,0,Capsule->GetScaledCapsuleHalfHeight());
 const FVector Center=Feet+FVector(0,0,HalfHeight+4);
 FCollisionQueryParams Params(SCENE_QUERY_STAT(ForestCrouchClearance),false,C);
 // Another NPC is a traffic obstruction, not a reason to duck.
 for(TActorIterator<APawn> It(GetWorld());It;++It)Params.AddIgnoredActor(*It);
 FHitResult Hit;
 return !GetWorld()->SweepSingleByChannel(Hit,Center,Center+Direction.GetSafeNormal2D()*Distance,FQuat::Identity,ECC_GameTraceChannel1,
  FCollisionShape::MakeCapsule(FMath::Max(1.f,Radius-1),FMath::Max(Radius,HalfHeight-2)),Params);
}
bool UForestAutoCrouchComponent::CanStand() const
{
 const auto* C=Cast<ACharacter>(GetOwner());if(!C)return false;
 const auto* Defaults=C->GetClass()->GetDefaultObject<ACharacter>();
 return CapsuleClear(Defaults->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),FVector::ZeroVector,0);
}
void UForestAutoCrouchComponent::SetManualCrouch(bool Enabled){bManual=Enabled;}
void UForestAutoCrouchComponent::QueuePassage(AForestCrouchPassage* Link)
{
 Passage=Link;bQueued=true;PassagePoints.Reset();
 if(auto* C=Cast<ACharacter>(GetOwner()))
  if(auto* AI=Cast<AAIController>(C->GetController()))
  {
   AI->GetPathFollowingComponent()->PauseMove();
   if(auto* Crowd=Cast<UCrowdFollowingComponent>(AI->GetPathFollowingComponent()))Crowd->SetCrowdSimulationState(ECrowdSimulationState::Disabled);
  }
}
void UForestAutoCrouchComponent::StartPassage(const TArray<FVector>& Points)
{
 // Path following already reached the entrance within the character radius.
 // Do not turn back to chase its exact centre after the smart link activates.
 bQueued=false;PassagePoints=Points;PointIndex=Points.Num()>1?1:0;
 LastProgress=GetOwner()->GetActorLocation();ProgressAt=GetWorld()->GetTimeSeconds();
 if(auto* C=Cast<ACharacter>(GetOwner())) {C->GetCharacterMovement()->StopMovementImmediately();C->Crouch();C->GetCharacterMovement()->MaxWalkSpeedCrouched=Passage->CrouchSpeed;}
 UE_LOG(LogForestCrouch,Log,TEXT("%s: CROUCH_ENTER %s"),*GetNameSafe(GetOwner()),*GetNameSafe(Passage.Get()));
}
void UForestAutoCrouchComponent::CancelPassage()
{
 auto* C=Cast<ACharacter>(GetOwner());auto* Link=Passage.Get();
 Passage.Reset();PassagePoints.Reset();bQueued=false;
 if(Link && C)Link->Release(C,false);
}
FString UForestAutoCrouchComponent::GetCrouchStatus() const
{
 const auto* C=Cast<ACharacter>(GetOwner());
 return Passage.IsValid()?(bQueued?TEXT("Waiting for crouch passage"):TEXT("Crouch passage")):
  C && C->bIsCrouched?TEXT("Crouched"):TEXT("Standing");
}
void UForestAutoCrouchComponent::TickComponent(float DT,ELevelTick Type,FActorComponentTickFunction* Fn)
{
 Super::TickComponent(DT,Type,Fn);
 auto* C=Cast<ACharacter>(GetOwner());if(!C)return;
 const double Now=GetWorld()->GetTimeSeconds();
 if(Passage.IsValid())
 {
  C->Crouch();ClearSince=0;
  if(bQueued || !C->bIsCrouched)return;
  while(PassagePoints.IsValidIndex(PointIndex) && FVector::Dist2D(C->GetActorLocation(),PassagePoints[PointIndex])<12)++PointIndex;
  if(!PassagePoints.IsValidIndex(PointIndex))
  {
   auto* Link=Passage.Get();Passage.Reset();PassagePoints.Reset();
   UE_LOG(LogForestCrouch,Log,TEXT("%s: CROUCH_EXIT %s"),*GetNameSafe(C),*GetNameSafe(Link));
   C->GetCharacterMovement()->MaxWalkSpeedCrouched=100;
   Link->Release(C,true);return;
  }
  if(FVector::Dist2D(C->GetActorLocation(),LastProgress)>15){LastProgress=C->GetActorLocation();ProgressAt=Now;}
  if(Now-ProgressAt>8)
  {
   UE_LOG(LogForestCrouch,Warning,TEXT("%s: crouch passage physically blocked at %s, point %d %s, floor normal %s, capsule %.0fx%.0f"),
    *GetNameSafe(C),*C->GetActorLocation().ToString(),PointIndex,*PassagePoints[PointIndex].ToString(),
    *C->GetCharacterMovement()->CurrentFloor.HitResult.ImpactNormal.ToString(),
    C->GetCapsuleComponent()->GetScaledCapsuleRadius(),C->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
   CancelPassage();return;
  }
  const float Distance=FVector::Dist2D(C->GetActorLocation(),PassagePoints[PointIndex]);
  C->GetCharacterMovement()->MaxWalkSpeedCrouched=FMath::Clamp(Distance/FMath::Max(0.01f,DT),15.f,Passage->CrouchSpeed);
  C->AddMovementInput((PassagePoints[PointIndex]-C->GetActorLocation()).GetSafeNormal2D());return;
 }
 FVector Direction=C->GetVelocity().GetSafeNormal2D();
 if(auto* AI=Cast<AAIController>(C->GetController());AI && AI->GetMoveStatus()==EPathFollowingStatus::Moving)
  Direction=(AI->GetPathFollowingComponent()->GetCurrentTargetLocation()-C->GetActorLocation()).GetSafeNormal2D();
 const float StandHalf=C->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
 const float CrouchHalf=C->GetCharacterMovement()->GetCrouchedHalfHeight()*C->GetCapsuleComponent()->GetShapeScale();
 bAutomatic=!Direction.IsNearlyZero() && !CapsuleClear(StandHalf,Direction,LookAhead) && CapsuleClear(CrouchHalf,Direction,LookAhead);
 if(bManual || bAutomatic){C->Crouch();ClearSince=0;}
 else if(C->bIsCrouched)
 {
  if(!CanStand()){ClearSince=0;return;}
  if(ClearSince==0)ClearSince=Now;
  if(Now-ClearSince>=StandDelay)C->UnCrouch();
 }
}
AForestCrouchPassage::AForestCrouchPassage()
{
 PrimaryActorTick.bCanEverTick=true;
 RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("PassageRoot"));
 PointLinks.Reset();bSmartLinkIsRelevant=true;
 PassagePath=CreateDefaultSubobject<USplineComponent>(TEXT("PassagePath"));PassagePath->SetupAttachment(RootComponent);
 PassagePath->ClearSplinePoints(false);PassagePath->AddSplinePoint(FVector(-200,0,0),ESplineCoordinateSpace::Local,false);
 PassagePath->AddSplinePoint(FVector(200,0,0),ESplineCoordinateSpace::Local,true);
}
void AForestCrouchPassage::RefreshLink()
{
 const int32 N=PassagePath->GetNumberOfSplinePoints();if(N<2)return;
 GetSmartLinkComp()->SetLinkData(PassagePath->GetLocationAtSplinePoint(0,ESplineCoordinateSpace::Local),PassagePath->GetLocationAtSplinePoint(N-1,ESplineCoordinateSpace::Local),ENavLinkDirection::BothWays);
 SetSmartLinkEnabled(bEnabled);
}
void AForestCrouchPassage::OnConstruction(const FTransform& T){Super::OnConstruction(T);RefreshLink();}
void AForestCrouchPassage::BeginPlay(){Super::BeginPlay();OnSmartLinkReached.AddDynamic(this,&AForestCrouchPassage::LinkReached);RefreshLink();}
void AForestCrouchPassage::LinkReached(AActor* Agent,const FVector& Exit)
{
 auto* C=Cast<ACharacter>(Agent);if(!C)return;
 auto* Component=C->FindComponentByClass<UForestAutoCrouchComponent>();
 if(!Component){ResumePathFollowing(Agent);return;}
 Component->QueuePassage(this);Waiting.Add({C,Exit});ActivateNext();
}
void AForestCrouchPassage::ActivateNext()
{
 if(Active.IsValid())return;
 while(!Waiting.IsEmpty())
 {
  const FWaitingAgent Entry=Waiting[0];Waiting.RemoveAt(0);
  auto* C=Entry.Character.Get();if(!C)continue;
  auto* Component=C->FindComponentByClass<UForestAutoCrouchComponent>();if(!Component)continue;
  Active=C;TArray<FVector> Points;
  const int32 N=PassagePath->GetNumberOfSplinePoints();
  const bool Forward=FVector::DistSquared2D(Entry.Exit,PassagePath->GetLocationAtSplinePoint(N-1,ESplineCoordinateSpace::World))<FVector::DistSquared2D(Entry.Exit,PassagePath->GetLocationAtSplinePoint(0,ESplineCoordinateSpace::World));
  for(int32 I=0;I<N;++I)Points.Add(PassagePath->GetLocationAtSplinePoint(Forward?I:N-1-I,ESplineCoordinateSpace::World));
  Component->StartPassage(Points);break;
 }
}
void AForestCrouchPassage::Release(ACharacter* C,bool Success)
{
 if(Active==C)Active.Reset();
 Waiting.RemoveAll([C](const FWaitingAgent& E){return E.Character==C;});
 if(auto* AI=Cast<AAIController>(C->GetController()))
 {
  ResumePathFollowing(C);
  if(auto* Crowd=Cast<UCrowdFollowingComponent>(AI->GetPathFollowingComponent()))Crowd->SetCrowdSimulationState(ECrowdSimulationState::Enabled);
  if(Success)AI->GetPathFollowingComponent()->ResumeMove();
  else{SetSmartLinkEnabled(false);AI->StopMovement();}
 }
 ActivateNext();
}
void AForestCrouchPassage::Tick(float DT){Super::Tick(DT);ActivateNext();}
void UForestCrouchPoseInstance::NativeUpdateAnimation(float DT)
{
 Super::NativeUpdateAnimation(DT);
 const auto* C=Cast<ACharacter>(TryGetPawnOwner());
 CrouchAlpha=FMath::FInterpTo(CrouchAlpha,C && C->bIsCrouched?1.f:0.f,DT,10.f);
}
