/*
 * Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "Test.h"
#include "RayCast.h"
#include <stdio.h>
#include <iostream>
#include "main.h"
#include <algorithm>
#include <Box2D/Common/b2Math.h>

void DestructionListener::SayGoodbye(b2Joint* joint)
{
	if (test->m_mouseJoint == joint)
	{
		test->m_mouseJoint = NULL;
	}
	else
	{
		test->JointDestroyed(joint);
	}
}

Test::Test()
{
	b2Vec2 gravity;
	gravity.Set(0.0f, 0.0f);
	m_world = new b2World(gravity);
	m_bomb = NULL;
	m_textLine = 30;
	m_mouseJoint = NULL;
	m_pointCount = 0;

	m_destructionListener.test = this;
	m_world->SetDestructionListener(&m_destructionListener);
	m_world->SetContactListener(this);
	m_world->SetDebugDraw(&m_debugDraw);

	m_bombSpawning = false;

	m_stepCount = 0;

	memset(&m_maxProfile, 0, sizeof (b2Profile));
	memset(&m_totalProfile, 0, sizeof (b2Profile));
}

Test::~Test()
{
	// By deleting the world, we delete the bomb, mouse joint, etc.
	delete m_world;
	m_world = NULL;
}

void Test::BeginContact(b2Contact* contact)
{
	b2Fixture *fixture_a = contact->GetFixtureA();
	b2Fixture *fixture_b = contact->GetFixtureB();

	VoidSerializer *a_object = dynamic_cast<VoidSerializer*>(static_cast<VoidSerializer*>(fixture_a->GetUserData()));
        VoidSerializer *b_object = dynamic_cast<VoidSerializer*>(static_cast<VoidSerializer*>(fixture_b->GetUserData()));

        if(a_object == nullptr || b_object == nullptr)
	{
		std::cout << "objects is nullptr" << std::endl;
                return;
	}

	if(a_object->get_serialize_class() == VoidSerializer::SERIALIZE_CLASS_BALL && b_object->get_serialize_class() == VoidSerializer::SERIALIZE_CLASS_CLIENT ||
		b_object->get_serialize_class() == VoidSerializer::SERIALIZE_CLASS_BALL && a_object->get_serialize_class() == VoidSerializer::SERIALIZE_CLASS_CLIENT)
	{
		contact->SetRestitution(0);
	}

}

void Test::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	const b2Manifold* manifold = contact->GetManifold();

	if (manifold->pointCount == 0)
	{
		return;
	}

	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
	b2GetPointStates(state1, state2, oldManifold, manifold);

	b2WorldManifold worldManifold;
	// <block for server>

	/*
        VoidSerializer *a_object = dynamic_cast<VoidSerializer*>(static_cast<VoidSerializer*>(fixtureA->GetUserData()));
        VoidSerializer *b_object = dynamic_cast<VoidSerializer*>(static_cast<VoidSerializer*>(fixtureB->GetUserData())

	if(a_object != nullptr && b_object != nullptr && (a_object->get_serialize_class() == VoidSerializer::SERIALIZE_CLASS_BALL && b_object->get_serialize_class() == VoidSerializer::SERIALIZE_CLASS_CLIENT ||
		b_object->get_serialize_class() == VoidSerializer::SERIALIZE_CLASS_BALL && a_object->get_serialize_class() == VoidSerializer::SERIALIZE_CLASS_CLIENT))
	{
		std::cout << "set contact to 0" << std::endl;
		contact->SetRestitution(0);
	}
	 * */
	// </block for server>

	contact->GetWorldManifold(&worldManifold);

	for (int32 i = 0; i < manifold->pointCount && m_pointCount < k_maxContactPoints; ++i)
	{
		ContactPoint* cp = m_points + m_pointCount;
		cp->fixtureA = fixtureA;
		cp->fixtureB = fixtureB;
		cp->position = worldManifold.points[i];
		cp->normal = worldManifold.normal;
		cp->state = state2[i];
		cp->normalImpulse = manifold->points[i].normalImpulse;
		cp->tangentImpulse = manifold->points[i].tangentImpulse;
		cp->separation = worldManifold.separations[i];
		++m_pointCount;
	}
}

void Test::DrawTitle(const char *string)
{
	m_debugDraw.DrawString(5, DRAW_STRING_NEW_LINE, string);
	m_textLine = 2 * DRAW_STRING_NEW_LINE;
}

class QueryCallback : public b2QueryCallback
{
public:

	QueryCallback(const b2Vec2& point)
	{
		m_point = point;
		m_fixture = NULL;
	}

	bool ReportFixture(b2Fixture* fixture)
	{
		b2Body* body = fixture->GetBody();
		if (body->GetType() == b2_dynamicBody)
		{
			bool inside = fixture->TestPoint(m_point);
			if (inside)
			{
				m_fixture = fixture;

				// We are done, terminate the query.
				return false;
			}
		}

		// Continue the query.
		return true;
	}

	b2Vec2 m_point;
	b2Fixture* m_fixture;
};

void Test::SetCurrentPlayer(Player * player)
{
	current_player = player;
}

void Test::MouseDownCallback()
{

}

void Test::MouseDown(const b2Vec2& p)
{
	m_mouseWorld = p;

	if (m_mouseJoint != NULL)
	{
		return;
	}

	// Make a small box.
	b2AABB aabb;
	b2Vec2 d;
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = p - d;
	aabb.upperBound = p + d;

	// Query the world for overlapping shapes.
	QueryCallback callback(p);
	m_world->QueryAABB(&callback, aabb);


	if (callback.m_fixture)
	{
		b2Body* body = callback.m_fixture->GetBody();

		if (body->GetUserData() != nullptr)
		{
			VoidSerializer * object = static_cast<VoidSerializer*> (body->GetUserData());
			switch (object->get_serialize_class())
			{
				case VoidSerializer::SERIALIZE_CLASS_CLIENT:
				{
					Player * player = object->get_pointer<Player>();
					current_player = player;
					current_ball = nullptr;
					ShowPlayerPanel(player);

					MouseDownCallback();
					break;
				}
				case VoidSerializer::SERIALIZE_CLASS_BALL:
				{
					BallBody *ball = object->get_pointer<BallBody>();
					current_player = nullptr;
					current_ball = ball;
					ShowBallPanel(ball);
					break;
				}
			}
		}
		else
			current_player = nullptr;

		b2MouseJointDef md;
		md.bodyA = m_groundBody;
		md.bodyB = body;
		md.target = p;
		md.maxForce = 1000.0f * body->GetMass();
		std::cout << "test2" << std::endl;
		m_mouseJoint = (b2MouseJoint*) m_world->CreateJoint(&md);
		std::cout << "test3" << std::endl;
		body->SetAwake(true);
	}
}

void Test::SpawnBomb(const b2Vec2& worldPt)
{
	m_bombSpawnPoint = worldPt;
	m_bombSpawning = true;
}

void Test::CompleteBombSpawn(const b2Vec2& p)
{
	if (m_bombSpawning == false)
	{
		return;
	}

	const float multiplier = 30.0f;
	b2Vec2 vel = m_bombSpawnPoint - p;
	vel *= multiplier;
	LaunchBomb(m_bombSpawnPoint, vel);
	m_bombSpawning = false;
}

void Test::ShiftMouseDown(const b2Vec2& p)
{
	m_mouseWorld = p;

	if (m_mouseJoint != NULL)
	{
		return;
	}

	SpawnBomb(p);
}

void Test::MouseUp(const b2Vec2& p)
{
	if (m_mouseJoint)
	{
		m_world->DestroyJoint(m_mouseJoint);
		m_mouseJoint = NULL;
	}

	if (m_bombSpawning)
	{
		CompleteBombSpawn(p);
	}
}

void Test::MouseMove(const b2Vec2& p)
{
	m_mouseWorld = p;

	if (m_mouseJoint)
	{
		m_mouseJoint->SetTarget(p);
	}
}

void Test::LaunchBomb()
{
	b2Vec2 p(RandomFloat(-15.0f, 15.0f), 30.0f);
	b2Vec2 v = -5.0f * p;
	LaunchBomb(p, v);
}

void Test::LaunchBomb(const b2Vec2& position, const b2Vec2& velocity)
{
	if (m_bomb)
	{
		m_world->DestroyBody(m_bomb);
		m_bomb = NULL;
	}

	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position = position;
	bd.bullet = true;
	m_bomb = m_world->CreateBody(&bd);
	m_bomb->SetLinearVelocity(velocity);

	b2CircleShape circle;
	circle.m_radius = 0.3f;

	b2FixtureDef fd;
	fd.shape = &circle;
	fd.density = 20.0f;
	fd.restitution = 0.0f;

	b2Vec2 minV = position - b2Vec2(0.3f, 0.3f);
	b2Vec2 maxV = position + b2Vec2(0.3f, 0.3f);

	b2AABB aabb;
	aabb.lowerBound = minV;
	aabb.upperBound = maxV;

	m_bomb->CreateFixture(&fd);
}

void Test::check_inner_map(b2Body *body, float radius)
{
	if(this->map_borders_fixture != nullptr && !this->dot_in_box(body->GetPosition(), b2Vec2(0,0), b2Vec2(this->map->width, this->map->height)))
	{
		b2Vec2 position = body->GetPosition();

		if(position.x < 0)
			position.x = radius;

		if(position.y < 0)
			position.y = radius;

		if(position.x > this->map->width)
			position.x = this->map->width - radius;

		if(position.y > this->map->height)
			position.y = this->map->height - radius;

		body->SetTransform(position, body->GetAngle());
	}
}

bool Test::dot_in_box(const b2Vec2 &dot, const b2Vec2 &left_bottom, const b2Vec2 &right_top)
{
	return dot.x > left_bottom.x && dot.x < right_top.x && dot.y > left_bottom.y && dot.y < right_top.y;
}

void Test::teleport(Player* player)
{
	if(player != nullptr && !player->was_teleport)
	{
		b2Vec2 buf = player->player_body->GetLinearVelocity();
		buf.Normalize();
		buf *= player->teleport_length;

		player->player_body->SetTransform(player->player_body->GetPosition() + buf, player->player_body->GetAngle());
		player->was_teleport = true;
	}
}

void Test::push_players(Player *player)
{
	if(player != nullptr && !player->was_push_players)
	{
		for(auto & player_iter: this->players)
		{
			if(player_iter != player)
			{
				b2Vec2 push_vector = player_iter->player_body->GetPosition() - player->player_body->GetPosition();

				if(push_vector.LengthSquared() <=  (player->push_players_radius + player_iter->player_fixture->GetShape()->m_radius) * (player->push_players_radius + player_iter->player_fixture->GetShape()->m_radius))
				{
					push_vector.Normalize();
					push_vector *=  player->push_players_power;
					player_iter->player_body->ApplyForceToCenter(push_vector, true);
				}

			}
		}

		player->was_push_players = true;
	}
}

void Test::Step(Settings* settings, double current_delay)
{
	double timeStep = current_delay/ 1000;

	uint32 flags = 0;
	flags += settings->drawShapes * b2Draw::e_shapeBit;
	flags += settings->drawJoints * b2Draw::e_jointBit;
	flags += settings->drawAABBs * b2Draw::e_aabbBit;
	flags += settings->drawCOMs * b2Draw::e_centerOfMassBit;
	m_debugDraw.SetFlags(flags);

	m_world->SetAllowSleeping(settings->enableSleep > 0);
	m_world->SetWarmStarting(settings->enableWarmStarting > 0);
	m_world->SetContinuousPhysics(settings->enableContinuous > 0);
	m_world->SetSubStepping(settings->enableSubStepping > 0);

	m_pointCount = 0;

	m_world->Step(timeStep, settings->velocityIterations, settings->positionIterations);

	m_world->DrawDebugData();

	if (timeStep > 0.0f)
	{
		++m_stepCount;
	}

	if (settings->drawStats)
	{
		int32 bodyCount = m_world->GetBodyCount();
		int32 contactCount = m_world->GetContactCount();
		int32 jointCount = m_world->GetJointCount();
		m_debugDraw.DrawString(5, m_textLine, "bodies/contacts/joints = %d/%d/%d", bodyCount, contactCount, jointCount);
		m_textLine += DRAW_STRING_NEW_LINE;

		int32 proxyCount = m_world->GetProxyCount();
		int32 height = m_world->GetTreeHeight();
		int32 balance = m_world->GetTreeBalance();
		float32 quality = m_world->GetTreeQuality();
		m_debugDraw.DrawString(5, m_textLine, "proxies/height/balance/quality = %d/%d/%d/%g", proxyCount, height, balance, quality);
		m_textLine += DRAW_STRING_NEW_LINE;
	}

	// Track maximum profile times
	{
		const b2Profile& p = m_world->GetProfile();
		m_maxProfile.step = b2Max(m_maxProfile.step, p.step);
		m_maxProfile.collide = b2Max(m_maxProfile.collide, p.collide);
		m_maxProfile.solve = b2Max(m_maxProfile.solve, p.solve);
		m_maxProfile.solveInit = b2Max(m_maxProfile.solveInit, p.solveInit);
		m_maxProfile.solveVelocity = b2Max(m_maxProfile.solveVelocity, p.solveVelocity);
		m_maxProfile.solvePosition = b2Max(m_maxProfile.solvePosition, p.solvePosition);
		m_maxProfile.solveTOI = b2Max(m_maxProfile.solveTOI, p.solveTOI);
		m_maxProfile.broadphase = b2Max(m_maxProfile.broadphase, p.broadphase);

		m_totalProfile.step += p.step;
		m_totalProfile.collide += p.collide;
		m_totalProfile.solve += p.solve;
		m_totalProfile.solveInit += p.solveInit;
		m_totalProfile.solveVelocity += p.solveVelocity;
		m_totalProfile.solvePosition += p.solvePosition;
		m_totalProfile.solveTOI += p.solveTOI;
		m_totalProfile.broadphase += p.broadphase;
	}

	if (settings->drawProfile)
	{
		const b2Profile& p = m_world->GetProfile();

		b2Profile aveProfile;
		memset(&aveProfile, 0, sizeof (b2Profile));
		if (m_stepCount > 0)
		{
			float32 scale = 1.0f / m_stepCount;
			aveProfile.step = scale * m_totalProfile.step;
			aveProfile.collide = scale * m_totalProfile.collide;
			aveProfile.solve = scale * m_totalProfile.solve;
			aveProfile.solveInit = scale * m_totalProfile.solveInit;
			aveProfile.solveVelocity = scale * m_totalProfile.solveVelocity;
			aveProfile.solvePosition = scale * m_totalProfile.solvePosition;
			aveProfile.solveTOI = scale * m_totalProfile.solveTOI;
			aveProfile.broadphase = scale * m_totalProfile.broadphase;
		}

		m_debugDraw.DrawString(5, m_textLine, "step [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.step, aveProfile.step, m_maxProfile.step);
		m_textLine += DRAW_STRING_NEW_LINE;
		m_debugDraw.DrawString(5, m_textLine, "collide [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.collide, aveProfile.collide, m_maxProfile.collide);
		m_textLine += DRAW_STRING_NEW_LINE;
		m_debugDraw.DrawString(5, m_textLine, "solve [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solve, aveProfile.solve, m_maxProfile.solve);
		m_textLine += DRAW_STRING_NEW_LINE;
		m_debugDraw.DrawString(5, m_textLine, "solve init [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solveInit, aveProfile.solveInit, m_maxProfile.solveInit);
		m_textLine += DRAW_STRING_NEW_LINE;
		m_debugDraw.DrawString(5, m_textLine, "solve velocity [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solveVelocity, aveProfile.solveVelocity, m_maxProfile.solveVelocity);
		m_textLine += DRAW_STRING_NEW_LINE;
		m_debugDraw.DrawString(5, m_textLine, "solve position [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solvePosition, aveProfile.solvePosition, m_maxProfile.solvePosition);
		m_textLine += DRAW_STRING_NEW_LINE;
		m_debugDraw.DrawString(5, m_textLine, "solveTOI [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solveTOI, aveProfile.solveTOI, m_maxProfile.solveTOI);
		m_textLine += DRAW_STRING_NEW_LINE;
		m_debugDraw.DrawString(5, m_textLine, "broad-phase [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.broadphase, aveProfile.broadphase, m_maxProfile.broadphase);
		m_textLine += DRAW_STRING_NEW_LINE;
	}

	if (m_mouseJoint)
	{
		b2Vec2 p1 = m_mouseJoint->GetAnchorB();
		b2Vec2 p2 = m_mouseJoint->GetTarget();

		b2Color c;
		c.Set(0.0f, 1.0f, 0.0f);
		m_debugDraw.DrawPoint(p1, 4.0f, c);
		m_debugDraw.DrawPoint(p2, 4.0f, c);

		c.Set(0.8f, 0.8f, 0.8f);
		m_debugDraw.DrawSegment(p1, p2, c);
	}

	if (m_bombSpawning)
	{
		b2Color c;
		c.Set(0.0f, 0.0f, 1.0f);
		m_debugDraw.DrawPoint(m_bombSpawnPoint, 4.0f, c);

		c.Set(0.8f, 0.8f, 0.8f);
		m_debugDraw.DrawSegment(m_mouseWorld, m_bombSpawnPoint, c);
	}

	if (settings->drawContactPoints)
	{
		const float32 k_impulseScale = 0.1f;
		const float32 k_axisScale = 0.3f;

		for (int32 i = 0; i < m_pointCount; ++i)
		{
			ContactPoint* point = m_points + i;

			if (point->state == b2_addState)
			{
				// Add
				m_debugDraw.DrawPoint(point->position, 10.0f, b2Color(0.3f, 0.95f, 0.3f));
			}
			else if (point->state == b2_persistState)
			{
				// Persist
				m_debugDraw.DrawPoint(point->position, 5.0f, b2Color(0.3f, 0.3f, 0.95f));
			}

			if (settings->drawContactNormals == 1)
			{
				b2Vec2 p1 = point->position;
				b2Vec2 p2 = p1 + k_axisScale * point->normal;
				m_debugDraw.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.9f));
			}
			else if (settings->drawContactImpulse == 1)
			{
				b2Vec2 p1 = point->position;
				b2Vec2 p2 = p1 + k_impulseScale * point->normalImpulse * point->normal;
				m_debugDraw.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
			}

			if (settings->drawFrictionImpulse == 1)
			{
				b2Vec2 tangent = b2Cross(point->normal, 1.0f);
				b2Vec2 p1 = point->position;
				b2Vec2 p2 = p1 + k_impulseScale * point->tangentImpulse * tangent;
				m_debugDraw.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
			}
		}
	}
}

void Test::kick(Player * player)
{
	if(player == nullptr)
		return;

	if(player->pressKey && !player->wasKick)
	{
		bool new_was_kick = false;
		for(auto iter : this->players)
			if(&*iter != player)
			{
				float kickDistance = iter->player_fixture->GetShape()->m_radius + player->leg_length + player->player_fixture->GetShape()->m_radius;
				if(b2Distance(iter->player_body->GetPosition(), player->player_body->GetPosition()) <= kickDistance)
				{
					this->kick(player->player_body, iter->player_body, player->players_kick_power);
					new_was_kick = true;
				}
			}

		for(auto ball : this->balls)
		{
			float kickDistance = ball.ball_fixture->GetShape()->m_radius + player->leg_length + player->player_fixture->GetShape()->m_radius;
			if(b2Distance(ball.body->GetPosition(), player->player_body->GetPosition()) <= kickDistance)
			{
				this->kick(player->player_body, ball.body, player->ball_kick_power);
				new_was_kick = true;
			}
		}

		player->wasKick = new_was_kick;
	}
}

void Test::threshold(b2Body* body, float limit)
{
	if(body->GetLinearVelocity().LengthSquared() <= limit*limit)
		body->SetLinearVelocity(b2Vec2(0,0));
}

void Test::kick(b2Body * kicker, b2Body *kickable, float kick_power)
{
	b2Vec2 forceVec2;
	forceVec2 = kickable->GetPosition() - kicker->GetPosition();
	forceVec2.Normalize();
	forceVec2 *= kick_power;
	kickable->ApplyForceToCenter(forceVec2, true);

	std::cout << "Result power " << forceVec2.x << " " << forceVec2.y << std::endl;
}

void Test::Move(Player * player, float32 x, float32 y, Settings * settings, bool nitroFlag, double delay)
{
	if (player != nullptr && (x != 0 || y != 0))
	{
		if(!settings->isMoveLinearDamping)
		{
			player->player_body->SetLinearDamping(0);
		}

		if (settings->forceToMove)
		{
			/*
			float move_impulse_force_buf = player->move_impulse_force;

			if(settings->useSpeedLimit)
			{
				float max_force = player->max_speed * player->player_body->GetMass() / (2.0 / settings->hz);

				move_impulse_force_buf = std::min<float>(max_force, move_impulse_force_buf);
			}

			player->player_body->ApplyForceToCenter(b2Vec2(x * move_impulse_force_buf, y * move_impulse_force_buf), true);
			 */
		}
		else
		{
			float move_impulse_force = player->move_impulse_force;

			if(nitroFlag)
				move_impulse_force += player->nitro_additional_move_impulse_force;

			if(player->pressKey && !player->wasKick)
			{
				move_impulse_force *= player->move_kick_impulse_modifier;
			}

			move_impulse_force *= (delay / 1000);
			player->player_body->ApplyLinearImpulse(b2Vec2(x * move_impulse_force, y * move_impulse_force), player->player_body->GetPosition(), true);
		}

		if(settings->useSpeedLimit && player->player_body->GetLinearVelocity().LengthSquared() > player->max_speed * player->max_speed)
		{
			float buf_max_speed = player->max_speed;
			b2Vec2 buf = player->player_body->GetLinearVelocity();
			buf.Normalize();

			if(nitroFlag)
				buf_max_speed += player->nitro_additional_max_speed;

			if(player->pressKey && !player->wasKick)
				buf_max_speed *= player->move_kick_impulse_modifier;

			buf *= buf_max_speed;

			player->player_body->SetLinearVelocity(buf);
		}
	}

	if(player != nullptr && (x == 0) && y == 0)
	{
		player->player_body->SetLinearDamping(player->linearDumping);
	}
}

void Test::ShiftOrigin(const b2Vec2& newOrigin)
{
	m_world->ShiftOrigin(newOrigin);
}
