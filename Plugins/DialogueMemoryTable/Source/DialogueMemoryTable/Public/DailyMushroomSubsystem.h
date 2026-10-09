#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFramework/SaveGame.h"
#include "AlchemyGameplayTypes.h"
#include "DailyMushroomSubsystem.generated.h"

USTRUCT(BlueprintType)
struct DIALOGUEMEMORYTABLE_API FDailyMushroomDefinition
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FName MushroomId = NAME_None;

    UPROPERTY(BlueprintReadOnly)
    FString DisplayName;

    UPROPERTY(BlueprintReadOnly)
    int32 Day = 1;

    UPROPERTY(BlueprintReadOnly)
    bool bAvailable = false;

    UPROPERTY(BlueprintReadOnly)
    FAlchemyVector Value;
};

UCLASS()
class DIALOGUEMEMORYTABLE_API UDailyMushroomSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY()
    int32 CurrentDay = 1;
};

/** Shared world-day state and deterministic daily mushroom availability/values. */
UCLASS()
class DIALOGUEMEMORYTABLE_API UDailyMushroomSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintPure, Category = "World|Day")
    int32 GetCurrentDay() const { return CurrentDay; }

    UFUNCTION(BlueprintCallable, Category = "World|Day")
    int32 AdvanceDay();

    UFUNCTION(BlueprintCallable, Category = "World|Day")
    bool SetCurrentDay(int32 NewDay);

    UFUNCTION(BlueprintPure, Category = "World|Mushrooms")
    TArray<FDailyMushroomDefinition> GetDailyMushrooms() const;

    UFUNCTION(BlueprintPure, Category = "World|Mushrooms")
    bool GetDailyMushroom(FName MushroomId, FDailyMushroomDefinition& OutDefinition) const;

    UFUNCTION(BlueprintPure, Category = "World|Mushrooms")
    bool IsMushroomAvailable(FName MushroomId) const;

    UFUNCTION(BlueprintPure, Category = "World|Mushrooms")
    bool GetMushroomValue(FName MushroomId, FAlchemyVector& OutValue) const;

    UFUNCTION(BlueprintPure, Category = "World|Day")
    FString GetDayStatusText() const;

    UFUNCTION(BlueprintCallable, Category = "World|Day")
    bool ConfigureSaveSlot(const FString& InSlotName, int32 InUserIndex = 0);

private:
    UPROPERTY(Transient)
    TObjectPtr<UDailyMushroomSaveGame> Save;

    UPROPERTY(Transient)
    int32 CurrentDay = 1;

    UPROPERTY(Transient)
    FString SaveSlotName = TEXT("WorldDayState");

    UPROPERTY(Transient)
    int32 SaveUserIndex = 0;

    bool Persist();
    static const TArray<FName>& MushroomCatalog();
    static int32 DailySeed(int32 Day, int32 Index);
    static FDailyMushroomDefinition MakeDefinition(int32 Day, int32 Index);
};
