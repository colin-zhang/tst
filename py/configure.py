#!/usr/bin/python3
import os
'''
build.ninja
#variable
cc = g++
cflags = -Wall
ldflags = -lssh2 -pthread

# rule
rule cc
    command = $cc $cflags -c $in -o $out
    description = compile .cc
rule ar
    command = $cc   $in  $ldflags -o $out

# build
build ssh2.o: cc ssh2.cc
build ssh_tool.o: cc ssh_tool.cc
build utils.o: cc utils.cc
build ssh_tool: ar ssh_tool.o ssh2.o utils.o

#apt install ninja-build
#yum install ninja-build
'''
here_dir = os.path.abspath(os.path.dirname(__file__))
build_dir = os.path.join(here_dir, "build")
src_exts = ['.c', '.cc', '.cpp'] 

def list_one_dir(dir, exts = src_exts, filter_str = "test"):
    files = []
    for f in os.listdir(dir):
        name , ext = os.path.splitext(f);
        if ext in exts and  not name.endswith(filter_str):
            files.append(os.path.join(dir, f))
    return files

class SingleUint(object):
    def __init__(self, file, tool, output):
        self.file_ = file
        #self.file_ = []
        #self.file_.append(file)
        self.tool_ = tool
        self.output_ = output
    def __eq__(self, other):
        return self.output_ == other.output_

class BuildUnit(object):
    def __init__(self, tool, output, deps):
        #self.files = []
        self.tool = tool
        self.output = output
        self.deps = deps
        self.file_uints = set()

    def print_uints(self):
        for u in self.file_uints:
            print(u.file_, u.tool_, u.output_)

    def  add_file(self, file, build_tool):
        base, _ = os.path.splitext(file)
        file = SingleUint(file, build_tool, base + ".o")
        self.file_uints.add(file)

    def  add_dir(self, dir, build_tool):
        for f in list_one_dir(dir):
            self.add_file(f, build_tool)

    def make_str(self):
            str = ""
            str = self.output + ":"
            for f in self.file_uints:
                str += f.output_ + " "
            str += "\n\t"
            str += self.tool + "-o $@"
            return str

class Make(object):
    def __init__(self):
        self.uints = []

    def  add(self, u):
        self.uints.append(u)

    def file(self):
        gen_all = "all :"
        gen_uint = ""
        gen_singal = ""
        singal_set = set()
        clean = "clean:\r\trm -rf  "
        for u in self.uints:
            gen_all +=   u.output + " "
            clean +=   u.output + " "
            gen_uint = "%s : "%(u.output)
            for f in u.file_uints:
                gen_uint += " %s"%(f.output_)
                singal_set.add(f)
            gen_uint += "\n\t%s"%(u.tool + " $^ " + u.deps + " -o  $@ ")

        for f in singal_set:
            gen_singal += "%s : %s\n\t%s -c $<   -o  $@ \n"%(f.output_, f.file_, f.tool_)
            clean += "%s "%(f.output_)

        with open("makefile", "w") as f:
            file_str = "%s\n\n%s\n\n%s\n%s\n"% (gen_all, gen_uint, gen_singal, clean)
            f.write(file_str)


def project():
    ssh_tool = BuildUnit("g++", "ssh_tool", "-lssh2 -pthread")
    ssh_tool.add_file("ssh2.cc", "g++")
    ssh_tool.add_file("utils.cc", "g++")
    ssh_tool.add_file("ssh_tool.cc", "g++")

    make = Make()
    make.add(ssh_tool)
    make.file()



def main():
    project()

if __name__ == '__main__':
    main()