#include "BallBody.h"

#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>

BallBody::BallBody (b2World *world, const MapBall& map_ball, const b2BodyDef& ball_definition, const b2Filter& ball_filter, uint8_t id_on_map): VoidSerializer(static_cast<void*>(this), VoidSerializer::SERIALIZE_CLASS_BALL),id_on_map(id_on_map) {
    this->body = world->CreateBody(&ball_definition);

    this->body->SetTransform(b2Vec2(map_ball.x, map_ball.y), 0);
    this->ball_fixture = this->body->CreateFixture(map_ball.shape, 1);
    this->ball_fixture->SetFilterData(ball_filter);
    //ball_fixture->SetFriction(map_ball.friction);
    this->ball_fixture->SetRestitution(map_ball.bounce);

    b2MassData mass_data;
    this->body->GetMassData(&mass_data);
    mass_data.mass = map_ball.mass;
    this->body->SetMassData(&mass_data);
    this->body->SetUserData(this);

    this->shape = static_cast<b2CircleShape*>(map_ball.shape);
}

void BallBody::move_to_start_position()
{
    this->body->SetTransform(this->shape->m_p, this->body->GetAngle());
}

