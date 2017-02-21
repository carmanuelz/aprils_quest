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
#include <ctime>


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
    
    blockGrid = new int*[height];
    for (int i = 0; i < height; ++i)
        blockGrid[i] = new int[width];
    
    for(int i = 0 ; i < height; i++)
    {
        for(int j = 0 ; j <  width; j++)
        {
            blockGrid[i][j] = 0;
        }
    }
    
    //srand(time(0));
    for (int y = 0; y < height; y++) {
        for(int x = 0 ; x < width; x++) {
            int probability = 0;
            int probabilityModifier = 0;
            if ( x<2 || x>(width-3) || y<2 || y>(height-3) ){
                probability=0;
            } else {
                probability = 15 + massAmount;
                
                if (x>2 && y>2){
                    int conformity =
                    (blockGrid[y-1][x-1] == blockGrid[y-2][x-1])+
                    (blockGrid[y-1][x-1] == blockGrid[y-1][x])+
                    (blockGrid[y-1][x-1] == blockGrid[y][x-1])+
                    (blockGrid[y-1][x-1] == blockGrid[y-1][x-2]);
                    if (conformity<2) {
                        if(blockGrid[y-1][x-1] == 0) {
                            blockGrid[y-1][x-1] = 1;
                        } else {
                            blockGrid[y-1][x-1] = 0;
                        }
                    }
                }
                probabilityModifier = (blockGrid[y][x-1]+blockGrid[y-1][x]+blockGrid[y-1][x+1])*(19+(massSize*1.4));
            }
            int rndm = rand() % 101;
            if(rndm<(probability+probabilityModifier)) {
                blockGrid[y][x]= 1;
            }
        }
    }
    
    for (int j = 0; j < height; j++ )
    {
        for (int k = 0; k < width; k++)
        {
            if (!blockGrid[j][k])
            {
                createBlockNode(Vector3(k/2.0f + 0.25,j/2.0f + 0.25,0));
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

int ** MapGenerator::getNodeGrid()
{
    return blockGrid;
}
