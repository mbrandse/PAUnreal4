// Fill out your copyright notice in the Description page of Project Settings.

#include "PA_BTS_HumanPathLocation.h"
#include "PiratesAdventure.h"
#include "PA_AI_ControllerHumanBase.h"
#include "PA_AI_CharacterHumanBase.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"

UPA_BTS_HumanPathLocation::UPA_BTS_HumanPathLocation()
{
	bCreateNodeInstance = true;

	//for now only accept vectors as viable input
	TargetLocation.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UPA_BTS_HumanPathLocation, TargetLocation));
}

void UPA_BTS_HumanPathLocation::SetOwner(AActor* ActorOwner)
{
	Super::SetOwner(ActorOwner);

	AIController = Cast<APA_AI_ControllerHumanBase>(ActorOwner);
	AIPawn = Cast<APA_AI_CharacterHumanBase>(AIController->GetCharacter());
}

void UPA_BTS_HumanPathLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//if (AIController)
	if(AIPawn && AIPawn->GetController())
	{
		//const UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
		UBlackboardComponent* BlackboardComponent = AIPawn->GetController()->FindComponentByClass<UBlackboardComponent>();

		//if (TargetLocation.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		//{
			const FVector TargetLoc = BlackboardComponent->GetValue<UBlackboardKeyType_Vector>(TargetLocation.GetSelectedKeyID());

			UE_LOG(AnyLog, Log, TEXT("TESTING TICKNODE ON CUSTOM SERVICE WITH TARGET LOC %s"), *(TargetLoc.ToString()));
		//}
	}
}


