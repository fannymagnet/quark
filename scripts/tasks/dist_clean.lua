function main(...)
    import("core.base.task")
    task.run("clean")

    if os.exists("build") then
        os.rmdir("build")
        os.mkdir("build")
    end

    path = os.curdir()
    liburing_src_path = path .. "/thrd-libs/liburing"
    install_path = path .. "/deps/liburing"

    os.tryrm(install_path)

    log_install_path = path .. "/deps/easyloggingpp"
    os.tryrm(log_install_path)

    os.exec("git submodule deinit --force --all") 
end
