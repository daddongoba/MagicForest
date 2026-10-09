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

bool UDailyMushroomSubsystem::ConfirmDryPotSynthesis()
{
    if (!Save)
    {
        return false;
    }

    Save->LastConfirmedSynthesisDay = CurrentDay;
    CurrentDay = FMath::Max(1, CurrentDay + 1);
    return Persist();
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

EAlchemyElement UDailyMushroomSubsystem::MushroomElementForIndex(const int32 Index)
{
    // Element is a property of the mushroom type. Day changes availability and pip value only.
    static const TArray<EAlchemyElement> Elements = {
        EAlchemyElement::Fire, EAlchemyElement::Wind, EAlchemyElement::Water,
        EAlchemyElement::Fire, EAlchemyElement::Wind, EAlchemyElement::Water,
        EAlchemyElement::Fire, EAlchemyElement::Wind, EAlchemyElement::Water,
        EAlchemyElement::Fire, EAlchemyElement::Wind, EAlchemyElement::Water,
        EAlchemyElement::Fire, EAlchemyElement::Wind, EAlchemyElement::Water,
        EAlchemyElement::Fire, EAlchemyElement::Wind, EAlchemyElement::Water,
        EAlchemyElement::Fire
    };
    return Elements.IsValidIndex(Index) ? Elements[Index] : EAlchemyElement::Fire;
}

FDailyMushroomCardRow UDailyMushroomSubsystem::MakeDefinition(const int32 Day, const int32 Index)
{
    const TArray<FName>& Catalog = MushroomCatalog();
    FDailyMushroomCardRow Definition;
    if (!Catalog.IsValidIndex(Index))
    {
        return Definition;
    }

    FRandomStream Random(DailySeed(Day, Index));
    Definition.MushroomId = Catalog[Index];
    Definition.DisplayName = Catalog[Index].ToString();
    Definition.Day = Day;
    Definition.bAvailable = Random.RandRange(0, 16) < 10;
    Definition.CardId = FName(*FString::Printf(TEXT("mushroom_day_%d_%02d"), Day, Index));
    Definition.Element = MushroomElementForIndex(Index);
    Definition.PipValue = Random.RandRange(1, 9);
    Definition.Weight = Random.RandRange(1, 3);
    Definition.Tags = TEXT("daily_mushroom,small_card");
    switch (Definition.Element)
    {
    case EAlchemyElement::Fire: Definition.Value.Fire = Definition.PipValue; break;
    case EAlchemyElement::Wind: Definition.Value.Wind = Definition.PipValue; break;
    case EAlchemyElement::Water: Definition.Value.Water = Definition.PipValue; break;
    }
    return Definition;
}

TArray<FDailyMushroomCardRow> UDailyMushroomSubsystem::GetDailyMushrooms() const
{
    TArray<FDailyMushroomCardRow> Result;
    for (int32 Index = 0; Index < MushroomCatalog().Num(); ++Index)
    {
        Result.Add(MakeDefinition(CurrentDay, Index));
    }
    return Result;
}

bool UDailyMushroomSubsystem::GetDailyMushroom(const FName MushroomId, FDailyMushroomCardRow& OutDefinition) const
{
    const int32 Index = MushroomCatalog().Find(MushroomId);
    if (Index == INDEX_NONE)
    {
        OutDefinition = FDailyMushroomCardRow();
        return false;
    }
    OutDefinition = MakeDefinition(CurrentDay, Index);
    return true;
}

bool UDailyMushroomSubsystem::IsMushroomAvailable(const FName MushroomId) const
{
    FDailyMushroomCardRow Definition;
    return GetDailyMushroom(MushroomId, Definition) && Definition.bAvailable;
}

bool UDailyMushroomSubsystem::GetMushroomValue(const FName MushroomId, FAlchemyVector& OutValue) const
{
    FDailyMushroomCardRow Definition;
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
    for (const FDailyMushroomCardRow& Definition : GetDailyMushrooms())
    {
        AvailableCount += Definition.bAvailable ? 1 : 0;
    }
    return FString::Printf(TEXT("第 %d 天 · 今日可采 %d 种蘑菇"), CurrentDay, AvailableCount);
}
