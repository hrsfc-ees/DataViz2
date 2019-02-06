// Fill out your copyright notice in the Description page of Project Settings.
#include "HandPawn.h"
#include "DataVisualisation.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"



// Sets default values
AHandPawn::AHandPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    //Create our components
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-60.0f, 0.0f, 0.0f));
    SpringArm->TargetArmLength = 400.f;
    SpringArm->bEnableCameraLag = false;
    SpringArm->CameraLagSpeed = 3.0f;
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hand Mesh"));
    MeshComponent->SetupAttachment(RootComponent);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> HandMesh(TEXT("StaticMesh'/Game/ThirdPersonCPP/Blueprints/EESHands.EESHands'"));
    if (HandMesh.Succeeded())
    {
        MeshComponent->SetStaticMesh(HandMesh.Object);
//        MeshComponent->SetRelativeLocation(FVector(200.0f, 0.0f, 0.0f));
        MeshComponent->SetWorldScale3D(FVector(10.0f));
    }
    //Set up pointer to Dynamic Material Instance
     static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Materials/MForceToColour.MForceToColour'"));
    if (Material.Succeeded()) 
    {
        FTC_M = Material.Object;
    }
//    Take control of the default Player
    AutoPossessPlayer = EAutoReceiveInput::Player0;

}

// Called when the game starts or when spawned
void AHandPawn::BeginPlay()
{
	Super::BeginPlay();
    PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    PC->ClientMessage(TEXT("Starting Thread?"));
    ReaderInst = PortReader::EasyInit(VoltageQueue, PC);
    
	
}

// Called every frame
void AHandPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

{
    FRotator NewRotation = SpringArm->GetComponentRotation();
    NewRotation.Yaw += CameraInput.X;
    SpringArm->SetWorldRotation(NewRotation);
}
{
    FRotator NewRotation = SpringArm->GetComponentRotation();
    NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + CameraInput.Y, -85.0f, 85.0f);
    SpringArm->SetWorldRotation(NewRotation);
}

}

// Called to bind functionality to input
void AHandPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis("LookVertical", this, &AHandPawn::PitchCamera);
    PlayerInputComponent->BindAxis("LookHorizontal", this, &AHandPawn::YawCamera);
    PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &AHandPawn::ZoomIn);
    PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &AHandPawn::ZoomOut);

}

void AHandPawn::PitchCamera(float AxisValue)
{
    CameraInput.Y = AxisValue;
}
void AHandPawn::YawCamera(float AxisValue)
{
    CameraInput.X = AxisValue;
}

void AHandPawn::ZoomOut()
{
        SpringArm->TargetArmLength += 10.0f;
}
void AHandPawn::ZoomIn()
{
        SpringArm->TargetArmLength += -10.0f;
}

void AHandPawn::OnConstruction(const FTransform& Transform)
{
    if (!FTCInstance_M){
        UE_LOG(LogTemp, Warning, TEXT("Material Instance To Be Created"))
        FTCInstance_M = UMaterialInstanceDynamic::Create(FTC_M, FTC_M);
        MeshComponent->SetMaterial(0, FTCInstance_M);
    }

}

void AHandPawn::EndPlay(const EEndPlayReason::Type Reason)
{
    ReaderInst->Shutdown();
    UE_LOG(LogTemp, Warning, TEXT("End Play Called"))
    Super::EndPlay(Reason);
}

bool AHandPawn::GetVoltage(TArray<float>& VoltageArray)
{
    if(!VoltageQueue.IsEmpty()){
        VoltageArray.Empty();
        VoltageQueue.Dequeue(VoltageArray);
        VoltageQueue.Empty();
        UE_LOG(LogTemp, Warning, TEXT("Size of array is: %i"), VoltageArray.Num())
        if(VoltageArray.Num() > 1){
//            UE_LOG(LogTemp, Warning, TEXT("Everything A-OK"))
            return true;
        } else {
//            UE_LOG(LogTemp, Warning, TEXT("Array not 2 numbers long :("))
            return false;
        }
    } else {
//        UE_LOG(LogTemp, Warning, TEXT("No Array Returned"))
        return false;
    }
}
void AHandPawn::ClearQueue()
{
    VoltageQueue.Empty();
}
