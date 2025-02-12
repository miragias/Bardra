#pragma once

#include "stdafx.h"
#include "EventSystem.h"
#include "Common.h"

static Event<Ogre::SceneNode**> OnSelectionChanged;
static Event<SelectionMode> OnSelectionModeChanged;
static Event<int> Doo;
