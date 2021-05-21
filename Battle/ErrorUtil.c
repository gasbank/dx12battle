#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>

void ThrowIfFailed(HRESULT hr)
{
#if defined(_DEBUG)
	if (hr != S_OK)
	{
		MessageBoxA(NULL, "Function call failed", "Error", MB_OK | MB_ICONERROR);
	}
#endif
}
