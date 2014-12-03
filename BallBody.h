#pragma once

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include "stdint.h"
#include "Map.h"
#include "VoidSerializer.h"

class BallBody : public VoidSerializer {
    friend class RayCast;
public:
    BallBody(b2World *, const MapBall&, const b2BodyDef&, const b2Filter&, uint8_t);
    void move_to_start_position();
    b2Body *body;
    b2Fixture *ball_fixture;
    b2CircleShape *shape;
    uint8_t id_on_map;
};