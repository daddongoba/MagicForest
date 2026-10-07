#include "ForestRoadTestCamera.h"
#include "ForestRoadAIController.h"
#include "ForestCrouchNavigation.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InputCoreTypes.h"

namespace { constexpr uint64 RoadCameraMessageKey = 0x464F524553544341; }

AForestRoadTestCamera::AForestRoadTestCamera()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FollowArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = FollowDistance;
    SpringArm->TargetOffset = FVector(0, 0, ShoulderHeight);
    SpringArm->SocketOffset = FVector(0, ShoulderOffset, 0);
    SpringArm->SetRelativeRotation(FRotator(FollowPitch, 0, 0));
    SpringArm->bDoCollisionTest = true;
    SpringArm->bInheritPitch = SpringArm->bInheritYaw = SpringArm->bInheritRoll = false;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 6.f;
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("TestCamera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->FieldOfView = FollowFOV;
    SetActorEnableCollision(false);
}

void AForestRoadTestCamera::BeginPlay()
{
    Super::BeginPlay();
    // This temporary observer never takes over the player's camera in a packaged game.
    if (!bEnabled || GetWorld()->WorldType != EWorldType::PIE)
        SetActorTickEnabled(false);
}

void AForestRoadTestCamera::RestorePlayerInput()
{
    if (bSuppressedPlayerInput)
    {
        if (APlayerController* PC = Observer.Get())
        {
            PC->SetIgnoreMoveInput(false);
            PC->SetIgnoreLookInput(false);
        }
        bSuppressedPlayerInput = false;
    }
}

void AForestRoadTestCamera::SetFollowing(bool bFollow)
{
    APlayerController* PC = Observer.Get();
    if (!PC || (bFollow && !IsValid(TargetNPC))) return;
    bFollowing = bFollow;
    if (bFollow)
    {
        SetActorLocation(TargetNPC->GetActorLocation());
        SpringArm->SetWorldRotation(FRotator(FollowPitch, TargetNPC->GetActorRotation().Yaw, 0));
        PC->SetViewTargetWithBlend(this, 0.35f);
        if (!bSuppressedPlayerInput)
        {
            PC->SetIgnoreMoveInput(true);
            PC->SetIgnoreLookInput(true);
            bSuppressedPlayerInput = true;
        }
    }
    else
    {
        RestorePlayerInput();
        if (PC->GetPawn()) PC->SetViewTargetWithBlend(PC->GetPawn(), 0.35f);
    }
}

void AForestRoadTestCamera::SelectNextNPC()
{
    TArray<APawn*> Candidates;
    for (TActorIterator<APawn> It(GetWorld()); It; ++It)
        if (Cast<AForestRoadAIController>(It->GetController())) Candidates.Add(*It);
    Candidates.Sort([](const APawn& A, const APawn& B) { return A.GetName() < B.GetName(); });
    if (Candidates.IsEmpty()) { TargetNPC = nullptr; SetFollowing(false); return; }
    const int32 Current = Candidates.IndexOfByKey(TargetNPC.Get());
    TargetNPC = Candidates[(Current + 1) % Candidates.Num()];
    if (bFollowing) SetFollowing(true);
}

void AForestRoadTestCamera::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    APlayerController* PC = Observer.Get();
    if (!PC)
    {
        PC = UGameplayStatics::GetPlayerController(this, 0);
        if (!PC) return;
        Observer = PC;
    }
    if (!IsValid(TargetNPC)) SelectNextNPC();
    // Wait for both pawn BeginPlay camera changes and AI possession to finish.
    if (!bInitialViewChosen && GetWorld()->GetTimeSeconds() > 1.f && PC->GetPawn() && IsValid(TargetNPC))
    {
        bInitialViewChosen = true;
        SetFollowing(bAutoFollowOnPlay);
    }
    if (PC->WasInputKeyJustPressed(EKeys::F6)) { bInitialViewChosen = true; SetFollowing(!bFollowing); }
    if (PC->WasInputKeyJustPressed(EKeys::F7)) SelectNextNPC();
    if (PC->WasInputKeyJustPressed(EKeys::F8) && IsValid(TargetNPC))
        if (auto* C=TargetNPC->FindComponentByClass<UForestAutoCrouchComponent>())
            C->SetManualCrouch(C->GetCrouchStatus()==TEXT("Standing"));
    if (PC->WasInputKeyJustPressed(EKeys::F9)) bShowRoadOverlay = !bShowRoadOverlay;
    if (bFollowing && IsValid(TargetNPC))
    {
        SetActorLocation(FMath::VInterpTo(GetActorLocation(), TargetNPC->GetActorLocation(), DeltaSeconds, 8.f));
        const FRotator Desired(FollowPitch, TargetNPC->GetActorRotation().Yaw, 0);
        SpringArm->SetWorldRotation(FMath::RInterpTo(SpringArm->GetComponentRotation(), Desired, DeltaSeconds, 3.f));
        SpringArm->TargetArmLength = FollowDistance;
        SpringArm->TargetOffset = FVector(0, 0, ShoulderHeight);
        SpringArm->SocketOffset = FVector(0, ShoulderOffset, 0);
        Camera->SetFieldOfView(FollowFOV);
    }
    if (GetWorld()->GetTimeSeconds() >= NextOverlayTime)
    {
        NextOverlayTime = GetWorld()->GetTimeSeconds() + 0.1;
        AForestRoadAIController* AI = IsValid(TargetNPC) ? Cast<AForestRoadAIController>(TargetNPC->GetController()) : nullptr;
        if (bShowRoadOverlay && AI) AI->DrawRoadDebug();
        if (GEngine && (bFollowing || bShowRoadOverlay))
        {
            const FString Status = AI ? AI->GetRoadDebugStatus() : TEXT("No road NPC found");
            const float Speed = IsValid(TargetNPC) ? TargetNPC->GetVelocity().Size2D() : 0.f;
            GEngine->AddOnScreenDebugMessage(RoadCameraMessageKey, 0.25f, FColor::Yellow,
                FString::Printf(TEXT("NPC TEST [%s]  F6: player/follow  F7: next NPC  F8: crouch  F9: path\nNPC: %s | speed XY: %.0f cm/s\n%s"),
                bFollowing ? TEXT("FOLLOW") : TEXT("PLAYER"), *GetNameSafe(TargetNPC), Speed, *Status));
        }
    }
}

void AForestRoadTestCamera::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    RestorePlayerInput();
    if (APlayerController* PC = Observer.Get())
        if (PC->GetViewTarget() == this && PC->GetPawn()) PC->SetViewTarget(PC->GetPawn());
    if (GEngine) GEngine->RemoveOnScreenDebugMessage(RoadCameraMessageKey);
    Super::EndPlay(EndPlayReason);
}
