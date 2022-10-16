Build Instructions
==================

Dependencies
------------

Getting The Code
----------------

:code:`cppql` can be cloned from GitHub. Note that since the repository contains submodules, you have to pass an
additional parameter.

```cmd
> git clone https://github.com/TimZoet/cppql.git source --recurse-submodules
```

Configuration
-------------

There are several CMake configuration variables. Below an overview.

`CPPQL_BIND_ZERO_BASED_INDICES` can be set to `ON` or `OFF`. When enabled, the indices passed to the various `bind`
methods this library provides as wrappers around the C functions become 0-based. Note that this of course does not apply
to any of the C functions, should you still use those.

`CPPQL_SHUTDOWN_DEFAULT_OFF` can be set to `ON` or `OFF`. When enabled, the database connection wrapper will no longer
call `sqlite3_shutdown` on destruction. This can be useful when opening multiple databases, both for performance reasons
and because `sqlite3_shutdown` is not thread safe.

`BUILD_TESTS` can be set to `ON` or `OFF`. When enabled, an application containing tests is created.

Windows
-------

Linux
-----

Running Tests
-------------

When `BUILD_TESTS` is enabled, an application called `cppql_test` is created (as well as some other targets the
application depends on). Building this application requires `CPPQL_BIND_ZERO_BASED_INDICES` to be enabled as well. In
order to run all tests, just run the application without any parameters. If :code:`cppql` was built successfully, they
should all run without issue. The tests are written using the [BetterTest](https://github.com/TimZoet/BetterTest)
library.
