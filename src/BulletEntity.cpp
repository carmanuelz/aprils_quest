#include "BulletEntity.h"
#include "Context.h"

BulletEntity::BulletEntity(Context* context) : LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

BulletEntity::~BulletEntity()
{
    //dtor
}

void BulletEntity::Start()
{
}

void BulletEntity::RegisterObject(Context* context)
{
	context->RegisterFactory<BulletEntity>();
}

void BulletEntity::DelayedStart()
{
}

void BulletEntity::Stop()
{
}

void BulletEntity::Update(float timeStep)
{
}

