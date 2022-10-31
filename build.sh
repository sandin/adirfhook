#ABI=armeabi-v7a
ABI=arm64-v8a

cmake \
  -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=$ABI \
  -DANDROID_NATIVE_API_LEVEL=19 \
  -DCMAKE_MAKE_PROGRAM=make \
  -DANDROID_STL=c++_static \
  -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=. \
  -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=. \
  -DCMAKE_BUILD_TYPE=Debug \
  ..

cmake --build .
