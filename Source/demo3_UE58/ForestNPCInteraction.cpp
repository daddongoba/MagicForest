#include "ForestNPCInteraction.h"
#include "ForestRoadAIController.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "InputCoreTypes.h"

TSharedRef<SWidget> UForestNPCInteractionWidget::RebuildWidget()
{
 if(!WidgetTree)WidgetTree=NewObject<UWidgetTree>(this,TEXT("EncounterWidgetTree"));
 auto* Canvas=WidgetTree->ConstructWidget<UCanvasPanel>();WidgetTree->RootWidget=Canvas;
 auto* Border=WidgetTree->ConstructWidget<UBorder>();Border->SetBrushColor(FLinearColor(0.02f,0.03f,0.025f,0.92f));Border->SetPadding(FMargin(20,14));
 auto* CanvasSlot=Canvas->AddChildToCanvas(Border);CanvasSlot->SetAnchors(FAnchors(0.5f,1.f));CanvasSlot->SetAlignment(FVector2D(0.5f,1.f));CanvasSlot->SetPosition(FVector2D(0,-40));CanvasSlot->SetAutoSize(true);
 auto* Box=WidgetTree->ConstructWidget<UVerticalBox>();Border->SetContent(Box);
 Title=WidgetTree->ConstructWidget<UTextBlock>();Message=WidgetTree->ConstructWidget<UTextBlock>();Prompt=WidgetTree->ConstructWidget<UTextBlock>();
 for(UTextBlock* Text:{Title.Get(),Message.Get(),Prompt.Get()})
 {Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));Text->SetWrapTextAt(520);Box->AddChildToVerticalBox(Text);}
 auto Font=Title->GetFont();Font.Size=22;Title->SetFont(Font);Font.Size=18;Message->SetFont(Font);Prompt->SetFont(Font);
 Prompt->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f,0.9f,0.65f)));
 return Super::RebuildWidget();
}
void UForestNPCInteractionWidget::ShowMessage(const FString& Name,const FString& Body,const FString& Hint)
{
 if(!Title)return;
 Title->SetText(FText::FromString(Name));Message->SetText(FText::FromString(Body));Prompt->SetText(FText::FromString(Hint));
 Title->SetVisibility(Name.IsEmpty()?ESlateVisibility::Collapsed:ESlateVisibility::HitTestInvisible);
 Message->SetVisibility(Body.IsEmpty()?ESlateVisibility::Collapsed:ESlateVisibility::HitTestInvisible);
 SetVisibility(Hint.IsEmpty()?ESlateVisibility::Hidden:ESlateVisibility::HitTestInvisible);
}
AForestNPCInteraction::AForestNPCInteraction(){PrimaryActorTick.bCanEverTick=true;SetActorEnableCollision(false);}
FString AForestNPCInteraction::CharacterName(const ACharacter* Character) const
{const FString Name=GetNameSafe(Character);return Name.Contains(TEXT("Natta"))?TEXT("Natta"):Name.Contains(TEXT("Fawnia"))?TEXT("Fawnia"):TEXT("森林女巫");}
bool AForestNPCInteraction::CanReach(ACharacter* Character) const
{
 auto* PC=Player.Get();if(!PC || !PC->GetPawn() || !Character || PC->GetViewTarget()!=PC->GetPawn())return false;
 if(FVector::Dist(PC->GetPawn()->GetActorLocation(),Character->GetActorLocation())>InteractionRange)return false;
 FCollisionQueryParams Params(SCENE_QUERY_STAT(ForestEncounterVisibility),false);Params.AddIgnoredActor(PC->GetPawn());Params.AddIgnoredActor(Character);
 FHitResult Hit;return !GetWorld()->LineTraceSingleByChannel(Hit,PC->GetPawn()->GetActorLocation()+FVector(0,0,40),Character->GetActorLocation()+FVector(0,0,40),ECC_Camera,Params);
}
bool AForestNPCInteraction::TryInteractWith(ACharacter* Character)
{
 auto* PC=Player.Get();auto* AI=Character?Cast<AForestRoadAIController>(Character->GetController()):nullptr;
 if(!PC || Active.IsValid() || !AI || !CanReach(Character) || !AI->BeginInteraction(PC->GetPawn()))return false;
 Active=AI;PC->SetIgnoreMoveInput(true);bLockedMovement=true;
 if(Widget)Widget->ShowMessage(CharacterName(Character),Greeting.ToString(),TEXT("E / Esc：结束交谈"));return true;
}
void AForestNPCInteraction::FinishInteraction()
{
 if(auto* AI=Active.Get())AI->EndInteraction();Active.Reset();
 if(bLockedMovement){if(auto* PC=Player.Get())PC->SetIgnoreMoveInput(false);bLockedMovement=false;}
 if(Widget)Widget->ShowMessage(TEXT(""),TEXT(""),TEXT(""));
}
void AForestNPCInteraction::Tick(float DT)
{
 Super::Tick(DT);auto* PC=Player.Get();if(!PC){PC=UGameplayStatics::GetPlayerController(this,0);Player=PC;}if(!PC || !PC->GetPawn())return;
 if(!Widget){Widget=CreateWidget<UForestNPCInteractionWidget>(PC);if(Widget){Widget->AddToViewport(20);Widget->ShowMessage(TEXT(""),TEXT(""),TEXT(""));}}
 if(Active.IsValid())
 {
  if(PC->WasInputKeyJustPressed(EKeys::E) || PC->WasInputKeyJustPressed(EKeys::Escape) || PC->GetViewTarget()!=PC->GetPawn() || !Active->GetPawn() || FVector::Dist(Active->GetPawn()->GetActorLocation(),PC->GetPawn()->GetActorLocation())>InteractionRange*1.8f)FinishInteraction();return;
 }
 if(bLockedMovement)FinishInteraction();
 ACharacter* Nearest=nullptr;float Distance=InteractionRange;
 for(TActorIterator<ACharacter> It(GetWorld());It;++It)
  if(Cast<AForestRoadAIController>(It->GetController()))
  {const float D=FVector::Dist(It->GetActorLocation(),PC->GetPawn()->GetActorLocation());if(D<Distance && CanReach(*It)){Distance=D;Nearest=*It;}}
 if(Widget)Widget->ShowMessage(TEXT(""),TEXT(""),Nearest?FString::Printf(TEXT("E：与 %s 交谈"),*CharacterName(Nearest)):TEXT(""));
 if(Nearest && PC->WasInputKeyJustPressed(EKeys::E))TryInteractWith(Nearest);
}
void AForestNPCInteraction::EndPlay(const EEndPlayReason::Type Reason)
{FinishInteraction();if(Widget)Widget->RemoveFromParent();Super::EndPlay(Reason);}
