#pragma once

#include <string>

namespace attitude {

/**
 * Opens a local file in the user's default web browser.
 * Returns false if the platform launcher could not be run; the caller should
 * then print the path so the user can open it manually.
 */
bool openInBrowser(const std::string& path);

} // namespace attitude
