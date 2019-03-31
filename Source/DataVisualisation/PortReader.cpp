// Fill out your copyright notice in the Description page of Project Settings.


#include "PortReader.h"

//***********************************************************
//Thread Worker Starts as NULL, prior to being instanced
//		This line is essential! Compiler error without it
PortReader* PortReader::Runnable = NULL;
//***********************************************************


PortReader::PortReader(TQueue<TArray<float> >& TheQueue, APlayerController* InPC)
:Queue(&TheQueue), ThePC(InPC), StopTaskCounter(0)
{
    Thread = FRunnableThread::Create(this, TEXT("PortReader"), 0, TPri_Highest);
    lastdata.Init(0,6);
}

PortReader::~PortReader()
{
    if(ListenerSocket){
        ListenerSocket->Close();
        delete ListenerSocket;
    }
    delete Thread;
    Thread = NULL;
}

bool PortReader::Init()
{
    ListenerSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("Port Reader"), false);
    
    
    FIPv4Address ip(127, 0, 0, 1);

    TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    addr->SetIp(ip.Value);
    addr->SetPort(12345);
    
    bool connected = ListenerSocket->Connect(*addr);
    
    Queue->Empty();
    if(ThePC) 
	{
		ThePC->ClientMessage("**********************************");
		ThePC->ClientMessage("Serial Thread Started!");
		ThePC->ClientMessage("**********************************");
        if(connected){
            ThePC->ClientMessage("**********************************");
            ThePC->ClientMessage("Socket Opened!");
            ThePC->ClientMessage("**********************************");
        } else {
            ThePC->ClientMessage("Not connected");
        }
	}
    return true;
}

uint32 PortReader::Run()
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

void PortReader::Stop()
{
    StopTaskCounter.Increment();
}

void PortReader::EnsureCompletion()
{
    Stop();
	Thread->WaitForCompletion();
}

PortReader* PortReader::EasyInit(TQueue<TArray<float> >& TheQueue, APlayerController* InPC)
{
    //Create new instance of thread if it does not exist
	//		and the platform supports multi threading!
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new PortReader(TheQueue,InPC);			
	}
	return Runnable;
}

void PortReader::Shutdown()
{
    if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = NULL;
	}
}


TArray<float> PortReader::GetLine()
{
    
    uint32 dataSize;
    FString out;
    TArray<float> Returner;
    
    //Dynamically resize receive buffer and fill it
    if(ListenerSocket->HasPendingData(dataSize)){
        ThePC->ClientMessage("Socket:");
        ThePC->ClientMessage(FString::FromInt(dataSize));
        TArray<uint8> data;
        data.Init(0,FMath::Min(dataSize, 65507u));
        int32 read;
        ListenerSocket->Recv(data.GetData(), data.Num(), read, ESocketReceiveFlags::None);
        out = StringFromBinaryArray(data);
        //Log data for debugging
        ThePC->ClientMessage(out);
        
        
    TArray<FString> OutArray;
    out.ParseIntoArray(OutArray, TEXT(","), true);
    for (auto& Str : OutArray)
    {   
        //Convert to float
        Returner.Add(FCString::Atof(*Str));
    }
    //Ensure we always have data to send.
    lastdata = Returner;
    return Returner;
    } else {
        ThePC->ClientMessage("Using Default");
        return lastdata;
    }
    
    
}

FString PortReader::StringFromBinaryArray(TArray<uint8> BinaryArray)
{
	BinaryArray.Add(0); 
	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData())));
}