#pragma once

#include "Mode.hpp"

#include "MeshBuffer.hpp"
#include "GL.hpp"
#include "Connection.hpp"
#include "Game.hpp"

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

// The 'GameMode' mode is the main gameplay mode:

struct GameMode : public Mode {
	GameMode(Client &client);
	virtual ~GameMode();

	//handle_event is called when new mouse or keyboard events are received:
	// (note that this might be many times per frame or never)
	//The function should return 'true' if it handled the event.
	virtual bool handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) override;

    char playerNum;
	struct {
                bool q = false;
                bool w = false;
                bool o = false;
                bool p = false;
        } controls;


	//update is called at the start of a new frame, after events are handled:
	virtual void update(float elapsed) override;

	//draw is called after update:
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//starts up a 'quit/resume' pause menu:
	void show_pause_menu();
	void show_win();
	void show_lose();

	//------- game state -------
	Game state;
    float total_time = 0.0f;
    bool player1win = false;
    bool player2win = false;
	//------ networking ------
	Client &client; //client object; manages connection to server.
};
