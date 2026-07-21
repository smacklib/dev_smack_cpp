/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests.
 *
 * Copyright © 2021-2026 Michael Binz
 */

#include <filesystem>
#include <string>
#include <gtest/gtest.h>
#include "test_common.hpp"

#if defined(__APPLE__)
#  include <mach-o/dyld.h>
#elif defined(_WIN32)
#  include <windows.h>
#endif

std::string smack::test::g_gtestExecutableName;

static std::filesystem::path executablePath()
{
#if defined(__APPLE__)
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::string buf(size, '\0');
    _NSGetExecutablePath(buf.data(), &size);
    return std::filesystem::canonical(buf);
#elif defined(_WIN32)
    wchar_t buf[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    return std::filesystem::path(buf, buf + len);
#else
    return std::filesystem::read_symlink("/proc/self/exe");
#endif
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    if ( argc > 0 )
        smack::test::g_gtestExecutableName = argv[0];

    std::filesystem::current_path(executablePath().parent_path());

    return RUN_ALL_TESTS();
}
