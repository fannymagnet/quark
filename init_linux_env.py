#!/usr/bin/python3
import os
import shutil

def init_liburing():
    path = os.getcwd()
    liburing_src_path = path + "/thrd-libs/liburing"
    os.chdir(liburing_src_path)
    install_path = path + "/deps/liburing"
    #print("install path: ", install_path)
    if os.path.exists(install_path):
        shutil.rmtree(install_path)
        os.mkdir(install_path)
    print("########Begin Configure#########")
    os.system("make clean")
    os.system("./configure --prefix="+install_path)

    print("########Begin Compile#########")
    os.system("make")
    os.system("make install")
    


if __name__ == "__main__":
    os.system("git submodule update --init --recursive")
    init_liburing()
