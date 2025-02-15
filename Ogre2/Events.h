#pragma once

#include "stdafx.h"
#include "EventSystem.h"
#include "Common.h"

static Event<Ogre::SceneNode**> g_OnSelectionChangedEvent;
static Event<SelectionMode> g_OnSelectionModeChangedEvent;
static Event<Ogre::SceneNode*> g_OnNodeDeleted;
