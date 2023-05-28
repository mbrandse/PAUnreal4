// Copyright belongs to Digital Hollywood and Michael Brandse

#include "PA_ImageLoader.h"
#include "PiratesAdventure.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "RenderUtils.h"
#include "Engine/Texture2D.h"

//Change the UE_LOG log category name below to whichever log category you want to use.
//#define UIL_LOG(Verbosity, Format, ...)	UE_LOG(LogImage, Verbosity, Format, __VA_ARGS__)

static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));

//See .h for why this has been commented out. 

//UPA_ImageLoader* UPA_ImageLoader::LoadImageFromDiscAsync(UObject* Outer, const FString& ImagePath)
//{
//	// This simply creates a new ImageLoader object and starts an asynchronous load.
//	UPA_ImageLoader* Loader = NewObject<UPA_ImageLoader>();
//	Loader->LoadImageAsync(Outer, ImagePath);
//	return Loader;
//}
//
//void UPA_ImageLoader::LoadImageAsync(UObject* Outer, const FString& ImagePath)
//{
//	// The asynchronous loading operation is represented by a Future, which will contain the result value once the operation is done.
//	// We store the Future in this object, so we can retrieve the result value in the completion callback below.
//	Future = LoadImageFromDiscAsync(Outer, ImagePath, [this]()
//	{
//		// This is the same Future object that we assigned above, but later in time.
//		// At this point, loading is done and the Future contains a value.
//		if (Future.IsValid())
//		{
//			// Notify listeners about the loaded texture on the game thread.
//			AsyncTask(ENamedThreads::GameThread, [this]() { OnLoadCompleted().Broadcast(Future.Get()); });
//		}
//	});
//}
//
//TFuture<UTexture2D*> UPA_ImageLoader::LoadImageFromDiscAsync(UObject* Outer, const FString& ImagePath, TFunction<void()> CompletionCallback)
//{
//	// Run the image loading function asynchronously through a lambda expression, capturing the ImagePath string by value.
//	// Run it on the thread pool, so we can load multiple images simultaneously without interrupting other tasks.
//
//	return Async<UTexture2D*>(EAsyncExecution::ThreadPool, [=]() { return LoadImageFromDisc(Outer, ImagePath); }, CompletionCallback);
//}

UTexture2D* UPA_ImageLoader::LoadImageFromDisc(UObject* Outer, const FString& ImagePath)
{
	//Check if the file exists first
	if (!FPaths::FileExists(ImagePath))
	{
		UE_LOG(AnyLog, Error, TEXT("File not found: %s"), *ImagePath);
		return nullptr;
	}

	// Load the compressed byte data from the file
	TArray<uint8> FileData;
	if (!FFileHelper::LoadFileToArray(FileData, *ImagePath))
	{
		UE_LOG(AnyLog, Error, TEXT("Failed to load file: %s"), *ImagePath);
		return nullptr;
	}

	// Detect the image type using the ImageWrapper module
	EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(FileData.GetData(), FileData.Num());
	if (ImageFormat == EImageFormat::Invalid)
	{
		UE_LOG(AnyLog, Error, TEXT("Failed to create image wrapper for file: %s"), *ImagePath);
		return nullptr;
	}

	// Create an image wrapper for the detected image format
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
	if (!ImageWrapper.IsValid())
	{
		UE_LOG(AnyLog, Error, TEXT("Failed to create image wrapper for file: %s"), *ImagePath);
		return nullptr;
	}

	// Decompress the image data
	const TArray<uint8>* RawData = nullptr;
	ImageWrapper->SetCompressed(FileData.GetData(), FileData.Num());
	ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData);
	if (RawData == nullptr)
	{
		UE_LOG(AnyLog, Error, TEXT("Failed to decompress image file: %s"), *ImagePath);
		return nullptr;
	}

	// Create the texture and upload the uncompressed image data
	FString TextureBaseName = TEXT("Texture_") + FPaths::GetBaseFilename(ImagePath);
	return CreateTexture(Outer, *RawData, ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), EPixelFormat::PF_B8G8R8A8, FName(*TextureBaseName));
}

UTexture2D* UPA_ImageLoader::CreateTexture(UObject* Outer, const TArray<uint8>& PixelData, int32 InSizeX, int32 InSizeY, EPixelFormat InFormat, FName BaseName)
{
	// Shamelessly copied from UTexture2D::CreateTransient with a few modifications
	if (InSizeX <= 0 || InSizeY <= 0 ||
		(InSizeX % GPixelFormats[InFormat].BlockSizeX) != 0 ||
		(InSizeY % GPixelFormats[InFormat].BlockSizeY) != 0)
	{
		UE_LOG(AnyLog, Warning, TEXT("Invalid parameters specified for UImageLoader::CreateTexture()"));
		return nullptr;
	}

	// Most important difference with UTexture2D::CreateTransient: we provide the new texture with a name and an owner
	FName TextureName = MakeUniqueObjectName(Outer, UTexture2D::StaticClass(), BaseName);
	UTexture2D* NewTexture = NewObject<UTexture2D>(Outer, TextureName, RF_Transient);

	NewTexture->PlatformData = new FTexturePlatformData();
	NewTexture->PlatformData->SizeX = InSizeX;
	NewTexture->PlatformData->SizeY = InSizeY;
	NewTexture->PlatformData->PixelFormat = InFormat;

	// Allocate first mipmap and upload the pixel data
	int32 NumBlocksX = InSizeX / GPixelFormats[InFormat].BlockSizeX;
	int32 NumBlocksY = InSizeY / GPixelFormats[InFormat].BlockSizeY;
	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	NewTexture->PlatformData->Mips.Add(Mip);
	//new(NewTexture->PlatformData->Mips) FTexture2DMipMap();
	Mip->SizeX = InSizeX;
	Mip->SizeY = InSizeY;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	void* TextureData = Mip->BulkData.Realloc(NumBlocksX * NumBlocksY * GPixelFormats[InFormat].BlockBytes);
	FMemory::Memcpy(TextureData, PixelData.GetData(), PixelData.Num());
	Mip->BulkData.Unlock();

	NewTexture->UpdateResource();
	return NewTexture;
}

FString UPA_ImageLoader::GetScreenshot(UObject* WorldContextObject, FString ScreenshotName)
{
	//First make a screenshot
	FScreenshotRequest::RequestScreenshot(ScreenshotName, false, false);

	//The get the screenshot path
	FString ScreenshotPath = FPaths::ProjectSavedDir();
	FString Platform = UGameplayStatics::GetPlatformName();
	ScreenshotPath += "Screenshots/" + Platform + "/" + ScreenshotName;

	return ScreenshotPath;
}