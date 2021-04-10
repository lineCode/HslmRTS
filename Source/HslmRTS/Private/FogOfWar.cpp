﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "FogOfWar.h"

#include "HslmRTS.h"
#include "FogOfWarSubsystem.h"
#include "RTSAgentComponent.h"
#include "RTSWorldVolume.h"
#include "Components/BrushComponent.h"
#include "Components/DecalComponent.h"

// Sets default values
AFogOfWar::AFogOfWar()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GridDecal =CreateDefaultSubobject<UDecalComponent>(TEXT("GridDecal"));
	RootComponent = GridDecal;
	GridDecal->bIsEditorOnly = true;
	GridDecal->SetRelativeRotation(FRotator(0.f,90.f, 0.f));
	
	TextureBuffer = nullptr;
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
}

void AFogOfWar::UpdateFogOfWar()
{
	// https://www.albertford.com/shadowcasting/#Quadrant
	auto FogOfWarSubsystem = GetWorld()->GetSubsystem<UFogOfWarSubsystem>();
	for (auto Agent : FogOfWarSubsystem->GetRTSAgents())
	{
		FRecursiveVisionContext Context;
		Context.MaxDepth = Agent->VisionRadius;
		// Context.OriginX = ;
		// Context.OriginY = ;
		RecursiveVision(Context,1, -1, 1);
	}
}

/*
oooo@oooo->(x)
ooo**oooo
oo****ooo
o******oo
********o
I
v(y)
*/

void AFogOfWar::RecursiveVision(FRecursiveVisionContext& Context, int32 Depth, int32 Start, int32 End)
{
	int32 y = Context.OriginY + Depth;
	for (int32 i = Start; i <= End; i++)
	{
		int32 x = Context.OriginX + i;
		if (!HasVision(Context.OriginX, Context.OriginY, x, y))
		{
			int32 NewStart = ((((Depth + 1) * Start) * 2 ) + Depth) / (Depth * 2);
			int32 NewEnd = ((((Depth + 1) * i) * 2 ) + Depth) / (Depth * 2);
			RecursiveVision(Context, Depth + 1, NewStart, NewEnd);
			Start = i;
		}
	}
	int32 NextDepth = Depth + 1;
	if (NextDepth < Context.MaxDepth)
	{
		int32 NewStart = ((((Depth + 1) * Start) * 2 ) + Depth) / (Depth * 2);
		int32 NewEnd = ((((Depth + 1) * End) * 2 ) + Depth) / (Depth * 2);
		RecursiveVision(Context, NextDepth, NewStart, NewEnd); 
	}
}

void AFogOfWar::Initialize()
{
	Cleanup();
	UBrushComponent* RTSWorldBrushComponent = RTSWorldVolume->GetBrushComponent();
	const FBoxSphereBounds RTSWorldBounds = RTSWorldBrushComponent->CalcBounds(RTSWorldBrushComponent->GetComponentTransform());
	// calculate tile resolution
	TileResolution = FIntVector(
		FMath::RoundUpToPowerOfTwo(FMath::CeilToInt(RTSWorldBounds.BoxExtent.X / TileSize) * 2),
		FMath::RoundUpToPowerOfTwo(FMath::CeilToInt(RTSWorldBounds.BoxExtent.Y / TileSize) * 2),
		0.f);
	OriginCoordinate = RTSWorldBounds.Origin - FVector(TileSize * TileResolution.X, TileSize * TileResolution.Y, 0.f);
	
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
	TextureBuffer = new uint8[TextureResolution.X * TextureResolution.Y * 4];
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
	Texture->AddToRoot();
	Texture->UpdateResource();
	// create update texture region
	TextureUpdateRegion = FUpdateTextureRegion2D(0, 0, 0, 0, TextureResolution.X, TextureResolution.Y);
	
}

void AFogOfWar::DestroyTexture()
{
	if(Texture)
	{
		Texture->RemoveFromRoot();
	}
	if(TextureBuffer)
	{
		delete[] TextureBuffer;
		TextureBuffer= nullptr;
	}
}

#define Floor FloorToInt
void AFogOfWar::WorldLocationToTileXY(FVector InWorldLocation, int32& TileX, int32& TileY)
{
	const FVector WorldLocation(InWorldLocation.X,InWorldLocation.Y,0.f);
	const FVector TileCoordinateSystemLocation = WorldLocation - OriginCoordinate;
	TileX = FMath::Floor(WorldLocation.X - OriginCoordinate.X);
	TileY = FMath::Floor(WorldLocation.Y - OriginCoordinate.Y);
}
#undef Floor
