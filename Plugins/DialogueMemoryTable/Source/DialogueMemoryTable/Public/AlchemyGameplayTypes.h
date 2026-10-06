#pragma once

#include "CoreMinimal.h"
#include "DialogueMemoryTypes.h"
#include "AlchemyGameplayTypes.generated.h"

UENUM(BlueprintType)
enum class EAlchemyElement : uint8
{
    Fire,
    Wind,
    Water
};

UENUM(BlueprintType)
enum class EAlchemyCardType : uint8
{
    Pip,
    Transform,
    Bracket,
    Prefix,
    Court,
    Seal
};

USTRUCT(BlueprintType)
struct DIALOGUEMEMORYTABLE_API FAlchemyVector
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int32 Fire = 0;

    UPROPERTY(BlueprintReadWrite)
    int32 Wind = 0;

    UPROPERTY(BlueprintReadWrite)
    int32 Water = 0;
};

USTRUCT(BlueprintType)
struct DIALOGUEMEMORYTABLE_API FAlchemyCardDefinition
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FName Id = NAME_None;

    UPROPERTY(BlueprintReadOnly)
    FString Name;

    UPROPERTY(BlueprintReadOnly)
    FString Latin;

    UPROPERTY(BlueprintReadOnly)
    FString ShortExpression;

    UPROPERTY(BlueprintReadOnly)
    FString Description;

    UPROPERTY(BlueprintReadOnly)
    EAlchemyCardType Type = EAlchemyCardType::Transform;

    UPROPERTY(BlueprintReadOnly)
    EAlchemyElement Element = EAlchemyElement::Fire;

    UPROPERTY(BlueprintReadOnly)
    int32 PipValue = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 Cost = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 BracketLength = 0;
};

USTRUCT(BlueprintType)
struct DIALOGUEMEMORYTABLE_API FAlchemyCardSequenceItem
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FName CardId = NAME_None;

    UPROPERTY(BlueprintReadWrite)
    EAlchemyCardType Type = EAlchemyCardType::Pip;

    UPROPERTY(BlueprintReadWrite)
    EAlchemyElement Element = EAlchemyElement::Fire;

    UPROPERTY(BlueprintReadWrite)
    int32 PipValue = 0;

    UPROPERTY(BlueprintReadWrite)
    int32 Cost = 0;

    UPROPERTY(BlueprintReadWrite)
    int32 BracketLength = 0;
};

USTRUCT(BlueprintType)
struct DIALOGUEMEMORYTABLE_API FAlchemyCommissionDefinition
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 OrderIndex = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 CustomerOrderIndex = 0;

    UPROPERTY(BlueprintReadOnly)
    FString Customer;

    UPROPERTY(BlueprintReadOnly)
    FName CharacterId = NAME_None;

    UPROPERTY(BlueprintReadOnly)
    FString Wish;

    UPROPERTY(BlueprintReadOnly)
    FString SensoryPrompt;

    UPROPERTY(BlueprintReadOnly)
    FString Teaching;

    UPROPERTY(BlueprintReadOnly)
    FString HandCardsSpec;

    UPROPERTY(BlueprintReadOnly)
    FString OpeningLine;

    UPROPERTY(BlueprintReadOnly)
    FAlchemyVector Target;

    UPROPERTY(BlueprintReadOnly)
    int32 SlotCount = 0;

    UPROPERTY(BlueprintReadOnly)
    bool bMustUseAll = false;

    UPROPERTY(BlueprintReadOnly)
    int32 CostLimit = 0;

    UPROPERTY(BlueprintReadOnly)
    TArray<FAlchemyCardDefinition> MustCards;
};

USTRUCT(BlueprintType)
struct DIALOGUEMEMORYTABLE_API FAlchemyEvaluationResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FAlchemyVector Target;

    UPROPERTY(BlueprintReadOnly)
    FAlchemyVector Actual;

    UPROPERTY(BlueprintReadOnly)
    float Projection = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float PerpendicularCost = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float Cosine = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float DoseRatio = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float SumAbsolute = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    FString Verdict;

    UPROPERTY(BlueprintReadOnly)
    FString VerdictCode;

    UPROPERTY(BlueprintReadOnly)
    int32 Reward = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 CostUsed = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 RemainingBudget = 5;

    UPROPERTY(BlueprintReadOnly)
    bool bMustUseAllSatisfied = true;

    UPROPERTY(BlueprintReadOnly)
    FString Reason;

    UPROPERTY(BlueprintReadOnly)
    bool bValid = false;
};

USTRUCT(BlueprintType)
struct DIALOGUEMEMORYTABLE_API FDialoguePersonaLayer
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 FamiliarityLevel = 1;

    UPROPERTY(BlueprintReadOnly)
    FString Title;

    UPROPERTY(BlueprintReadOnly)
    FString Summary;

    UPROPERTY(BlueprintReadOnly)
    FString Prompt;

    UPROPERTY(BlueprintReadOnly)
    TArray<FString> TriggerKeywords;

    UPROPERTY(BlueprintReadOnly)
    bool bUnlockedByDefault = false;
};

USTRUCT(BlueprintType)
struct DIALOGUEMEMORYTABLE_API FDialoguePersonaProfile
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FName PersonaId = NAME_None;

    UPROPERTY(BlueprintReadOnly)
    FString Name;

    UPROPERTY(BlueprintReadOnly)
    FString Title;

    UPROPERTY(BlueprintReadOnly)
    FString Axis;

    UPROPERTY(BlueprintReadOnly)
    FString VoiceTone;

    UPROPERTY(BlueprintReadOnly)
    FString DailyState;

    UPROPERTY(BlueprintReadOnly)
    FString SecretPast;

    UPROPERTY(BlueprintReadOnly)
    TArray<FString> QuickTags;

    UPROPERTY(BlueprintReadOnly)
    TArray<FDialoguePersonaLayer> Layers;
};

USTRUCT(BlueprintType)
struct DIALOGUEMEMORYTABLE_API FDialogueMessage
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FString Role;

    UPROPERTY(BlueprintReadWrite)
    FString Content;
};
