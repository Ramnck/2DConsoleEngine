#pragma once

#ifndef MULTI_CONSOLE_LIBRARY
#define MULTI_CONSOLE_LIBRARY

#define FCMD_OPTIMIZE

#include <string>           // string, to_string
#include <Windows.h>        // WinApi functions

void setFont(int size);
void setConsole(int w, int h);


namespace cmv {
    class AsciiImage;
}

namespace sge_impl
{
    class Sprite;
}
struct Coord {
    int X;
    int Y;
    Coord(int x, int y);
};

namespace fcmd // fast console    
{
    enum command {endl, clrs, retc, disp};

    class Screen {
    protected:
        Coord pointer;
        int width_m;
        char* buffer;
        HANDLE buf_handler;
        DWORD bytes_written;
        bool external_console;
        COORD orig_size;
        CONSOLE_FONT_INFOEX orig_font;
        static bool origins_initialisated;

        void orig_size_init();
        void setWindow(int font);

        void roll(int height);

        Coord& mp(int length);
        Coord& mp(int _height, int _width);
        
        char& pix();
        char& pix(int _height, int _width);

    public:

        int width();
        int height();
        int height_m;

        Screen();
        Screen(int _width, int _height, int external_console, int font);

        Screen(cmv::AsciiImage& img, int external_console, int font);
        Screen(cmv::AsciiImage&& img, double scale, int external_console, int font);
        Screen& operator<<(cmv::AsciiImage& input);
        Screen& operator<<(cmv::AsciiImage&& input);

        Screen& operator=(Screen&& scr);
        Screen& operator=(Screen&) = delete;

        ~Screen();

        Screen& operator<<(Coord input);
        
        Screen& operator<<(std::string input);
        Screen& operator<<(command input);
        Screen& operator<<(char input);
        Screen& operator<<(const char* input);
        template<typename T>
        Screen& operator<<(T input);

        virtual Screen& display();
        Screen& clear();
    
    };

}

#endif /*  MULTI_CONSOLE_LIBRARY  */