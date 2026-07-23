$buildDir = ".\build"

if (Test-Path $buildDir) {
    Remove-Item -Path $buildDir -Recurse -Force
}
New-Item -ItemType Directory -Path $buildDir | Out-Null

# Classic cmake project generation.
# Set-Location $buildDir
# cmake -DENABLE_TESTS=ON -DENABLE_EXAMPLES=ON -G "Visual Studio 18 2026" ..

# New CMake presets project generation.
# This is based on the CMakePresets.json file in the root of the project.
cmake --preset windows-vs
Set-Location $buildDir

#
# Sample commands (this is a multi-configuration generator, so you can build Debug or Release):
# * Build: cmake --build . --config Debug/Release
# * Test: ctest -C Debug/Release
# * Clean: cmake --build . --config Debug/Release --target clean
# * Run: cmake --build . --config Debug/Release --target run
#
