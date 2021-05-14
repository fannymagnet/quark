function main(...)
    os.exec("git submodule update --init --recursive")
    path = os.curdir()
    liburing_src_path = path .. "/thrd-libs/liburing"
    os.cd(liburing_src_path)
    os.exec("make clean")
    install_path = path .. "/deps/liburing"
    if os.exists(install_path) then
        os.rmdir(install_path)
        os.mkdir(install_path)
    end
    print("########Begin Configure#########")
    os.exec("./configure --prefix="..install_path)
    os.exec("make")
    os.exec("make install")
end
