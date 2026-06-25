#include "Game/Manager/ContextManager.h"
#include "Game/ReLog.h"

EContext UContextManager::GetContext() const
{
	return Context;
}

void UContextManager::SetContext(EContext InContext)
{
	if (InContext == Context)
	{
		RE_WARN("Avoid setting to the same Context.");
		return;
	}
	
	// Skip broadcast for SetContext(EContext::None).

	if (InContext == EContext::None)
	{
		RE_LOG
		(
			"Context changed from {0} to {1}.",
			UEnum::GetValueAsString(Context),
			UEnum::GetValueAsString(InContext)
		);
		
		Context = InContext;
		return;
	}

	// RootContexts.

	if (InContext == EContext::Overworld || InContext == EContext::Combat)
	{
		ContextHistory.Empty();
		
		if (InContext != RootContext)
		{
			RE_LOG
			(
				"RootContext changed from {0} to {1}.",
				UEnum::GetValueAsString(RootContext),
				UEnum::GetValueAsString(InContext)
			);
		
			RootContext = InContext;
		}
	}

	// Avoid looping while backing.

	else if (!ContextHistory.IsEmpty() && InContext == ContextHistory.Last())
	{
		ContextHistory.Pop();
	}

	// None and Popup aren't allowed in ContextHistory.
	
	else if (Context != EContext::None && Context != EContext::Popup)
	{
		ContextHistory.Push(Context);
	}
	
	ApplyContext(InContext);
}

void UContextManager::Escape()
{
	switch (Context)
	{
		case EContext::Overworld:
		{
			SetContext(EContext::MainMenu);
			break;
		}
		
		case EContext::Conversation:
		{
			break;
		}
		
		default:
		{
			SetContext(RootContext);
			break;
		}
	}
}

void UContextManager::Back()
{
	if (!ContextHistory.IsEmpty())
	{
		ApplyContext(ContextHistory.Pop());
	}
}

void UContextManager::ToggleInventory()
{
	switch (Context)
	{
		case EContext::Conversation:
		{
			break;
		}
		
		case EContext::InventoryMenu:
		{
			SetContext(RootContext);
			break;
		}
		
		default:
		{
			SetContext(EContext::InventoryMenu);
			break;
		}
	}
}

void UContextManager::ApplyContext(EContext InContext)
{
	RE_LOG
	(
		"Context changed from {0} to {1}.",
		UEnum::GetValueAsString(Context),
		UEnum::GetValueAsString(InContext)
	);
	
	Context = InContext;
	OnContextChangedDelegate.Broadcast(Context);
	
	RE_LOG
	(
		"{0} applied.",
		UEnum::GetValueAsString(Context)
	);
}