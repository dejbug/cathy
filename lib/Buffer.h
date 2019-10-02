#pragma once
#include "Defines.h"
#include "Common.h"

BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (mem)

template <class DATA> class Buffer
{
protected:
	size_t size;
	DATA * data;
public:
	Buffer () {
		size = 0;
		data = 0;
	}

	~Buffer () {
		Delete ();
	}
public:
	inline operator size_t () const {
		return size;
	}

	inline operator DATA * () const {
		return data;
	}
public:
	bool Create (size_t size) {
		Delete ();
		if (!size) return false;
		data = new DATA [size];
		if (!data) return false;;
		data [0] = 0;
		this->size = size;
		return true;
	}

	bool Delete () {
		if (data) delete data;
		data = 0;
		size = 0;
		return true;
	}

	bool Fill (int datum) {
		if (!data || !size) return false;
		memset (data, datum, size);
		return true;
	}
};

END_NAMESPACE (mem)
END_NAMESPACE (lib)
