#include "EnemyEntity.h"
#include "Urho3D/Core/Context.h"
#include "Urho3D/Urho2D/RigidBody2D.h"
#include "Urho3D/Urho2D/AnimatedSprite2D.h"
#include "Urho3D/Urho2D/AnimationSet2D.h"
#include "BulletEntity.h"
#include "Urho3D/Resource/ResourceCache.h"
#include "Urho3D/Scene/Scene.h"
#include "Urho3D/Urho2D/CollisionCircle2D.h"
#include "Urho3D/Input/Input.h"
#include "Urho3D/Graphics/Graphics.h"
#include "AStarFinder.h"
#include "Urho3D/Graphics/DebugRenderer.h"
#include "Urho3D/Urho2D/PhysicsWorld2D.h"
#include "Urho3D/Urho2D/Sprite2D.h"

EnemyEntity::EnemyEntity(Context* context) : LogicComponent(context)
{
	SetUpdateEventMask(USE_UPDATE);
}

EnemyEntity::~EnemyEntity()
{
}

void EnemyEntity::RegisterObject(Context* context)
{
	context->RegisterFactory<EnemyEntity>();
}

void EnemyEntity::Start()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Sprite2D* object = cache->GetResource<Sprite2D>("Urho2D/object.png");
    if (!object)
        return;

    AnimationSet2D* animationSet = cache->GetResource<AnimationSet2D>("Urho2D/soldado.scml");
    if (!animationSet)
        return;

    AnimatedSprite2D* animatedSprite = node_->CreateComponent<AnimatedSprite2D>();
    animatedSprite->SetLayer(2);
    // Set animation
    animatedSprite->SetAnimationSet(animationSet);
    animatedSprite->SetAnimation("idle");
    animatedSprite->SetSpeed(1.5f);

    objectsprite = node_->CreateComponent<StaticSprite2D>();
	objectsprite->SetSprite(object);
	objectsprite->SetColor(Color::TRANSPARENT);

    RigidBody2D* bodysprite = node_->CreateComponent<RigidBody2D>();
    bodysprite->SetBodyType(BT_DYNAMIC);
    bodysprite->SetFixedRotation(true);
    bodysprite->SetBullet(false);
    bodysprite->SetLinearDamping(20.0f);
    bodysprite->SetLinearVelocity(Vector2::ZERO);
    CollisionCircle2D* circle = node_->CreateComponent<CollisionCircle2D>();
    // Set radius
    circle->SetRadius(0.16f);
    // Set density
    circle->SetDensity(1.0f);
    // Set friction.
    circle->SetFriction(0.0f);
    // Set restitution
    circle->SetRestitution(0.0f);

    circle->SetCategoryBits(8192);
}

void EnemyEntity::DelayedStart()
{

}

void EnemyEntity::Stop()
{
}

void EnemyEntity::Update(float timeStep)
{
    RigidBody2D* body = GetComponent<RigidBody2D>();
    UpdateFind();
    body->SetLinearVelocity(Vector2::ZERO);
    body->SetLinearVelocity(vel);
    if(timeBusy < MAX_TIME)
        timeBusy+= timeStep;
    if(timeBusy > timeAtack)
        isBusy = false;
}

void EnemyEntity::Shoot()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Vector2 worldPoint = target_->GetPosition2D();
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

    circleshape->SetMaskBits(57342);

    //circleshape->SetMaskBits(65534);
	/// create Enemy component which controls the Enemy behavior

	bulletBody->SetLinearVelocity(dir*5);
	bulletNode_->CreateComponent<BulletEntity>();

	isBusy = true;
	timeBusy = 0;
}

void EnemyEntity::SetTarget()
{
    objectsprite->SetColor(Color::YELLOW);
}

void EnemyEntity::SetUntarget()
{
    objectsprite->SetColor(Color::TRANSPARENT);
}

void EnemyEntity::SetObjetive()
{
    objectsprite->SetColor(Color::RED);
}
void EnemyEntity::findto(Vector2 to)
{
    findto(to.x_, to.y_);
}

void EnemyEntity::findto(float tox, float toy)
{
    Vector2 pos = node_->GetPosition2D();
    AStarFinder* finder = GetScene()->GetComponent<AStarFinder>();
    float tilesize = finder->tilesize;
    int endX = floor(tox/tilesize);
    int endY = floor(toy/tilesize);
    float floatX = pos.x_/tilesize;
    float floatY = pos.y_/tilesize;
    int startX;
    int startY;
    if((floatX - preenemmyX)>1.5f || (preenemmyX - floatX)>0.5f)
    {
        startX = floor(floatX);
        preenemmyX = startX;
        flagdestinityX = true;
    }
    else
    {
        startX = preenemmyX;
        flagdestinityX = false;
    }

    if((floatY - preenemmyY)>1.5f || (preenemmyY - floatY)>0.5f)
    {
        startY = floor(floatY);
        preenemmyY = startY;
        flagdestinityY = true;
    }
    else
    {
        startY = preenemmyY;
        flagdestinityY = false;
    }
    if((flagdestinityY || flagdestinityX) && finder->isWalkableAt(startX,startY) && finder->isWalkableAt(endX,endY))
    {
        route = finder->findPath(startX,startY,endX,endY);
        int len = route.Size();
        if(len>1)
        {
            Vector2 difstep((route[len-2]->x_ - route[len-1]->x_)*tilesize, (route[len-2]->y_ - route[len-1]->y_)*tilesize);
            float modenemy = sqrt(pow(difstep.x_,2)+pow(difstep.y_,2));
            if(modenemy != 0)
            {
                Vector2 enemmyVU((difstep.x_)/modenemy,(difstep.y_)/modenemy);
                Vector2 enemyVel(enemmyVU.x_*facVel,enemmyVU.y_*facVel);
                vel = enemyVel;
                if(enemyVel.x_ > 0)
                    setRight();
                else
                {
                    if(enemyVel.x_ < 0)
                        setLeft();
                    else
                    {
                        if(tox < pos.x_)
                            setLeft();

                        else
                            setRight();
                    }
                }
            }
        }
    }
}

void EnemyEntity::setRight()
{
    AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();
    animatesprite->SetFlipX(false);
}

void EnemyEntity::setLeft()
{
    AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();
    animatesprite->SetFlipX(true);
}

void EnemyEntity::UpdateFind()
{
    AStarFinder* finder = GetScene()->GetComponent<AStarFinder>();
    Vector2 pos = node_->GetPosition2D();

    if(target_ != 0)
    {
        /*if(currenttimefind > limittimefind)
        {
            Target = 0;
            GotoPosition(Origin.x, Origin.y);
        }
        else
        {*/
            if(findflag && !isBusy)
            {
                AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();
                if(animatesprite->GetAnimation()!= "run")
                    animatesprite->SetAnimation("run", animatesprite->GetLoopMode());
                findto(target_->GetPosition2D());
                    //std::cout<<vel.x_<<" _ "<<vel.y_<<std::endl;

            }
            else
            {
                preenemmyX = 0;
                preenemmyY = 0;
                vel = Vector2::ZERO;

            }
        //}
    }
    else
    {
        if(gotoflag)
        {
            if( (flagdestinityY || flagdestinityX) &&
                floor(pos.x_/finder->tilesize)==floor(findposition.x_/finder->tilesize) &&
                floor(pos.y_/finder->tilesize)==floor(findposition.y_/finder->tilesize))
            {
                vel = Vector2::ZERO;
                gotoflag = false;
                AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();
                if(animatesprite->GetAnimation()!= "idle")
                    animatesprite->SetAnimation("idle", animatesprite->GetLoopMode());
                //currenttimefind = 0;
            }
            else
                findto(findposition.x_,findposition.y_);
        }
    }
}

void EnemyEntity::GotoPosition(Vector2 to)
{
    AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();
    if(animatesprite->GetAnimation()!= "run")
        animatesprite->SetAnimation("run", animatesprite->GetLoopMode());
    gotoflag = true;
    findposition = to;
}

void EnemyEntity::CastTarget(Node* target, bool isdebug)
{
    PhysicsWorld2D* physicsWorld = GetScene()->GetComponent<PhysicsWorld2D>();

    Vector2 targetpos = target->GetPosition2D();
    Vector2 originpos = node_->GetPosition2D();

    Vector2 direction = targetpos - originpos;
    direction.Normalize();

    Vector2 originA = originpos + Vector2(direction.y_*0.1f,-direction.x_*0.1f);
    Vector2 originB = originpos + Vector2(-direction.y_*0.1f,direction.x_*0.1f);

    Vector2 FinishA = originA + Vector2(direction.x_*2,direction.y_*2);
    Vector2 FinishB = originB + Vector2(direction.x_*2,direction.y_*2);
    Vector2 FinishC = originpos + Vector2(direction.x_*2,direction.y_*2);

    /*Vector2 targetbehaviour;
        if(isright)
            targetbehaviour = sf::Vector2f(originpos.x*PPM+50, originpos.y*PPM);
        else
            targetbehaviour = sf::Vector2f(originpos.x*PPM-50, originpos.y*PPM);*/

    PhysicsRaycastResult2D RayCastA;
    PhysicsRaycastResult2D RayCastB;
    PhysicsRaycastResult2D RayCastC;

    if(hasrange)
    {
        physicsWorld->RaycastSingle(RayCastA, originA , FinishA,1);
        physicsWorld->RaycastSingle(RayCastB, originB , FinishB,1);
        if (RayCastA.body_ == RayCastB.body_ && RayCastA.body_)
        {
            Node* node = RayCastA.body_->GetNode();
            if(node->GetName() == "Player")
            {
                if(target_ == 0 && !gotoflag)
                {
                    //setAnimCicle(2);
                    SetTarget(target);
                }
                findflag = false;
            }
            else
            {
                findflag = true;
            }

            FinishA = RayCastA.position_;
            FinishB = RayCastB.position_;

            //if(!gotoflag)
                //targetbehaviour = sf::Vector2f(FinishC.x*PPM, FinishC.y*PPM);
        }
        else
            findflag = true;
    }
    else
    {
        physicsWorld->RaycastSingle(RayCastC, originpos , FinishC,1);
        if ( RayCastC.body_)
        {
            Node* node = RayCastC.body_->GetNode();
            if(target_ == 0 && !gotoflag)
            {
                if(node->GetName() == "Player")
                {
                    //setAnimCicle(2);
                    SetTarget(target);
                }
            }
            FinishC = RayCastC.position_;
        }
    }

    if(hasrange)
    {
        //updatebehaviour(targetbehaviour.x, targetbehaviour.y);
        if(!isBusy && target_ != 0)
        {
            if(!findflag)
            {
                AnimatedSprite2D* animatesprite = GetComponent<AnimatedSprite2D>();
                if(animatesprite->GetAnimation()!= "idle")
                    animatesprite->SetAnimation("idle", animatesprite->GetLoopMode());
                Shoot();
            }
        }
    }

    if(isdebug)
    {
        DebugRenderer* debug = GetScene()->GetComponent<DebugRenderer>();
        if(hasrange)
        {
            debug->AddLine( Vector3(originA.x_, originA.y_,0),
                            Vector3(FinishA.x_, FinishA.y_,0),
                            Color(0, 1, 1, 1),
                            false );

            debug->AddLine( Vector3(originB.x_, originB.y_,0),
                            Vector3(FinishB.x_, FinishB.y_,0),
                            Color(0, 1, 1, 1),
                            false );
        }
        else
        {
            debug->AddLine( Vector3(originpos.x_, originpos.y_,0),
                            Vector3(FinishC.x_, FinishC.y_,0),
                            Color(0, 1, 1, 1),
                            false );
        }
        if(target_ != 0)
        {
            for(int index = 1; index < route.Size(); index++)
            {
                debug->AddLine(Vector3(route[index-1]->x_*0.5f+0.25f, route[index-1]->y_*0.5f+0.25f,0),
                               Vector3(route[index]->x_*0.5f+0.25f, route[index]->y_*0.5f+0.25f,0),
                               Color(0, 1, 1, 1),
                               false );
            }
        }
    }
}

void EnemyEntity::SetTarget(Node* target)
{
    target_ = target;
}
