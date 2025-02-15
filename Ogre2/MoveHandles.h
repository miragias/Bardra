#pragma once
#include "stdafx.h"
#include "Common.h"
#include "Events.h"

class MoveHandles {
public:
    MoveHandles(Ogre::SceneManager* sceneMgr,
        Ogre::SceneNode** targetNode, Ogre::Camera* camera, SelectionMode* currentSelectionmode,
            std::unordered_map<Ogre::SceneNode*, int>* vertexToNodeIndex,
            std::vector<Ogre::Vector3>* vertices)
        : m_SceneMgr(sceneMgr)
        , m_TargetNode(targetNode)
        , m_Camera(camera)
        , m_SelectedAxis(None)
        , m_IsDragging(false)
        , m_VertexNodeToIndex(vertexToNodeIndex)
        , m_Vertices(vertices)
    {
        setupHandles();
        g_OnSelectionModeChangedEvent.Subscribe([this](SelectionMode newNode) {
            onSelectionChanged(newNode);
		});
    }

    enum Axis {
        None,
        X,
        Y,
        Z
    };

    void Update();
    bool MousePressed(const Ogre::Vector2& mousePos);
    bool MouseReleased();
    void MouseMove(const Ogre::Vector2& mousePos);

private:
    static const float HANDLE_SCALE;
    static const float HANDLE_LENGTH;

    Axis m_SelectedAxis;
    bool m_IsDragging;
    Ogre::Vector3 m_LastMousePos;
    Ogre::SceneManager* m_SceneMgr;
    Ogre::SceneNode** m_TargetNode;
    Ogre::Camera* m_Camera;
    Ogre::SceneNode* m_HandleNode;
    Ogre::SceneNode* m_XHandle;
    Ogre::SceneNode* m_YHandle;
    SelectionMode* m_CurrentSelectionMode;

    Event<SelectionMode> m_OnSelectionModeChanged;
    std::unordered_map<Ogre::SceneNode*, int>* m_VertexNodeToIndex;
    std::vector<Ogre::Vector3>* m_Vertices;
    void setupHandles();
    void setVisible(bool visible);
    void onSelectionChanged(SelectionMode currentMode);
    Axis getSelectedAxis(const Ogre::Vector2& mousePos);
    Ogre::Vector3 getMouseWorldPos(const Ogre::Vector2& mousePos, const Ogre::Plane& plane);

};
