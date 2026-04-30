#include "Paths.h"

#include <filesystem>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include <climits>
#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#include <climits>
#endif

namespace fs = std::filesystem;

static fs::path g_assetRoot;

void pumaInitAssetRoot()
{
#if defined(__APPLE__)
	char buf[PATH_MAX];
	uint32_t sz = sizeof(buf);
	if (_NSGetExecutablePath(buf, &sz) == 0)
		g_assetRoot = fs::path(buf).parent_path();
	else
		g_assetRoot = fs::current_path();
#elif defined(_WIN32)
	char buf[MAX_PATH];
	DWORD n = GetModuleFileNameA(nullptr, buf, MAX_PATH);
	if (n > 0)
		g_assetRoot = fs::path(std::string(buf, n)).parent_path();
	else
		g_assetRoot = fs::current_path();
#else
	char buf[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
	if (len != -1)
	{
		buf[len] = '\0';
		g_assetRoot = fs::path(buf).parent_path();
	}
	else
		g_assetRoot = fs::current_path();
#endif
}

std::string pumaResolveAsset(const std::string& relative)
{
	if (relative.empty())
		return g_assetRoot.string();
	return (g_assetRoot / relative).string();
}
