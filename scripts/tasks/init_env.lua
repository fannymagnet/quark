function main(...)
    os.exec("git submodule update --init --recursive")
    path = os.curdir()

    log_src_path = path .. "/thrd-libs/easyloggingpp"
    log_install_path = path .. "/deps/easyloggingpp"
    if os.exists(log_install_path) then
        os.rmdir(log_install_path)
    end
	os.mkdir(log_install_path)
	os.cp(log_src_path .."/src/*", log_install_path)

    liburing_src_path = path .. "/thrd-libs/liburing"
    os.cd(liburing_src_path)
    os.exec("make clean")
    install_path = path .. "/deps/liburing"
    if os.exists(install_path) then
        os.rmdir(install_path)
        os.mkdir(install_path)
    end

    os.exec("make clean")

    print("########Begin Configure#########")
    os.exec("./configure --prefix="..install_path)
    os.exec("make")
    os.exec("make install")

    os.exec("xmake require -f -y")
end
