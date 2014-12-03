#pragma once

#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>

#include <cstdint>
#include <vector>
#include <boost/property_tree/ptree.hpp>

namespace Client{
        extern uint8_t TEAM_MAX;
}

struct Shape
{
	b2Shape *shape;
	Shape(b2Shape *shape)
	{
		this->shape = shape;
	}
};

struct MapShape : public Shape
{
	bool player_collide = false;
	bool ball_collide = false;
	MapShape(bool player_collide, bool ball_collide, b2Shape *shape) :
		Shape(shape), player_collide(player_collide), ball_collide(ball_collide)
	{};
};

struct MapGate : public Shape
{
	uint32_t team;
	MapGate(uint32_t team, b2Shape *shape) :
		Shape(shape), team(team)
	{};
};

struct MapBall : public Shape
{
	float bounce;
	float friction;
	float mass;
	float x;
	float y;
	MapBall(float bounce, float friction, float mass, b2CircleShape *shape, float x, float y) :
		Shape(shape), bounce(bounce), friction(friction), mass(mass), x(x), y(y)
	{};
};

class Map
{
	friend class RayCast;
public:
	typedef std::vector<b2Vec2> points_t;
	typedef std::vector<MapShape> shapes_t;
	typedef std::vector<MapGate> gates_t;
	typedef std::vector<MapBall> balls_t;

private:
	uint8_t id;

	char *original_data;
	size_t original_data_size;

	boost::property_tree::ptree pt;

	void load_lines();
	void load_gates();
	void load_balls();
	void load_points();

public:
	uint32_t width;
	uint32_t height;
	points_t respawn_points[3];
	shapes_t shapes;
	gates_t gates;
	balls_t balls;
	
	Map(const char *data, size_t size);
	~Map();
};