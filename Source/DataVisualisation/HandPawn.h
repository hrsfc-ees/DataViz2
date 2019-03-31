// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <iostream>
#include <fstream>
#include <string>
#include "PortReader.h"
#include "Kismet/GameplayStatics.h"
#include "HandPawn.generated.h"

UCLASS()
class DATAVISUALISATION_API AHandPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHandPawn();
    //Input variables
    float ArmLength;
    FVector2D CameraInput;
    //Material Reference to use
    UMaterial* FTC_M;
    //Material Instance Dynamic that is being used
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInstanceDynamic* FTCInstance_M = nullptr;
    // Input functions
    void PitchCamera(float AxisValue);
    void YawCamera(float AxisValue);
    void ZoomIn();
    void ZoomOut();
    
    
    // Begin Play Override
    virtual void BeginPlay() override;
    
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    
    // A queue that is accessesd by USerialReader to enable multithreading
    TQueue<TArray<float>> VoltageQueue;
    
    PortReader* ReaderInst = nullptr;

    APlayerController* PC = nullptr;
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Voltage Array"), Category = "Serial", meta = (Keywords = "voltage port arduino serial start"))
    bool GetVoltage(TArray<float>& VoltageArray);
    
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Clear Queue"), Category = "Serial", meta = (Keywords = "clear arduino queue voltage"))
    void ClearQueue();


protected:
    // Edit Anywhere Spring Arm component
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class USpringArmComponent* SpringArm;
    class UCameraComponent* Camera;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UStaticMeshComponent* MeshComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    // Construction Script Equivalent
    virtual void OnConstruction(const FTransform& Transform) override;

	
	
};
