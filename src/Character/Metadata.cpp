
#include "Metadata.h"

String Metadata::getComponentName()
{
    return ComponentName;
}


Metadata::Metadata(Context* context):
Component(context)
{
    
}

void Metadata::RegisterObject(Context* context)
{
    context->RegisterFactory<Metadata>();
}
