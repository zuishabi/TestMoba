//
// Created by 张俏悦 on 2025/10/19.
//

#include "server.h"
#include "worldManager.h"

// 初始化世界中的地形
void CustomServer::InitializeWorld() {

   // 世界尺寸（以左上角 (0,0) 为原点，内部为 x:[0,worldWidth], y:[0,worldHeight]）
    const float worldWidth = 1280.0f;   // 整个世界宽（不再是半宽）
    const float worldHeight = 640.0f;   // 整个世界高（不再是半高）
    const float wallThickness = 4.0f;   // 墙厚度

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 0.0f; // 静态
    shapeDef.material.friction = 0.3f;
    shapeDef.material.restitution = 0.0f;
    // 设置碰撞层级
    shapeDef.filter.categoryBits = CATEGORY_BOUNDARY;
    shapeDef.filter.maskBits = MASK_BOUNDARY;

    // 顶墙（水平），中心在 (worldWidth/2, -wallThickness/2)
    {
        b2BodyDef bd = b2DefaultBodyDef();
        bd.type = b2_staticBody;
        bd.position = b2Vec2{ worldWidth * 0.5f, -wallThickness * 0.5f };
        b2BodyId body = b2CreateBody(GameWorld::World, &bd);
        b2Polygon box = b2MakeBox(worldWidth * 0.5f, wallThickness * 0.5f); // half-width, half-height
        b2CreatePolygonShape(body, &shapeDef, &box);
        std::cout << "top pos:" << bd.position.x << "," << bd.position.y << std::endl;
    }

    // 底墙（水平），中心在 (worldWidth/2, worldHeight + wallThickness/2)
    {
        b2BodyDef bd = b2DefaultBodyDef();
        bd.type = b2_staticBody;
        bd.position = b2Vec2{ worldWidth * 0.5f, worldHeight + wallThickness * 0.5f };
        b2BodyId body = b2CreateBody(GameWorld::World, &bd);
        b2Polygon box = b2MakeBox(worldWidth * 0.5f, wallThickness * 0.5f);
        b2CreatePolygonShape(body, &shapeDef, &box);
        std::cout << "bottom pos:" << bd.position.x << "," << bd.position.y << std::endl;
    }

    // 左墙（垂直），中心在 (-wallThickness/2, worldHeight/2)
    {
        b2BodyDef bd = b2DefaultBodyDef();
        bd.type = b2_staticBody;
        bd.position = b2Vec2{ -wallThickness * 0.5f, worldHeight * 0.5f };
        b2BodyId body = b2CreateBody(GameWorld::World, &bd);
        b2Polygon box = b2MakeBox(wallThickness * 0.5f, worldHeight * 0.5f);
        b2CreatePolygonShape(body, &shapeDef, &box);
        std::cout << "left pos:" << bd.position.x << "," << bd.position.y << std::endl;
    }

    // 右墙（垂直），中心在 (worldWidth + wallThickness/2, worldHeight/2)
    {
        b2BodyDef bd = b2DefaultBodyDef();
        bd.type = b2_staticBody;
        bd.position = b2Vec2{ worldWidth + wallThickness * 0.5f, worldHeight * 0.5f };
        b2BodyId body = b2CreateBody(GameWorld::World, &bd);
        b2Polygon box = b2MakeBox(wallThickness * 0.5f, worldHeight * 0.5f);
        b2CreatePolygonShape(body, &shapeDef, &box);
        std::cout << "right pos:" << bd.position.x << "," << bd.position.y << std::endl;
    }


    // 创建地形
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_staticBody;
    bd.position = b2Vec2(296,328);
    b2BodyId body = b2CreateBody(GameWorld::World, &bd);
    b2Polygon box = b2MakeBox(24,24);
    b2CreatePolygonShape(body,&shapeDef,&box);
}

