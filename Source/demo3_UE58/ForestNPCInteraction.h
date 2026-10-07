#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "ForestNPCInteraction.generated.h"
class UTextBlock;
class ACharacter;
class APlayerController;
class AForestRoadAIController;

UCLASS()
class DEMO3_UE58_API UForestNPCInteractionWidget : public UUserWidget
{
 GENERATED_BODY()
public:
 void ShowMessage(const FString& Name,const FString& Body,const FString& Hint);
protected:
 virtual TSharedRef<SWidget> RebuildWidget() override;
private:
 UPROPERTY(Transient) TObjectPtr<UTextBlock> Title;
 UPROPERTY(Transient) TObjectPtr<UTextBlock> Message;
 UPROPERTY(Transient) TObjectPtr<UTextBlock> Prompt;
};

/** A small usable encounter interaction; dialogue systems can use the controller event. */
UCLASS(Blueprintable)
class DEMO3_UE58_API AForestNPCInteraction : public AActor
{
 GENERATED_BODY()
public:
 AForestNPCInteraction();
 virtual void Tick(float DT) override;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NPC Interaction") float InteractionRange=250;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="NPC Interaction") FText Greeting=FText::FromString(TEXT("你好，旅行者。很高兴在森林里遇见你。"));
 UFUNCTION(BlueprintCallable,Category="NPC Interaction") bool TryInteractWith(ACharacter* Character);
 UFUNCTION(BlueprintCallable,Category="NPC Interaction") void FinishInteraction();
 UFUNCTION(BlueprintPure,Category="NPC Interaction") bool IsDialogueOpen() const {return Active.IsValid();}
protected:
 virtual void EndPlay(const EEndPlayReason::Type Reason) override;
private:
 TWeakObjectPtr<APlayerController> Player;
 TWeakObjectPtr<AForestRoadAIController> Active;
 UPROPERTY(Transient) TObjectPtr<UForestNPCInteractionWidget> Widget;
 bool bLockedMovement=false;
 bool CanReach(ACharacter* Character) const;
 FString CharacterName(const ACharacter* Character) const;
};
