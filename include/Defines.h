#pragma once

#include <Always.h>

typedef int LEPTON;


struct CELL
{
	explicit CELL(short x = 0, short y = 0)
		: X{ x }
		, Y{ y }
	{}

	CLASS_CANDEFAULTCOPY(CELL);
	CLASS_CANMOVE(CELL);

	CELL operator+(const CELL& another) const
	{
		CELL ret(*this);

		ret -= another;

		return ret;
	}

	CELL operator-(const CELL& another) const
	{
		CELL ret(*this);

		ret -= another;

		return ret;
	}

	CELL& operator+=(const CELL& another)
	{
		this->X += another.X;
		this->Y += another.Y;

		return *this;
	}

	CELL& operator-=(const CELL& another)
	{
		this->X -= another.X;
		this->Y -= another.Y;

		return *this;
	}

	std::strong_ordering operator<=>(const CELL& another) const
	{
		if (this->X < another.X)
			return std::strong_ordering::less;
		else if (this->X > another.X)
			return std::strong_ordering::greater;
		else if (this->Y < another.Y)
			return std::strong_ordering::less;
		else if (this->Y > another.Y)
			return std::strong_ordering::greater;
		else
			return std::strong_ordering::equal;
	}

	short X;
	short Y;
};

struct COORDINATE
{
	explicit COORDINATE(int x = 0, int y = 0, int z = 0)
		: X{ x }
		, Y{ y }
		, Z{ z }
	{}

	CLASS_CANDEFAULTCOPY(COORDINATE);
	CLASS_CANMOVE(COORDINATE);

	COORDINATE operator+(const COORDINATE& another) const
	{
		COORDINATE ret(*this);

		ret += another;

		return ret;
	}

	COORDINATE operator-(const COORDINATE& another) const
	{
		COORDINATE ret(*this);

		ret -= another;

		return ret;
	}

	COORDINATE& operator+=(const COORDINATE& another)
	{
		this->X += another.X;
		this->Y += another.Y;
		this->Z += another.Z;

		return *this;
	}

	COORDINATE& operator-=(const COORDINATE& another)
	{
		this->X -= another.X;
		this->Y -= another.Y;
		this->Z -= another.Z;

		return *this;
	}

	std::strong_ordering operator<=>(const COORDINATE& another) const
	{
		if (this->X < another.X)
			return std::strong_ordering::less;
		else if (this->X > another.X)
			return std::strong_ordering::greater;
		else if (this->Y < another.Y)
			return std::strong_ordering::less;
		else if (this->Y > another.Y)
			return std::strong_ordering::greater;
		else if (this->Z < another.Z)
			return std::strong_ordering::less;
		else if (this->Z > another.Z)
			return std::strong_ordering::greater;
		else
			return std::strong_ordering::equal;
	}

	int X;
	int Y;
	int Z;
};