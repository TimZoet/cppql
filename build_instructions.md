# Build Instructions

## Dependencies

The following Conan packages are always required:

1. [pyreq](https://github.com/TimZoet/pyreq)
2. [common](https://github.com/TimZoet/common)
3. [sqlite3](https://www.sqlite.org/index.html)

Depending on enabled options, you may also need:

1. [bettertest](https://github.com/TimZoet/BetterTest)

## Getting The Code

This project uses git submodules. Cloning therefore requires an additional flag:

```cmd
git clone https://github.com/TimZoet/cppql.git source --recurse-submodules
```

## Exporting to Conan

To export the `cppql` package to your local Conan cache:

```cmd
cd source
conan export . user/channel
```

## Configuration

There are several CMake configuration variables and matching Conan options. Below an overview.

| CMake | Conan | Type (Default) | Description |
|---|---|---|---|
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

## Running Tests

When `BUILD_TESTS` is enabled, an application called `cppql_test` is created. Building this application requires `CPPQL_BIND_ZERO_BASED_INDICES` to be enabled as well. In order to run all tests, just run the application without any
parameters. If `cppql` was built successfully, they should all run without issue. The tests are written using the
[BetterTest](https://github.com/TimZoet/BetterTest) library.
