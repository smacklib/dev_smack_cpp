$buildDir = ".\build"

if (Test-Path $buildDir) {
    Remove-Item -Path $buildDir -Recurse -Force
}
New-Item -ItemType Directory -Path $buildDir | Out-Null
Set-Location $buildDir

# Generation of tests and examples is enabled here.
cmake -DENABLE_TESTS=ON -DENABLE_EXAMPLES=ON -G "Visual Studio 18 2026" ..
