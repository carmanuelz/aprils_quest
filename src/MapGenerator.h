//
//  MapGenerator.hpp
//  aprils_quest
//
//  Created by Carlos Zarate Florian on 10/20/16.
//
//

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <math.h>
#include "Urho3D/Core/Object.h"
#include "Urho3D/Core/Context.h"
#include "Urho3D/Scene/Component.h"
#include "Urho3D/Resource/JSONValue.h"
#include "Urho3D/Scene/Scene.h"
#include "Urho3D/Urho2D/SpriteSheet2D.h"
#include "Urho3D/Urho2D/Sprite2D.h"


using namespace Urho3D;

class MapGenerator : public Component
{
    URHO3D_OBJECT(MapGenerator, Component);
public:
    MapGenerator(Context* context);
    static void RegisterObject(Context* context);
    
    void initMap(int width, int height, int massAmount, int massSize);
private:
    void createBlockNode(Vector3 position);
    void createFloorNode(Vector3 position);
    void createFloor(int width, int height);
    void createBlocks(int width, int height);
    SharedPtr<Node> getBaseBlock();
    SharedPtr<Node> getBaseFloor();
    
    SharedPtr<Node> baseBox;
    SharedPtr<Node> baseFloor;
    SharedPtr<Node> floorNode;
    SharedPtr<Node> BlockNode;
    
    HashMap< String, SharedPtr< Sprite2D > > TileSetMap;
};

