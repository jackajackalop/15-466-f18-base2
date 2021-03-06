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

Scene::Transform *body_transform = nullptr;
Scene::Transform *bicepL_transform = nullptr;
Scene::Transform *bicepR_transform = nullptr;
Scene::Transform *forearmL_transform = nullptr;
Scene::Transform *forearmR_transform = nullptr;
Scene::Transform *thighL_transform = nullptr;
Scene::Transform *thighR_transform = nullptr;
Scene::Transform *calfL_transform = nullptr;
Scene::Transform *calfR_transform = nullptr;

Scene::Transform *body_transform2 = nullptr;
Scene::Transform *bicepL_transform2 = nullptr;
Scene::Transform *bicepR_transform2 = nullptr;
Scene::Transform *forearmL_transform2 = nullptr;
Scene::Transform *forearmR_transform2 = nullptr;
Scene::Transform *thighL_transform2 = nullptr;
Scene::Transform *thighR_transform2 = nullptr;
Scene::Transform *calfL_transform2 = nullptr;
Scene::Transform *calfR_transform2 = nullptr;

Scene::Transform *shell = nullptr;
Scene::Transform *banana = nullptr;
Scene::Transform *star  = nullptr;

Scene::Transform *ground = nullptr;
Scene::Transform *bg = nullptr;
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
            forearmR_transform->position.x-=0.1f;
        }
        if (t->name == "Thigh_R") {
            if (thighR_transform) throw std::runtime_error("Multiple 'Thigh_R' transforms in scene.");
            thighR_transform = t;
        }
        if (t->name == "Calf_R") {
            if (calfR_transform) throw std::runtime_error("Multiple 'Calf_R' transforms in scene.");
            calfR_transform = t;
            calfR_transform->position.y -= 0.8f;
            calfR_transform->position.x += 1.5f;
        }
        if (t->name == "Thigh_L") {
            if (thighL_transform) throw std::runtime_error("Multiple 'Thigh_L' transforms in scene.");
            thighL_transform = t;
        }
        if (t->name == "Calf_L") {
            if (calfL_transform) throw std::runtime_error("Multiple 'Calf_L' transforms in scene.");
            calfL_transform = t;
            calfL_transform->position.x -= 0.2f;
        }

        //PLAYER 2
        if (t->name == "Body2") {
            if (body_transform2) throw std::runtime_error("Multiple 'Body2' transforms in scene.");
            body_transform2 = t;
        }
        if (t->name == "Bicep_R2") {
            if (bicepR_transform2) throw std::runtime_error("Multiple 'Bicep_R' transforms in scene.");
            bicepR_transform2 = t;
            bicepR_transform2->position.y -= 1.5f;
        }
        if (t->name == "Bicep_L2") {
            if (bicepL_transform2) throw std::runtime_error("Multiple 'Bicep_L' transforms in scene.");
            bicepL_transform2 = t;
            bicepL_transform2->position.y -= 1.5f;
        }
        if (t->name == "Forearm_L2") {
            if (forearmL_transform2) throw std::runtime_error("Multiple 'Forearm_L' transforms in scene.");
            forearmL_transform2 = t;
        }
        if (t->name == "Forearm_R2") {
            if (forearmR_transform2) throw std::runtime_error("Multiple 'Forearm_R' transforms in scene.");
            forearmR_transform2 = t;
            forearmR_transform2->position.x-= 0.1f;
        }
        if (t->name == "Thigh_R2") {
            if (thighR_transform2) throw std::runtime_error("Multiple 'Thigh_R' transforms in scene.");
            thighR_transform2 = t;
            thighR_transform2->position.y -= 1.5f;
        }
        if (t->name == "Calf_R2") {
            if (calfR_transform2) throw std::runtime_error("Multiple 'Calf_R' transforms in scene.");
            calfR_transform2 = t;
            calfR_transform2->position.y -= 0.8f;
            calfR_transform2->position.x +=1.5f;
        }
        if (t->name == "Thigh_L2") {
            if (thighL_transform2) throw std::runtime_error("Multiple 'Thigh_L' transforms in scene.");
            thighL_transform2 = t;
            thighL_transform2->position.y -= 1.5f;
        }
        if (t->name == "Calf_L2") {
            if (calfL_transform2) throw std::runtime_error("Multiple 'Calf_L' transforms in scene.");
            calfL_transform2 = t;
            calfL_transform2->position.x -= 0.2f;
        }

        //BACKGROUND
        if(t->name == "bg"){
            if(bg) throw std::runtime_error("Multiple 'bg' transforms in scene.");
            bg = t;
        }
        if(t->name == "ground"){
            if(ground) throw std::runtime_error("Multiple 'ground' transforms in scene.");
            ground = t;
        }

        //ITEMS
        if(t->name == "banana"){
            if(banana) throw std::runtime_error("Multiple 'banana' transforms in scene.");
            banana = t;
            banana->position.x = -9999.0f;
        }
        if(t->name == "star"){
            if(star) throw std::runtime_error("Multiple 'star' transforms in scene.");
            star = t;
            star->position.x = -9999.0f;
        }
        if(t->name == "shell"){
            shell = t;
            shell->position.x = -9999.0f;
        }
        }

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

        bicepR_transform2->set_parent(body_transform2, nullptr);
        bicepL_transform2->set_parent(body_transform2, nullptr);
        forearmL_transform2->set_parent(bicepL_transform2, nullptr);
        forearmR_transform2->set_parent(bicepR_transform2, nullptr);
        thighR_transform2->set_parent(body_transform2, nullptr);
        calfR_transform2->set_parent(thighR_transform2, nullptr);
        thighL_transform2->set_parent(body_transform2, nullptr);
        calfL_transform2->set_parent(thighL_transform2, nullptr);
        //look up the camera:
        for (Scene::Camera *c = ret->first_camera; c != nullptr; c = c->alloc_next) {
            if (c->transform->name == "Camera") {
                if (camera) throw std::runtime_error("Multiple 'Camera' objects in scene.");
                camera = c;
            }
        }
        if (!camera) throw std::runtime_error("No 'Camera' camera in scene.");
        //TODO once theres a bg
        return ret;
});

GameMode::GameMode(Client &client_) : client(client_) {
    //TODO
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
        }
        if (evt.key.keysym.scancode == SDL_SCANCODE_W) {
            controls.w = (evt.type == SDL_KEYDOWN);
        }
        if (evt.key.keysym.scancode == SDL_SCANCODE_O) {
            controls.o = (evt.type == SDL_KEYDOWN);
        }
        if (evt.key.keysym.scancode == SDL_SCANCODE_P) {
            controls.p = (evt.type == SDL_KEYDOWN);
        }
        if (evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            //open pause menu on 'ESCAPE':
            show_pause_menu();
            return true;
        }
    }


    return false;
}

glm::quat getQuat(float angle){
    glm::quat dr = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    float amt = angle*0.1f;
    dr = glm::angleAxis(amt, glm::vec3(0.0f, 0.0f, 1.0f)) * dr;
    return dr;
}


void GameMode::update(float elapsed) {

    if(playerNum == '0'){
        if (controls.q && state.thighR_angle<10) {
            state.thighR_angle+=1.0f;
            state.thighL_angle-=1.0f;
        }
        if (controls.w && state.thighL_angle<10) {
            state.thighR_angle-=1.0f;
            state.thighL_angle+=1.0f;
        }
        if (controls.o && state.calfR_angle>-20) {
            state.calfR_old = state.calfR_angle;
            state.calfL_old = state.calfL_angle;
            state.calfR_angle-=1.0f;
            if(state.calfL_angle<0)
                state.calfL_angle+=1.0f;
        }
        if (controls.p && state.calfL_angle>-20) {
            state.calfR_old = state.calfR_angle;
            state.calfL_old = state.calfL_angle;
            state.calfL_angle-=1.0f;
            if(state.calfR_angle<0)
                state.calfR_angle+=1.0f;
        }
    }else{
        if (controls.q && state.thighR_angle2<10) {
            state.thighR_angle2+=1.0f;
            state.thighL_angle2-=1.0f;
        }
        if (controls.w && state.thighL_angle2<10) {
            state.thighR_angle2-=1.0f;
            state.thighL_angle2+=1.0f;
        }
        if (controls.o && state.calfR_angle2>-20) {
            state.calfR_old2 = state.calfR_angle2;
            state.calfL_old2 = state.calfL_angle2;
            state.calfR_angle2-=1.0f;
            if(state.calfL_angle2<0)
                state.calfL_angle2+=1.0f;
        }
        if (controls.p && state.calfL_angle2>-20) {
            state.calfR_old2 = state.calfR_angle2;
            state.calfL_old2 = state.calfL_angle2;
            state.calfL_angle2-=1.0f;
            if(state.calfR_angle2<0)
                state.calfR_angle2+=1.0f;
        }

    }

    state.update(elapsed, playerNum);

    if (client.connection) {
        //send game state to server:
        //TODO turns out its sending other things thats breaking it
        if(playerNum == '0'){
            client.connection.send_raw("a", 1);
            client.connection.send_raw(&state.body_pos.x, sizeof(float));
            client.connection.send_raw(&state.body_pos.y, sizeof(float));
            client.connection.send_raw(&state.thighR_angle, sizeof(float));
            client.connection.send_raw(&state.thighL_angle, sizeof(float));
            client.connection.send_raw(&state.calfR_angle, sizeof(float));
            client.connection.send_raw(&state.calfL_angle, sizeof(float));
        }else{
            client.connection.send_raw("a", 1);
            client.connection.send_raw(&state.body_pos2.x, sizeof(float));
            client.connection.send_raw(&state.body_pos2.y, sizeof(float));
            client.connection.send_raw(&state.thighR_angle2, sizeof(float));
            client.connection.send_raw(&state.thighL_angle2, sizeof(float));
            client.connection.send_raw(&state.calfR_angle2, sizeof(float));
            client.connection.send_raw(&state.calfL_angle2, sizeof(float));

        }
    }

    client.poll([&](Connection *c, Connection::Event event){
            if (event == Connection::OnOpen) {
            //probably won't get this.
            } else if (event == Connection::OnClose) {
            std::cerr << "Lost connection to server." << std::endl;
            } else { assert(event == Connection::OnRecv);}

            if(c->recv_buffer[0] == 'p'){
            if(c->recv_buffer.size() < 2){
            return;
            }else{
            memcpy(&playerNum, c->recv_buffer.data()+1, 1);
            c->recv_buffer.erase(c->recv_buffer.begin(),
                    c->recv_buffer.begin() + 2);
            }
            }else if (c->recv_buffer[0] == 'i'){
                if(c->recv_buffer.size() < 1 + sizeof(float)){
                    return;
                }else{
                    uint rand = std::rand()%10+1;
                    if(rand<4){
                        memcpy(&banana->position.x, c->recv_buffer.data()+1,
                            sizeof(float));
                    }else if(rand<7){
                        memcpy(&star->position.x, c->recv_buffer.data()+1,
                            sizeof(float));
                    }else{
                        memcpy(&shell->position.x, c->recv_buffer.data()+1,
                            sizeof(float));
                    }
                    c->recv_buffer.erase(c->recv_buffer.begin(),
                            c->recv_buffer.begin() + 1 + sizeof(float));
                }
            }else if (c->recv_buffer[0] == 'a') {
            if (c->recv_buffer.size() < 1 + sizeof(float)) {
            return; //wait for more data
            } else {
            if(playerNum == '0'){
                memcpy(&state.body_pos2.x,
                        c->recv_buffer.data()+1,
                        sizeof(float));
                c->recv_buffer.erase(c->recv_buffer.begin(),
                        c->recv_buffer.begin() + 1 + sizeof(float));
                memcpy(&state.body_pos2.y,
                        c->recv_buffer.data(),
                        sizeof(float));
                c->recv_buffer.erase(c->recv_buffer.begin(),
                        c->recv_buffer.begin() + sizeof(float));
                memcpy(&state.thighR_angle2,
                        c->recv_buffer.data(),
                        sizeof(float));
                c->recv_buffer.erase(c->recv_buffer.begin(),
                        c->recv_buffer.begin() + sizeof(float));
                memcpy(&state.thighL_angle2,
                        c->recv_buffer.data(),
                        sizeof(float));
                c->recv_buffer.erase(c->recv_buffer.begin(),
                        c->recv_buffer.begin() + sizeof(float));
                memcpy(&state.calfR_angle2,
                        c->recv_buffer.data(),
                        sizeof(float));
                c->recv_buffer.erase(c->recv_buffer.begin(),
                        c->recv_buffer.begin() + sizeof(float));
                memcpy(&state.calfL_angle2,
                        c->recv_buffer.data(),
                        sizeof(float));
                c->recv_buffer.erase(c->recv_buffer.begin(),
                        c->recv_buffer.begin() + sizeof(float));
            }else{
                memcpy(&state.body_pos.x,
                        c->recv_buffer.data()+1,
                        sizeof(float));
                c->recv_buffer.erase(c->recv_buffer.begin(),
                        c->recv_buffer.begin() + 1 + sizeof(float));
                memcpy(&state.body_pos.y,
                        c->recv_buffer.data(),
                        sizeof(float));
                c->recv_buffer.erase(c->recv_buffer.begin(),
                        c->recv_buffer.begin() + sizeof(float));
                memcpy(&state.thighR_angle,
                        c->recv_buffer.data(),
                        sizeof(float));
                c->recv_buffer.erase(c->recv_buffer.begin(),
                        c->recv_buffer.begin() + sizeof(float));
                memcpy(&state.thighL_angle,
                        c->recv_buffer.data(),
                        sizeof(float));
                c->recv_buffer.erase(c->recv_buffer.begin(),
                        c->recv_buffer.begin() + sizeof(float));
                memcpy(&state.calfR_angle,
                        c->recv_buffer.data(),
                        sizeof(float));
                c->recv_buffer.erase(c->recv_buffer.begin(),
                        c->recv_buffer.begin() + sizeof(float));
                memcpy(&state.calfL_angle,
                        c->recv_buffer.data(),
                        sizeof(float));
                c->recv_buffer.erase(c->recv_buffer.begin(),
                        c->recv_buffer.begin() + sizeof(float));

            }
            }
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
    body_transform2->position.x = state.body_pos2.x - 2.0f;
    body_transform2->position.y = state.body_pos2.y + 1.5f;
    thighR_transform2->rotation = getQuat(state.thighR_angle2);
    thighL_transform2->rotation = getQuat(state.thighL_angle2);
    calfL_transform2->rotation = getQuat(state.calfL_angle2);
    calfR_transform2->rotation = getQuat(state.calfR_angle2);
    bicepR_transform2->rotation = getQuat(state.thighL_angle2);
    bicepL_transform2->rotation = getQuat(state.thighR_angle2);
    if(playerNum == '0'){
        camera->transform->position = body_transform->position;
        bg->position.x = body_transform->position.x;
        ground->position.x = body_transform->position.x;
    }else{
        camera->transform->position = body_transform2->position;
        bg->position.x = body_transform2->position.x;
        ground->position.x = body_transform2->position.x;
    }
    camera->transform->position.z += 30.0f;
    camera->transform->position.y -= 0.5f;

    //TODO
    //shell handling
    if(std::abs(state.body_pos.x - shell->position.x) <0.4f
                && state.body_pos.y <0.3f){
        player1win = false;
        player2win = true;
    }else if(std::abs(state.body_pos2.x - shell->position.x) <0.4f
            && state.body_pos2.y <0.3f){
        player1win = true;
        player2win = false;
    }
    //star handling
    if(std::abs(state.body_pos.x - star->position.x) <0.4f
                && state.body_pos.y <0.4f){
        state.body_velocity.x = 20.0f;
        state.body_velocity.y = 5.0f;
    }else if(std::abs(state.body_pos2.x - star->position.x) <0.4f
            && state.body_pos2.y <0.4f){
        state.body_velocity2.x = 20.0f;
        state.body_velocity2.y = 5.0f;
    }
    //banana handling
    if(std::abs(state.body_pos.x - banana->position.x) <0.4f
                && state.body_pos.y <0.4f){
        state.body_velocity.x = -20.0f;
        state.body_velocity.y = 5.0f;
    }else if(std::abs(state.body_pos2.x - banana->position.x) <0.4f
            && state.body_pos2.y <0.4f){
        state.body_velocity2.x = -20.0f;
        state.body_velocity2.y = 5.0f;
    }

    if(player1win || player2win){
        if((player1win&&playerNum == '0') || (player2win&&playerNum=='1'))
            show_win();
        else
            show_lose();
    }


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

void GameMode::show_pause_menu() {
    std::shared_ptr< MenuMode > menu = std::make_shared< MenuMode >();

    std::shared_ptr< Mode > game = shared_from_this();
    menu->background = game;

    menu->choices.emplace_back("PAUSED");
    menu->choices.emplace_back("RESUME", [game](){
            Mode::set_current(game);
            });
    menu->choices.emplace_back("QUIT", [](){
            Mode::set_current(nullptr);
            });

    menu->selected = 1;

    Mode::set_current(menu);
}
void GameMode::show_lose() {
    std::shared_ptr< MenuMode > menu = std::make_shared< MenuMode >();

    std::shared_ptr< Mode > game = shared_from_this();
    menu->background = game;

    menu->choices.emplace_back("YOU DIED LOSER");
    menu->choices.emplace_back("QUIT", [](){
            Mode::set_current(nullptr);
            });

    menu->selected = 1;

    Mode::set_current(menu);
}
void GameMode::show_win() {
    std::shared_ptr< MenuMode > menu = std::make_shared< MenuMode >();

    std::shared_ptr< Mode > game = shared_from_this();
    menu->background = game;

    menu->choices.emplace_back("FOE DIED SO YOU WIN");
    menu->choices.emplace_back("QUIT", [](){
            Mode::set_current(nullptr);
            });

    menu->selected = 1;

    Mode::set_current(menu);
}
