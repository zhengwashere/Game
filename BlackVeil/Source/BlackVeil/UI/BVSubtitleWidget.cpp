// Copyright (c) 2024 Black Veil Studios. All Rights Reserved.
#include "BVSubtitleWidget.h"

void UBVSubtitleWidget::ShowLine(FName SpeakerID, const FText& SpeakerName, const FText& Line, float Duration)
{
	CurrentSpeaker = SpeakerName;
	CurrentLine = Line;
	RemainingTime = Duration > 0.f ? Duration : 4.f;
	bShowing = true;
	OnLineShown(CurrentSpeaker, CurrentLine);
}

void UBVSubtitleWidget::ClearLine()
{
	bShowing = false;
	RemainingTime = 0.f;
	CurrentLine = FText::GetEmpty();
	CurrentSpeaker = FText::GetEmpty();
	OnLineHidden();
}

void UBVSubtitleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bShowing) { return; }

	RemainingTime -= InDeltaTime;
	if (RemainingTime <= 0.f)
	{
		ClearLine();
	}
}
