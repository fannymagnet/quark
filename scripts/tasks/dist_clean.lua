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

    if os.exists(install_path) then
        os.rmdir(install_path)
    end

    if os.exists(liburing_src_path) then
        --os.rmdir(liburing_src_path)
        --os.mkdir(liburing_src_path)
    end
end
