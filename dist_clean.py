#!/usr/bin/python3
import shutil
import os

def remove_liburing():
    path = os.getcwd()
    liburing_src_path = path + "/thrd-libs/liburing"
    install_path = path + "/deps/liburing"

    if os.path.exists(install_path):
        shutil.rmtree(install_path)

    if os.path.exists(liburing_src_path):
        shutil.rmtree(liburing_src_path)

if __name__ == "__main__":
    os.system("xmake clean")
    remove_liburing()