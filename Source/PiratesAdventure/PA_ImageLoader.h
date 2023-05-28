// Copyright belongs to Digital Hollywood and Michael Brandse

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PixelFormat.h"
#include "PA_ImageLoader.generated.h"

//forward declarations
class UTexture2D;

//Utility class for asynchronously loading an image into a texture.
//Allows Blueprint scripts to request asynchronous loading of an image and be notified when loading is complete.
UCLASS(BlueprintType)
class PIRATESADVENTURE_API UPA_ImageLoader : public UObject
{
	GENERATED_BODY()
	
public:

	//The Async functions now have problems with the ASync call inside one of them. Considering this class is way above my skill level, let's just comment it out.
	//Since 4.23 it seems like the ASync call may have changed. No idea how to correct it though. 
	//Possibly keep an eye on this article; they may post an update. https://wiki.unrealengine.com/Asynchronous_Image_Loading_from_Disk

	////Loads an image from the disk into a texture on a worker thread. This will not block the calling thread.
	////Be aware this is process heavy, so don't use it continuously. 
	////@return; an imageloader event with an OnCompleted event that users can bind to, to get notified when loading is done.
	//UFUNCTION(BlueprintCallable, Category = ImageLoader, meta = (HidePin = "Outer", DefaultToSelf = "Outer"))
	//	static UPA_ImageLoader* LoadImageFromDiscAsync(UObject* Outer, const FString& ImagePath);

	////Loads an image from the disk into a texture on a worker thread. This will not block the calling thread.
	////@return; a future object which will hold the texture once loading is done
	//static TFuture<UTexture2D*> LoadImageFromDiscAsync(UObject* Outer, const FString& ImagePath, TFunction<void()> CompletionCallback);

	//Loads an image from the disk into a texture. This will block the calling thread.
	//@return; A texture created from the loaded image file.
	UFUNCTION(BlueprintCallable, Category = ImageLoader, meta = (HidePin = "Outer", DefaultToSelf = "Outer"))
		static UTexture2D* LoadImageFromDisc(UObject* Outer, const FString& ImagePath);

public:

	//Declare a broadcast-style delegate type, which is used for the load completed event.
	//Dynamic multicast delegates are the only type of event delegates that Blueprint scripts can bind to.
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnImageLoadCompleted, UTexture2D*, Texture);

	//This accessor function allows C++ code to bind to the event.
	FOnImageLoadCompleted& OnLoadCompleted()
	{
		return LoadCompleted;
	}

private:
	////Helper function that initiates the loading operation and fires the event when loading is done.
	//void LoadImageAsync(UObject* Outer, const FString& ImagePath);

	//Helper function to dynamically create a new texture from raw pixel data.
	static UTexture2D* CreateTexture(UObject* Outer, const TArray<uint8>& PixelData, int32 InSizeX, int32 InSizeY, EPixelFormat InFormat = EPixelFormat::PF_B8G8R8A8, FName BaseName = NAME_None);
	
private:

	
	//Holds the load completed event delegate.
	//Giving Blueprint access to this private variable allows Blueprint scripts to bind to the event.
	UPROPERTY(BlueprintAssignable, Category = ImageLoader, meta = (AllowPrivateAccess = true))
		FOnImageLoadCompleted LoadCompleted;

	//Holds the future value which represents the asynchronous loading operation.
	TFuture<UTexture2D*> Future;

public:

	UFUNCTION(BlueprintCallable, Category = ImageLoader)
		FString GetScreenshot(UObject* WorldContextObject, FString ScreenshotName);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnImageSaveCompleted);

	FOnImageSaveCompleted& OnSaveCompleted()
	{
		return SaveCompleted;
	}

	UPROPERTY(BlueprintAssignable, Category = ImageLoader, meta = (AllowPrivateAccess = true))
		FOnImageSaveCompleted SaveCompleted;
	
};
