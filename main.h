#pragma once

#include "glui/glui.h"
#include "Map.h"
#include <fstream>
#include <iostream>
#include <signal.h>
#include <stdlib.h>

#include <stdio.h>
#include "Test.h"
#include "BallBody.h"


namespace {
	int32 testIndex;
	int32 testSelection;
	int32 testCount;
	Test* test;
	Settings settings;
	int32 width;
	int32 height;
	int32 framePeriod;
	int32 mainWindow;
	float settingsHz;
	GLUI *glui;
	float32 viewZoom;
	int tx, ty, tw, th;
	bool rMouseDown;
	b2Vec2 lastp;
	Map *map;
	float UnitToPixel;

	GLUI_Rollout *player_panel;
	GLUI_Spinner* player_radius;
	GLUI_Spinner* player_mass;
	GLUI_Spinner* player_move_force;
	GLUI_Spinner* player_liner_dumping;
	GLUI_Spinner* player_player_kick_power;
	GLUI_Spinner* player_ball_kick_power;
	GLUI_Spinner* player_bounce;
	GLUI_Spinner* player_max_speed;
	GLUI_Spinner* player_leg_length;
	GLUI_Spinner* player_threshold;
	GLUI_Spinner* player_move_kick_impulse_modifier;
	GLUI_Spinner* player_nitro_additional_move_force;
	GLUI_Spinner* player_nitro_additional_max_speed;

	GLUI_Rollout *ball_panel;
	GLUI_Spinner* ball_radius;
	GLUI_Spinner* ball_mass;
	GLUI_Spinner* ball_liner_dumping;
	GLUI_Spinner* ball_angular_dumping;
	GLUI_Spinner* ball_bounce;
	GLUI_Spinner* ball_threshold;

	GLUI_EditText *current_player_ball_speed;

	float last_show_speed = 0;
}

void ShowPlayerPanel(Player *);

void ShowBallPanel(BallBody *);

void ShowCurrentSpeed(float);