#include "Browser.h"

#include <cstdlib>
#include <filesystem>

namespace attitude {

bool openInBrowser(const std::string& path) {
    // Absolute path so the launcher does not depend on the working directory.
    const std::string absolute = std::filesystem::absolute(path).string();

#if defined(_WIN32)
    const std::string cmd = "start \"\" \"" + absolute + "\"";
#elif defined(__APPLE__)
    const std::string cmd = "open \"" + absolute + "\"";
#else
    // Redirect output: xdg-open is chatty when no browser is available.
    const std::string cmd = "xdg-open \"" + absolute + "\" >/dev/null 2>&1";
#endif
    return std::system(cmd.c_str()) == 0;
}

} // namespace attitude
