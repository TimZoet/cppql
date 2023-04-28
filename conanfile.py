from conan import ConanFile
from conan.tools.files import copy

class CppqlConan(ConanFile):
    ############################################################################
    ## Package info.                                                          ##
    ############################################################################
    
    name = "cppql"
    
    description = "C++ sqlite library."
    
    url = "https://github.com/TimZoet/cppql"

    @property
    def user(self):
        return getattr(self, "_user", "timzoet")
    
    @user.setter
    def user(self, value):
        self._user = value
    
    @property
    def channel(self):
        return getattr(self, "_channel", f"v{self.version}")
    
    @channel.setter
    def channel(self, value):
        self._channel = value

    ############################################################################
    ## Settings.                                                              ##
    ############################################################################

    python_requires = "pyreq/1.0.0@timzoet/v1.0.0"
    
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
        base.set_version(self, "cppqlVersionString.cmake", "CPPQL_VERSION")
    
    def init(self):
        base = self.python_requires["pyreq"].module.BaseConan
        self.settings = base.settings
        self.options.update(base.options, base.default_options)
    
    ############################################################################
    ## Building.                                                              ##
    ############################################################################

    def export_sources(self):
        copy(self, "CMakeLists.txt", self.recipe_folder, self.export_sources_folder)
        copy(self, "cppqlVersionString.cmake", self.recipe_folder, self.export_sources_folder)
        copy(self, "license", self.recipe_folder, self.export_sources_folder)
        copy(self, "readme.md", self.recipe_folder, self.export_sources_folder)
        copy(self, "buildtools/*", self.recipe_folder, self.export_sources_folder)
        copy(self, "modules/*", self.recipe_folder, self.export_sources_folder)
    
    def config_options(self):
        base = self.python_requires["pyreq"].module.BaseConan
        base.config_options(self)
        if self.settings.os == "Windows":
            del self.options.fPIC
    
    def configure(self):
        base = self.python_requires["pyreq"].module.BaseConan
        base.configure2(self)
        if self.options.build_manual:
            self.options.manual_repository = "git@github.com:TimZoet/cppql-manual.git"
    
    def requirements(self):
        base = self.python_requires["pyreq"].module.BaseConan
        base.requirements(self)
        
        self.requires("common/1.0.0@timzoet/v1.0.0")
        self.requires("sqlite3/3.40.0")
        
        if self.options.build_tests:
            self.requires("bettertest/1.0.0@timzoet/v1.0.0")
    
    def package_info(self):
        self.cpp_info.components["core"].libs = ["cppql"]
        self.cpp_info.components["core"].requires = ["cmake-modules::cmake-modules", "common::common", "sqlite3::sqlite3"]
    
    def generate(self):
        base = self.python_requires["pyreq"].module.BaseConan
        
        tc = base.generate_toolchain(self)
        
        if self.options.build_manual:
            tc.variables["MANUAL_TAG"] = self.version
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
        return cmake

    def build(self):
        cmake = self.configure_cmake()
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = self.configure_cmake()
        cmake.install()
