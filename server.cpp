#include "Connection.hpp"
#include "Game.hpp"

#include <iostream>
#include <set>
#include <chrono>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage:\n\t./server <port>" << std::endl;
        return 1;
    }

    Server server(argv[1]);

    Game state1;
    Game state2;

    Game temp;

    while (1) {
        server.poll([&](Connection *c, Connection::Event evt){
                if (evt == Connection::OnOpen) {
                } else if (evt == Connection::OnClose) {
                } else { assert(evt == Connection::OnRecv);
                if (c->recv_buffer[0] == 'h') {
                c->recv_buffer.erase(c->recv_buffer.begin(),
                        c->recv_buffer.begin() + 1);
                std::cout << c << ": Got hello." << std::endl;
                }else if (c->recv_buffer [0] == 'a'){
                    if (c->recv_buffer.size() < 1 + sizeof(float))
                        return; //wait for more data
                    else{
                        memcpy(&temp.body_pos.x, c->recv_buffer.data()+1, sizeof(float));
                        c->recv_buffer.erase(c->recv_buffer.begin(), c->recv_buffer.begin() + 1 + sizeof(float));
                        memcpy(&temp.body_pos.y, c->recv_buffer.data(), sizeof(float));
                        c->recv_buffer.erase(c->recv_buffer.begin(), c->recv_buffer.begin() +  sizeof(float));
                        memcpy(&temp.thighR_angle, c->recv_buffer.data(), sizeof(float));
                        c->recv_buffer.erase(c->recv_buffer.begin(), c->recv_buffer.begin() +  sizeof(float));
                        memcpy(&temp.thighL_angle, c->recv_buffer.data(), sizeof(float));
                        c->recv_buffer.erase(c->recv_buffer.begin(), c->recv_buffer.begin() +  sizeof(float));
                        memcpy(&temp.calfR_angle, c->recv_buffer.data(), sizeof(float));
                        c->recv_buffer.erase(c->recv_buffer.begin(), c->recv_buffer.begin() +  sizeof(float));
                        memcpy(&temp.calfL_angle, c->recv_buffer.data(), sizeof(float));
                        c->recv_buffer.erase(c->recv_buffer.begin(), c->recv_buffer.begin() +  sizeof(float));
                        c->send_raw("a", 1);
                        if(c->ID == 0){
                            state1 = temp;
                            c->send_raw(&state2.body_pos.x, sizeof(float));
                            c->send_raw(&state2.body_pos.y, sizeof(float));
                            c->send_raw(&state2.thighR_angle, sizeof(float));
                            c->send_raw(&state2.thighL_angle, sizeof(float));
                            c->send_raw(&state2.calfR_angle, sizeof(float));
                            c->send_raw(&state2.calfL_angle, sizeof(float));
                        }else{
                            state2 = temp;
                            c->send_raw(&state1.body_pos.x, sizeof(float));
                            c->send_raw(&state1.body_pos.y, sizeof(float));
                            c->send_raw(&state1.thighR_angle, sizeof(float));
                            c->send_raw(&state1.thighL_angle, sizeof(float));
                            c->send_raw(&state1.calfR_angle, sizeof(float));
                            c->send_raw(&state1.calfL_angle, sizeof(float));
                        }


                    }
                }
                }
        }, 0.01);
        //every second or so, dump the current paddle position:
        static auto then = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        if (now > then + std::chrono::seconds(1)) {
            then = now;
            //TODO
            std::cout<<"Body pos of player 1: " <<state1.body_pos.x<<std::endl;
            std::cout<<"Body pos of player 2: " <<state2.body_pos.x<<std::endl;
        }
    }
}
