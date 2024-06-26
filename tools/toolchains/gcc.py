"""
mbed SDK
Copyright (c) 2011-2013 ARM Limited

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""
import re
from os.path import join, basename, splitext, dirname, exists
from distutils.spawn import find_executable

from tools.toolchains import mbedToolchain, TOOLCHAIN_PATHS
from tools.hooks import hook_tool

class GCC(mbedToolchain):
    LINKER_EXT = '.ld'
    LIBRARY_EXT = '.a'

    STD_LIB_NAME = "lib%s.a"
    DIAGNOSTIC_PATTERN = re.compile('((?P<file>[^:]+):(?P<line>\d+):)(\d+:)? (?P<severity>warning|[eE]rror|fatal error): (?P<message>.+)')
    INDEX_PATTERN  = re.compile('(?P<col>\s*)\^')

    def __init__(self, target,  notify=None, macros=None,
                 silent=False, extra_verbose=False, build_profile=None,
                 build_dir=None):
        mbedToolchain.__init__(self, target, notify, macros, silent,
                               extra_verbose=extra_verbose,
                               build_profile=build_profile, build_dir=build_dir)

        tool_path=TOOLCHAIN_PATHS['GCC_ARM']
        # Add flags for current size setting
        default_lib = "std"
        if hasattr(target, "default_lib"):
            default_lib = target.default_lib
        elif hasattr(target, "default_build"): # Legacy
            default_lib = target.default_build

        if default_lib == "small":
            self.flags["common"].append("-DMBED_RTOS_SINGLE_THREAD")
            self.flags["ld"].append("--specs=nano.specs")

        if target.core == "Cortex-M0+":
            cpu = "cortex-m0plus"
        elif target.core == "Cortex-M4F":
            cpu = "cortex-m4"
        elif target.core == "Cortex-M7F":
            cpu = "cortex-m7"
        elif target.core == "Cortex-M7FD":
            cpu = "cortex-m7"
        elif target.core == "Cortex-M23-NS":
            cpu = "cortex-m23"
        elif target.core == "Cortex-M33-NS":
            cpu = "cortex-m33"
        else:
            cpu = target.core.lower()

        self.cpu = ["-mcpu=%s" % cpu]
        if target.core.startswith("Cortex-M"):
            self.cpu.append("-mthumb")

        # FPU handling, M7 possibly to have double FPU
        if target.core == "Cortex-M4F":
            self.cpu.append("-mfpu=fpv4-sp-d16")
            self.cpu.append("-mfloat-abi=softfp")
        elif target.core == "Cortex-M7F":
            self.cpu.append("-mfpu=fpv5-sp-d16")
            self.cpu.append("-mfloat-abi=softfp")
        elif target.core == "Cortex-M7FD":
            self.cpu.append("-mfpu=fpv5-d16")
            self.cpu.append("-mfloat-abi=softfp")

        if target.core == "Cortex-A9":
            self.cpu.append("-mthumb-interwork")
            self.cpu.append("-marm")
            self.cpu.append("-march=armv7-a")
            self.cpu.append("-mfpu=vfpv3")
            self.cpu.append("-mfloat-abi=hard")
            self.cpu.append("-mno-unaligned-access")

        if target.core.startswith("Cortex-M23"):
            self.cpu.append("-march=armv8-m.base")
        elif target.core.startswith("Cortex-M33"):
            self.cpu.append("-march=armv8-m.main")

        if target.core == "Cortex-M23" or target.core == "Cortex-M33":
            self.cpu.append("-mcmse")

        self.flags["common"] += self.cpu

        main_cc = join(tool_path, "arm-none-eabi-gcc")
        main_cppc = join(tool_path, "arm-none-eabi-g++")
        self.asm = [main_cc] + self.flags['asm'] + self.flags["common"]
        self.cc  = [main_cc]
        self.cppc =[main_cppc]
        self.cc += self.flags['c'] + self.flags['common']
        self.cppc += self.flags['cxx'] + self.flags['common']

        self.flags['ld'] += self.cpu
        self.ld = [join(tool_path, "arm-none-eabi-gcc")] + self.flags['ld']
        self.sys_libs = ["stdc++", "supc++", "m", "c", "gcc", "nosys"]
        self.preproc = [join(tool_path, "arm-none-eabi-cpp"), "-E", "-P"]

        self.ar = join(tool_path, "arm-none-eabi-ar")
        self.elf2bin = join(tool_path, "arm-none-eabi-objcopy")

    def is_not_supported_error(self, output):
        return "error: #error [NOT_SUPPORTED]" in output

    def parse_output(self, output):
        # The warning/error notification is multiline
        msg = None
        for line in output.splitlines():
            match = self.DIAGNOSTIC_PATTERN.search(line)
            if match is not None:
                if msg is not None:
                    self.cc_info(msg)
                    msg = None
                msg = {
                    'severity': match.group('severity').lower(),
                    'file': match.group('file'),
                    'line': match.group('line'),
                    'col': 0,
                    'message': match.group('message'),
                    'text': '',
                    'target_name': self.target.name,
                    'toolchain_name': self.name
                }
            elif msg is not None:
                # Determine the warning/error column by calculating the ^ position
                match = self.INDEX_PATTERN.match(line)
                if match is not None:
                    msg['col'] = len(match.group('col'))
                    self.cc_info(msg)
                    msg = None
                else:
                    msg['text'] += line+"\n"

        if msg is not None:
            self.cc_info(msg)

    def get_dep_option(self, object):
        base, _ = splitext(object)
        dep_path = base + '.d'
        return ["-MD", "-MF", dep_path]

    def get_config_option(self, config_header):
        return ['-include', config_header]

    def get_compile_options(self, defines, includes, for_asm=False):
        opts = ['-D%s' % d for d in defines]
        if self.RESPONSE_FILES:
            opts += ['@%s' % self.get_inc_file(includes)]
        else:
            opts += ["-I%s" % i for i in includes]

        if not for_asm:
            config_header = self.get_config_header()
            if config_header is not None:
                opts = opts + self.get_config_option(config_header)
        return opts

    @hook_tool
    def assemble(self, source, object, includes):
        # Build assemble command
        cmd = self.asm + self.get_compile_options(self.get_symbols(True), includes) + ["-o", object, source]

        # Call cmdline hook
        cmd = self.hook.get_cmdline_assembler(cmd)

        # Return command array, don't execute
        return [cmd]

    @hook_tool
    def compile(self, cc, source, object, includes):
        # Build compile command
        cmd = cc + self.get_compile_options(self.get_symbols(), includes)

        cmd.extend(self.get_dep_option(object))

        cmd.extend(["-o", object, source])

        # Call cmdline hook
        cmd = self.hook.get_cmdline_compiler(cmd)

        return [cmd]

    def compile_c(self, source, object, includes):
        return self.compile(self.cc, source, object, includes)

    def compile_cpp(self, source, object, includes):
        return self.compile(self.cppc, source, object, includes)

    @hook_tool
    def link(self, output, objects, libraries, lib_dirs, mem_map):
        libs = []
        for l in libraries:
            name, _ = splitext(basename(l))
            libs.append("-l%s" % name[3:])
        libs.extend(["-l%s" % l for l in self.sys_libs])

        # Preprocess
        if mem_map:
            preproc_output = join(dirname(output), ".link_script.ld")
            cmd = (self.preproc + [mem_map] + self.ld[1:] +
                   [ "-o", preproc_output])
            self.cc_verbose("Preproc: %s" % ' '.join(cmd))
            self.default_cmd(cmd)
            mem_map = preproc_output

        # Build linker command
        map_file = splitext(output)[0] + ".map"
        cmd = self.ld + ["-o", output, "-Wl,-Map=%s" % map_file] + objects + ["-Wl,--start-group"] + libs + ["-Wl,--end-group"]
        if mem_map:
            cmd.extend(['-T', mem_map])

        for L in lib_dirs:
            cmd.extend(['-L', L])
        cmd.extend(libs)

        # Call cmdline hook
        cmd = self.hook.get_cmdline_linker(cmd)

        if self.RESPONSE_FILES:
            # Split link command to linker executable + response file
            cmd_linker = cmd[0]
            link_files = self.get_link_file(cmd[1:])
            cmd = [cmd_linker, "@%s" % link_files]

        # Exec command
        self.cc_verbose("Link: %s" % ' '.join(cmd))
        self.default_cmd(cmd)

    @hook_tool
    def archive(self, objects, lib_path):
        if self.RESPONSE_FILES:
            param = ["@%s" % self.get_arch_file(objects)]
        else:
            param = objects

        # Exec command
        self.default_cmd([self.ar, 'rcs', lib_path] + param)

    @hook_tool
    def binary(self, resources, elf, bin):
        # Build binary command
        _, fmt = splitext(bin)
        bin_arg = {'.bin': 'binary', '.hex': 'ihex'}[fmt]
        cmd = [self.elf2bin, "-O", bin_arg, elf, bin]

        # Call cmdline hook
        cmd = self.hook.get_cmdline_binary(cmd)

        # Exec command
        self.cc_verbose("FromELF: %s" % ' '.join(cmd))
        self.default_cmd(cmd)

    @staticmethod
    def name_mangle(name):
        return "_Z%i%sv" % (len(name), name)

    @staticmethod
    def make_ld_define(name, value):
        return "-D%s=0x%x" % (name, value)

    @staticmethod
    def redirect_symbol(source, sync, build_dir):
        return "-Wl,--defsym=%s=%s" % (source, sync)

    @staticmethod
    def check_executable():
        """Returns True if the executable (arm-none-eabi-gcc) location
        specified by the user exists OR the executable can be found on the PATH.
        Returns False otherwise."""
        if not TOOLCHAIN_PATHS['GCC_ARM'] or not exists(TOOLCHAIN_PATHS['GCC_ARM']):
            if find_executable('arm-none-eabi-gcc'):
                TOOLCHAIN_PATHS['GCC_ARM'] = ''
                return True
            else:
                return False
        else:
            exec_name = join(TOOLCHAIN_PATHS['GCC_ARM'], 'arm-none-eabi-gcc')
            return exists(exec_name) or exists(exec_name + '.exe')

class GCC_ARM(GCC):
    pass
