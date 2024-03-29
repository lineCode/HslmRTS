﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "FogOfWar.h"

#include "HslmRTS.h"
#include "FogOfWarSubsystem.h"
#include "HslmRTSFunctionLibrary.h"
#include "RTSAgentComponent.h"
#include "RTSWorldVolume.h"
#include "Components/BrushComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/PostProcessVolume.h"

typedef uint8_t* Pixels4x4;

uint32_t UpscaleMapping[16 * 16]=
{
	0x00000000, 0x00000000, 0x00000000, 0x00000000, // 0
	0x00000000, 0x00000000, 0x00000000, 0x00000000, // 0x00, 0x00
	0x00000000, 0x00000000, 0x00000000, 0x00000000, // 0x00, 0x00
	0x00000000, 0x00000000, 0x00000000, 0x00000000,

	0x00FF0000, 0x00800000, 0x00000000, 0x00000000, // 1
    0x00800000, 0x00000000, 0x00000000, 0x00000000, // 0xFF, 0x00
    0x00000000, 0x00000000, 0x00000000, 0x00000000, // 0x00, 0x00
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
	
	0x00000000, 0x00000000, 0x00800000, 0x00FF0000, // 2
	0x00000000, 0x00000000, 0x00000000, 0x00800000, // 0x00, 0xFF
	0x00000000, 0x00000000, 0x00000000, 0x00000000, // 0x00, 0x00
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	
	0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, // 3
    0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, // 0xFF, 0xFF
    0x00000000, 0x00000000, 0x00000000, 0x00000000, // 0x00, 0x00
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
	
	0x00000000, 0x00000000, 0x00000000, 0x00000000, // 4
    0x00000000, 0x00000000, 0x00000000, 0x00000000, // 0x00, 0x00
    0x00800000, 0x00000000, 0x00000000, 0x00000000, // 0xFF, 0x00
    0x00FF0000, 0x00800000, 0x00000000, 0x00000000,

	0x00FF0000, 0x00FF0000, 0x00000000, 0x00000000, // 5
    0x00FF0000, 0x00FF0000, 0x00000000, 0x00000000, // 0xFF, 0x00
    0x00FF0000, 0x00FF0000, 0x00000000, 0x00000000, // 0xFF, 0x00
    0x00FF0000, 0x00FF0000, 0x00000000, 0x00000000,
	
	0x00000000, 0x00000000, 0x00800000, 0x00FF0000, // 6
	0x00000000, 0x00000000, 0x00000000, 0x00800000, // 0x00, 0xFF
	0x00800000, 0x00000000, 0x00000000, 0x00000000, // 0xFF, 0x00
	0x00FF0000, 0x00800000, 0x00000000, 0x00000000,
	
	0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, // 7
    0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, // 0xFF, 0xFF
    0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00800000, // 0xFF, 0x00
    0x00FF0000, 0x00FF0000, 0x00800000, 0x00000000,

	0x00000000, 0x00000000, 0x00000000, 0x00000000, // 8
	0x00000000, 0x00000000, 0x00000000, 0x00000000, // 0x00, 0x00
	0x00000000, 0x00000000, 0x00000000, 0x00800000, // 0x00, 0xFF
	0x00000000, 0x00000000, 0x00800000, 0x00FF0000,
		
	0x00FF0000, 0x00800000, 0x00000000, 0x00000000, // 9
    0x00800000, 0x00000000, 0x00000000, 0x00000000, // 0xFF, 0x00
    0x00000000, 0x00000000, 0x00000000, 0x00800000, // 0x00, 0xFF
    0x00000000, 0x00000000, 0x00800000, 0x00FF0000,

	0x00000000, 0x00000000, 0x00FF0000, 0x00FF0000, // 10
	0x00000000, 0x00000000, 0x00FF0000, 0x00FF0000, // 0x00, 0xFF
	0x00000000, 0x00000000, 0x00FF0000, 0x00FF0000, // 0x00, 0xFF
	0x00000000, 0x00000000, 0x00FF0000, 0x00FF0000,

	0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, // 11
    0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, // 0xFF, 0xFF
    0x00800000, 0x00FF0000, 0x00FF0000, 0x00FF0000, // 0x00, 0xFF
    0x00000000, 0x00800000, 0x00FF0000, 0x00FF0000,
	
	0x00000000, 0x00000000, 0x00000000, 0x00000000, // 12
	0x00000000, 0x00000000, 0x00000000, 0x00000000, // 0x00, 0x00
	0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, // 0xFF, 0xFF
	0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000,

	0x00FF0000, 0x00FF0000, 0x00800000, 0x00000000, // 13
	0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00800000, // 0xFF, 0x00
	0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, // 0xFF, 0xFF
	0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000,
	
	0x00000000, 0x00800000, 0x00FF0000, 0x00FF0000, // 14
	0x00800000, 0x00FF0000, 0x00FF0000, 0x00FF0000, // 0x00, 0xFF
	0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, // 0xFF, 0xFF
	0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000,

	0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, // 15
	0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, // 0xFF, 0xFF
	0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, // 0xFF, 0xFF
	0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000,
};

// Sets default values
AFogOfWar::AFogOfWar()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GridDecal =CreateDefaultSubobject<UDecalComponent>(TEXT("GridDecal"));
	RootComponent = GridDecal;
	GridDecal->bIsEditorOnly = true;
	GridDecal->SetRelativeRotation(FRotator(0.f,90.f, 0.f));

	TileSize = 100.f;

}

// Called when the game starts or when spawned
void AFogOfWar::BeginPlay()
{
	Super::BeginPlay();
	Initialize();
	CreateTexture();
}

void AFogOfWar::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyTexture();
	Cleanup();
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AFogOfWar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateFogOfWar();
}

void AFogOfWar::UpdateFogOfWar()
{
	auto FogOfWarSubsystem = GetWorld()->GetSubsystem<UFogOfWarSubsystem>();
	for (int32 Y = 0; Y < TextureResolution.Y; ++Y)
	{
		for (int32 X = 0; X < TextureResolution.X; ++X)
		{
			const int i = Y * TextureResolution.Y + X;

			const int iBlue = i * 4 + 0;
			const int iGreen = i * 4 + 1;
			const int iRed = i * 4 + 2;
			const int iAlpha = i * 4 + 3;
			TextureBuffer[iBlue] = 0;
			TextureBuffer[iGreen] = 0;
			TextureBuffer[iRed] = 0;
			TextureBuffer[iAlpha] = 0;
		}
	}
	for (auto Agent : FogOfWarSubsystem->GetRTSAgents())
	{
		FRecursiveVisionContext Context;
		Context.MaxDepth = FMath::FloorToFloat(Agent->VisionRadius / TileSize);
		Context.MaxDepthSquare = (Context.MaxDepth + 0.5) * (Context.MaxDepth + 0.5);
		WorldLocationToTileXY(Agent->GetComponentLocation(), Context.OriginX, Context.OriginY);
		MarkVision(Context.OriginX, Context.OriginY);
		IterateVisionBottom(Context,1);
		IterateVisionTop(Context,1);
		IterateVisionRight(Context,1);
		IterateVisionLeft(Context,1);
	}
	if(bUseUpscaleTexture)
	{
		for (int32 Y = 0; Y < TextureResolution.Y; ++Y)
		{
			FString TempString;
			for (int32 X = 0; X < TextureResolution.X; ++X)
			{

				int32 NeighborY = FMath::Min(Y + 1, TextureResolution.Y - 1);
				int32 NeighborX = FMath::Min(X + 1, TextureResolution.X - 1);
				uint8 Pixel11 = TextureBuffer[(Y * TextureResolution.X + X)*4 + 2];
				uint8 Pixel12 = TextureBuffer[(Y * TextureResolution.X + NeighborX)*4 + 2];
				uint8 Pixel21 = TextureBuffer[(NeighborY * TextureResolution.X + X)*4 + 2];
				uint8 Pixel22 = TextureBuffer[(NeighborY * TextureResolution.X + NeighborX)*4 + 2];
				
				int32 Index = ((Pixel11 & 1)|(Pixel12 & 2)|(Pixel21 & 4)|(Pixel22 & 8)) << 4;
				check((Index /16 * 16 == Index) && Index < 256);
				uint32_t* UpscaleMappingPtr = &UpscaleMapping[Index];
				TempString += FString::Printf(TEXT("%02d"), Index / 16);
				for (int32 Y2 = 0; Y2 < 4; Y2++)
				{
					const int i = ((Y * 4 + Y2) * TextureResolution.X) * 4 * 4 + (X * 4) * 4;
					FMemory::Memcpy(&UpscaleTextureBuffer[i], &UpscaleMappingPtr[Y2 * 4], 4 * sizeof(uint8) * 4);
					// for (int32 X2 = 0; X2 < 4; X2++)
					// {
					// 	const int i = ((Y * 4 + Y2) * TextureResolution.X) * 4 * 4 + (X * 4) * 4 + X2 * 4;
					// 	FMemory::Memcpy(&UpscaleTextureBuffer[i], &UpscaleMappingPtr[Y2 * 4 + X2], 4 * sizeof(uint8));
					// }
				}
			}
			//UE_LOG(LogHslmRTS, Log, TEXT("%s"), *TempString);
		}

		TArray<float> GaussianBlurKernel = UHslmRTSFunctionLibrary::ComputeGaussianKernel(7,0.84089642);
		// FString TempString;
		// float sum = 0;
		// for (int32 Y = 0; Y < GaussianBlurKernel.Num(); ++Y)
		// {
		// 	TempString += FString::Printf(TEXT(" %f "), GaussianBlurKernel[Y] );
		// 	sum +=GaussianBlurKernel[Y] ;
		// }
		// UE_LOG(LogHslmRTS, Log, TEXT("%s"), *TempString);
		// UE_LOG(LogHslmRTS, Log, TEXT("%f"), sum);
		double GaussianBlurTime;
		{
			SCOPE_SECONDS_COUNTER(GaussianBlurTime);
			int32 GaussianBlurKernelNum = GaussianBlurKernel.Num();
			int32 HalfGaussianBlurKernelNum = GaussianBlurKernelNum / 2;
			
			for (int32 Y = 0; Y < UpscaleTextureResolution.Y; ++Y)
			{
				for (int32 X = 0; X < HalfGaussianBlurKernelNum; ++X)
				{
					float Sum = 0;
					for (int32 i = -HalfGaussianBlurKernelNum; i <= HalfGaussianBlurKernelNum; ++i)
					{
						if(X + i > 0)
						{
							Sum += UpscaleTextureBuffer[(Y * UpscaleTextureResolution.X + X + i) * 4 + 2] * GaussianBlurKernel[i + HalfGaussianBlurKernelNum];
						}
					}
					UpscaleTextureHorizontalBlurData[(Y * UpscaleTextureResolution.X + X) * 4 + 2] = static_cast<uint8>(Sum);
				}
				for (int32 X = HalfGaussianBlurKernelNum; X < UpscaleTextureResolution.X - HalfGaussianBlurKernelNum; ++X)
				{
					float Sum = 0;
					for (int32 i = -HalfGaussianBlurKernelNum; i <= HalfGaussianBlurKernelNum; ++i)
					{
						Sum += (UpscaleTextureBuffer[(Y * UpscaleTextureResolution.X + X + i) * 4 + 2] * GaussianBlurKernel[i + HalfGaussianBlurKernelNum]);
					}
					UpscaleTextureHorizontalBlurData[(Y * UpscaleTextureResolution.X + X) * 4 + 2] = static_cast<uint8>(Sum);
				}
				for (int32 X = UpscaleTextureResolution.X  - HalfGaussianBlurKernelNum; X < UpscaleTextureResolution.X; ++X)
				{
					float Sum = 0;
					for (int32 i = -HalfGaussianBlurKernelNum; i <= HalfGaussianBlurKernelNum; ++i)
					{
						if(X + i < UpscaleTextureResolution.X)
						{
							Sum += UpscaleTextureBuffer[(Y * UpscaleTextureResolution.X + X + i) * 4 + 2] * GaussianBlurKernel[i + HalfGaussianBlurKernelNum];
						}
					}
					UpscaleTextureHorizontalBlurData[(Y * UpscaleTextureResolution.X + X) * 4 + 2] = static_cast<uint8>(Sum);
				}
			}
			//FMemory::Memcpy(UpscaleTextureBuffer.GetData(),UpscaleTextureHorizontalBlurData.GetData(),UpscaleTextureResolution.X * UpscaleTextureResolution.Y * 4);

			for (int32 X = 0; X < UpscaleTextureResolution.X; ++X)
			{
				for (int32 Y = 0; Y < HalfGaussianBlurKernelNum; ++Y)
				{
					float Sum = 0;
					for (int32 i = -HalfGaussianBlurKernelNum; i <= HalfGaussianBlurKernelNum; ++i)
					{
						if(Y + i > 0)
						{
							Sum += UpscaleTextureHorizontalBlurData[((Y + i) * UpscaleTextureResolution.X + X) * 4 + 2] * GaussianBlurKernel[i + HalfGaussianBlurKernelNum];
						}
					}
					UpscaleTextureBuffer[(Y * UpscaleTextureResolution.X + X) * 4 + 2] = static_cast<uint8>(Sum);
				}
				for (int32 Y = HalfGaussianBlurKernelNum; Y < UpscaleTextureResolution.Y - HalfGaussianBlurKernelNum; ++Y)
				{
					float Sum = 0;
					for (int32 i = -HalfGaussianBlurKernelNum; i <= HalfGaussianBlurKernelNum; ++i)
					{
						Sum += UpscaleTextureHorizontalBlurData[((Y + i) * UpscaleTextureResolution.X + X) * 4 + 2] * GaussianBlurKernel[i + HalfGaussianBlurKernelNum];
					}
					UpscaleTextureBuffer[(Y * UpscaleTextureResolution.X + X) * 4 + 2] = static_cast<uint8>(Sum);
				}
				for (int32 Y = UpscaleTextureResolution.Y  - HalfGaussianBlurKernelNum; Y < UpscaleTextureResolution.Y; ++Y)
				{
					float Sum = 0;
					for (int32 i = -HalfGaussianBlurKernelNum; i <= HalfGaussianBlurKernelNum; ++i)
					{
						if(Y + i < UpscaleTextureResolution.Y)
						{
							Sum += UpscaleTextureHorizontalBlurData[((Y + i) * UpscaleTextureResolution.X + X) * 4 + 2] * GaussianBlurKernel[i + HalfGaussianBlurKernelNum];
						}
					}
					UpscaleTextureBuffer[(Y * UpscaleTextureResolution.X + X) * 4 + 2] = static_cast<uint8>(Sum);
				}
			}
		}
		UE_LOG(LogHslmRTS, Log, TEXT("GaussianBlurTime %f"), GaussianBlurTime);  
		// for (int32 X = 0; X < TextureResolution.X * 4; ++X)
		// {
		// 	for (int32 Y = 0; Y < HalfGaussianBlurKernelNum; ++Y)
		// 	{
		// 		float Sum = 0;
		// 		for (int32 i = -HalfGaussianBlurKernelNum; i <= HalfGaussianBlurKernelNum; ++i)
		// 		{
		// 			if(Y + i > 0)
		// 			{
		// 				Sum += UpscaleTextureHorizontalBlurData[((Y + i) * TextureResolution.X * 4 + X) * 4 + 1] * GaussianBlurKernel[i + HalfGaussianBlurKernelNum];
		// 			}
		// 		}
		// 		UpscaleTextureBuffer[(Y * TextureResolution.X * 4 + X) * 4] = Sum;
		// 	}
		// 	for (int32 Y = HalfGaussianBlurKernelNum; Y < TextureResolution.Y * 4  - HalfGaussianBlurKernelNum; ++Y)
		// 	{
		// 		float Sum = 0;
		// 		for (int32 i = -HalfGaussianBlurKernelNum; i <= HalfGaussianBlurKernelNum; ++i)
		// 		{
		// 			Sum += UpscaleTextureHorizontalBlurData[((Y + i) * TextureResolution.X * 4 + X) * 4 + 1] * GaussianBlurKernel[i + HalfGaussianBlurKernelNum];
		// 		}
		// 		UpscaleTextureBuffer[(Y * TextureResolution.X * 4 + X) * 4] = Sum;
		// 	}
		// 	for (int32 Y = TextureResolution.Y * 4  - HalfGaussianBlurKernelNum; Y < TextureResolution.X * 4; ++Y)
		// 	{
		// 		float Sum = 0;
		// 		for (int32 i = -HalfGaussianBlurKernelNum; i <= HalfGaussianBlurKernelNum; ++i)
		// 		{
		// 			if(Y + i < TextureResolution.Y * 4)
		// 			{
		// 				Sum += UpscaleTextureHorizontalBlurData[((Y + i) * TextureResolution.X * 4 + X) * 4 + 1] * GaussianBlurKernel[i + HalfGaussianBlurKernelNum];
		// 			}
		// 		}
		// 		UpscaleTextureBuffer[(Y * TextureResolution.X * 4 + X) * 4] = Sum;
		// 	}
		// }
		UpscaleTexture->UpdateTextureRegions(0, 1, &UpscaleTextureUpdateRegion, UpscaleTextureResolution.X * 4, 4, UpscaleTextureBuffer.GetData());
	}else{
		Texture->UpdateTextureRegions(0, 1, &TextureUpdateRegion, TextureResolution.X * 4, 4, TextureBuffer.GetData());
	}
}

void AFogOfWar::Initialize()
{
	Cleanup();
	if(RTSWorldVolume == nullptr)
	{
		return;
	}
	UBrushComponent* RTSWorldBrushComponent = RTSWorldVolume->GetBrushComponent();
	const FBoxSphereBounds RTSWorldBounds = RTSWorldBrushComponent->CalcBounds(RTSWorldBrushComponent->GetComponentTransform());
	// calculate tile resolution
	TileResolution = FIntVector(
		FMath::RoundUpToPowerOfTwo(FMath::CeilToInt(RTSWorldBounds.BoxExtent.X / TileSize) * 2),
		FMath::RoundUpToPowerOfTwo(FMath::CeilToInt(RTSWorldBounds.BoxExtent.Y / TileSize) * 2),
		0.f);
	OriginCoordinate = RTSWorldBounds.Origin - FVector(TileSize * TileResolution.X / 2, TileSize * TileResolution.Y / 2, 0.f);

	// generate tile info
	TileInfos.Empty();
	TileInfos.SetNumUninitialized(TileResolution.X * TileResolution.Y);
	for (int32 TileY = 0; TileY < TileResolution.Y; ++TileY)
	{
		for (int32 TileX = 0; TileX < TileResolution.X; ++TileX)
		{
			FVector2D WorldPosition =
				FVector2D(OriginCoordinate.X,OriginCoordinate.Y) +
				FVector2D(TileSize * (TileX + 0.5f), TileSize * (TileY + 0.5f));
			int16 HeightLevel = CalculateWorldHeightLevelAtLocation(WorldPosition);
			TileInfos[TileY * TileResolution.X + TileX] = uint16(HeightLevel);
		}
	}
	// generate tile agent cache
	TileAgentCache.Empty();
	TileAgentCache.SetNum(TileResolution.X * TileResolution.Y);
}

void AFogOfWar::Cleanup()
{

}

int16 AFogOfWar::CalculateWorldHeightLevelAtLocation(const FVector2D WorldLocation)
{
	// Cast ray to hit world.
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(
		HitResult,
		FVector(WorldLocation.X, WorldLocation.Y, 128 * 256 * 100.0f),
		FVector(WorldLocation.X, WorldLocation.Y, -128 * 256 * 100.0f),
		ECC_RTSMovementTraceChannel))
	{
		return HitResult.Location.Z / 100.f;
	}
	return INT16_MIN;
}

void AFogOfWar::CreateTexture()
{
	TextureResolution.X = TileResolution.X;
	TextureResolution.Y = TileResolution.Y;

	// new and init texture buffer
	TextureBuffer.SetNumZeroed(TextureResolution.X * TextureResolution.Y * 4);
	for (int32 Y = 0; Y < TextureResolution.Y; ++Y)
	{
		for (int32 X = 0; X < TextureResolution.X; ++X)
		{
			const int i = Y * TextureResolution.Y + X;

			const int iBlue = i * 4 + 0;
			const int iGreen = i * 4 + 1;
			const int iRed = i * 4 + 2;
			const int iAlpha = i * 4 + 3;
			TextureBuffer[iBlue] = 0;
			TextureBuffer[iGreen] = 0;
			TextureBuffer[iRed] = 0;
			TextureBuffer[iAlpha] = 0;
		}
	}
	
	// create texture obj
	Texture = UTexture2D::CreateTransient(TextureResolution.X, TextureResolution.Y);
	Texture->Filter = TextureFilter::TF_Trilinear;
	Texture->UpdateResource();
	// create update texture region
	TextureUpdateRegion = FUpdateTextureRegion2D(0, 0, 0, 0, TextureResolution.X, TextureResolution.Y);
	if(bUseUpscaleTexture)
	{
		// new and init texture buffer
		UpscaleTextureResolution = TextureResolution * 4;
		UpscaleTextureBuffer.SetNumZeroed(UpscaleTextureResolution.X * UpscaleTextureResolution.Y * 4);
		UpscaleTextureHorizontalBlurData.SetNumZeroed(UpscaleTextureResolution.X * UpscaleTextureResolution.Y * 4);
		// for (int32 Y = 0; Y < UpscaleTextureResolution.Y; ++Y)
  //       {
  //       	for (int32 X = 0; X < UpscaleTextureResolution.X; ++X)
  //       	{
  //       		const int i = Y * UpscaleTextureResolution.X  + X;
  //       		const int iBlue = i * 4 + 0;
  //       		const int iGreen = i * 4 + 1;
  //       		const int iRed = i * 4 + 2;
  //       		const int iAlpha = i * 4 + 3;
  //       		UpscaleTextureBuffer[iBlue] = 0;
  //       		UpscaleTextureBuffer[iGreen] = 0;
  //       		UpscaleTextureBuffer[iRed] = 0;
  //       		UpscaleTextureBuffer[iAlpha] = 0;
  //       	}
  //       }
		UpscaleTexture = UTexture2D::CreateTransient(UpscaleTextureResolution.X, UpscaleTextureResolution.Y);
		UpscaleTexture->Filter = TextureFilter::TF_Trilinear;
		UpscaleTexture->UpdateResource();
		// create update texture region
		UpscaleTextureUpdateRegion = FUpdateTextureRegion2D(0, 0, 0, 0, UpscaleTextureResolution.X, UpscaleTextureResolution.Y);
	}
	if(PostProcessVolume && PostProcessMaterialInstance)
	{
		PostProcessMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(PostProcessMaterialInstance, nullptr);
		PostProcessMaterialInstanceDynamic->SetTextureParameterValue(FName("FogOfWarRenderTarget"), bUseUpscaleTexture? UpscaleTexture : Texture);
		PostProcessMaterialInstanceDynamic->SetVectorParameterValue(FName("OriginCoordinate"), OriginCoordinate);
		PostProcessMaterialInstanceDynamic->SetScalarParameterValue(FName("TileSize"), TileSize);
		PostProcessMaterialInstanceDynamic->SetVectorParameterValue(FName("MappingWorldSize"), FVector(TileSize * TileResolution.X, TileSize * TileResolution.Y, 0.f));
		
		PostProcessVolume->AddOrUpdateBlendable(PostProcessMaterialInstanceDynamic);
	}
}

void AFogOfWar::DestroyTexture()
{

}

#define Floor FloorToInt
void AFogOfWar::WorldLocationToTileXY(FVector InWorldLocation, int32& TileX, int32& TileY)
{
	const FVector WorldLocation(InWorldLocation.X,InWorldLocation.Y,0.f);
	const FVector TileCoordinateSystemLocation = WorldLocation - OriginCoordinate;
	TileX = FMath::Floor(TileCoordinateSystemLocation.X / TileSize);
	TileY = FMath::Floor(TileCoordinateSystemLocation.Y / TileSize);
}
#undef Floor

// IterateVision BEGIN

//   IterateVision Macro BEGIN
#define USE_ITERATE_VISION_MACRO_VERSION
// https://www.albertford.com/shadowcasting/#Quadrant
#define ITERATE_VISION_FUNCTION(SimulationTileXCalculator, SimulationTileYCalculator, IterateVisionFunction) \
void AFogOfWar::IterateVisionFunction(FRecursiveVisionContext& Context, int32 Depth, float StartSlope, float EndSlope)\
{\
	int32 TileY, TileX;\
	SimulationTileXCalculator;\
	int32 StartTileX = StartSlope * Depth + FMath::Sign(StartSlope) * 0.5;\
	int32 EndTileX = EndSlope * Depth + FMath::Sign(EndSlope) * 0.5;\
	int32 TileXMaxSquare = Context.MaxDepthSquare - Depth * Depth;\
	if (StartTileX * StartTileX > TileXMaxSquare) {\
		if (StartTileX > 0)\
		{\
			return;\
		}\
		\
		StartTileX++;\
		while (StartTileX * StartTileX > TileXMaxSquare)\
		{\
			StartTileX++;\
		}\
		StartSlope = (float(StartTileX) - 0.5f) / float(Depth);\
	}\
	if (EndTileX * EndTileX > TileXMaxSquare) {\
		if (EndTileX < 0)\
		{\
			return;\
		}\
		EndTileX--;\
		while (EndTileX * EndTileX > TileXMaxSquare)\
		{\
			EndTileX--;\
		}\
		EndSlope = (float(EndTileX) + 0.5f) / float(Depth);\
	}\
	if(EndTileX < StartTileX){\
		return;\
	}\
	for (int32 i = StartTileX; i <= EndTileX; i++)\
	{\
		SimulationTileYCalculator;\
		if (!HasVision(Context.OriginX, Context.OriginY, TileX, TileY))\
		{\
			IterateVisionFunction(Context, Depth + 1, StartSlope, (float(i) - 0.5f) / float(Depth));\
			StartSlope = (float(i) + 0.5f) / float(Depth);\
		}else{\
			MarkVision(TileX, TileY);\
		}\
	}\
	int32 NextDepth = Depth + 1;\
	if (NextDepth <= Context.MaxDepth)\
	{\
		IterateVisionFunction(Context, NextDepth, StartSlope, EndSlope);\
	}\
}\
//   IterateVision Macro END

namespace IterateVisionDirection
{
	enum Type
	{
		Bottom = 0,
        Top = 1,
        Right = 2,
        Left = 3,
		
    };
}

void AFogOfWar::IterateVisionBase(FRecursiveVisionContext& Context, int32 Depth, float StartSlope /*= -1.f*/, float EndSlope /*= 1.f*/, int32 Direction)
{
	// https://www.albertford.com/shadowcasting/#Quadrant
	int32 TileY, TileX;
	switch (Direction)
	{
	case IterateVisionDirection::Bottom:
		TileY = Context.OriginY + Depth;
		break;
	case IterateVisionDirection::Top:
		TileY = Context.OriginY - Depth;
		break;
	case IterateVisionDirection::Right:
		TileX = Context.OriginX + Depth;
		break;
	case IterateVisionDirection::Left:
		TileX = Context.OriginX - Depth;
		break;
	}
	
	int32 StartTileX = StartSlope * Depth + FMath::Sign(StartSlope) * 0.5;
	int32 EndTileX = EndSlope * Depth + FMath::Sign(EndSlope) * 0.5;
	int32 TileXMaxSquare = Context.MaxDepthSquare - Depth * Depth;
	// int32 StartTileXSquare = StartTileX * StartTileX;
	// int32 EndTileXSquare = EndTileX * EndTileX;
	// just check in start and end , rad check
	if (StartTileX * StartTileX > TileXMaxSquare) {
		if (StartTileX > 0)
		{
			return;
		}

		StartTileX++;
		while (StartTileX * StartTileX > TileXMaxSquare)
		{
			StartTileX++;
		}
		StartSlope = (float(StartTileX) - 0.5f) / float(Depth);
	}
	if (EndTileX * EndTileX > TileXMaxSquare) {
		if (EndTileX < 0)
		{
			return;
		}
		EndTileX--;
		while (EndTileX * EndTileX > TileXMaxSquare)
		{
			EndTileX--;
		}
		EndSlope = (float(EndTileX) + 0.5f) / float(Depth);
	}
	if(EndTileX < StartTileX){
		return;
	}
	for (int32 i = StartTileX; i <= EndTileX; i++)
	{
		switch (Direction)
		{
		case IterateVisionDirection::Bottom:
			TileX = Context.OriginX + i;
			break;
		case IterateVisionDirection::Top:
			TileX = Context.OriginX - i;
			break;
		case IterateVisionDirection::Right:
			TileY = Context.OriginY + i;
			break;
		case IterateVisionDirection::Left:
			TileY = Context.OriginY - i;
			break;
		}
		if (!HasVision(Context.OriginX, Context.OriginY, TileX, TileY))
		{
			IterateVisionBase(Context, Depth + 1, StartSlope, (float(i) - 0.5f) / float(Depth), Direction);
			StartSlope = (float(i) + 0.5f) / float(Depth);
		}else{
			MarkVision(TileX, TileY);
		}
	}
	int32 NextDepth = Depth + 1;
	if (NextDepth <= Context.MaxDepth)
	{
		IterateVisionBase(Context, NextDepth, StartSlope, EndSlope, Direction);
	}
}
#ifdef USE_ITERATE_VISION_MACRO_VERSION
ITERATE_VISION_FUNCTION((TileY = Context.OriginY + Depth), (TileX = Context.OriginX + i), IterateVisionBottom)
#else
void AFogOfWar::IterateVisionBottom(FRecursiveVisionContext& Context, int32 Depth, float StartSlope /*= -1.f*/, float EndSlope /*= 1.f*/)
{
	IterateVisionBase(Context, Depth, StartSlope, EndSlope, IterateVisionDirection::Bottom);
}
#endif

#ifdef USE_ITERATE_VISION_MACRO_VERSION
ITERATE_VISION_FUNCTION((TileY = Context.OriginY - Depth), (TileX = Context.OriginX - i), IterateVisionTop)
#else
void AFogOfWar::IterateVisionTop(FRecursiveVisionContext& Context, int32 Depth, float StartSlope /*= -1.f*/, float EndSlope /*= 1.f*/)
{
	IterateVisionBase(Context, Depth, StartSlope, EndSlope, IterateVisionDirection::Top);
}
#endif

#ifdef USE_ITERATE_VISION_MACRO_VERSION
ITERATE_VISION_FUNCTION((TileX = Context.OriginX + Depth), (TileY = Context.OriginY + i), IterateVisionRight)
#else
void AFogOfWar::IterateVisionRight(FRecursiveVisionContext& Context, int32 Depth, float StartSlope /*= -1.f*/, float EndSlope /*= 1.f*/)
{
	IterateVisionBase(Context, Depth, StartSlope, EndSlope, IterateVisionDirection::Right);
}

#endif

#ifdef USE_ITERATE_VISION_MACRO_VERSION
ITERATE_VISION_FUNCTION((TileX = Context.OriginX - Depth), (TileY = Context.OriginY - i), IterateVisionLeft)
#else
void AFogOfWar::IterateVisionLeft(FRecursiveVisionContext& Context, int32 Depth, float StartSlope /*= -1.f*/, float EndSlope /*= 1.f*/)
{
	IterateVisionBase(Context, Depth, StartSlope, EndSlope, IterateVisionDirection::Left);
}
#endif
// IterateVision END