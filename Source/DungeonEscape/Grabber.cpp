// Copyright subculture software 2016

#include "DungeonEscape.h"
#include "Grabber.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

void UGrabber::FindPhysicsHandleComponent()
{
	/// Look for attached Physics Handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No PhysicsHandle attached to %s useless bitch!"), *GetOwner()->GetName());
	}
}

/// Look for attached input component (this component only appears at run time)
void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) 
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		 UE_LOG(LogTemp, Error, TEXT("No Input Component is attached to %s, useless bitch!"), *GetOwner()->GetName());
	}
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	/// Draw a red trace in the world to visualise 
/*	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation, 
		LineTraceEnd,
		FColor(255, 0, 0),
		false,
		0.f,
		0,
		3.f
	);
*/
	/// Setup query parameters
	FCollisionQueryParams TraceParameters (FName(TEXT("")), false, GetOwner());
	/// Line-trace (AKA Ray-cast) out to player reach distance
	FHitResult HitResult;
	bool GotHit = GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);
	return HitResult;
}

void UGrabber::Grab()
{
		/// Line trace and see if we reach any actors with physics body collision channel set
		auto HitResult = GetFirstPhysicsBodyInReach();
		auto ComponentToGrab = HitResult.GetComponent(); // gets the mesh in our case
		auto ActorHit = HitResult.GetActor();
		/// If we hit something then attach a physics handle
		if (ActorHit)
		{
			if (!PhysicsHandle) { return; }
				///  attach physics handle
				PhysicsHandle->GrabComponent(
				ComponentToGrab,
				NAME_None, // no bones needed
				ComponentToGrab->GetOwner()->GetActorLocation(),
				true // allow rotation
			);
		}
}
  
void UGrabber::Release()
{
		if (!PhysicsHandle) { return; }
		PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (!PhysicsHandle) { return; }
	// if the physics handle is attached
	if (PhysicsHandle->GrabbedComponent) {

		PhysicsHandle->SetTargetLocation(GetReachLineEnd());

	}

}
 
FVector UGrabber::GetReachLineStart()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation;
}

FVector UGrabber::GetReachLineEnd()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}