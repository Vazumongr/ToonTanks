#include "AWSDynamoDBModule.h"
#include "AWSDynamoDBPrivatePCH.h"
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"


void* FAWSDynamoDBModule::DYNAMODBTPModuleDll = nullptr;


TSet<void*> FAWSDynamoDBModule::ValidDllHandles = TSet<void*>();

void FAWSDynamoDBModule::StartupModule(){
#if PLATFORM_WINDOWS && PLATFORM_64BITS
    //If we are on a windows platform we need to Load the DLL's
    UE_LOG(LogTemp, Display, TEXT("Start Loading DLL's"));
    const FString PluginDir = IPluginManager::Get().FindPlugin("AwsDemo")->GetBaseDir();
    

    
    const FString DYNAMODBTPModuleName = "aws-cpp-sdk-dynamodb";
    FString DYNAMODBTPModuleDllDir = FPaths::Combine(*PluginDir, TEXT("Source"), TEXT("ThirdParty"), TEXT("DYNAMODBTPModule"), TEXT("Win64"));
    const FString DYNAMODBTPModulePath = FPaths::Combine(DYNAMODBTPModuleDllDir, DYNAMODBTPModuleName) + TEXT(".") + FPlatformProcess::GetModuleExtension();
    if (!FAWSDynamoDBModule::LoadDll(DYNAMODBTPModulePath, FAWSDynamoDBModule::DYNAMODBTPModuleDll, DYNAMODBTPModuleName)) {
        FAWSDynamoDBModule::FreeAllDll();
    }
    
#endif
}

void FAWSDynamoDBModule::ShutdownModule(){   
#if PLATFORM_WINDOWS && PLATFORM_64BITS
    FAWSDynamoDBModule::FreeAllDll();
#endif
}

bool FAWSDynamoDBModule::LoadDll(const FString path, void*& dll_ptr, const FString name) {
    //load the passed in dll and if it successeds then add it to the valid set
    UE_LOG(LogTemp, Error, TEXT("Attempting to load DLL %s from %s"), *name, *path);
    dll_ptr = FPlatformProcess::GetDllHandle(*path);

    if (dll_ptr == nullptr) {
        UE_LOG(LogTemp, Error, TEXT("Could not load %s from %s"), *name, *path);
        return false;
    }

    UE_LOG(LogTemp, Display, TEXT("Loaded %s from %s"), *name, *path);
    FAWSDynamoDBModule::ValidDllHandles.Add(dll_ptr);
    return true;
}

void FAWSDynamoDBModule::FreeDll(void*& dll_ptr) {
    //free the dll handle
    if (dll_ptr != nullptr) {
        FPlatformProcess::FreeDllHandle(dll_ptr);
        dll_ptr = nullptr;
    }
}

void FAWSDynamoDBModule::FreeAllDll() {
    //Free all the current valid dll's
    for (auto dll : FAWSDynamoDBModule::ValidDllHandles) {
        FAWSDynamoDBModule::ValidDllHandles.Remove(dll);
        FAWSDynamoDBModule::FreeDll(dll);
    }
}


IMPLEMENT_MODULE(FAWSDynamoDBModule, AWSDynamoDB);