//
//  MapGenerator.cpp
//  aprils_quest
//
//  Created by Carlos Zarate Florian on 10/20/16.
//
//

#include "MapGenerator.h"
#include "Urho3D/IO/File.h"
#include "Urho3D/Graphics/DebugRenderer.h"
#include "Urho3D/Urho2D/RigidBody2D.h"
#include "Urho3D/Urho2D/CollisionBox2D.h"
#include "Urho3D/Resource/ResourceCache.h"
#include "Urho3D/Urho2D/StaticSprite2D.h"


MapGenerator::MapGenerator(Context* context): Component(context)
{
    baseBox = 0;
    baseFloor = 0;
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SpriteSheet2D* SSTileSet = cache->GetResource<SpriteSheet2D>("Urho2D/sprites.xml");
    TileSetMap = SSTileSet->GetSpriteMapping();
}

void MapGenerator::RegisterObject(Context* context)
{
    context->RegisterFactory<MapGenerator>();
}

void MapGenerator::initMap(int width, int height, int massAmount, int massSize)
{
    createFloor(width, height);
    
    int **blockgrid = new int*[height];
    for (int i = 0; i <height; ++i)
        blockgrid[i] = new int[width];
    
    for(int i = 0 ; i < height; i++)
    {
        for(int j = 0 ; j <  width; j++)
        {
            blockgrid[j][i] = 0;
        }
    }
    
    for (int y = 0; y < width; y++) {
        for(int x = 0 ; x < height; x++) {
            int probability = 0;
            int probabilityModifier = 0;
            if (x%width<2|| x%width>(width-3)||y%height<2||y%height>(height-3)){
                probability=0;
            } else {
                probability = 15 + massAmount;
                
                if (x>2 && y>2){
                    int conformity =
                    (blockgrid[y-1][x-1] == blockgrid[y-2][x-1])+
                    (blockgrid[y-1][x-1] == blockgrid[y-1][x])+
                    (blockgrid[y-1][x-1] == blockgrid[y][x-1])+
                    (blockgrid[y-1][x-1] == blockgrid[y-1][x-2]);
                    if (conformity<2) {
                        if(blockgrid[y-1][x-1] == 0) {
                            blockgrid[y-1][x-1] = 1;
                        } else {
                            blockgrid[y-1][x-1] = 0;
                        }
                    }
                }
                probabilityModifier = (blockgrid[y][x-1]+blockgrid[y-1][x]+blockgrid[y-1][x+1])*(19+(massSize*1.4));
            }
            
            int rndm = rand() % 101;
            if(rndm<(probability+probabilityModifier)) {
                blockgrid[y][x]= 1;
            }
        }
    }
    
    for (int j = height-1; j >= 0; j-- )
    {
        for (int k = 0; k < height; k++)
        {
            if (!blockgrid[j][k])
            {
                createBlockNode(Vector3(j/2.0f - 0.25,k/2.0f - 0.25,0));
            }
        }
    }
    baseBox->Remove();
}

SharedPtr<Node> MapGenerator::getBaseBlock()
{
    if(baseBox == 0) {
        baseBox = GetScene()->CreateChild("Box");
        Sprite2D* currenttile = TileSetMap["tile8.png"];
        StaticSprite2D* targetstaticsprite = baseBox->CreateComponent<StaticSprite2D>();
        targetstaticsprite->SetSprite(currenttile);
        targetstaticsprite->SetOrderInLayer(65000);
        
        RigidBody2D* boxBody = baseBox->CreateComponent<RigidBody2D>();
        boxBody->SetBodyType(BT_STATIC);
        boxBody->SetLinearDamping(0.0f);
        boxBody->SetAngularDamping(0.0f);
        
        CollisionBox2D* shape = baseBox->CreateComponent<CollisionBox2D>();
        
        shape->SetSize(Vector2(0.5, 0.5));
        shape->SetDensity(1.0f);
        shape->SetFriction(0.5f);
        shape->SetRestitution(0.1f);
        shape->SetCategoryBits(32768);
    }
    return baseBox;
}

SharedPtr<Node> MapGenerator::getBaseFloor()
{
    if(baseFloor == 0) {
        baseFloor = GetScene()->CreateChild("floorTile");
        Sprite2D* currenttile = TileSetMap["tile1.png"];
        StaticSprite2D* targetstaticsprite = baseFloor->CreateComponent<StaticSprite2D>();
        targetstaticsprite->SetSprite(currenttile);
        targetstaticsprite->SetOrderInLayer(0);
    }
    return baseFloor;
}

void MapGenerator::createFloor(int width, int height)
{
    floorNode = GetScene()->CreateChild("floor");
    
    for(int i = 0 ; i < height; i++)
    {
        for(int j = 0 ; j <  width; j++)
        {
            createFloorNode(Vector3(i/2.0f - 0.25,j/2.0f - 0.25,0));
        }
    }
    
}

void MapGenerator::createFloorNode(Vector3 position)
{
    Node* box = getBaseFloor()->Clone();
    box->SetPosition(position);
}

void MapGenerator::createBlockNode(Vector3 position)
{
    Node* box = getBaseBlock()->Clone();
    box->SetPosition(position);
}
