#pragma once

#include <Bites/OgreApplicationContext.h>
#include <iostream>
#include "MoveHandles.h"

class CustomInput : public OgreBites::InputListener
{
	public:
        CustomInput(OgreBites::ApplicationContext* ctx) : m_Ctx(ctx), m_MoveHandles(nullptr) {}

		bool keyPressed(const OgreBites::KeyboardEvent& evt) override;


        void setMoveHandles(MoveHandles* handles) { m_MoveHandles = handles; }

        bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override {
            Ogre::RenderWindow* window = m_Ctx->getRenderWindow();

            if (m_MoveHandles) {
                m_MoveHandles->mouseMove(Ogre::Vector2(
                    evt.x / float(window->getWidth()),
                    evt.y / float(window->getHeight())
                ));
            }
            return true;
        }

        bool mousePressed(const OgreBites::MouseButtonEvent& evt) override {
            if (m_MoveHandles && evt.button == OgreBites::BUTTON_LEFT) 
            {
				Ogre::RenderWindow* window = m_Ctx->getRenderWindow();
                return m_MoveHandles->mousePressed(Ogre::Vector2(
                    evt.x / float(window->getWidth()),
                    evt.y / float(window->getHeight())
                ));
            }
            return true;
        }

        bool mouseReleased(const OgreBites::MouseButtonEvent& evt) override {
            if (m_MoveHandles && evt.button == OgreBites::BUTTON_LEFT) {
                return m_MoveHandles->mouseReleased();
            }
            return true;
        }


    private:
        OgreBites::ApplicationContext* m_Ctx;
		MoveHandles* m_MoveHandles;
};
