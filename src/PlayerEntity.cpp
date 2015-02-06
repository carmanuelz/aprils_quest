#include "PlayerEntity.h"
#include "Context.h"
#include "Component.h"
#include "Node.h"
#include "RigidBody2D.h"
#include "iostream"
#include "AnimatedSprite2D.h"
#include "AnimationSet2D.h"
#include "PhysicsEvents2D.h"
#include "BulletEntity.h"
#include "StaticSprite2D.h"
#include "ResourceCache.h"
#include "Scene.h"
#include "CollisionCircle2D.h"
#include "RigidBody2D.h"
#include "Input.h"
#include "Vector3.h"
#include "Graphics.h"
#include "Camera.h"
#include "PhysicsWorld2D.h"
#include "DebugRenderer.h"
#include "EnemyEntity.h"
#include <math.h>

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

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
    animatedSprite->SetLayer(2);
    // Set animation
    animatedSprite->SetAnimation(animationSet, "Idle");
    animatedSprite->SetSpeed(1.5f);

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

    if((controls_.IsDown(LOOK_LEFT) && controls_.IsDown(CTRL_RIGHT)) ||
       (!controls_.IsDown(LOOK_LEFT) && controls_.IsDown(CTRL_LEFT)))
        animatesprite->SetSpeed(-1.5f);
    else
        animatesprite->SetSpeed(1.5f);

    if(!moveDir.Equals(Vector2::ZERO))
    {
        if(animatesprite->GetAnimation()!= "Run")
            animatesprite->SetAnimation("Run", animatesprite->GetLoopMode());
    }
    else
    {
        if(animatesprite->GetAnimation()!= "Idle")
            animatesprite->SetAnimation("Idle", animatesprite->GetLoopMode());
    }
    body->SetLinearVelocity(moveDir*MOVE_FORCE);

    AutoTarget();
}

void PlayerEntity::Shoot()
{
    if(!CurrentTarget)
        return;
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Input* input = GetSubsystem<Input>();

    Vector2 worldPoint = CurrentTarget->GetPosition2D();
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
