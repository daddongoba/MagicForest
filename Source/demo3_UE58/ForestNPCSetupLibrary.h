#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ForestNPCSetupLibrary.generated.h"
UCLASS()
class DEMO3_UE58_API UForestNPCSetupLibrary : public UBlueprintFunctionLibrary
{
 GENERATED_BODY()
public:
 UFUNCTION(BlueprintCallable,Category="NPC Setup",meta=(WorldContext="Context")) static FString ConfigureNPCCollision(UObject* Context);
 UFUNCTION(BlueprintCallable,Category="NPC Setup") static bool ConfigureCrouchPose(UObject* AnimBlueprint);
 UFUNCTION(BlueprintCallable,Category="NPC Setup",meta=(WorldContext="Context")) static FString BuildNavigationHelpers(UObject* Context);
 UFUNCTION(BlueprintCallable,Category="NPC Setup",meta=(WorldContext="Context")) static FString CheckDestinationPaths(UObject* Context);
 UFUNCTION(BlueprintCallable,Category="NPC Setup",meta=(WorldContext="Context")) static FString BuildGapPassage(UObject* Context,FVector Start,FVector End,const FString& Label);
 UFUNCTION(BlueprintCallable,Category="NPC Setup",meta=(WorldContext="Context")) static FString CreateArrivalPoints(UObject* Context);
};
