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
    Thread = FRunnableThread::Create(this, TEXT("PortReader"), 0, TPri_BelowNormal);
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
    
    SStream.rdbuf()->pubsetbuf(0,0);
    
    
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
    uint8 data[1024];
    int32 read;
    
    if(ListenerSocket->HasPendingData(dataSize)){
        ListenerSocket->Recv(data, 1024, read, ESocketReceiveFlags::None);
        SStream.write((char*)data, read);
    }
    
    
    
    std::string s;
    std::getline(SStream, s);
    FString out(s.c_str());
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