﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FogOfWar.generated.h"

class UFogOfWarSubsystem;

struct FFogOfWarTile
{
	FFogOfWarTile() : VisibleCounter(0){ }
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
	int32 MaxDepth;
};

UCLASS()
class HSLMRTS_API AFogOfWar : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFogOfWar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	void UpdateFogOfWar();
	void RecursiveVision(FRecursiveVisionContext& Context, int32 Depth, int32 Start, int32 End);


	void WorldLocationToTileXY(FVector InWorldLocation, int32 TileX, int32 TileY);


	
	int32 GetWorldTileIndex(int32 X, int32 Y) const { return X + Y * GetTileNumber();}
	int32 GetWorldHeightLevel(int32 X, int32 Y){ return WorldTileInfos[GetWorldTileIndex(X,Y)] & 0xFFFF;}
	bool IsBlock(int32& X, int32& Y){ return (WorldTileInfos[GetWorldTileIndex(X, Y)] & (1 << 31)) != 0; }
	bool HasVision(int32& OriginX, int32& OriginY, int32& TargetX, int32& TargetY)
	{
		return !IsBlock(TargetX,TargetY) && GetWorldHeightLevel(OriginX,OriginY) >= GetWorldHeightLevel(TargetX,TargetY);
	}
	
	int32 GetTileNumber() const { return TileNumber; }
	
	// 
	int32 TileSize;
	// 
	int32 TileNumber;
	
	// Bit32 is Block flag, Low16Bit Is HeightLevel
	TArray<int32> WorldTileInfos;

	// Cache for tiles current has agents;
	TArray<FFogOfWarTile> FogOfWarTiles;
};
