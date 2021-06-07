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

    if os.exists(liburing_src_path) then
        --os.rmdir(liburing_src_path)
        --os.mkdir(liburing_src_path)
    end
    os.exec("git submodule deinit --force --all") 
end
