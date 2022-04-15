
#include <asciiImgLib.hpp>
#include <screen.hpp>

using namespace fcmd;

bool Screen::origins_initialisated = false;

const double scr_w = GetSystemMetrics(SM_CXSCREEN);
const double scr_h = GetSystemMetrics(SM_CYSCREEN);

void setFont(int size) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX fontInfo;
    fontInfo.cbSize = sizeof( fontInfo );
    GetCurrentConsoleFontEx(hConsole, 0, &fontInfo);
    
    size = std::max(2, size);

    fontInfo.dwFontSize.Y = size;
    fontInfo.dwFontSize.X = size / 2;
    
    SetCurrentConsoleFontEx(hConsole, TRUE, &fontInfo); 
}

void setConsole(int w, int h) {
    std::string text = "mode con cols=";
    text += std::to_string(w);
    text += " lines=";
    text += std::to_string(h);
    system(text.c_str());
}

Coord::Coord(int x, int y) : X(x), Y(y) {}

void Screen::orig_size_init() {
    
    origins_initialisated = true;
    
    _COORD coord = {height_m, width_m};
    _SMALL_RECT Rect = {0,0,coord.X - 1, coord.Y - 1};
    CONSOLE_SCREEN_BUFFER_INFO csbiData;
    GetConsoleScreenBufferInfo(GetStdHandle (STD_OUTPUT_HANDLE), &csbiData);
    orig_size.X = csbiData.dwSize.X;
    orig_size.Y = csbiData.srWindow.Bottom - csbiData.srWindow.Top + 1;

    ZeroMemory(&orig_font, sizeof(orig_font));
    orig_font.cbSize = sizeof( orig_font );
    GetCurrentConsoleFontEx(GetStdHandle (STD_OUTPUT_HANDLE), 0, &orig_font);
}

void Screen::setWindow(int font) {

    font = std::min( font, (int) std::min( scr_w / (double)width_m, scr_h / (double)height_m) );

    if (external_console) {
        FreeConsole();
        AllocConsole();
    }

    _COORD coord = {height_m, width_m};
    _SMALL_RECT Rect = {0,0,coord.X - 1, coord.Y - 1};

    setFont(font);
    setConsole(width_m, height_m);
    
    buffer = new char[height_m*width_m];
    buf_handler = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); 

    SetConsoleActiveScreenBuffer(buf_handler);
    SetConsoleWindowInfo(buf_handler, TRUE, &Rect);
    SetConsoleScreenBufferSize(buf_handler, coord);

}

Screen::Screen() : width_m(10), height_m(10), orig_font(),
            orig_size(), external_console(0),
            buffer(nullptr), buf_handler(nullptr), bytes_written(), pointer({0,0}) {
    if(!origins_initialisated)
        orig_size_init();
    // setWindow(15);
}

Screen::Screen(int _width, int _height, int _external_console, int font = 15) :
            width_m(_width), height_m(_height), orig_font(),
            orig_size(), external_console(_external_console),
            buffer(nullptr), buf_handler(), bytes_written(), pointer({0,0}) {
    if(!origins_initialisated)
        orig_size_init();
    setWindow(font);

}

Screen::~Screen() {
    if (buf_handler != nullptr && buf_handler != 0) {
        delete[] buffer;
        if (external_console) {
            FreeConsole();
            AttachConsole((DWORD)-1);
        }
        HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleActiveScreenBuffer(Handle);
        SetCurrentConsoleFontEx(Handle, TRUE, &orig_font);
        if (!external_console)
            setConsole(orig_size.X, orig_size.Y);
        else {
            _SMALL_RECT Rect = {0, 0, orig_size.X - 1, orig_size.Y - 1};
            SetConsoleWindowInfo(Handle, TRUE, &Rect);
            SetConsoleScreenBufferSize(Handle, orig_size);
        }
    }
}


Screen& Screen::operator=(Screen&& scr) {
    pointer = scr.pointer;
    width_m = scr.width_m;
    height_m = scr.height_m;
    buffer = scr.buffer;
    scr.buffer = nullptr;
    buf_handler = scr.buf_handler;
    scr.buf_handler = nullptr;
    external_console = scr.external_console;
    scr.external_console = 0;
    return *this;
}

#ifdef ASCII_IMAGE_LIBRARY

Screen::Screen(cmv::AsciiImage& img, int _external_console = 0, int font = 15) : 
            width_m(0), height_m(0), orig_font(), 
            orig_size(), external_console(_external_console),
            buffer(nullptr), buf_handler(), bytes_written(), pointer({0,0}) {

    if (img.w > scr_w - 10 || img.h > scr_h - 10) 
        img.scale(std::max((double)img.w / (scr_w - 20.0), (double)img.h / (scr_h - 20.0)));

    width_m = img.w;
    height_m = img.h;

    if (!origins_initialisated)
        orig_size_init();

    setWindow(font);

    (*this) << img;
}

Screen::Screen(cmv::AsciiImage&& img, double scale = 1.0, int _external_console = 0, int font = 15) : 
            width_m(0), height_m(0), orig_font(), 
            orig_size(), external_console(_external_console),
            buffer(nullptr), buf_handler(), bytes_written(), pointer({0,0}) {
    
    img.scale(scale);

    width_m = img.w;
    height_m = img.h;

    if (width_m > scr_w - 10 || height_m > scr_h - 10) 
        img.scale(std::max((double)width_m / (scr_w - 20.0), (double)height_m / (scr_h - 20.0)));

    width_m = img.w;
    height_m = img.h;

    if (!origins_initialisated)
        orig_size_init();

    setWindow(font);

    (*this) << std::move(img);
}


#endif /*  MULTI_CONSOLE_LIBRARY  */

void Screen::roll(int _height) {
    memcpy(buffer, &pix(_height, 0), (height_m - std::min(_height, height_m)) * width_m);
    memset(buffer + (height_m - _height) * width_m, ' ', (height_m - _height) * width_m);
    pointer.Y -= std::min(_height, height_m);
}

Coord& Screen::mp(int length) {
    pointer.X += length;
    if (pointer.Y > height_m - 1) {
        roll(std::min(length, height_m));
        pointer.Y = height_m - 1;
    }
    return this->pointer;
}

Coord& Screen::mp(int _height, int _width) {
    this->mp(_height * width_m + _width);
    return this->pointer;
}

Screen& Screen::display() {
    Screen::buffer[Screen::width_m * Screen::height_m - 1] = '\0';
    WriteConsoleOutputCharacter(buf_handler, (LPCSTR) Screen::buffer, Screen::width_m * Screen::height_m + 1, { 0, 0 }, &bytes_written);
    return *this;
}

char& Screen::pix(int _height, int _width = 0) {
    return *(buffer + _height * width_m + _width);
}

char& Screen::pix() {
    return this->pix(pointer.Y, pointer.X);
}

Screen& Screen::clear() {
    pointer.X = 0;
    pointer.Y = 0;
    memset(buffer, ' ', width_m*height_m);
    return *this;
}

int Screen::width() {
    return width_m;
}

int Screen::height() {
    return height_m;
}

#ifdef ASCII_IMAGE_LIBRARY

Screen& Screen::operator<<(cmv::AsciiImage& input) {

    if (input.w > scr_w - 10 || input.h > scr_h - 10) 
        input.scale(std::max((double)input.w / (scr_w - 20.0), (double)input.h / (scr_h - 20.0)));

    width_m = input.w;
    height_m = input.h;

    int CRLF = 0;

    memcpy(buffer, &input(0,0), width_m * height_m);

#ifndef FCMD_OPTIMIZE 
    this->display();
#endif
    return *this;   
}

Screen& Screen::operator<<(cmv::AsciiImage&& input) {

    if (input.w > scr_w - 10 || input.h > scr_h - 10) 
        input.scale(std::max((double)input.w / (scr_w - 20.0), (double)input.h / (scr_h - 20.0)));

    width_m = input.w;
    height_m = input.h;

    buffer = input.bmp;
    input.bmp = nullptr;
    
#ifndef FCMD_OPTIMIZE 
    this->display();
#endif
    return *this;
}

#endif

Screen& Screen::operator<<(char input) {

    switch (input)
    {
    case '\n': case '\0':
        (*this) << endl;
        break;
    case '\t':
        (*this) << "\\t is forbidden";
        break;
    case '\b':
        mp(-1);
        break;
    case '\r':
        pointer.X = 0;
        break;
    default:
        this->pix() = input;
        mp(0, 1);
        break;
    }
    
#ifndef FCMD_OPTIMIZE 
    this->display();
#endif
    return *this;
}

Screen& Screen::operator<<(Coord input) {

    if (input.X + 1 > width_m) throw std::runtime_error("ERROR: POINTER.X > width");
    if (input.Y + 1 > height_m) throw std::runtime_error("ERROR: POINTER.Y > height");
    pointer = input;
    return *this;
}

Screen& Screen::operator<<(command input) {

    switch (input)
    {
    case endl:
        pointer.X = 0;
        if(pointer.Y++ == height_m - 1)
            roll(1);
        break;
    case clrs:
        clear();
        break;
    case retc:
        pointer.X = 0;
        break;
    case disp:
#if defined(FCMD_OPTIMIZE)
        this->display();
#endif // FCMD_OPTIMIZE
        break;
    default:
        break;
    }
#ifndef FCMD_OPTIMIZE
    this->display();
#endif
    return *this;
}

Screen& Screen::operator<<(std::string input) {
    memcpy(&this->pix(), input.data(), input.length());
    mp(input.length());
#ifndef FCMD_OPTIMIZE 
        this->display();
#endif
    return *this;
}

Screen& Screen::operator<<(const char* input) {
    return (*this) << std::string(input);
}

template<typename T>
Screen& Screen::operator<<(T input) {
    return (*this) << std::to_string(input);
}

template Screen& Screen::operator<<<int>(int);
template Screen& Screen::operator<<<double>(double);
