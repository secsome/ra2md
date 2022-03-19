#include <Pipe.h>

Pipe::Pipe() noexcept
	: ChainTo{ nullptr }
	, ChainFrom{ nullptr }
{
}

Pipe::~Pipe()
{
}

int Pipe::Flush()
{
	return ChainTo != nullptr ? ChainTo->Flush() : 0;
}

int Pipe::End()
{
	return Flush();
}

void Pipe::Put_To(Pipe* pipe)
{
	if (ChainTo != pipe) 
	{
		if (pipe != nullptr && pipe->ChainFrom != nullptr) 
		{
			pipe->ChainFrom->Put_To(nullptr);
			pipe->ChainFrom = nullptr;
		}

		if (ChainTo != nullptr)
		{
			ChainTo->ChainFrom = nullptr;
			ChainTo->Flush();
		}

		ChainTo = pipe;
		if (ChainTo != nullptr)
			ChainTo->ChainFrom = this;
	}
}

int Pipe::Put(const void* source, int slen)
{
	return ChainTo != nullptr ? ChainTo->Put(source, slen) : slen;
}
