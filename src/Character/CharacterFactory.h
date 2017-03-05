#pragma once

#include <Urho3D/Scene/Node.h>

using namespace Urho3D;

namespace Urho3D
{
    class Node;
    class String;
}

class CharacterFactory {
public:
    static Node* createCharacter(String type);
};
