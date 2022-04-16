#include<engine.hpp>

#define do_movement row_m -= resultant_m.y_m; col_m += resultant_m.x_m;
#define undo_movement row_m += resultant_m.y_m; col_m -= resultant_m.x_m;

using namespace sge_impl;

template<typename T>
inline int znak(T num) {
	if (num > 0)
		return 1;
	else if (num == 0)
		return 0;
	else 
		return -1;
}

template<typename T>
inline int Dznak(T num) { 			// NO ZERO, ONLY DEFENITE SIGN
	if (num >= 0)
		return 1;
	else 
		return -1;
}

template<typename T>
bool between(T l, T m, T r) {
	if (r > l)
		if (m > l && m < r) return true;
		else return false;
	else 
		if (m > r && m < l)	return true;
		else return false;
}

Sprite::Sprite(int _col, int _row, int _w, int _h, state _kind) : Sprite(_col, _row, AsciiImage(_w, _h), _kind, 1.0) {}
Sprite::Sprite(int _col, int _row, int _w, int _h) : Sprite(_col, _row, AsciiImage(_w, _h), barier, 1.0) {}

Sprite::Sprite(int _col, int _row, const uint8_t* data_ptr, state _kind) : AsciiImage(data_ptr, 0), mass_m(1.0), resultant_m(), col_m(_col), row_m(_row), kind_m(_kind)  {} 
Sprite::Sprite(int _col, int _row, std::string filename, state _kind) : AsciiImage(filename, 0), mass_m(1.0), resultant_m(), col_m(_col), row_m(_row), kind_m(_kind)  {}
Sprite::Sprite(int _col, int _row, std::string filename, state _kind, double mass) : AsciiImage(filename, 0), mass_m(mass), resultant_m(), col_m(_col), row_m(_row), kind_m(_kind)  {}

Sprite::Sprite(int _col, int _row, AsciiImage& img, state _kind, double mass) : mass_m(mass), AsciiImage(img), resultant_m(), col_m(_col), row_m(_row), kind_m(_kind) {}
Sprite::Sprite(int _col, int _row, AsciiImage&& img, state _kind, double mass) : mass_m(mass), AsciiImage(std::move(img)), resultant_m(), col_m(_col), row_m(_row), kind_m(_kind) {}

Sprite & Sprite::offset() {
	for(int i = 0; i < h; i++) {
		char left_reserved = bmp[i * w];
		memcpy(bmp + i * w, bmp + i * w + 1, w - 1);
		bmp[i * w + w - 1] = left_reserved;
	}
	return *this;
}

int Sprite::operator&(Sprite &enemy) {
	if (Down() < enemy.Top())					// down border
		return 0;
	else {
		if (Top() > enemy.Down())			// top border
			return 0;
		else {
			if (Right() < enemy.Left())			// right border
				return 0;
			else {
				if(Left() > enemy.Right())	// left border
					return 0;
				else
					return 1;
			}
		}

	}
}

Sprite& Sprite::stop() {

	resultant_m = Vector2D(0,0);

	return *this;
}

Sprite& Sprite::update() {

	switch (kind_m)
	{
	case character:
		collision();
		
	case background:
		do_movement
		break;

	case barier:
		break;

	default:
		report("ERROR: undefined update model");
		break;
	}
	return *this;
}

void Sprite::collision() {
	for (auto& spr : collisions_m) {
		Point2D location = (Center() & (spr->Center()));
		nextStepCollision(*spr, location.X, location.Y);		
	}
}

void Sprite::nextStepCollision(const Sprite& spr, const int X, const int Y) {

	Vector2D marker;

	switch (X)
	{
	case 1: {
		switch (Y)
		{
		case 1: {
			marker = resultant_m(D_L()).real();
			if (spr.TopSide() & marker) {
				if (Dznak(resultant_m.y_m) == -1) resultant_m.y_m = 0;
				row_m = spr.Top() - h - 1; 
			}
			else if (spr.RightSide() & marker) {
				if (Dznak(resultant_m.x_m) == -1) resultant_m.x_m = 0;
				col_m = spr.Right() + 1;
			}
			break;
		}
		
		case 0: {
			
			if (resultant_m.direction().Y == -1)
				marker = resultant_m(T_L()).real();
			else
				marker = resultant_m(D_L()).real();
			
			if (spr.RightSide() & marker) {
				if (Dznak(resultant_m.x_m) == -1) resultant_m.x_m = 0;
				col_m = spr.Right() + 1;
			}
			break;
		}

		case -1: {
			marker = resultant_m(T_L()).real();
			if (spr.DownSide() & marker) {
				if (Dznak(resultant_m.y_m) == 1) resultant_m.y_m = 0;
				row_m = spr.Down() + 1; 
			}
			else if (spr.RightSide() & marker) {
				if (Dznak(resultant_m.x_m) == -1) resultant_m.x_m = 0;
				col_m = spr.Right() + 1;
			}
			break;
		}
		
		default:
			report("ERROR: undefined collision location defenition error (X == 1)")
			break;
		}
		break;
	}

	case 0: {
		switch (Y)
		{
		case 1: {

			if (resultant_m.direction().X == -1)
				marker = resultant_m(D_R()).real();
			else
				marker = resultant_m(D_L()).real();

			if (spr.TopSide() & marker) {
				if (Dznak(resultant_m.y_m) == -1) resultant_m.y_m = 0;
				row_m = spr.Top() - h - 1; 
			}
			break;
		}
		
		case 0: {
			break;
		}

		case -1: {
			
			if (resultant_m.direction().X == -1)
				marker = resultant_m(T_R()).real();
			else
				marker = resultant_m(T_L()).real();

			if (spr.DownSide() & marker) {
				if (Dznak(resultant_m.y_m) == 1) resultant_m.y_m = 0;
				row_m = spr.Down() + 1; 
			}
			break;
		}
		
		default:
			report("ERROR: undefined collision location defenition error (X == 0)")
			break;
		}
		break;
	}

	case -1: {
		switch (Y)
		{
		case 1: {
			marker = resultant_m(D_R()).real();
			if (spr.TopSide() & marker) {
				if (Dznak(resultant_m.y_m) == -1) resultant_m.y_m = 0;
				row_m = spr.Top() - h - 1; 
			}
			else if (spr.LeftSide() & marker) {
				if (Dznak(resultant_m.x_m) == 1) resultant_m.x_m = 0;
				col_m = spr.Left() - w - 1;
			}
			break;
		}

		case 0: {
			
			if (resultant_m.direction().Y == -1)
				marker = resultant_m(T_R()).real();
			else
				marker = resultant_m(D_R()).real();

			if (spr.LeftSide() & marker) {
				if (Dznak(resultant_m.x_m) == 1) resultant_m.x_m = 0;
				col_m = spr.Left() - w - 1;
			}
			break;
		}

		case -1: {
			marker = resultant_m(T_R()).real();
			if (spr.DownSide() & marker) {
				if (Dznak(resultant_m.y_m) == 1) resultant_m.y_m = 0;
				row_m = spr.Down() + 1; 
			}
			else if (spr.LeftSide() & marker) {
				if (Dznak(resultant_m.x_m) == 1) resultant_m.x_m = 0;
				col_m = spr.Left() - w - 1;
			}
			break;
		}
		
		default:
			report("ERROR: undefined collision location defenition error (X == -1)")
			break;
		}
		break;
	}

	default:
		report("ERROR: undefined collision location defenition error (X == ?)")
		break;
	}

}


Sprite& Sprite::operator<<(const Vector2D& vec) {

	resultant_m += vec;

	return *this;
}

inline Sprite& Sprite::operator<<(const Sprite& spr) {

	resultant_m += spr.resultant_m;

	return *this;
}

Sprite& Sprite::operator<(const Vector2D& vec) {

	// resultant_m += vec;
	resultant_m.x_m -= std::min(abs(resultant_m.x_m * resultant_m.x_m * vec.x_m) + 0.3, abs(resultant_m.x_m)) * (resultant_m.x_m >= 0 ? 1 : -1);
	resultant_m.y_m -= std::min(abs(resultant_m.y_m * resultant_m.y_m * vec.y_m) + 0.3, abs(resultant_m.y_m)) * (resultant_m.y_m >= 0 ? 1 : -1);

	return *this;
}

inline Sprite& Sprite::operator<(const Sprite& spr) {
	return (*this) < spr.resultant_m;
}

Sprite& Sprite::operator*(Sprite& spr) {
	collisions_m.push_back(&spr);
	return *this;
}

Sprite& Sprite::operator/(Sprite& spr) {
	collisions_m.erase(std::find(collisions_m.begin(), collisions_m.end(), &spr));
	return *this;
}

double Sprite::Top() const { return row_m; }
double Sprite::Down() const { return row_m + double(h); }
double Sprite::Left() const { return col_m; }
double Sprite::Right() const { return col_m + double(w); }


inline Vector2D Sprite::TopSide() const { return Vector2D(T_L(), T_R()); }
inline Vector2D Sprite::DownSide() const { return Vector2D(D_L(), D_R()); }
inline Vector2D Sprite::LeftSide() const { return Vector2D(T_L(), D_L()); }
inline Vector2D Sprite::RightSide() const { return Vector2D(T_R(), D_R()); }


inline Point2D Sprite::T_L() const { return Point2D(Left(), Top()); }
inline Point2D Sprite::T_R() const { return Point2D(Right(), Top()); }
inline Point2D Sprite::D_L() const { return Point2D(Left(), Down()); }
inline Point2D Sprite::D_R() const { return Point2D(Right(), Down()); }
inline Point2D Sprite::Center() const { return (T_L() + Vector2D(w/2,h/2)); }

Point2D Point2D::operator&(const Point2D& p) const { return Point2D(znak(X - p.X), znak(p.Y - Y)); } // расположение второй точки относительно первой 

Point2D::Point2D(double x, double y) : X(x), Y(y) {}
inline int Point2D::in(const Sprite& spr) const { if (spr.col_m < X && X < spr.col_m + spr.w && spr.row_m > Y && Y > spr.row_m - spr.h) return 1; else return 0; }
bool Point2D::operator==(const Point2D& p) const { return (X == p.X) && (Y == p.Y); }

Point2D sge_impl::operator+(Point2D p, Vector2D vec) { return Point2D(p.X + vec.x_m, p.Y + vec.y_m);}

inline Vector2D Vector2D::operator()(Point2D p) { Vector2D v(*this); return v.setPoint(p); }

Vector2D::Vector2D() : start_m(Point2D(0,0)), x_m(0), y_m(0) {}
Vector2D::Vector2D(double x, double y) : start_m(Point2D(0,0)), x_m(x), y_m(y) {}
Vector2D::Vector2D(Point2D p) : start_m(0,0), x_m(p.X), y_m(p.Y) {}
Vector2D::Vector2D(Point2D p1, Point2D p2) : start_m(p1), x_m(), y_m() { x_m = p2.X - p1.X; y_m = p2.Y - p1.Y;}
Vector2D::Vector2D(double x, double y, Point2D p) : start_m(p), x_m(x), y_m(y) {}
Vector2D::Vector2D(double x1, double y1, double x2, double y2) : start_m(Point2D(0,0)), x_m(), y_m() { x_m = x2-x1; y_m = y2-y1; }

Vector2D Vector2D::operator-() const { Vector2D copy(*this); copy.x_m=-x_m; copy.y_m=-y_m; return copy; }

Vector2D Vector2D::operator*(const double c) const { Vector2D copy(*this); copy.x_m*=c; copy.y_m*=c; return copy; }
Vector2D Vector2D::operator/(const double c) const { Vector2D copy(*this); copy.x_m/=c; copy.y_m/=c; return copy; }
Vector2D Vector2D::operator+(const double c) const { Vector2D copy(*this); copy.x_m+=c; copy.y_m+=c; return copy; }
Vector2D Vector2D::operator-(const double c) const { Vector2D copy(*this); copy.x_m-=c; copy.y_m-=c; return copy; }
Vector2D Vector2D::operator+(const Vector2D& vec) const { Vector2D copy(*this); copy.x_m+=vec.x_m; copy.y_m+=vec.x_m; return copy; }
inline double Vector2D::operator*(const Vector2D& vec) const { return x_m * vec.x_m + y_m * vec.y_m; }
Vector2D Vector2D::operator-(const Vector2D& vec) const { Vector2D copy(*this); copy.x_m-=vec.x_m; copy.y_m-=vec.y_m; return copy; }

Vector2D& Vector2D::operator*=(const double c) { x_m*=c; y_m*=c; return *this; }
Vector2D& Vector2D::operator/=(const double c) { x_m/=c; y_m/=c; return *this; }
Vector2D& Vector2D::operator+=(const double c) { x_m+=c; y_m+=c; return *this; }
Vector2D& Vector2D::operator-=(const double c) { x_m-=c; y_m-=c; return *this; }
Vector2D& Vector2D::operator+=(const Vector2D& vec) { x_m+=vec.x_m; y_m+=vec.y_m; return *this; }
Vector2D& Vector2D::operator-=(const Vector2D& vec) { x_m-=vec.x_m; y_m-=vec.y_m; return *this; }

Vector2D Vector2D::real() {return Vector2D(x_m,-y_m,start_m);}


double Vector2D::absoluteAngle() const {
	double angle = acos(cos_angle(Vector2D(Point2D(1,0))));
	if ((end() & start()).Y == -1) angle = 2*M_PI - angle;
	return angle; 	
} 

double Vector2D::angle(const Vector2D& vec) const {
	double angle = vec.absoluteAngle() - absoluteAngle(); 
	if (angle > M_PI)
		angle -= 2*M_PI;
	else if (angle < -M_PI)
		angle =+ 2*M_PI;
	return angle;
}


inline Point2D Vector2D::start() const { return start_m; }
inline Point2D Vector2D::end() const { return Point2D(start_m.X + x_m, start_m.Y + y_m); }


bool Vector2D::operator&(const Vector2D& vec) const {

	if(Dznak(Vector2D(start_m, vec.start_m).angle(vec)) != Dznak(Vector2D(end(), vec.start_m).angle(vec))
									&&
	Dznak(Vector2D(vec.start_m, start_m).angle(*this)) != Dznak(Vector2D(vec.end(), start_m).angle(*this)))
		return true;
	
	return false;
}

inline double Vector2D::length() const { return sqrt(abs(x_m*x_m) + abs(y_m*y_m)); }
inline Point2D Vector2D::direction() const { return Point2D(znak(x_m), znak(y_m));}
inline double Vector2D::cos_angle(const Vector2D& vec) const { return ((*this) * vec) / length() / vec.length();}


inline Vector2D& Vector2D::setPoint(double x, double y) { start_m.X = x; start_m.Y = y; return *this;}
inline Vector2D& Vector2D::setPoint(Point2D p) { start_m = p; return *this; }


GameScreen::GameScreen() : Screen(), row_m(0), col_m(0) {}
GameScreen::GameScreen(int _width, int _height) : Screen(_width, _height, 0, 15), row_m(0), col_m(0) {}
GameScreen::GameScreen(cmv::AsciiImage& img) : Screen(img, 0, 15), row_m(0), col_m(0) {}
GameScreen::GameScreen(cmv::AsciiImage&& img) : Screen(std::move(img), 1.0 , 0, 15), row_m(0), col_m(0) {}
GameScreen::GameScreen(cmv::AsciiImage&& img, double scale) : Screen(std::move(img), 1.0 , 0, 15), row_m(0), col_m(0) {}

GameScreen& GameScreen::operator=(GameScreen&& scr) {
	pointer = scr.pointer;
    width_m = scr.width_m;
    height_m = scr.height_m;
    buffer = scr.buffer;
    scr.buffer = nullptr;
    buf_handler = scr.buf_handler;
    scr.buf_handler = nullptr;
    external_console = scr.external_console;
    scr.external_console = 0;
	objects_m.clear();
	std::copy(scr.objects_m.begin(), scr.objects_m.end(), objects_m.begin());
    scr.objects_m.clear();
	return *this;
}

GameScreen& GameScreen::print(const Sprite& spr) {
    int l_offset = 0, r_offset = spr.w, u_offset = 0, d_offset = spr.h;
    int row = spr.row_m + row_m, col = spr.col_m - col_m;
	

    if (col > (width_m - 1) || col + spr.w < 0 || row + spr.h < 0 || row > (height_m - 1)) return *this;
    
    if (col < 0) l_offset = 0 - col;
    if (col >= width_m - 1 - spr.w) r_offset = (width_m - col);
    
    if (row < 0) u_offset = 0 - row;
    if (row + spr.h > height_m - 1) d_offset = (height_m - row);

    if (spr.kind_m == character) {	// yep transparency
        for (int _h = u_offset; _h < d_offset; _h++)
            for (int _w = l_offset; _w < r_offset; _w++)
                if(spr(_h, _w) != ' ')
                    pixel(col + _w, row + _h, spr(_h, _w));
    } else {
        for (int _h = u_offset; _h < d_offset; _h++)
            memcpy(buffer + (row + _h) * width_m + col + l_offset, &spr(_h, l_offset), spr.w - l_offset - (r_offset == spr.w ? 0 : r_offset));
    }
    return *this;
}

GameScreen& GameScreen::print(const Point2D& p) {
	if (!(between(col_m, int(p.X), col_m + width_m) && between(row_m, int(p.Y), row_m + height_m)))
		pixel(p.X - col_m, p.Y - row_m, '@');
	return *this;
}


GameScreen& GameScreen::print(const Vector2D& line) {
	Point2D start = line.start(), end = line.end();
	double multiplier = line.x_m != 0 ? line.y_m / line.x_m : line.y_m;
	// double znak = multiplier >= 0 ? 1.0 : -1.0, amultiplier = abs(multiplier); 
	if (start.X > end.X)
		std::swap(start, end);
	for (int x = 0; x <= end.X - start.X; x++)
		// for (double y = (x - znak) * multiplier; y <= x * multiplier; y++)
			// pixel(start.X + x, start.Y + y,'o');
			pixel(start.X + x, start.Y + x * multiplier,'o');
	return *this;
}

inline void GameScreen::pixel(int col, int row, char colour) {
	// col -= col_m, row += row_m;
	if (col > width_m) return;
	if (col < 0) return;

	if (row < 0) return;
	if (row > height_m) return;

    buffer[row * width_m + col] = colour;
}

GameScreen& GameScreen::operator*(Sprite& spr) {

	objects_m.push_back(&spr);

	return *this;
}

GameScreen& GameScreen::operator/(Sprite& spr) {

	objects_m.erase(std::find(objects_m.begin(), objects_m.end(), &spr));

	return *this;
}

GameScreen& GameScreen::operator*(Sprite&& spr) {

	Sprite* spr_p = new Sprite(spr.col_m, spr.row_m, std::move(cmv::AsciiImage(spr)), spr.kind_m, spr.mass_m);

	objects_m.push_back(spr_p);

	return *this;
}

GameScreen& GameScreen::display() {

	for (auto i : objects_m)
		print(i->update());

	fcmd::Screen::display();
	clear();

	return *this;
}

void KeyHandler::operator()() const { if (GetKeyState(ctl_char) & 0x8000) func_m(); };