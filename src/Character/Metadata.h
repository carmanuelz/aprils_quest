

#pragma once

#include <stdio.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Core/Context.h>

using namespace Urho3D;

namespace Urho3D
{
    class String;
    class Sprite;
}

class Metadata: public Component
{
URHO3D_OBJECT(Metadata, Component);
public:
    static void RegisterObject(Context* context);
    Metadata(Context* context);
    String getComponentName();
private:
    String ComponentName;
};
