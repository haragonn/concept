#ifndef INCLUDE_IDEA_IDEACOLOR_H
#define INCLUDE_IDEA_IDEACOLOR_H

#include "ideaUtility.h"

struct Color{
	union{
		struct{
			float r;
			float g;
			float b;
			float a;
		};

		float v[4];
	};

	Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f){}
	Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a){}

	float& operator[](const int idx)
	{
		Assert(idx < 2);

		return v[idx];
	}

	Color& operator =(const Color& c)
	{
		r = c.r;
		g = c.g;
		b = c.b;
		a = c.a;

		return *this;
	}

	Color operator +(const Color& c)const
	{
		return Color(r + c.r, g + c.g, b + c.b, a);
	}

	Color operator +=(const Color& c)
	{
		r += c.r;
		g += c.g;
		b += c.b;

		return *this;
	}

	Color operator -(const Color& c)const
	{
		return Color(r - c.r, g - c.g, b - c.b, a);
	}

	Color operator -=(const Color& c)
	{
		r -= c.r;
		g -= c.g;
		b -= c.b;

		return *this;
	}

	Color operator *(float f)const
	{
		return Color(r * f, g * f, b * f, a);
	}

	Color ReplaceAlpha(float _a)const
	{
		return Color(r, g, b, _a);
	}

};

namespace ideaColor{
	const Color CLEAR = Color(0.0f, 0.0f, 0.0f, 0.0f);
	const Color WHITE = Color(1.0f, 1.0f, 1.0f, 1.0f);
	const Color BLACK = Color(0.0f, 0.0f, 0.0f, 1.0f);
	const Color GRAY = Color(0.5f, 0.5f, 0.5f, 1.0f);
	const Color SILVER = Color(0.753f, 0.753f, 0.753f, 1.0f);
	const Color RED = Color(1.0f, 0.0f, 0.0f, 1.0f);
	const Color GREEN = Color(0.0f, 1.0f, 0.0f, 1.0f);
	const Color BLUE = Color(0.0f, 0.0f, 1.0f, 1.0f);
	const Color MAGENTA = Color(1.0f, 0.0f, 1.0f, 1.0f);
	const Color CYAN = Color(0.0f, 1.0f, 1.0f, 1.0f);
	const Color YELLOW = Color(1.0f, 0.92f, 0.002f, 1.0f);
	const Color PINK = Color(1.0f, 0.753f, 0.796f, 1.0f);
	const Color ORANGE = Color(1.0f, 0.647f, 0.0f, 1.0f);
	const Color MIDNIGHTBLUE = Color(0.098f, 0.098f, 0.439f, 1.0f);
	const Color SOFT_RED = Color(0.8745f, 0.1177f, 0.3529f, 1.0f);
	const Color SOFT_GREEN = Color(0.1765f, 0.7137f, 0.4863f, 1.0f);
	const Color SOFT_BLUE = Color(0.2118f, 0.7725f, 0.9529f, 1.0f);
	const Color SOFT_YELLOW = Color(0.9098f, 0.8784f, 0.3608f, 1.0f);
	const Color SOFT_BLACK = Color(0.1373f, 0.1333f, 0.1961f, 1.0f);
}

#endif	// #ifndef INCLUDE_IDEA_IDEACOLOR_H
