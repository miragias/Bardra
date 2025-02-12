#pragma once

#include "EventSystem.h"
#include "Common.h"
#include "Ogre.h"

static Event<Ogre::SceneNode**> OnSelectionChanged;
static Event<SelectionMode> OnSelectionModeChanged;
static Event<int> Doo;
