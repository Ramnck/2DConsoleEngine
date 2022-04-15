#pragma once

#ifndef ASCII_IMAGE_LIBRARY
#define ASCII_IMAGE_LIBRARY

#include <cstdio>       // FILE, fseek, fread, rewind, ftell
#include <string>       // memset, memcpy
#include <cstring>      // strlen, strcat, strcpy
#include <cmath>        // ceil
#include <exception>    // runtime_error, exception
#include <memory>       // unique_ptr

namespace cmv
{

#pragma pack(push, 1)
    typedef struct {
        char r;
        char g;
        char b;
    } RGB;

    typedef struct {
        char r;
        char g;
        char b;
        char a;
    } RGBA;
#pragma pack(pop)

    class AsciiImage {
    public:
        char* bmp;
        int w;
        int h;
        char* palette;

        AsciiImage(const uint8_t* data_ptr);
        AsciiImage(const uint8_t* data_ptr, int color);
        AsciiImage(std::string filename);                            // Initializating from file (it can be it ./res folder)
        AsciiImage(std::string filename, int color);                            // Initializating from file (it can be it ./res folder)
        AsciiImage(int width, int height);
        AsciiImage(const uint8_t* array, int width, int height, int color);     // Initializating from uint8 rgba array;

        AsciiImage();
        AsciiImage(const AsciiImage& img);
        AsciiImage& operator=(AsciiImage& img);                                 // Copy
        AsciiImage& operator=(AsciiImage&& img);                                // Move

        ~AsciiImage();

        const char& operator()(int height, int width) const;

        AsciiImage& scale(double scale);
        // /*
        #ifdef _GLIBCXX_IOSFWD
        friend ::std::ostream& operator<<(std::ostream& out, AsciiImage* img);
        friend ::std::ostream& operator<<(std::ostream& out, AsciiImage& img);
        #endif
        // */
    };

    void reverse(char* array);

}
#endif  /*  ASCII_IMAGE_LIBRARY  */