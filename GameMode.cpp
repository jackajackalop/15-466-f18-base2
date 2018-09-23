#include "GameMode.hpp"

#include "MenuMode.hpp"
#include "Load.hpp"
#include "MeshBuffer.hpp"
#include "Scene.hpp"
#include "gl_errors.hpp" //helper for dumpping OpenGL error messages
#include "read_chunk.hpp" //helper for reading a vector of structures from a file
#include "data_path.hpp" //helper to get paths relative to executable
#include "compile_program.hpp" //helper to compile opengl shader programs
#include "draw_text.hpp" //helper to... um.. draw text
#include "vertex_color_program.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <map>
#include <cstddef>
#include <random>


Load< MeshBuffer > meshes(LoadTagDefault, [](){
		return new MeshBuffer(data_path("paddle-ball.pnc"));
		});

Load< GLuint > meshes_for_vertex_color_program(LoadTagDefault, [](){
		return new GLuint(meshes->make_vao_for_program(vertex_color_program->program));
		});

Scene::Transform *paddle_transform = nullptr;
//Scene::Transform *ball_transform = nullptr;
Scene::Transform *body_transform = nullptr;
Scene::Transform *bicepL_transform = nullptr;
Scene::Transform *bicepR_transform = nullptr;
Scene::Transform *forearmL_transform = nullptr;
Scene::Transform *forearmR_transform = nullptr;
Scene::Transform *thighL_transform = nullptr;
Scene::Transform *thighR_transform = nullptr;
Scene::Transform *calfL_transform = nullptr;
Scene::Transform *calfR_transform = nullptr;



Scene::Camera *camera = nullptr;

Load< Scene > scene(LoadTagDefault, [](){
		Scene *ret = new Scene;
		//load transform hierarchy:
		ret->load(data_path("paddle-ball.scene"), [](Scene &s, Scene::Transform *t, std::string const &m){
				Scene::Object *obj = s.new_object(t);

				obj->program = vertex_color_program->program;
				obj->program_mvp_mat4  = vertex_color_program->object_to_clip_mat4;
				obj->program_mv_mat4x3 = vertex_color_program->object_to_light_mat4x3;
				obj->program_itmv_mat3 = vertex_color_program->normal_to_light_mat3;

				MeshBuffer::Mesh const &mesh = meshes->lookup(m);
				obj->vao = *meshes_for_vertex_color_program;
				obj->start = mesh.start;
				obj->count = mesh.count;
				});

		//look up paddle and ball transforms:
		for (Scene::Transform *t = ret->first_transform; t != nullptr; t = t->alloc_next) {
		if (t->name == "Paddle") {
		if (paddle_transform) throw std::runtime_error("Multiple 'Paddle' transforms in scene.");
		paddle_transform = t;
		}
		if (t->name == "Body") {
			if (body_transform) throw std::runtime_error("Multiple 'Body' transforms in scene.");
			body_transform = t;
		}
		if (t->name == "Bicep_R") {
			if (bicepR_transform) throw std::runtime_error("Multiple 'Bicep_R' transforms in scene.");
			bicepR_transform = t;
		}
		if (t->name == "Bicep_L") {
			if (bicepL_transform) throw std::runtime_error("Multiple 'Bicep_L' transforms in scene.");
			bicepL_transform = t;
		}
		if (t->name == "Forearm_L") {
			if (forearmL_transform) throw std::runtime_error("Multiple 'Forearm_L' transforms in scene.");
			forearmL_transform = t;
		}
		if (t->name == "Forearm_R") {
			if (forearmR_transform) throw std::runtime_error("Multiple 'Forearm_R' transforms in scene.");
			forearmR_transform = t;
		}
		if (t->name == "Thigh_R") {
			if (thighR_transform) throw std::runtime_error("Multiple 'Thigh_R' transforms in scene.");
			thighR_transform = t;
		}
		if (t->name == "Calf_R") {
			if (calfR_transform) throw std::runtime_error("Multiple 'Calf_R' transforms in scene.");
			calfR_transform = t;
			calfR_transform->position.x += 0.2f;
		}
		if (t->name == "Thigh_L") {
			if (thighL_transform) throw std::runtime_error("Multiple 'Thigh_L' transforms in scene.");
			thighL_transform = t;
		}
		if (t->name == "Calf_L") {
			if (calfL_transform) throw std::runtime_error("Multiple 'Calf_L' transforms in scene.");
			calfL_transform = t;
			calfL_transform->position.x += 2.0f;
		}

		}
		if (!paddle_transform) throw std::runtime_error("No 'Paddle' transform in scene.");
		if (!body_transform) throw std::runtime_error("No 'Body' transform in scene.");
		if (!bicepR_transform) throw std::runtime_error("No 'Bicep_R' transform in scene.");
		if (!bicepL_transform) throw std::runtime_error("No 'Bicep_L' transform in scene.");
		if (!forearmL_transform) throw std::runtime_error("No 'Forearm_L' transform in scene.");
		if (!forearmR_transform) throw std::runtime_error("No 'Forearm_R' transform in scene.");
		if (!thighL_transform) throw std::runtime_error("No 'Thigh_L' transform in scene.");
		if (!thighR_transform) throw std::runtime_error("No 'Thigh_R' transform in scene.");
		if (!calfL_transform) throw std::runtime_error("No 'Calf_L' transform in scene.");
		if (!calfR_transform) throw std::runtime_error("No 'Calf_R' transform in scene.");
		//attach body parts
		bicepR_transform->set_parent(body_transform, nullptr);
		bicepL_transform->set_parent(body_transform, nullptr);
		forearmL_transform->set_parent(bicepL_transform, nullptr);
		forearmR_transform->set_parent(bicepR_transform, nullptr);
		thighR_transform->set_parent(body_transform, nullptr);
		calfR_transform->set_parent(thighR_transform, nullptr);
		thighL_transform->set_parent(body_transform, nullptr);
		calfL_transform->set_parent(thighL_transform, nullptr);
		//look up the camera:
		for (Scene::Camera *c = ret->first_camera; c != nullptr; c = c->alloc_next) {
			if (c->transform->name == "Camera") {
				if (camera) throw std::runtime_error("Multiple 'Camera' objects in scene.");
				camera = c;
			}
		}
		if (!camera) throw std::runtime_error("No 'Camera' camera in scene.");
		return ret;
});

GameMode::GameMode(Client &client_) : client(client_) {
	client.connection.send_raw("h", 1); //send a 'hello' to the server
}

GameMode::~GameMode() {
}

bool GameMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	//ignore any keys that are the result of automatic key repeat:
	if (evt.type == SDL_KEYDOWN && evt.key.repeat) {
//		return false;
	}
	//move duck jump angle and power
	if (evt.type == SDL_KEYDOWN|| evt.type == SDL_KEYUP) {
		if (evt.key.keysym.scancode == SDL_SCANCODE_Q) {
			controls.q = (evt.type == SDL_KEYDOWN);
			return true;
		} else if (evt.key.keysym.scancode == SDL_SCANCODE_W) {
			controls.w = (evt.type == SDL_KEYDOWN);
			return true;
		}else if (evt.key.keysym.scancode == SDL_SCANCODE_O) {
			controls.o = (evt.type == SDL_KEYDOWN);
			return true;
		} else if (evt.key.keysym.scancode == SDL_SCANCODE_P) {
			controls.p = (evt.type == SDL_KEYDOWN);
			return true;
		}
	}

	if (evt.type == SDL_MOUSEMOTION) {
		state.paddle.x = (evt.motion.x - 0.5f * window_size.x) / (0.5f * window_size.x) * Game::FrameWidth;
		state.paddle.x = std::max(state.paddle.x, -0.5f * Game::FrameWidth + 0.5f * Game::PaddleWidth);
		state.paddle.x = std::min(state.paddle.x,  0.5f * Game::FrameWidth - 0.5f * Game::PaddleWidth);
	}

	return false;
}

glm::quat getQuat(float angle){
	glm::quat dr = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	float amt = angle*0.1f;
	if (angle<90 && angle>-90) {
		dr = glm::angleAxis(amt, glm::vec3(0.0f, 0.0f, 1.0f)) * dr;
	}
	// if (dr != glm::quat()) {
	//         glm::quat &r = cursor_rotation;
	//         r = glm::normalize(dr * r);
	// }
	return dr;
}


void GameMode::update(float elapsed) {
	if (controls.q && state.thighR_angle<90) {
		state.thighR_angle+=1.0f;
		state.thighL_angle-=1.0f;
        }else if (controls.w && state.thighL_angle<90) {
		state.thighR_angle-=1.0f;
		state.thighL_angle+=1.0f;
	}else if (controls.o && state.calfL_angle>-90) {
		state.calfR_angle-=1.0f;
		state.calfL_angle+=1.0f;
        }else if (controls.p && state.calfR_angle<90) {
		state.calfR_angle+=1.0f;
		state.calfL_angle-=1.0f;
	}


	state.update(elapsed);

	if (client.connection) {
		//send game state to server:
		client.connection.send_raw("s", 1);
		client.connection.send_raw(&state.paddle.x, sizeof(float));
	}

	client.poll([&](Connection *c, Connection::Event event){
			if (event == Connection::OnOpen) {
			//probably won't get this.
			} else if (event == Connection::OnClose) {
			std::cerr << "Lost connection to server." << std::endl;
			} else { assert(event == Connection::OnRecv);
			std::cerr << "Ignoring " << c->recv_buffer.size() << " bytes from server." << std::endl;
			c->recv_buffer.clear();
			}
			});

	//copy game state to scene positions:
	body_transform->position.x = state.body_pos.x;
	body_transform->position.y = state.body_pos.y;
	thighR_transform->rotation = getQuat(state.thighR_angle);
	thighL_transform->rotation = getQuat(state.thighL_angle);
	calfL_transform->rotation = getQuat(state.calfL_angle);
	calfR_transform->rotation = getQuat(state.calfR_angle);
	bicepR_transform->rotation = getQuat(state.thighL_angle);
	bicepL_transform->rotation = getQuat(state.thighR_angle);

	paddle_transform->position.x = state.paddle.x;
	paddle_transform->position.y = state.paddle.y;
}

void GameMode::draw(glm::uvec2 const &drawable_size) {
	camera->aspect = drawable_size.x / float(drawable_size.y);

	glClearColor(0.25f, 0.0f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set up basic OpenGL state:
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//set up light positions:
	glUseProgram(vertex_color_program->program);

	glUniform3fv(vertex_color_program->sun_color_vec3, 1, glm::value_ptr(glm::vec3(0.81f, 0.81f, 0.76f)));
	glUniform3fv(vertex_color_program->sun_direction_vec3, 1, glm::value_ptr(glm::normalize(glm::vec3(-0.2f, 0.2f, 1.0f))));
	glUniform3fv(vertex_color_program->sky_color_vec3, 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.3f)));
	glUniform3fv(vertex_color_program->sky_direction_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));

	scene->draw(camera);

	GL_ERRORS();
}
