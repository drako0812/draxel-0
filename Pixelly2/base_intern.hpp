#pragma once

#include <string>
#include <functional>
#include <optional>

void pxly_assert(bool condition, std::optional<std::function<const std::string()>> func = std::nullopt);
