from conans import ConanFile

class CppqlConan(ConanFile):
    ############################################################################
    ## Package info.                                                          ##
    ############################################################################
    
    name = "cppql"
    
    description = "C++ sqlite library."
    
    url = "https://github.com/TimZoet/cppql"

    ############################################################################
    ## Settings.                                                              ##
    ############################################################################

    python_requires = "pyreq/1.0.0@timzoet/stable"
    
    python_requires_extend = "pyreq.BaseConan"
    
    options = {
        "zero_based_indices": [True, False],
        "shutdown_default_off": [True, False]
    }
    
    default_options = {
        "zero_based_indices": True,
        "shutdown_default_off": False
    }
    
    ############################################################################
    ## Base methods.                                                          ##
    ############################################################################
    
    def set_version(self):
        base = self.python_requires["pyreq"].module.BaseConan
        base.set_version(self, "modules/cppql-core/cppql_version_string.cmake", "CPPQL_VERSION")
    
    def init(self):
        base = self.python_requires["pyreq"].module.BaseConan
        self.generators = base.generators + self.generators
        self.settings = base.settings + self.settings
        self.options = {**base.options, **self.options}
        self.default_options = {**base.default_options, **self.default_options}
    
    ############################################################################
    ## Building.                                                              ##
    ############################################################################
    
    def export_sources(self):
        self.copy("CMakeLists.txt")
        self.copy("license")
        self.copy("readme.md")
        self.copy("cmake/*")
        self.copy("modules/*")
    
    def config_options(self):
        base = self.python_requires["pyreq"].module.BaseConan
        if self.settings.os == "Windows":
            del self.options.fPIC
    
    def requirements(self):
        base = self.python_requires["pyreq"].module.BaseConan
        base.requirements(self)
        
        self.requires("common/1.0.0@timzoet/stable")
        self.requires("sqlite3/3.37.0")
        
        if self.options.build_tests:
            self.requires("bettertest/1.0.0@timzoet/stable")
    
    def package_info(self):
        self.cpp_info.components["core"].libs = ["cppql-core"]
        self.cpp_info.components["core"].requires = ["common::common", "SQLite::SQLite3"]
        
        self.cpp_info.components["typed"].libs = ["cppql-typed"]
        self.cpp_info.components["typed"].requires = ["core"]
    
    def generate(self):
        base = self.python_requires["pyreq"].module.BaseConan
        
        tc = base.generate_toolchain(self)
        
        if self.options.zero_based_indices:
            tc.variables["CPPQL_BIND_ZERO_BASED_INDICES"] = True
        if self.options.shutdown_default_off:
            tc.variables["CPPQL_SHUTDOWN_DEFAULT_OFF"] = True
        
        tc.generate()
        
        deps = base.generate_deps(self)
        deps.generate()
    
    def configure_cmake(self):
        base = self.python_requires["pyreq"].module.BaseConan
        cmake = base.configure_cmake(self)
        
        if self.options.zero_based_indices:
            cmake.definitions["CPPQL_BIND_ZERO_BASED_INDICES"] = True
        if self.options.shutdown_default_off:
            cmake.definitions["CPPQL_SHUTDOWN_DEFAULT_OFF"] = True
        
        return cmake

    def build(self):
        cmake = self.configure_cmake()
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = self.configure_cmake()
        cmake.install()



    