#include "DailyMushroomSubsystem.h"

#include "Kismet/GameplayStatics.h"

void UDailyMushroomSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ConfigureSaveSlot(SaveSlotName, SaveUserIndex);
}

bool UDailyMushroomSubsystem::ConfigureSaveSlot(const FString& InSlotName, const int32 InUserIndex)
{
    if (InSlotName.IsEmpty())
    {
        return false;
    }

    SaveSlotName = InSlotName;
    SaveUserIndex = FMath::Max(0, InUserIndex);
    Save = nullptr;

    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
    {
        Save = Cast<UDailyMushroomSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));
    }

    if (!Save)
    {
        Save = Cast<UDailyMushroomSaveGame>(UGameplayStatics::CreateSaveGameObject(UDailyMushroomSaveGame::StaticClass()));
    }

    CurrentDay = Save ? FMath::Max(1, Save->CurrentDay) : 1;
    if (Save)
    {
        Save->CurrentDay = CurrentDay;
        Persist();
    }
    return Save != nullptr;
}

bool UDailyMushroomSubsystem::Persist()
{
    if (!Save)
    {
        return false;
    }
    Save->CurrentDay = CurrentDay;
    return UGameplayStatics::SaveGameToSlot(Save, SaveSlotName, SaveUserIndex);
}

int32 UDailyMushroomSubsystem::AdvanceDay()
{
    CurrentDay = FMath::Max(1, CurrentDay + 1);
    Persist();
    return CurrentDay;
}

bool UDailyMushroomSubsystem::SetCurrentDay(const int32 NewDay)
{
    if (NewDay < 1)
    {
        return false;
    }
    CurrentDay = NewDay;
    return Persist();
}

const TArray<FName>& UDailyMushroomSubsystem::MushroomCatalog()
{
    static const TArray<FName> Catalog = {
        TEXT("SM_Env_Mushroom_01"), TEXT("SM_Env_Mushroom_02"), TEXT("SM_Env_Mushroom_03"),
        TEXT("SM_Env_Mushroom_04"), TEXT("SM_Env_Mushroom_05"), TEXT("SM_Env_Mushroom_Cap_01"),
        TEXT("SM_Env_Mushroom_Cap_02"), TEXT("SM_Env_Mushroom_Cap_03"), TEXT("SM_Env_Mushroom_Cap_04"),
        TEXT("SM_Env_Mushroom_Small_01"), TEXT("SM_Env_Mushroom_Small_02"), TEXT("SM_Env_Mushroom_Small_03"),
        TEXT("SM_Env_Mushroom_Small_04"), TEXT("SM_Env_Mushroom_Small_05"), TEXT("SM_Env_Mushroom_Small_06"),
        TEXT("SM_Env_Mushroom_Small_07"), TEXT("SM_Env_Mushroom_Small_08"), TEXT("SM_Env_Mushroom_Small_09"),
        TEXT("SM_Env_Mushroom_Small_10")
    };
    return Catalog;
}

int32 UDailyMushroomSubsystem::DailySeed(const int32 Day, const int32 Index)
{
    return FMath::Abs(Day * 92821 + Index * 68917 + 17);
}

FDailyMushroomDefinition UDailyMushroomSubsystem::MakeDefinition(const int32 Day, const int32 Index)
{
    const TArray<FName>& Catalog = MushroomCatalog();
    FDailyMushroomDefinition Definition;
    if (!Catalog.IsValidIndex(Index))
    {
        return Definition;
    }

    const int32 Seed = DailySeed(Day, Index);
    Definition.MushroomId = Catalog[Index];
    Definition.DisplayName = Catalog[Index].ToString();
    Definition.Day = Day;
    Definition.bAvailable = (Seed % 17) < 10;
    Definition.Value.Fire = (Seed / 7) % 9 - 3;
    Definition.Value.Wind = (Seed / 11) % 9 - 3;
    Definition.Value.Water = (Seed / 13) % 9 - 3;
    if (Definition.Value.Fire == 0 && Definition.Value.Wind == 0 && Definition.Value.Water == 0)
    {
        Definition.Value.Fire = 1;
    }
    return Definition;
}

TArray<FDailyMushroomDefinition> UDailyMushroomSubsystem::GetDailyMushrooms() const
{
    TArray<FDailyMushroomDefinition> Result;
    for (int32 Index = 0; Index < MushroomCatalog().Num(); ++Index)
    {
        Result.Add(MakeDefinition(CurrentDay, Index));
    }
    return Result;
}

bool UDailyMushroomSubsystem::GetDailyMushroom(const FName MushroomId, FDailyMushroomDefinition& OutDefinition) const
{
    const int32 Index = MushroomCatalog().Find(MushroomId);
    if (Index == INDEX_NONE)
    {
        OutDefinition = FDailyMushroomDefinition();
        return false;
    }
    OutDefinition = MakeDefinition(CurrentDay, Index);
    return true;
}

bool UDailyMushroomSubsystem::IsMushroomAvailable(const FName MushroomId) const
{
    FDailyMushroomDefinition Definition;
    return GetDailyMushroom(MushroomId, Definition) && Definition.bAvailable;
}

bool UDailyMushroomSubsystem::GetMushroomValue(const FName MushroomId, FAlchemyVector& OutValue) const
{
    FDailyMushroomDefinition Definition;
    if (!GetDailyMushroom(MushroomId, Definition))
    {
        OutValue = FAlchemyVector();
        return false;
    }
    OutValue = Definition.Value;
    return true;
}

FString UDailyMushroomSubsystem::GetDayStatusText() const
{
    int32 AvailableCount = 0;
    for (const FDailyMushroomDefinition& Definition : GetDailyMushrooms())
    {
        AvailableCount += Definition.bAvailable ? 1 : 0;
    }
    return FString::Printf(TEXT("第 %d 天 · 今日可采 %d 种蘑菇"), CurrentDay, AvailableCount);
}
