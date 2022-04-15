#include <engine.hpp>
#include <iostream>
#include <ctime>

using Force = sge_impl::Vector2D;
using Key = sge_impl::KeyHandler;
using Dot = sge_impl::Point2D;

// #define DINO1_BINARY "run1.bmp"
// #define DINO2_BINARY "run2.bmp"
// #define DINOJUMP_BINARY "jump.bmp"
// #define GND_BINARY "ground.bmp"
// #define SKY_BINARY "clouds.bmp"
// #define CACTUS_BINARY "kamen.bmp" 
// /*
#define DINO1_BINARY _binary_run1_bmp_start
#define DINO2_BINARY _binary_run2_bmp_start
#define DINOJUMP_BINARY _binary_jump_bmp_start
#define GND_BINARY _binary_ground_bmp_start
#define SKY_BINARY _binary_clouds_bmp_start
#define CACTUS_BINARY _binary_kamen_bmp_start

extern uint8_t DINO1_BINARY[];
extern uint8_t DINO2_BINARY[];
extern uint8_t DINOJUMP_BINARY[];
extern uint8_t CACTUS_BINARY[];
extern uint8_t GND_BINARY[];
extern uint8_t SKY_BINARY[];
// */
using namespace std;
using namespace sge_impl;

void test(uint8_t* ptr) {
    cout << ptr[0] << ptr[1] << endl;
}

int main(int argc, char** argv) {

    try {
        constexpr double FPS = 60.0;
        int tick = 0;

        double FRQ = 1.0/FPS;
    	
        clock_t start;

        cmv::AsciiImage sys1(DINO1_BINARY), sys2(DINO2_BINARY), sysj(DINOJUMP_BINARY);

        char* bmps[] = {sys1.bmp, sys2.bmp, sysj.bmp};

        GameScreen screen(cmv::AsciiImage(128,64));
        Sprite  
        dino(10,0, sys1.w / 2, sys1.h, character),
        sky(0,0,SKY_BINARY, background),
        cactus1(screen.width(), 0, CACTUS_BINARY, character),
        cactus2(screen.width() * 3 / 2 + (cactus1.w / 2), 0, CACTUS_BINARY, character),
        gnd(0,0,GND_BINARY, background);

        dino.bmp = bmps[0];
        gnd.row_m = screen.height() - gnd.h;
        
        Force gravity(0,-0.2), jump(0,5), speed(3,0);

        KeyProcessor keys{Key(W, [&](){if(dino.bmp != bmps[2]) dino << jump;}), Key(UA, [&](){if(dino.bmp != bmps[2]) dino << jump;}), Key(SPACE, [](){report("Game ended")})};
        
        dino * gnd;

        screen * sky * dino * cactus1 * cactus2 * gnd;

        cactus1 << -speed;
        cactus2 << -speed;

        cactus1.row_m = gnd.Top() - cactus1.h;
        cactus2.row_m = gnd.Top() - cactus2.h;

        while(1) {
            if (cactus1.Right() < 0) cactus1.col_m = screen.width();
            if (cactus2.Right() < 0) cactus2.col_m = screen.width();

    		start = std::clock();
            if (dino & cactus1) throw runtime_error("Game over: dino touched 1st cactus");
            if (dino & cactus2) throw runtime_error("Game over: dino touched 2nd cactus");

            if (dino.Down() < gnd.Top() - 1) dino.bmp = bmps[2];
            else if (tick < 3) dino.bmp = bmps[0];
            else dino.bmp = bmps[1];

            if(!((tick + 1) % 3)) sky.offset().offset().offset();

            keys();
            dino << gravity;

            gnd.offset().offset().offset();
            screen.display();
            tick = (tick + 1) % 6;
    		while ((( std::clock() - start ) / (double) CLOCKS_PER_SEC) < FRQ);
        }

    } catch (exception& ex) {
        {fcmd::Screen s(cmv::AsciiImage(20,20), 1.0, 1,15); Sleep(10);}
        cout << ex.what();
    }
    return 0;
}