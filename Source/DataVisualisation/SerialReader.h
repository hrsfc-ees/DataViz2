// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <iostream>
#include <fstream>
#include <string>

/**
 * 
 */

//class EESSERIAL_API USerialReader : public FRunnable
class USerialReader : public FRunnable
{
// static reference to this instance of the class   
    static USerialReader* Runnable;
    std::ifstream* ArduinoInput;
    
//    Thread that the process is running on
    FRunnableThread* Thread;
    
//    Data queue pointer to write to
    TQueue<TArray<float>>* Queue;
    
    /** The PC */
	APlayerController* ThePC;
	
	/** Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;
    

    
public:
    USerialReader(std::ifstream* ArduinoInput, TQueue<TArray<float>>& TheQueue, APlayerController* InPC);
    ~USerialReader();
    
    TArray<float> GetLine();
    
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	// End FRunnable interface
	
	/** Makes sure this thread has stopped properly */
	void EnsureCompletion();
	
	
	
	//~~~ Starting and Stopping Thread ~~~
	
	
	
	/* 
		Start the thread and the worker from static (easy access)! 
        This code ensures only 1 instance will run at a time
		This function returns a handle to the newly started instance.
	*/
	static USerialReader* EasyInit(std::ifstream* ArduinoInput, TQueue<TArray<float>>& TheQueue, APlayerController* InPC);

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	static void Shutdown();

    
	
};
