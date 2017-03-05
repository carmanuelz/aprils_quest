//
//  Collisionable.h
//  aprils_quest
//
//  Created by Carlos Zarate Florian on 2/21/17.
//
//

#ifndef Collisionable_h
#define Collisionable_h

#include "Urho3D/Urho3D.h"

namespace Urho3D
{
    class Node;
}

using namespace Urho3D;

//TODO: Animation speed is defined through characters velocity
class Collisionable
{
    
public:
    virtual void handleCollision(Node* collisionNode) = 0;
};


#endif /* Collisionable_h */
