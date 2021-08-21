# Development setup for Mac.
#
# Note that this switches to a gcc that is installed
# by 'brew install gcc' since xcode does not and will
# never support std::filesytem on my ancient Mac.
buildDir=build

rm -rf $buildDir
mkdir $buildDir
cd $buildDir

# Generation of tests and examples is enabled for.
cmake \
    -DENABLE_TESTS=ON \
    -DENABLE_EXAMPLES=ON \
    -DCMAKE_C_COMPILER=/usr/local/Cellar/gcc/11.2.0/bin/gcc-11 \
    -DCMAKE_CXX_COMPILER=/usr/local/Cellar/gcc/11.2.0/bin/g++-11 \
    ..
