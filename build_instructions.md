# Build Instructions

## Getting The Code

To retrieve the code from GitHub:

```cmd
git clone https://github.com/TimZoet/cppql.git source
```

## Exporting to Conan

To export the `cppql` package to your local Conan cache:

```cmd
conan export --user timzoet --channel v1.0.0 source
```

Make sure to update the channel when the version is different.

## Configuration

There are several CMake configuration variables and matching Conan options. Below an overview.

| CMake | Conan | Type (Default) | Description |
|---|---|---|---|
| BUILD_MANUAL | build_manual | bool (false) | When enabled, a target to build the manual is created. |
| BUILD_TESTS | build_tests | bool (false) | When enabled, an application containing tests is created. |
| CPPQL_BIND_ZERO_BASED_INDICES | zero_based_indices | bool (true) | When enabled, the indices passed to the various `bind` methods this library provides as wrappers around the C functions become 0-based. Note that this of course does not apply to any of the C functions, should you still use those. |
| CPPQL_SHUTDOWN_DEFAULT_OFF | shutdown_default_off | bool (false) | When enabled, the database connection wrapper will no longer call `sqlite3_shutdown` on destruction. This can be useful when opening multiple databases, both for performance reasons and because `sqlite3_shutdown` is not thread safe. |

To do e.g. a release build with Visual Studio 2022, you can run the following:

```cmd
mkdir build_release
cd build_release
conan install -pr:h=vs2022-release -pr:b=vs2022-release -o build_tests=True --build=missing ../source
cmake -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake ..\source
```

## Building Tests

Invoke `conan install`:

```cmd
conan install -pr:h=source/buildtools/profiles/cppql-test-vs2022-release -pr:b=source/buildtools/profiles/cppql-test-vs2022-release -s build_type=Release --build=missing -of=build source
```

Then generate and build with CMake:

```cmd
cmake -S source -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE:FILEPATH=conan_toolchain.cmake
cmake --build build --config Release
```

Finally, run the test application:

```cmd
cd build/bin/tests
.\cppql_test
```
