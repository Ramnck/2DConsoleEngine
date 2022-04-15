#pragma once
#ifndef ASCII_SPRITE_LIBRARY
#define ASCII_SPRITE_LIBRARY

#if !defined(M_PI)
#define M_PI		3.14159265358979323846
#endif // M_PI

#include <asciiImgLib.hpp>
#include <screen.hpp>

#include <algorithm>	// find
#include <vector>		// vector, erase

#define W 0x57
#define	A 0x41
#define S 0x53
#define D 0x44

#define DA VK_DOWN
#define UA VK_UP
#define RA VK_RIGHT
#define LA VK_LEFT

#define SPACE VK_SPACE

#define LSHIFT VK_LSHIFT
#define RSHIFT VK_RSHIFT

#define report(x) throw std::runtime_error(x);

namespace fcmd
{
	class Screen;	
}

namespace sge_impl		// simple game engine
{

	enum state { background, character, barier };

	class Sprite;

	class Point2D {
	public:
		double X;
		double Y;
		Point2D(double x, double y);
		inline int in(const Sprite& spr) const;
		bool operator==(const Point2D& p) const;
		Point2D operator&(const Point2D& p) const;	// расположение второй точки относительно первой
	};

	class Vector2D {
	public:
		Point2D start_m;
		double x_m;
		double y_m;

		Vector2D();
		Vector2D(double x, double y);
		Vector2D(Point2D p);
		Vector2D(Point2D p1, Point2D p2);
		Vector2D(double x, double y, Point2D p);
		Vector2D(double x1, double y1, double x2, double y2);
		
		Vector2D real();

		inline Vector2D operator()(Point2D p);
		Vector2D operator-() const;
		Vector2D operator*(const double c) const;
		Vector2D operator/(const double c) const;
		Vector2D operator+(const double c) const;
		Vector2D operator-(const double c) const;
		Vector2D operator+(const Vector2D& vec) const;
		inline double operator*(const Vector2D& vec) const;
		Vector2D operator-(const Vector2D& vec) const;

		Vector2D& operator*=(const double c);
		Vector2D& operator/=(const double c);
		Vector2D& operator+=(const double c);
		Vector2D& operator-=(const double c);
		Vector2D& operator+=(const Vector2D& vec);
		Vector2D& operator-=(const Vector2D& vec); 

		bool operator&(const Vector2D& vec) const;
		inline Point2D direction() const;
		inline Point2D start() const;
		inline Point2D end() const;
		inline double length() const;
		inline double cos_angle(const Vector2D& vec) const;

		double absoluteAngle() const;
		double angle(const Vector2D& vec) const;

		inline Vector2D& setPoint(double x, double y);
		inline Vector2D& setPoint(Point2D p);
	};


	Point2D operator+(Point2D p, Vector2D vec);

	class Sprite : public cmv::AsciiImage {

		void nextStepCollision(const Sprite& spr, const int X, const int Y);

	public:
		state kind_m;
		double col_m;
		double row_m;
		Vector2D resultant_m;
		double mass_m;
		std::vector<Sprite*> collisions_m;

		Sprite(int _col, int _row, int _w, int _h);
		Sprite(int _col, int _row, int _w, int _h, state _kind);

		Sprite(int _col, int _row, const uint8_t* data_ptr, state _kind);
		Sprite(int _col, int _row, std::string filename, state _kind);
		Sprite(int _col, int _row, std::string filename, state _kind, double mass);

		Sprite(int _col, int _row, AsciiImage& img, state _kind, double mass);
		Sprite(int _col, int _row, AsciiImage&& img, state _kind, double mass);

		Sprite& offset();

		int operator&(Sprite &enemy);

		inline Vector2D& resultant();

		double Top() const ;
		double Down() const ;
		double Left() const ;
		double Right() const ;

		inline Vector2D TopSide() const ;
		inline Vector2D DownSide() const ;
		inline Vector2D LeftSide() const ;
		inline Vector2D RightSide() const ;

		inline Point2D T_L() const ;
		inline Point2D T_R() const ;
		inline Point2D D_L() const ;
		inline Point2D D_R() const ;
		inline Point2D Center() const ;

		void collision();

		Sprite& stop();
		Sprite& update();
		Sprite& operator<(const Vector2D& vec);
		Sprite& operator<<(const Vector2D& vec);

		Sprite& operator*(Sprite& spr);
		Sprite& operator/(Sprite& spr);

		inline Sprite& moveX(double X);
		inline Sprite& moveY(double Y);

		inline Sprite& operator<(const Sprite& spr);
		inline Sprite& operator<<(const Sprite& spr);
	};

	class GameScreen : public fcmd::Screen {
		
		std::vector<Sprite*> objects_m;

	public:

		int row_m; 
		int col_m;

		GameScreen();
		GameScreen(int _width, int _height);

		GameScreen(cmv::AsciiImage& img);
		GameScreen(cmv::AsciiImage&& img);
		GameScreen(cmv::AsciiImage&& img, double scale);

		GameScreen& operator=(GameScreen&& scr);
		GameScreen& operator=(Screen&) = delete;

		GameScreen& operator *(Sprite& spr);
		GameScreen& operator *(Sprite&& spr);
		GameScreen& operator /(Sprite& spr);

		virtual GameScreen& display() override;
		
		inline void pixel(int col, int row, char colour);

		GameScreen& print(const Sprite& spr);
		GameScreen& print(const Vector2D& line);
		GameScreen& print(const Point2D& p);

	};

	class KeyHandler {

		int ctl_char;
		std::function<void()> func_m;

	public:

		KeyHandler() : ctl_char(0), func_m(nullptr) {};
		KeyHandler(const KeyHandler& cp_h) : ctl_char(cp_h.ctl_char), func_m(cp_h.func_m) {};
		KeyHandler(int key, std::function<void()> func) : ctl_char(key), func_m(func) {};

		KeyHandler& operator+(const KeyHandler& oth) {(*this)(); oth(); return *this;};

		void operator()() const;
	};

	class KeyProcessor {

		std::vector<KeyHandler> vec_m;

	public:

		KeyProcessor(const std::initializer_list<KeyHandler> &list) : vec_m(list) {};

		void operator()() {for (const KeyHandler& handler : vec_m) handler();};
	};

}

#endif /*  ASCII_SPRITE_LIBRARY  */