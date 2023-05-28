// Copyright belongs to Michael Brandse and Digital Hollywood unless otherwise noted.

#include "PA_PatrolPoint.h"
//#include "PA_AI_CharacterBase.h"


// Sets default values
APA_PatrolPoint::APA_PatrolPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APA_PatrolPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APA_PatrolPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//void APA_PatrolPoint::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
//{
//	Super::PostEditChangeProperty(PropertyChangedEvent);
//
//#if WITH_EDITOR
//
//	for (TObjectIterator<APA_AI_CharacterBase> Itr; Itr; ++Itr)
//	{
//		APA_AI_CharacterBase *Character = *Itr;
//		if (Character)
//		{
//			int32 Index = INDEX_NONE;
//			Index = Character->PatrolPoints.Find(this);
//			if (Index != INDEX_NONE)
//			{
//				Character->SetupDebugPatrolMeshes();
//			}
//		}
//	}
//
//#endif //WITH_EDITOR
//}