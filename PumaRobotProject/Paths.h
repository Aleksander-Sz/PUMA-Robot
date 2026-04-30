#pragma once

#include <string>

// Call once at program start (before loading shaders/models). Resolves assets
// next to the executable (matches CMake POST_BUILD copy layout).
void pumaInitAssetRoot();

std::string pumaResolveAsset(const std::string& relative);
