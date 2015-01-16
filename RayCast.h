#pragma once

#include "Map.h"
#include <list>
#include <iostream>

#include "BallBody.h"
#include "Test.h"
#include "main.h"

class RayCast : public Test
{
public:
	b2BodyDef pickable_definition;
	b2BodyDef map_definition;
	b2BodyDef player_definition;
        b2BodyDef ball_definition;
	b2BodyDef bullet_definition;

        b2Filter player_ball_collide_filter;
	b2Filter player_collide_filter;
        b2Filter ball_collide_filter;
	b2Filter player_filter;
	b2Filter pickable_filter;
        b2Filter ball_filter;
	bool* keyStates = new bool[256];
	bool wasKick;

	virtual void MouseDownCallback()
	{
		for(int i =0; i<256; i++)
			keyStates[i] = false;
	}

	void KeyboardUp (unsigned char key) {
		keyStates[key] = false; // Set the state of the current key to not pressed
	}

	enum Category : uint16_t
	{
		CATEGORY_PLAYER_COLLIDE	= 0x0001,
                CATEGORY_BALL_COLLIDE   = 0x0002,
		CATEGORY_PLAYER		= 0x0004,
                CATEGORY_BALL           = 0x0008,
		CATEGORY_PICKABLE	= 0x00016
	};

	RayCast(Map *map)
	{
		this->map = map;
		std::cout << "End creating objects" << std::endl;

		pickable_definition.active = true;
		pickable_definition.allowSleep = true;
		pickable_definition.angle = 0;
		pickable_definition.angularDamping = 0;
		pickable_definition.angularVelocity = 0;
		pickable_definition.awake = true;
		pickable_definition.bullet = false;
		pickable_definition.fixedRotation = true;
		pickable_definition.gravityScale = 1;
		pickable_definition.linearDamping = 0;
		pickable_definition.linearVelocity.Set(0, 0);
		pickable_definition.position.Set(0, 0);
		pickable_definition.type = b2_staticBody;
		pickable_definition.userData = nullptr;

		map_definition.active = true;
		map_definition.allowSleep = true; //разрешить не участвовать в просчете если нет аабб
		map_definition.angle = 0;//начальный угол
		map_definition.angularDamping = 0;//затухание угла
		map_definition.angularVelocity = 0;//затухание скорости
		map_definition.awake = true;//
		map_definition.bullet = false;//ccd
		map_definition.fixedRotation = true;//не вращать
		map_definition.gravityScale = 1;
		map_definition.linearDamping = 0;
		map_definition.linearVelocity.Set(0, 0);
		map_definition.position.Set(0, 0);
		map_definition.type = b2_staticBody;
		map_definition.userData = nullptr;

		player_definition.active = true;
		player_definition.allowSleep = true;
		player_definition.angle = 0;
		player_definition.angularDamping = 0;
		player_definition.angularVelocity = 0;
		player_definition.awake = true;
		player_definition.bullet = false;
		player_definition.fixedRotation = false;
		player_definition.gravityScale = 1;
		player_definition.linearDamping = 3;
		player_definition.linearVelocity.Set(0, 0);
		player_definition.position.Set(0, 0);
		player_definition.type = b2_dynamicBody;
		player_definition.userData = nullptr;

		ball_definition.active = true;
		ball_definition.allowSleep = false;
		ball_definition.angle = 0;
		ball_definition.angularDamping = 0;
		ball_definition.angularVelocity = 0;
		ball_definition.awake = true;
		ball_definition.bullet = false;
		ball_definition.fixedRotation = false;
		ball_definition.gravityScale = 1;
		ball_definition.linearDamping = 1;
		ball_definition.linearVelocity.Set(0, 0);
		ball_definition.position.Set(-10, -10);
		ball_definition.type = b2_dynamicBody;
		ball_definition.userData = nullptr;

		player_ball_collide_filter.categoryBits = CATEGORY_BALL_COLLIDE | CATEGORY_PLAYER_COLLIDE;
		player_ball_collide_filter.groupIndex = 0;
		player_ball_collide_filter.maskBits = CATEGORY_BALL | CATEGORY_PLAYER;

		player_collide_filter.categoryBits = CATEGORY_PLAYER_COLLIDE;
		player_collide_filter.groupIndex = 0;
		player_collide_filter.maskBits = CATEGORY_PLAYER;

		ball_collide_filter.categoryBits = CATEGORY_BALL_COLLIDE;
		ball_collide_filter.groupIndex = 0;
		ball_collide_filter.maskBits = CATEGORY_BALL;

		player_filter.categoryBits = CATEGORY_PLAYER;
		player_filter.groupIndex = 0;
		player_filter.maskBits = CATEGORY_PLAYER_COLLIDE | CATEGORY_PLAYER | CATEGORY_BALL;

		ball_filter.categoryBits = CATEGORY_BALL;
		ball_filter.groupIndex = 0;
		ball_filter.maskBits = CATEGORY_BALL | CATEGORY_BALL_COLLIDE | CATEGORY_PLAYER;

		pickable_filter.categoryBits = CATEGORY_PICKABLE;
		pickable_filter.groupIndex = 0;
		pickable_filter.maskBits = CATEGORY_PLAYER;

		b2Body *map_body = this->m_world->CreateBody(&map_definition);
		m_groundBody = map_body;

		create_map_borders(*map, map_body);

		for (auto iter = map->shapes.begin(); iter != map->shapes.end(); ++iter)
		{
		    b2Fixture * fixture = map_body->CreateFixture(iter->shape, 0);

		    if(iter->player_collide && iter->ball_collide)
			fixture->SetFilterData(player_ball_collide_filter);
		    else if(iter->player_collide)
			fixture->SetFilterData(player_filter);
		    else if(iter->ball_collide)
			fixture->SetFilterData(ball_collide_filter);
		}

		for (auto iter = map->gates.begin(); iter != map->gates.end(); ++iter)
		{
		    b2Fixture * fixture = map_body->CreateFixture(iter->shape, 0);

		    fixture->SetFilterData(ball_collide_filter);
		    fixture->SetSensor(true);
		}

		uint8_t balls_counter = 0;
		for(auto iter = map->balls.begin(); iter != map->balls.end(); ++iter)
		{
			this->balls.emplace_back(m_world, (*iter), ball_definition, ball_filter, balls_counter++);
		}

		for(auto i = 0; i<3; i++)
		{
			if(!map->respawn_points[i].empty())
				for(auto iter = map->respawn_points[i].begin(); iter != map->respawn_points[i].end(); ++iter)
				{
					b2Body * player_body = m_world->CreateBody(&player_definition);
					player_body->SetTransform(b2Vec2(iter->x, iter->y), 0);
					b2CircleShape *circle = new b2CircleShape();
					circle->m_p.x = 0;
					circle->m_p.y = 0;
					circle->m_radius = 1;

					b2Fixture * fixture = player_body->CreateFixture(circle, 1);
					fixture->SetFilterData(player_filter);
					Player *new_player = new Player();
					new_player->player_body = player_body;
					new_player->player_fixture = fixture;
					fixture->SetUserData(new_player);
					player_body->SetUserData(new_player);

					b2MassData buf;
					player_body->GetMassData(&buf);
					buf.mass = 1;
					player_body->SetMassData(&buf);

					new_player->linearDumping = player_body->GetLinearDamping();
					this->players.push_back(new_player);
				}
		}
	}

	void create_map_borders(const Map &map, b2Body* map_body)
	{
		b2ChainShape chain;
		b2Vec2 vs[4];

		int32_t map_width = map.width;
		int32_t map_height = map.height;

		vs[0].Set(0, 0);
		vs[1].Set(map_width, 0);
		vs[2].Set(map_width, map_height);
		vs[3].Set(0, map_height);

		chain.CreateLoop(vs, 4);

		this->map_borders_fixture = map_body->CreateFixture(&chain, 0);
		this->map_borders_fixture->SetFilterData(player_ball_collide_filter);
	}

	void DestroyBody()
	{

	}

	void Keyboard(unsigned char key)
	{
		keyStates[key] = true; // Set the state of the current key to pressed
	}

	void kick()
	{
		if(keyStates[' '])
		{
			test->current_player->pressKey = true;
		}
		else
		{
			test->current_player->pressKey = false;
			test->current_player->wasKick = false;
		}

		if(test->current_player->pressKey && !test->current_player->wasKick)
		{
			b2CircleShape *player_shape = static_cast<b2CircleShape*>(this->current_player->player_fixture->GetShape());
			b2Color kick_player_color;
			kick_player_color.b = 0;
			kick_player_color.r = 1;
			kick_player_color.g = 0.3;
			this->m_debugDraw.DrawCircle(this->current_player->player_body->GetPosition(), player_shape->m_radius + this->current_player->leg_length,kick_player_color);

		}

		Test::kick(test->current_player);
	}

	void Step(Settings* settings, double delay)
	{
		Test::Step(settings, delay);

		if(this->current_player != nullptr)
		{
			this->move(settings, delay);
			this->kick();
			this->teleport();
			b2Color current_player_color;
			current_player_color.b = 1;
			current_player_color.r = 1;
			current_player_color.g = 1;
			this->m_debugDraw.DrawCircle(this->current_player->player_body->GetPosition(), 0.5, current_player_color);

			ShowCurrentSpeed(this->current_player->player_body->GetLinearVelocity().Length());
			settings->viewCenter = this->current_player->player_body->GetPosition();
		}

		if(this->current_ball != nullptr)
		{
			b2Color current_ball_color;
			current_ball_color.b = 0.5;
			current_ball_color.r = 1;
			current_ball_color.g = 0.5;
			this->m_debugDraw.DrawCircle(this->current_ball->body->GetPosition(), 0.3, current_ball_color);

			if(this->current_player == nullptr)
			{
				ShowCurrentSpeed(this->current_ball->body->GetLinearVelocity().Length());
				settings->viewCenter = this->current_ball->body->GetPosition();
			}
		}

		for(auto player : this->players)
			this->threshold(player->player_body, settings->playerThreshold);

		for(auto ball : this->balls)
			this->threshold(ball.body, settings->ballThreshold);

		if(this->current_player)
			this->check_inner_map(this->current_player->player_body, this->current_player->player_fixture->GetShape()->m_radius);


	}

	void teleport()
	{
		if(keyStates['1'])
		{
			Test::teleport(this->current_player);
		}
		else if(this->current_player)
			this->current_player->was_teleport = false;
	}

	void move(Settings *settings, double delay)
	{
		b2Vec2 direction(0,0);
		bool nitroKey = false;

		if(keyStates['w'] || keyStates['W'] || keyStates[72])
		{
			direction.y += 1;
		}

		if(keyStates['s'] || keyStates['S'] || keyStates[80])
		{
			direction.y -= 1;
		}
		if(keyStates['a'] || keyStates['A'] || keyStates[75])
		{
			direction.x -= 1;
		}
		if(keyStates['d'] || keyStates['D'] || keyStates[77])
		{
			direction.x += 1;
		}

		if(keyStates['<'] || keyStates['>'] || keyStates[','] || keyStates['.'])
			nitroKey = true;

		direction.Normalize();

		Test::Move(test->current_player, direction.x, direction.y, settings, nitroKey, delay);
	}

};