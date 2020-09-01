from conans import ConanFile, tools, VisualStudioBuildEnvironment
import os

class transFunConan(ConanFile):
    name = "transFun"
    version = "0.0"
    license = "BSD"
    author = "Long Chen"
    url = "https://github.com/afei190/TransferFunctionTool"
    description = "A Tool to Visualize Transfer Function"
    settings = "os", "compiler", "build_type", "arch"
    generators = "qmake", "cmake"
    requires = "mkl-include/2019.4@simcenter/stable", "mkl-static/2019.4@simcenter/stable"
    build_policy = "missing"


    def build_requirements(self):
        if self.settings.os == "Windows":
            self.build_requires("jom_installer/1.1.2@bincrafters/stable")

    def package_id(self):
        del self.info.options.withQt


    def build(self):
        if self.settings.os == "Windows":
            env_build = VisualStudioBuildEnvironment(self)
            with tools.environment_append(env_build.vars):
                vcvars = tools.vcvars_command(self.settings)
            
            qmake = "%s && qmake" % (vcvars)
            makeCommand = "%s && jom" % (vcvars)
        else:
            qmake = 'qmake'
            makeCommand = 'make'

        qmakeCommand = '%s "CONFIG+=%s" %s/transFunc.pro' % (qmake, self.settings.build_type, self.source_folder)

        self.run(qmakeCommand, run_environment=True) 
        self.run(makeCommand, run_environment=True) 
   
    def imports(self):
        if self.settings.os == "Windows":
            output = './%s' % self.settings.build_type
            self.copy("lib*.dll", dst=output, src="bin")

