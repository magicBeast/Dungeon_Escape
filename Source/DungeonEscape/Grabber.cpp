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
	if (PhysicsHandle)
	{
		// Physics handle is found
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No PhysicsHandle attached to %s useless bitch!"), *GetOwner()->GetName());
	}
}

void UGrabber::SetupInputComponent()
{
	/// Look for attached input component (this component only appears at run time)
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Input Component Found"));
		/// Bind the input axis 
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
		// if the physics handle is attached
		// move the object that we're holding
	/// Get player view point this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation, 
		OUT PlayerViewPointRotation
	);
	
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

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
	FHitResult Hit;
	bool GotHit = GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	/// See what we hit
	if (GotHit) 
	{
		AActor* ActorHit = Hit.GetActor();
		UE_LOG(LogTemp, Warning, TEXT("%s was hit - ouch!"), *(ActorHit->GetName()));
	}
	return Hit;
}

void UGrabber::Grab()
{
		UE_LOG(LogTemp, Warning, TEXT("S_P_U_R_T!!!!"));

		/// Line trace and see if we reach any actors with physics body collision channel set
		auto HitResult = GetFirstPhysicsBodyInReach();
		auto ComponentToGrab = HitResult.GetComponent();
		auto ActorHit = HitResult.GetActor();
		/// If we hit something then attach a physics handle
		if (ActorHit)
		{
			///  attach physics handle
				PhysicsHandle->GrabComponent(
				ComponentToGrab,
				NAME_None,
				ComponentToGrab->GetOwner()->GetActorLocation(),
				true // allow rotation
			);
		}
}
  
void UGrabber::Release()
{
		UE_LOG(LogTemp, Warning, TEXT("U_N_S_P_U_R_T!!!!"));
		//  release physics handle
		PhysicsHandle->ReleaseComponent();
}
// Called every frame
void UGrabber::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	/// Get player view point this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	// if the physics handle is attached
	if (PhysicsHandle->GrabbedComponent) {

		PhysicsHandle->SetTargetLocation(LineTraceEnd);

	}

}
 