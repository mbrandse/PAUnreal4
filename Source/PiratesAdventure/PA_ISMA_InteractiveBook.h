// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "PA_ISMA_InteractiveBook.generated.h"

/**
 * 
 */
UCLASS()
class PIRATESADVENTURE_API APA_ISMA_InteractiveBook : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:

	APA_ISMA_InteractiveBook();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Static Mesh", meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
		UStaticMeshComponent* MenuItemsPlane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Static Mesh", meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
		UStaticMeshComponent* MenuContentsLevel1Plane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Static Mesh", meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
		UStaticMeshComponent* MenuContentsLevel2Plane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Static Mesh", meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
		UStaticMeshComponent* MenuContentsLevel3Plane;

	//we should make this just a mesh we generate at runtime.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Static Mesh", meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
		UStaticMeshComponent* MenuContentsPlaneTab;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Book UI")
		class UPA_HudElements_IngameMenu* IngameMenuUI;

	void OpenIngameMenu();
	void CloseIngameMenu();
	void OpenMainMenu();
	void CloseMainMenu();
	void OpenSubLevel(int32 SubLevelIndex);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interactive Book", meta = (DisplayName = "Open Ingame Menu"))
		void ReceiveOpenIngameMenu();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interactive Book", meta = (DisplayName = "Close Ingame Menu"))
		void ReceiveCloseIngameMenu();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interactive Book", meta = (DisplayName = "Open Main Menu"))
		void ReceiveOpenMainMenu();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interactive Book", meta = (DisplayName = "Close Main Menu"))
		void ReceiveCloseMainMenu();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interactive Book", meta = (DisplayName = "Open Sub Level"))
		void ReceiveOpenSubLevel(int32 SubLevelIndex);

	UFUNCTION(BlueprintPure, Category = "Interactive Book")
		UStaticMeshComponent* GetSubLevelMesh(int32 SubLevelIndex);

	UFUNCTION(BlueprintCallable, Category = "Book UI")
		void FinalizeCloseMenu();

	UFUNCTION(BlueprintPure, Category = "Book UI")
		float GetFullscreenTransitionDuration();

	UFUNCTION(BlueprintCallable, Category = "Book UI")
		void SetIngameUIElementsVisible();

	UFUNCTION(BlueprintCallable, Category = "Book UI")
		void SetIngameUIElementsHidden();

	UFUNCTION(BlueprintCallable, Category = "Book UI")
		void StartAnimatingSublevelMask();

	UFUNCTION(BlueprintCallable, Category = "Book UI")
		void StopAnimatingSublevelMask();
};
