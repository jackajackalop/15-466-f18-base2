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
    int players = 0;
    int elapsed = 0;
    bool item = false;
    float item_pos = 0.0f;

    std::vector <bool> sent = {false, false};

    Game state1;
    Game state2;

    Game temp;

    while (1) {
        server.poll([&](Connection *c, Connection::Event evt){
                if (evt == Connection::OnOpen) {
                } else if (evt == Connection::OnClose) {
                    throw std::runtime_error("Player has exited. A TWO player game cannot go on with a single person");
                } else { assert(evt == Connection::OnRecv);
                if (c->recv_buffer[0] == 'h') {
                    c->recv_buffer.erase(c->recv_buffer.begin(),
                        c->recv_buffer.begin() + 1);
                    if(players == 0)
                        c->send_raw("p0", 2);
                    else if(players == 1)
                        c->send_raw("p1", 2);
                    else
                        std::cout<<"Again, this is a two player game"<<std::endl;
                    players++;
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
                if(item){
                    if(c->ID == 0){
                        if(!sent[0]){
                           c->send_raw("i", 1);
                           c->send_raw(&item_pos, sizeof(float));
                           sent[0] = true;
                        }
                    }else{
                        if(!sent[1]){
                            c->send_raw("i", 1);
                            c->send_raw(&item_pos, sizeof(float));
                            sent[1] = true;
                        }
                    }
                    if(sent[0] && sent[1]){
                        item = false;
                        sent[0] = false;
                        sent[1] = false;
                    }
                }
            }
        }, 0.01);
        //every second or so, dump the current paddle position:
        static auto then = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        if (now > then + std::chrono::seconds(1)) {
            elapsed++;
            if(!item && elapsed%25==0){
                item = true;
                item_pos = std::max(state1.body_pos.x, state2.body_pos.x)
                    +15.0f;
                elapsed++;
            }
            then = now;
            std::cout<<"Body pos of player 1: " <<state1.body_pos.x<<std::endl;
            std::cout<<"Body pos of player 2: " <<state2.body_pos.x<<std::endl;
        }
    }
}
