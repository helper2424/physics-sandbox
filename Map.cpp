#include "Map.h"

#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>

#include <cstdint>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

uint8_t Client::TEAM_MAX = 3;

Map::Map(const char *data, size_t size)
{
	std::stringstream ss;
	ss << data;
	boost::property_tree::json_parser::read_json(ss, this->pt);

	this->width = this->pt.get<uint32_t>("width");
	this->height = this->pt.get<uint32_t>("height");
	this->original_data = strndup(data, size);
	this->original_data_size = size;

	this->load_balls();
	this->load_gates();
	this->load_lines();
	this->load_points();
}

void Map::load_lines()
{
	boost::property_tree::ptree lines = this->pt.get_child("lines");
	for (auto &line : lines)
	{
		bool is_player_collide = line.second.get<bool>("player_collision");
		bool is_ball_collide = line.second.get<bool>("ball_collision");

		float r1 = line.second.get<float>("r1");
		float r2 = line.second.get<float>("r2");
		float x1 = line.second.get<float>("x1");
		float x2 = line.second.get<float>("x2");
		float y1 = line.second.get<float>("y1");
		float y2 = line.second.get<float>("y2");

		b2EdgeShape *lineShape = new b2EdgeShape();
		lineShape->Set(b2Vec2(x1, y1), b2Vec2(x2, y2));

		this->shapes.emplace_back(is_player_collide, is_ball_collide, lineShape);

		if (r1 > 0)
		{
			b2CircleShape *circle1 = new b2CircleShape();
			circle1->m_p.Set(x1, y1);
			circle1->m_radius = r1;

			this->shapes.emplace_back(is_player_collide, is_ball_collide, circle1);
		}

		if (r2 > 0)
		{
			b2CircleShape *circle2 = new b2CircleShape();
			circle2->m_p.Set(x2, y2);
			circle2->m_radius = r2;

			this->shapes.emplace_back(is_player_collide, is_ball_collide, circle2);
		}
	}
	if (lines.empty())
		std::cout << "Undefined any lines";
}

void Map::load_balls()
{
	boost::property_tree::ptree balls = this->pt.get_child("balls");
	for (auto &ball : balls)
	{
		float x = ball.second.get<float>("x");
		float y = ball.second.get<float>("y");
		float bounce = ball.second.get<float>("bounce");
		float mass = ball.second.get<float>("mass");
		float radius = ball.second.get<float>("radius");
		float friction = ball.second.get<float>("friction");

		b2CircleShape *circle = new b2CircleShape();
		circle->m_p.Set(0,0);
		circle->m_radius = radius;

		this->balls.emplace_back(bounce, friction, mass, circle, x, y);
	}

	if (this->balls.empty())
		std::cout << "Undefined any balls";
}

void Map::load_points()
{
	boost::property_tree::ptree points = this->pt.get_child("start_positions");
	for (auto &point : points)
	{
		uint8_t team = point.second.get<uint8_t>("team");
		float x = point.second.get<float>("x");
		float y = point.second.get<float>("y");

		if (team >= Client::TEAM_MAX)
		{
			std::cout << "Invalid team id";
			continue;
		}

		this->respawn_points[team].emplace_back(x, y);
	}
}

void Map::load_gates()
{
	boost::property_tree::ptree gates = this->pt.get_child("gates");
	for (auto &gate : gates)
	{
		uint32_t team = gate.second.get<uint32_t>("team");

		if (team >= Client::TEAM_MAX)
		{
			std::cout << "Invalid team id in gates";
			continue;
		}

		float r1 = gate.second.get<float>("r1");
		float r2 = gate.second.get<float>("r2");
		float x1 = gate.second.get<float>("x1");
		float x2 = gate.second.get<float>("x2");
		float y1 = gate.second.get<float>("y1");
		float y2 = gate.second.get<float>("y2");

		b2EdgeShape *lineShape = new b2EdgeShape();
		lineShape->Set(b2Vec2(x1, y1), b2Vec2(x2, y2));

		this->gates.emplace_back(team, lineShape);

		if (r1 > 0)
		{
			b2CircleShape *circle1 = new b2CircleShape();
			circle1->m_p.Set(x1, y1);
			circle1->m_radius = r1;
			this->shapes.emplace_back(true, true, circle1);
		}

		if (r2 > 0)
		{
			b2CircleShape *circle2 = new b2CircleShape();
			circle2->m_p.Set(x2, y2);
			circle2->m_radius = r2;

			this->shapes.emplace_back(true, true, circle2);
		}
	}

	if (this->gates.empty())
		std::cout << "Undefined any gates";
}

Map::~Map()
{
	free(this->original_data);
}