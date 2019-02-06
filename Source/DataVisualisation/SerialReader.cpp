// Fill out your copyright notice in the Description page of Project Settings.


#include "SerialReader.h"

//***********************************************************
//Thread Worker Starts as NULL, prior to being instanced
//		This line is essential! Compiler error without it
USerialReader* USerialReader::Runnable = NULL;
//***********************************************************





USerialReader::USerialReader(std::ifstream* ArduinoInput, TQueue<TArray<float> >& TheQueue, APlayerController* InPC)
: ArduinoInput(ArduinoInput), Queue(&TheQueue), ThePC(InPC), StopTaskCounter(0)
{
    Thread = FRunnableThread::Create(this, TEXT("SerialReader"), 0, TPri_BelowNormal);
}
USerialReader::~USerialReader(){
    delete Thread;
    Thread = NULL;
}

bool USerialReader::Init()
{
//  Initialise the queue and data
    
    Queue->Empty();
    
    if(ThePC) 
	{
		ThePC->ClientMessage("**********************************");
		ThePC->ClientMessage("Serial Thread Started!");
		ThePC->ClientMessage("**********************************");
	}
    return true;
}

uint32 USerialReader::Run()
{
    //Sleep before starting
    FPlatformProcess::Sleep(0.05);
    
	while (StopTaskCounter.GetValue() == 0)
	{
		
		Queue->Enqueue(GetLine());
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//prevent thread from using too many resources
		FPlatformProcess::Sleep(0.01);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	}
	
	//Run USerialReader::Shutdown() from the timer in Game Thread that is watching to see if process is finished
	
	return 0;
}

TArray<float> USerialReader::GetLine()
{
    std::string s;
    std::getline(*ArduinoInput, s);
    FString out(s.c_str());
//    ThePC->ClientMessage(out);
//    ThePC->ClientMessage("Sent String Your Way :)");
    TArray<FString> OutArray;
    out.ParseIntoArray(OutArray, TEXT(","), true);
    FString ToPrint;
    TArray<float> Returner;
    for (auto& Str : OutArray)
    {
        Returner.Add(FCString::Atof(*Str));
        
    }
    return Returner;
    
    
}

void USerialReader::Stop()
{
    StopTaskCounter.Increment();
}

void USerialReader::EnsureCompletion()
{
    Stop();
	Thread->WaitForCompletion();
}

USerialReader* USerialReader::EasyInit(std::ifstream* ArduinoInput, TQueue<TArray<float> >& TheQueue, APlayerController* InPC)
{
    //Create new instance of thread if it does not exist
	//		and the platform supports multi threading!
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new USerialReader(ArduinoInput,TheQueue,InPC);			
	}
	return Runnable;
}

void USerialReader::Shutdown()
{
    if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = NULL;
	}
}


