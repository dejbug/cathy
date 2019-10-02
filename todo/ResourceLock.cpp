#include "util.h"
#include <assert.h>

util::ResourceLock::ResourceLock(WORD id, HINSTANCE hi)
{
	HRSRC res_find_handle = FindResource(hi, MAKEINTRESOURCE(id), RT_RCDATA);
	assert(!!res_find_handle);

	size = (size_t) SizeofResource(hi, res_find_handle);

	HGLOBAL res_load_handle = LoadResource(hi, res_find_handle);
	assert(!!res_load_handle);

	LPVOID res_lock_handle = LockResource(res_load_handle);
	assert(!!res_lock_handle);

	data = (char const *) res_lock_handle;
}

util::ResourceLock::~ResourceLock()
{
	// if (data) FreeResource(data);
}
