#pragma once

#include <optional>
#include <string_view>

#include <spdlog/common.h>

namespace sispop::log {

using Level = spdlog::level::level_enum;

/// Returns a string representation of the log level (e.g. "trace", "warning")
std::string_view to_string(Level lvl);

/// Takes a log level string (case-insensitive) and converts it to a Level value.  Throws
/// std::invalid_argument if the string isn't a valid log level.  Valid strings are:
/// - "trace"
/// - "debug"
/// - "info"
/// - "warn" or "warning"
/// - "error" or "err"
/// - "critical"
/// - "none" or "off"
Level level_from_string(std::string level);

}  // namespace sispop::log
