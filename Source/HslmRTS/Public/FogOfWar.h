﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FogOfWar.generated.h"

class UFogOfWarSubsystem;
class ARTSWorldVolume;
class APostProcessVolume;

struct FAgentCache
{
	FAgentCache() : VisibleCounter(0){ }
	void AddAgent(){ VisibleCounter++; }
	void RemoveAgent() { VisibleCounter--; }
	bool IsVisible() const { return VisibleCounter > 0; }
	int32 VisibleCounter;
};

struct FRecursiveVisionContext
{
	UFogOfWarSubsystem* FogOfWarSubsystem;
	int32 OriginX;
	int32 OriginY;
	float MaxDepth;
	float MaxDepthSquare;
};

UCLASS()
class HSLMRTS_API AFogOfWar : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FogOfWar", meta=(AllowPrivateAccess = "true"))
	UDecalComponent* GridDecal;

	// Sets default values for this actor's properties
	AFogOfWar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdateFogOfWar();
	
	UFUNCTION(BlueprintCallable, CallInEditor)
	void Initialize();
	void Cleanup();
	int16 CalculateWorldHeightLevelAtLocation(const FVector2D WorldLocation);

	void CreateTexture();
	void DestroyTexture();

	// FogOfWar Texture BEGIN
	// the texture
	UPROPERTY()
	UTexture2D* Texture;
	TArray<uint8> TextureBuffer;
	uint32* TextureBufferSize;
	FUpdateTextureRegion2D TextureUpdateRegion;
	FIntVector TextureResolution;
	
	UPROPERTY()
	UTexture2D* UpscaleTexture;
	TArray<uint8> UpscaleTextureBuffer;
	uint32* UpscaleTextureBufferSize;
	FUpdateTextureRegion2D UpscaleTextureUpdateRegion;
	FIntVector UpscaleTextureResolution;
	
	TArray<uint8> UpscaleTextureHorizontalBlurData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FogOfWar")
	bool bUseUpscaleTexture = false;
	
	// FogOfWar resolution

	// FogOfWar Texture END



	//Tile info BEGIN

	void WorldLocationToTileXY(FVector InWorldLocation, int32& TileX, int32& TileY);

	int32 GetWorldTileIndex(int32 X, int32 Y) const { return X + Y * GetTileResolutionX();}
	int16 GetWorldHeightLevel(int32 X, int32 Y){ return TileInfos[GetWorldTileIndex(X,Y)] & 0xFFFF;}
	bool IsBlock(int32& X, int32& Y){ return (TileInfos[GetWorldTileIndex(X, Y)] & (1 << 31)) != 0; }
	bool HasVision(int32& OriginX, int32& OriginY, int32& TargetX, int32& TargetY)
	{
		if (TileInfos.IsValidIndex(GetWorldTileIndex(TargetX,TargetY)))
		{
			return !IsBlock(TargetX,TargetY) && GetWorldHeightLevel(OriginX,OriginY) >= GetWorldHeightLevel(TargetX,TargetY);
		}
		return false;
	}
	void MarkVision(int32& TileX,int32& TileY)
	{
		int32 Index = GetWorldTileIndex(TileX, TileY);
		const int32 iBlue = Index * 4 + 0;
		const int32 iGreen = Index * 4 + 1;
		const int32 iRed = Index * 4 + 2;
		const int32 iAlpha = Index * 4 + 3;
// 		TextureBuffer[iBlue] = 0;
// 		TextureBuffer[iGreen] = 0;
		TextureBuffer[iRed] = 255;
		//TextureBuffer[iAlpha] = 0;
	}


	int32 GetTileResolutionX() const { return TileResolution.X; }

	// The size of tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FogOfWar")
	float TileSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FogOfWar")
	ARTSWorldVolume* RTSWorldVolume;

	FIntVector TileResolution;

	FVector OriginCoordinate;

	// Bit32 is Block flag, Low16Bit Is HeightLevel
	TArray<uint32> TileInfos;
	// Cache for tiles current has agents;
	TArray<FAgentCache> TileAgentCache;

	UPROPERTY(EditInstanceOnly, Category = "FogOfWar")
	APostProcessVolume* PostProcessVolume;
	UPROPERTY(EditInstanceOnly, Category="FogOfWar")
	UMaterialInterface* PostProcessMaterialInstance;
	UPROPERTY()
	UMaterialInstanceDynamic* PostProcessMaterialInstanceDynamic;

private:
	// for debug, macro version is fast
	void IterateVisionBase(FRecursiveVisionContext& Context, int32 Depth, float StartSlope = -1.f, float EndSlope = 1.f, int32 Direction = 0);
	
	void IterateVisionBottom(FRecursiveVisionContext& Context, int32 Depth, float StartSlope = -1.f, float EndSlope = 1.f);
	void IterateVisionTop(FRecursiveVisionContext& Context, int32 Depth, float StartSlope = -1.f, float EndSlope = 1.f);
	void IterateVisionRight(FRecursiveVisionContext& Context, int32 Depth, float StartSlope = -1.f, float EndSlope = 1.f);
	void IterateVisionLeft(FRecursiveVisionContext& Context, int32 Depth, float StartSlope = -1.f, float EndSlope = 1.f);

	
};
