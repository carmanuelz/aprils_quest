#include "PlayerEntity.h"
#include "Urho3D/Core/Context.h"
#include "Urho3D/Scene/Component.h"
#include "Urho3D/Scene/Node.h"
#include "Urho3D/Urho2D/RigidBody2D.h"
#include "iostream"
#include "Urho3D/Urho2D/AnimatedSprite2D.h"
#include "Urho3D/Urho2D/AnimationSet2D.h"
#include "Urho3D/Urho2D/PhysicsEvents2D.h"
#include "BulletEntity.h"
#include "Urho3D/Urho2D/StaticSprite2D.h"
#include "Urho3D/Resource/ResourceCache.h"
#include "Urho3D/Scene/Scene.h"
#include "Urho3D/Urho2D/CollisionCircle2D.h"
#include "Urho3D/Urho2D/RigidBody2D.h"
#include "Urho3D/Input/Input.h"
#include "Urho3D/Math/Vector3.h"
#include "Urho3D/Graphics/Graphics.h"
#include "Urho3D/Graphics/Camera.h"
#include "Urho3D/Urho2D/PhysicsWorld2D.h"
#include "Urho3D/Graphics/DebugRenderer.h"
#include "EnemyEntity.h"
#include "Urho3D/Urho2D/Sprite2D.h"
#include <math.h>

#define DEGTORAD 0.0174532925199432957f
//#define RADTODEG 57.295779513082320876f
#define M_PI 3.14159265358979323846

PlayerEntity::PlayerEntity(Context* context) : LogicComponent(context)
{
	SetUpdateEventMask(USE_UPDATE);
}

PlayerEntity::~PlayerEntity()
{
}

void PlayerEntity::RegisterObject(Context* context)
{
	context->RegisterFactory<PlayerEntity>();
}

void PlayerEntity::Start()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    AnimationSet2D* animationSet = cache->GetResource<AnimationSet2D>("Urho2D/April.scml");
    if (!animationSet)
        return;

    AnimatedSprite2D* animatedSprite = node_->CreateComponent<AnimatedSprite2D>();
    animatedSprite->SetLayer(100);
    // Set animation
    animatedSprite->SetAnimationSet(animationSet);
    animatedSprite->SetAnimation("idle");
    animatedSprite->SetSpeed(-1.5f);

    RigidBody2D* bodysprite = node_->CreateComponent<RigidBody2D>();
    bodysprite->SetBodyType(BT_DYNAMIC);
    bodysprite->SetFixedRotation(true);
    bodysprite->SetLinearDamping(20.0f);
    CollisionCircle2D* circle = node_->CreateComponent<CollisionCircle2D>();
    // Set radius
    circle->SetRadius(0.16f);
    // Set density
    circle->SetDensity(1.0f);
    // Set friction.
    circle->SetFriction(0.0f);
    // Set restitution
    circle->SetRestitution(0.0f);

    circle->SetCategoryBits(16384);

    weaponnode = node_->CreateChild("weapon");
    AnimationSet2D* weaponanimset = cache->GetResource<AnimationSet2D>("Urho2D/gun.scml");
    if (!weaponanimset)
        return;
    AnimatedSprite2D* gunanim = weaponnode->CreateComponent<AnimatedSprite2D>();
    gunanim->SetAnimationSet(weaponanimset);
    gunanim->SetAnimation("shoot");
    gunanim->SetOrderInLayer(7);
}

void PlayerEntity::DelayedStart()
{

}

void PlayerEntity::Stop()
{
}

void PlayerEntity::Update(float timeStep)
{
    RigidBody2D* body = GetComponent<RigidBody2D>();
    AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();

    UpdateBehaviour();

    Vector2 moveDir = Vector2::ZERO;
    body->SetLinearVelocity(Vector2::ZERO);

    if (controls_.IsDown(CTRL_UP))
        moveDir += Vector2::UP;
    if (controls_.IsDown(CTRL_DOWN))
        moveDir += Vector2::DOWN;
    if (controls_.IsDown(CTRL_LEFT))
        moveDir += Vector2::LEFT;
    if (controls_.IsDown(CTRL_RIGHT))
        moveDir += Vector2::RIGHT;

    if(controls_.IsDown(LOOK_LEFT))
        animatesprite->SetFlipX(true);
    else
        animatesprite->SetFlipX(false);

    /*if((controls_.IsDown(LOOK_LEFT) && controls_.IsDown(CTRL_RIGHT)) ||
       (!controls_.IsDown(LOOK_LEFT) && controls_.IsDown(CTRL_LEFT))) {
        animatesprite->SetSpeed(-1.5f);
    } else {
        animatesprite->SetSpeed(1.5f);
    }*/

    if(!moveDir.Equals(Vector2::ZERO))
    {
        if(animatesprite->GetAnimation()!= "run")
            animatesprite->SetAnimation("run", animatesprite->GetLoopMode());
    }
    else
    {
        if(animatesprite->GetAnimation()!= "idle")
            animatesprite->SetAnimation("idle", animatesprite->GetLoopMode());
    }
    body->SetLinearVelocity(moveDir*MOVE_FORCE);

    AutoTarget();
}

void PlayerEntity::Shoot()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Input* input = GetSubsystem<Input>();

    Vector2 worldPoint = GetScene()->GetChild("Target",false)->GetPosition2D();
    Vector2 dir = Vector2(worldPoint.x_,worldPoint.y_);
    dir = (dir - node_->GetPosition2D());
    dir.Normalize();

    Sprite2D* bulletSprite = cache->GetResource<Sprite2D>("Urho2D/bullet.png");
    if (!bulletSprite)
        return;

	SharedPtr<Node> bulletNode_(GetScene()->CreateChild("bullet"));
	bulletNode_->SetPosition2D(node_->GetPosition2D());
	bulletNode_->SetDirection(Vector3(dir.x_,-dir.y_,0));
	StaticSprite2D* staticSprite = bulletNode_->CreateComponent<StaticSprite2D>();
	staticSprite->SetSprite(bulletSprite);
	staticSprite->SetLayer(6 * 10);

	RigidBody2D* bulletBody = bulletNode_->CreateComponent<RigidBody2D>();
    bulletBody->SetBodyType(BT_DYNAMIC);
    bulletBody->SetFixedRotation(true);

    CollisionCircle2D* circleshape = bulletNode_->CreateComponent<CollisionCircle2D>();
    // Set radius
    circleshape->SetRadius(0.08f);
    // Set density
    circleshape->SetDensity(1.0f);
    // Set friction.
    circleshape->SetFriction(0.0f);
    // Set restitution
    circleshape->SetRestitution(0.0f);

    circleshape->SetMaskBits(49150);


	/// create Enemy component which controls the Enemy behavior

	bulletBody->SetLinearVelocity(dir*5);
	BulletEntity* b = bulletNode_->CreateComponent<BulletEntity>();
}

void PlayerEntity::AutoTarget()
{
    PhysicsWorld2D* physicsWorld = GetScene()->GetComponent<PhysicsWorld2D>();

    for(int index = 0; index < TargetList.Size(); index++)
    {
        EnemyEntity* Enemy = TargetList[index]->GetComponent<EnemyEntity>();
        Enemy->SetUntarget();
    }

    TargetList.Clear();

    for(int angle = 0; angle < 360; angle++)
    {
        Node* LastTargetNode;
        PhysicsRaycastResult2D raycast;
        float currentRayAngle = angle*DEGTORAD;
        float rayLength = 5; //long enough to hit the walls
        Vector2 p1(node_->GetPosition2D());
        Vector2 p2 = p1 + rayLength * Vector2( sinf(currentRayAngle), cosf(currentRayAngle) );
        physicsWorld->RaycastSingle(raycast,p1,p2,1);
        if(raycast.body_)
        {
            Node* TargetNode = raycast.body_->GetNode();
            if(LastTargetNode == TargetNode)
            {
                continue;
            }
            LastTargetNode = TargetNode;

            if(TargetNode->GetName() == "EnemyNode")
            {
                EnemyEntity* Enemy = TargetNode->GetComponent<EnemyEntity>();
                Enemy->SetTarget();
                TargetList.Push(TargetNode);
            }
        }
    }

    if(TargetList.Size() > 0)
    {
        if(CurrentTarget == 0)
        {
            CurrentTarget = TargetList[0];
        }
        else
        {
            bool band = true;
            for(int index = 0; index < TargetList.Size(); index++)
            {
                if(CurrentTarget == TargetList[index])
                {
                    band = false;
                    break;
                }
            }
            if(band)
            {
                CurrentTarget = 0;
            }
        }
    }
    else
        CurrentTarget = 0;

    if(CurrentTarget)
        CurrentTarget->GetComponent<EnemyEntity>()->SetObjetive();
}

void PlayerEntity::ChangeTarget()
{
    if(TargetList.Size() >1)
    {
        bool band = true;
        for(int index = 0; index < TargetList.Size()-1; index++)
        {
            if(CurrentTarget == TargetList[index])
            {
                CurrentTarget->GetComponent<EnemyEntity>()->SetTarget();
                TargetList[index+1]->GetComponent<EnemyEntity>()->SetObjetive();
                CurrentTarget = TargetList[index+1];
                band = false;
                break;
            }
        }
        if(band)
        {
            CurrentTarget->GetComponent<EnemyEntity>()->SetTarget();
            TargetList[0]->GetComponent<EnemyEntity>()->SetObjetive();
            CurrentTarget = TargetList[0];
        }
    }
}


void PlayerEntity::UpdateBehaviour()
{
    AnimatedSprite2D* weaponanim = weaponnode->GetComponent<AnimatedSprite2D>();
    Vector2 dirtarget = GetScene()->GetChild("Target",false)->GetPosition2D()-node_->GetPosition2D();

    float angle = fmod(atan2(dirtarget.y_, dirtarget.x_)*180/M_PI,360);

    angle = angle - weaponnode->GetRotation2D();
    float currentangle = angle + weaponnode->GetRotation2D();

    if(currentangle < 0)
        currentangle = 360 + currentangle;

    if(currentangle < 360 && currentangle >= 270)
    {
        weaponanim->SetFlipY(false);
        //weaponnode->SetPosition2D(21/100,30/100);
        weaponnode->RotateAround2D(Vector2(0,0),angle);
    }
    if(currentangle < 270 && currentangle >= 180)
    {
        weaponanim->SetFlipY(true);
        //weaponnode->SetPosition2D(27/100,30/100);
        weaponnode->RotateAround2D(Vector2(36/100,0),angle);
    }
    if(currentangle < 180 && currentangle >= 90)
    {
        weaponanim->SetFlipY(true);
        //weaponnode->SetPosition2D(18/100,40/100);
        weaponnode->RotateAround2D(Vector2(27/100,8/100),angle);
    }

    if(currentangle < 90 && currentangle >= 0)
    {
        weaponanim->SetFlipY(false);
        //weaponnode->SetPosition2D(28/100,40/100);
        weaponnode->RotateAround2D(Vector2(8/100,8/100),angle);
    }
}
