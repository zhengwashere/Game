// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BlackVeil.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogBlackVeil);

IMPLEMENT_PRIMARY_GAME_MODULE(FBlackVeilModule, BlackVeil, "BlackVeil");

void FBlackVeilModule::StartupModule()
{
	UE_LOG(LogBlackVeil, Log, TEXT("Black Veil module started."));
}

void FBlackVeilModule::ShutdownModule()
{
	UE_LOG(LogBlackVeil, Log, TEXT("Black Veil module shut down."));
}
