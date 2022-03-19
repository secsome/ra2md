#include <Straw.h>

Straw::Straw() noexcept
	: ChainTo{ nullptr }
	, ChainFrom{ nullptr }
{
}

Straw::~Straw()
{
}

void Straw::Get_From(Straw* straw)
{
	if (ChainTo != straw)
	{
		if (straw != nullptr && straw->ChainFrom != nullptr)
		{
			straw->ChainFrom->Get_From(nullptr);
			straw->ChainFrom = nullptr;
		}

		if (ChainTo != nullptr)
			ChainTo->ChainFrom = nullptr;

		ChainTo = straw;
		if (ChainTo != nullptr)
			ChainTo->ChainFrom = this;
	}
}

int Straw::Get(void* buffer, int length)
{
	return ChainTo != nullptr ? ChainTo->Get(buffer, length) : 0;
}
