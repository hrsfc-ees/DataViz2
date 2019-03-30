// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Networking.h"
#include "UnrealString.h"
#include <string>
#include <istream>
#include <deque>


/**
 * 
 */
class PortReader : public FRunnable
{
    
    // static reference to this instance of the class   
    static PortReader* Runnable;
    
    //    Thread that the process is running on
    FRunnableThread* Thread;
    
    //    Data queue pointer to write to
    TQueue<TArray<float>>* Queue;
    
    /** The PC */
	APlayerController* ThePC;
	
	/** Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;
    
    // String stream for data parsing
//    std::stringstream SStream;
//    TCircularBuffer<uint8> Buf;
//    std::deque <uint8> Dataqueue;
//    
public:
	PortReader(TQueue<TArray<float> >& TheQueue, APlayerController* InPC);
	~PortReader();
    
    
    FSocket* ListenerSocket;
public:

    TArray<float> GetLine();
    FString StringFromBinaryArray(TArray<uint8> BinaryArray);
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	// End FRunnable interface
//	TArray<FString> ParsePort();
	/** Makes sure this thread has stopped properly */
	void EnsureCompletion();
    TArray<float> lastdata;
	
	
	
	//~~~ Starting and Stopping Thread ~~~
	
	
	
	/* 
		Start the thread and the worker from static (easy access)! 
        This code ensures only 1 instance will run at a time
		This function returns a handle to the newly started instance.
	*/
	static PortReader* EasyInit(TQueue<TArray<float>>& TheQueue, APlayerController* InPC);

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	static void Shutdown();
};
