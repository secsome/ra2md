#pragma once

#include <Always.h>

#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

template<class T>
class IndexClass
{
public:
	IndexClass();

	CLASS_NOCOPY(IndexClass);
	CLASS_CANMOVE(IndexClass);

	~IndexClass();

	bool Add_Index(int id, T data);
	bool Remove_Index(int id);
	bool Is_Present(int id) const;
	int Count() const;
	T Fetch_Index(int id) const;
	void Clear();

private:
	
	struct NodeElement 
	{
		int ID;
		T Data;
	};

	NodeElement* IndexTable;
	int IndexCount;
	int IndexSize;
	bool IsSorted;
	NodeElement const* Archive;

	bool Increase_Table_Size(int amount);
	bool Is_Archive_Same(int id) const;
	void Invalidate_Archive(void);
	
	void Set_Archive(NodeElement const* node);
	NodeElement const* Search_For_Node(int id) const;

	static int __cdecl search_compfunc(void const* ptr1, void const* ptr2);
};

template<class T>
IndexClass<T>::IndexClass() :
	IndexTable{ 0 },
	IndexCount{ 0 },
	IndexSize{ 0 },
	IsSorted{ false },
	Archive{ 0 }
{
	Invalidate_Archive();
}

template<class T>
IndexClass<T>::~IndexClass()
{
	Clear();
}

template<class T>
void IndexClass<T>::Clear()
{
	delete[] IndexTable;
	IndexTable = 0;
	IndexCount = 0;
	IndexSize = 0;
	IsSorted = false;
	Invalidate_Archive();
}

template<class T>
bool IndexClass<T>::Increase_Table_Size(int amount)
{
	if (amount < 0) 
		return false;

	NodeElement* table = new NodeElement[IndexSize + amount];
	if (table != nullptr)
	{
		for (int index = 0; index < IndexCount; ++index)
			table[index] = std::move(IndexTable[index]);

		delete[] IndexTable;
		IndexTable = table;
		IndexSize += amount;
		Invalidate_Archive();

		return true;
	}

	return false;
}

template<class T>
int IndexClass<T>::Count() const
{
	return IndexCount;
}

template<class T>
bool IndexClass<T>::Is_Present(int id) const
{
	if (IndexCount == 0)
		return false;

	if (Is_Archive_Same(id))
		return true;

	NodeElement const* nodeptr = Search_For_Node(id);

	if (nodeptr != nullptr) 
	{
		((IndexClass<T> *)this)->Set_Archive(nodeptr);
		return true;
	}

	return false;
}

template<class T>
T IndexClass<T>::Fetch_Index(int id) const
{
	return Is_Present(id) ? Archive->Data : T();
}

template<class T>
bool IndexClass<T>::Is_Archive_Same(int id) const
{
	return Archive != 0 && Archive->ID == id;
}

template<class T>
void IndexClass<T>::Invalidate_Archive()
{
	Archive = 0;
}

template<class T>
void IndexClass<T>::Set_Archive(NodeElement const* node)
{
	Archive = node;
}

template<class T>
bool IndexClass<T>::Add_Index(int id, T data)
{
	if (IndexCount + 1 > IndexSize) 
	{
		if (!Increase_Table_Size(IndexSize == 0 ? 10 : IndexSize)) 
			return false;
	}

	IndexTable[IndexCount].ID = id;
	IndexTable[IndexCount].Data = data;
	++IndexCount;
	IsSorted = false;

	return true;
}

template<class T>
bool IndexClass<T>::Remove_Index(int id)
{
	int found_index = -1;
	for (int index = 0; index < IndexCount; ++index) 
	{
		if (IndexTable[index].ID == id) 
		{
			found_index = index;
			break;
		}
	}

	if (found_index != -1)
	{
		for (int index = found_index + 1; index < IndexCount; ++index)
			IndexTable[index - 1] = std::move(IndexTable[index]);
		
		--IndexCount;

		NodeElement fake;
		fake.ID = 0;
		fake.Data = T();
		IndexTable[IndexCount] = fake;		// zap last (now unused) element

		Invalidate_Archive();
		return true;
	}

	return false;
}

template<class T>
int __cdecl IndexClass<T>::search_compfunc(void const* ptr1, void const* ptr2)
{
	if (*(int const*)ptr1 == *(int const*)ptr2) {
		return(0);
	}
	if (*(int const*)ptr1 < *(int const*)ptr2) {
		return(-1);
	}
	return(1);
}

template<class T>
typename IndexClass<T>::NodeElement const* IndexClass<T>::Search_For_Node(int id) const
{
	if (IndexCount == 0)
		return 0;

	if (!IsSorted) 
	{
		qsort(&IndexTable[0], IndexCount, sizeof(IndexTable[0]), search_compfunc);
		((IndexClass<T> *)this)->Invalidate_Archive();
		((IndexClass<T> *)this)->IsSorted = true;
	}

	NodeElement node;
	node.ID = id;
	return((NodeElement const*)bsearch(&node, &IndexTable[0], IndexCount, sizeof(IndexTable[0]), search_compfunc));
}