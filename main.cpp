/*
* Copyright (c) 2006-2007 Erin Catto http://www.box2d.org
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

#include "main.h"

#include <fstream>
#include <iostream>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include "Raycast.h"

#include <stdio.h>

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 100);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

static void Resize(int32 w, int32 h)
{
        if(map != nullptr)
        {
                width = w;
                height = h;

                GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
                glViewport(tx, ty, tw, th);

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                float32 ratio = float32(tw) / float32(th);

                b2Vec2 extents(ratio * UnitToPixel, UnitToPixel);
                extents *= viewZoom;

                b2Vec2 lower = settings.viewCenter - extents;
                b2Vec2 upper = settings.viewCenter + extents;

                float32 convertCoeff = viewZoom * UnitToPixel;
                // L/R/B/T
                gluOrtho2D(-2, tw/convertCoeff -2, (map->height + 2) - (th/convertCoeff), (map->height + 2));
        }
}

static b2Vec2 ConvertScreenToWorld(int32 x, int32 y)
{
        float32 convertCoeff = viewZoom * UnitToPixel;
        
	b2Vec2 p;
	p.x = x/convertCoeff -2;
	p.y = map->height + 2 - y/convertCoeff;
        
	return p;
}

// This is used to control the frame rate (60Hz).
static void Timer(int)
{
	glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(framePeriod, Timer, 0);
}

static void SimulationLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	b2Vec2 oldCenter = settings.viewCenter;
	settings.hz = settingsHz;
	test->Step(&settings);
	if (oldCenter.x != settings.viewCenter.x || oldCenter.y != settings.viewCenter.y)
	{
		Resize(width, height);
	}

	glutSwapBuffers();
}

static void load_map(const char * path)
{
        std::ifstream file(path);
        
        if(!file)
        {
                std::cout << "Undefined map data in " << path << "\n";
                abort();
        }
        
        file.seekg (0, file.end);
        int length = file.tellg();
        file.seekg (0, file.beg);
        
        char * buffer = new char [length];

        std::cout << "Reading " << length << " characters... " << std::endl;
        // read data as a block:
        file.read (buffer,length);

        if (file)
                std::cout << "all characters read successfully." << std::endl;
        else
                std::cout << "error: only " << file.gcount() << " could be read";
        file.close();

        map = new Map(buffer, length);

        delete[] buffer;
}

static void Keyboard(unsigned char key, int x, int y)
{
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	switch (key)
	{
	case 27:
#ifndef __APPLE__
		// freeglut specific function
		glutLeaveMainLoop();
#endif
		exit(0);
		break;

		// Press 'z' to zoom out.
	case 'z':
		viewZoom = b2Min(1.1f * viewZoom, 20.0f);
		Resize(width, height);
		break;

		// Press 'x' to zoom in.
	case 'x':
		viewZoom = b2Max(0.9f * viewZoom, 0.02f);
		Resize(width, height);
		break;

		// Press space to launch a bomb.
 
	case 'p':
		settings.pause = !settings.pause;
		break;

		// Press [ to prev test.
                        
	default:
		if (test)
		{
			test->Keyboard(key);
		}
	}
}

static void KeyboardSpecial(int key, int x, int y)
{
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	int mod = glutGetModifiers();

	switch (key)
	{
		// Press left to pan left.
	case GLUT_KEY_LEFT:
		if (mod == GLUT_ACTIVE_CTRL)
		{
			b2Vec2 newOrigin(2.0f, 0.0f);
			test->ShiftOrigin(newOrigin);
		}
		else
		{
			settings.viewCenter.x -= 0.5f;
			Resize(width, height);
		}
		break;

		// Press right to pan right.
	case GLUT_KEY_RIGHT:
		if (mod == GLUT_ACTIVE_CTRL)
		{
			b2Vec2 newOrigin(-2.0f, 0.0f);
			test->ShiftOrigin(newOrigin);
		}
		else
		{
			settings.viewCenter.x += 0.5f;
			Resize(width, height);
		}
		break;

		// Press down to pan down.
	case GLUT_KEY_DOWN:
		if (mod == GLUT_ACTIVE_CTRL)
		{
			b2Vec2 newOrigin(0.0f, 2.0f);
			test->ShiftOrigin(newOrigin);
		}
		else
		{
			settings.viewCenter.y -= 0.5f;
			Resize(width, height);
		}
		break;

		// Press up to pan up.
	case GLUT_KEY_UP:
		if (mod == GLUT_ACTIVE_CTRL)
		{
			b2Vec2 newOrigin(0.0f, -2.0f);
			test->ShiftOrigin(newOrigin);
		}
		else
		{
			settings.viewCenter.y += 0.5f;
			Resize(width, height);
		}
		break;

		// Press home to reset the view.
	case GLUT_KEY_HOME:
		viewZoom = 1.0f;
		settings.viewCenter.Set(0.0f, 20.0f);
		Resize(width, height);
		break;
	}
}

static void KeyboardUp(unsigned char key, int x, int y)
{
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	if (test)
	{
		test->KeyboardUp(key);
	}
}

static void Mouse(int32 button, int32 state, int32 x, int32 y)
{
        std::cout << ConvertScreenToWorld(x, y).x << " " << ConvertScreenToWorld(x,y).y << std::endl;
	// Use the mouse to move things around.
	if (button == GLUT_LEFT_BUTTON)
	{
		int mod = glutGetModifiers();
		b2Vec2 p = ConvertScreenToWorld(x, y);
		if (state == GLUT_DOWN)
		{
			b2Vec2 p = ConvertScreenToWorld(x, y);
			if (mod == GLUT_ACTIVE_SHIFT)
			{
				test->ShiftMouseDown(p);
			}
			else
			{
				test->MouseDown(p);
			}
		}
		
		if (state == GLUT_UP)
		{
			test->MouseUp(p);
		}
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{	
			lastp = ConvertScreenToWorld(x, y);
			rMouseDown = true;
		}

		if (state == GLUT_UP)
		{
			rMouseDown = false;
		}
	}
}

void HidePanels()
{
        ball_panel->disable();
        player_panel->disable();
}

void ShowPlayerPanel(Player *player)
{
        HidePanels();
        
        player_radius->set_float_val(test->current_player->player_fixture->GetShape()->m_radius);
        player_mass->set_float_val(test->current_player->player_body->GetMass());
        player_move_force->set_float_val(test->current_player->move_impulse_force);
        player_liner_dumping->set_float_val(test->current_player->player_body->GetLinearDamping());
        player_player_kick_power->set_float_val(test->current_player->players_kick_power);
        player_bounce->set_float_val(test->current_player->player_fixture->GetRestitution());
        player_max_speed->set_float_val(test->current_player->max_speed);
        
        player_panel->enable();
}

void ShowBallPanel(BallBody* ball)
{
        HidePanels();
        
        ball_radius->set_float_val(test->current_ball->ball_fixture->GetShape()->m_radius);
	ball_liner_dumping->set_float_val(test->current_ball->body->GetLinearDamping());
	ball_angular_dumping->set_float_val(test->current_ball->body->GetAngularDamping());
        ball_bounce->set_float_val(test->current_ball->ball_fixture->GetRestitution());
        ball_mass->set_float_val(test->current_ball->body->GetMass());

        ball_panel->enable();
}

static void UpdatePlayer(int)
{
        if(test->current_player != nullptr)
        {
                test->current_player->player_fixture->GetShape()->m_radius = player_radius->get_float_val();
                test->current_player->move_impulse_force = player_move_force->get_float_val();
                test->current_player->player_body->SetLinearDamping(player_liner_dumping->get_float_val());
                test->current_player->players_kick_power = player_player_kick_power->get_float_val();
                
                b2MassData buf;
                test->current_player->player_body->GetMassData(&buf);
                buf.mass = player_mass->get_float_val();
                test->current_player->player_body->SetMassData(&buf);
                
                test->current_player->player_fixture->SetRestitution(player_bounce->get_float_val());
                test->current_player->max_speed = player_max_speed->get_float_val();
        }
}

static void UpdateBall(int)
{
        if(test->current_ball != nullptr)
        {
                test->current_ball->ball_fixture->GetShape()->m_radius = ball_radius->get_float_val();
                test->current_ball->body->SetLinearDamping(ball_liner_dumping->get_float_val());
                test->current_ball->body->SetAngularDamping(ball_angular_dumping->get_float_val());
                test->current_ball->ball_fixture->SetRestitution(ball_bounce->get_float_val());
               
                b2MassData buf;
                test->current_ball->body->GetMassData(&buf);
                buf.mass = ball_mass->get_float_val();
                test->current_ball->body->SetMassData(&buf);
        }
}

static void MouseMotion(int32 x, int32 y)
{
	b2Vec2 p = ConvertScreenToWorld(x, y);
	test->MouseMove(p);
	
	if (rMouseDown)
	{
		b2Vec2 diff = p - lastp;
		settings.viewCenter.x -= diff.x;
		settings.viewCenter.y -= diff.y;
		Resize(width, height);
		lastp = ConvertScreenToWorld(x, y);
	}
}

#ifdef FREEGLUT
static void MouseWheel(int wheel, int direction, int x, int y)
{
	B2_NOT_USED(wheel);
	B2_NOT_USED(x);
	B2_NOT_USED(y);
	if (direction > 0)
	{
		viewZoom /= 1.1f;
	}
	else
	{
		viewZoom *= 1.1f;
	}
	Resize(width, height);
}
#endif

static void Restart(int)
{
	delete test;
	test = new RayCast(map);
        Resize(width, height);
}

static void Pause(int)
{
	settings.pause = !settings.pause;
}

static void Exit(int code)
{
        if(map != nullptr)
                delete map;
        
        if(test != nullptr)
                delete test;
        
        std::cout << "Graceful end" << std::endl;
        
	// TODO: freeglut is not building on OSX
#ifdef FREEGLUT
	glutLeaveMainLoop();
#endif
	exit(code);
}

static void SingleStep(int)
{
	settings.pause = 1;
	settings.singleStep = 1;
}

void add_to_panel(GLUI_Panel* panel, GLUI_Spinner** spinner, const char * name, float& val, float min_limit = 0.1, float max_limit = 1000, float speed = 0.01)
{
        *spinner = glui->add_spinner_to_panel(panel, name, GLUI_SPINNER_FLOAT);
        (*spinner)->set_float_limits(min_limit, max_limit);
        (*spinner)->set_speed(speed); 
}

int main(int argc, char** argv)
{
        testIndex = 0;
        testSelection = 0;
        testCount = 0;
        width = 800;
        height = 600;
        framePeriod = 16;
        settingsHz = 60.0;
        viewZoom = 1.0f;
        UnitToPixel = 20.0f;

        signal(SIGSEGV, handler);  
        load_map("./map.json");
	test = new RayCast(map);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	char title[32];
	sprintf(title, "Megaball physics sandbox");
	mainWindow = glutCreateWindow(title);
	//glutSetOption (GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutDisplayFunc(SimulationLoop);
	GLUI_Master.set_glutReshapeFunc(Resize);  
	GLUI_Master.set_glutKeyboardFunc(Keyboard);
	GLUI_Master.set_glutSpecialFunc(KeyboardSpecial);
	GLUI_Master.set_glutMouseFunc(Mouse);
#ifdef FREEGLUT
	glutMouseWheelFunc(MouseWheel);
#endif
	glutMotionFunc(MouseMotion);

	glutKeyboardUpFunc(KeyboardUp);

	glui = GLUI_Master.create_glui_subwindow( mainWindow, 
		GLUI_SUBWINDOW_RIGHT );

	GLUI_Spinner* velocityIterationSpinner =
		glui->add_spinner("Vel Iters", GLUI_SPINNER_INT, &settings.velocityIterations);
	velocityIterationSpinner->set_int_limits(1, 500);

	GLUI_Spinner* positionIterationSpinner =
		glui->add_spinner("Pos Iters", GLUI_SPINNER_INT, &settings.positionIterations);
	positionIterationSpinner->set_int_limits(0, 100);

	GLUI_Spinner* hertzSpinner =
		glui->add_spinner("Hertz", GLUI_SPINNER_FLOAT, &settingsHz);

	hertzSpinner->set_float_limits(5.0f, 200.0f);

	glui->add_checkbox("Sleep", &settings.enableSleep);
	glui->add_checkbox("Warm Starting", &settings.enableWarmStarting);
	glui->add_checkbox("Time of Impact", &settings.enableContinuous);
	glui->add_checkbox("Sub-Stepping", &settings.enableSubStepping);

	//glui->add_separator();

	GLUI_Panel* drawPanel =	glui->add_panel("Draw");
	glui->add_checkbox_to_panel(drawPanel, "Shapes", &settings.drawShapes);
	glui->add_checkbox_to_panel(drawPanel, "Joints", &settings.drawJoints);
	glui->add_checkbox_to_panel(drawPanel, "AABBs", &settings.drawAABBs);
	glui->add_checkbox_to_panel(drawPanel, "Contact Points", &settings.drawContactPoints);
        glui->add_checkbox_to_panel(drawPanel, "Moving with force", &settings.forceToMove);
        glui->add_checkbox_to_panel(drawPanel, "Limit velocity", &settings.useSpeedLimit);
	//glui->add_checkbox_to_panel(drawPanel, "Contact Normals", &settings.drawContactNormals);
	//glui->add_checkbox_to_panel(drawPanel, "Contact Impulses", &settings.drawContactImpulse);
	//glui->add_checkbox_to_panel(drawPanel, "Friction Impulses", &settings.drawFrictionImpulse);
	//glui->add_checkbox_to_panel(drawPanel, "Center of Masses", &settings.drawCOMs);
	//glui->add_checkbox_to_panel(drawPanel, "Statistics", &settings.drawStats);
	//glui->add_checkbox_to_panel(drawPanel, "Profile", &settings.drawProfile);

	glui->add_button("Pause", 0, Pause);
	glui->add_button("Single Step", 0, SingleStep);
	glui->add_button("Restart", 0, Restart);
                
        //Player panel
        player_panel = glui->add_panel("Player", GLUI_PANEL_EMBOSSED);
        float val = 20;
        add_to_panel(player_panel, &player_radius, "radius", val);
        add_to_panel(player_panel, &player_mass, "mass", val);
        add_to_panel(player_panel, &player_move_force, "move impulse force", val, 1, 100000, 1);
        add_to_panel(player_panel, &player_liner_dumping, "velocity dumping", val);
        add_to_panel(player_panel, &player_player_kick_power, "player kick power", val);
        add_to_panel(player_panel, &player_bounce,  "bounce", val);
        add_to_panel(player_panel, &player_max_speed, "max velocity", val);
        
        glui->add_button_to_panel(player_panel, "Update", 0, UpdatePlayer);
                
        //Ball panel
        ball_panel = glui->add_panel("Ball", GLUI_PANEL_EMBOSSED);
        float val1 = 1;
        add_to_panel(ball_panel, &ball_radius, "radius", val);
        add_to_panel(ball_panel, &ball_mass, "mass", val);
        add_to_panel(ball_panel, &ball_liner_dumping, "velocity dumping", val);
        add_to_panel(ball_panel, &ball_bounce, "bounce",val);
        add_to_panel(ball_panel, &ball_angular_dumping, "angular dumping", val);
        
        glui->add_button_to_panel(ball_panel, "Update", 0, UpdateBall);
        
        HidePanels();
        
        //-----------------------------
        
	glui->set_main_gfx_window( mainWindow );

	// Use a timer to control the frame rate.
	glutTimerFunc(framePeriod, Timer, 0);

	glutMainLoop();
        
	return 0;
}
