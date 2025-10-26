//
// Created by 张俏悦 on 2025/10/24.
//

#include "skill.h"
#include "../components/components.h"
#include "../../objects/worldManager.h"


void ezQ::Update() {
    for (int i = 0;i < skills->infos.size();i++) {
        int capacity = b2Shape_GetSensorCapacity( skills->infos[i].shapeID );
        std::vector<b2ShapeId> overlaps;
        overlaps.resize( capacity );

        // Now get all overlaps and record the actual count
        int count = b2Shape_GetSensorOverlaps( skills->infos[i].shapeID, overlaps.data(), capacity );
        overlaps.resize( count );


        for ( int j = 0; j < count; ++j )
        {
            b2ShapeId visitorId = overlaps[j];

            // Ensure the visitorId is valid
            if ( b2Shape_IsValid( visitorId ) == false )
            {
                continue;
            }
            std::cout << "collide" << std::endl;
            // process overlap using game logic
        }
    }
}


void ezQ::Execute(SkillInfo info) {

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_kinematicBody;
    bodyDef.position = info.pos;
    bodyDef.isBullet = true;
    b2BodyId myBodyId = b2CreateBody(World,&bodyDef);
    b2Body_SetLinearVelocity(myBodyId,info.direction*100);

    b2Capsule capsule;
    capsule.center1 = {0,4};
    capsule.center2 = {0,-4};
    capsule.radius = 3;

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.isSensor = true;
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 0.3f;
    shapeDef.material.restitution = 0;
    shapeDef.filter.categoryBits = CATEGORY_SKILL;
    shapeDef.filter.maskBits = MASK_SKILL;
    shapeDef.enableSensorEvents = true;
    b2ShapeId bodyShape = b2CreateCapsuleShape(myBodyId, &shapeDef, &capsule);

    auto id = b2StoreBodyId(myBodyId);
    StraightBulletSkillInfo i = {id,myBodyId,bodyShape};
    skills->infos.push_back(i);
}
