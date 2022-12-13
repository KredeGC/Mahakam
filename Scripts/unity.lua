local p = premake

if not p.modules.unity then
    p.modules.unity = {}

    p.api.register {
        name = "unity",
        scope = "project",
        kind = "string"
    }
    
    p.api.register {
        name = "unityexclude",
        scope = "config",
        kind = "file"
    }

    function write_unity_file(prj, files)
        local filename = path.join(prj.location, "Unity-Build."..(p.project.iscpp(prj) and "cpp" or "c"))
        local location = path.getdirectory(filename)

        local buf = buffered.new()
        buffered.writeln(buf, "// GENERATED UNITY FILE. CHANGES WON'T BE PRESERVED")
        buffered.writeln(buf, "")

        local pch = {}
        for cfg in p.project.eachconfig(prj) do
            if not cfg.flags.NoPCH and cfg.pchheader ~= nil and not pch[cfg.pchheader] then
                buffered.writeln(buf, '#include "' .. cfg.pchheader .. '"')
                pch[cfg.pchheader] = true
            end
        end

        for _, file in ipairs(files) do
            local fn = path.getrelative(location, file)
            buffered.writeln(buf, '#include "' .. fn .. '"')
        end

        buffered.writeln(buf, "")
        local content = buffered.tostring(buf)
        buffered.close(buf)

        os.mkdir(location)
        local f, err = os.writefile_ifnotequal(content, filename);
        if (f < 0) then
            error(err, 0)
        elseif (f > 0) then
            printf("Generated %s...", p.workspace.getrelative(prj.workspace, filename))
        end

        return filename
    end


    function generate_unity_files(prj, files)

        local fileList = {}
        local excludeList = {}
        
        if prj.unityexclude then
            table.foreachi(prj.unityexclude, function(pattern)
                excludeList = table.join(excludeList, os.matchfiles(pattern))
            end)
        end

        local addFile = function(cfg, fname)
            if not files[fname] then
                local fcfg = p.fileconfig.new(fname, prj)
                fcfg.vpath = path.join("Unity Files", fcfg.name)

                files[fname] = fcfg
                table.insert(files, fcfg)
            end
            p.fileconfig.addconfig(files[fname], cfg)
        end

        table.foreachi(files, function(file)
            -- only process compile units.
            if not path.iscppfile(file.abspath) then
                return
            end

            for cfg in p.project.eachconfig(prj) do
                local fcfg = p.fileconfig.getconfig(file, cfg)

                -- any file that has any file settings can't be added to the unityFile
                if p.fileconfig.hasFileSettings(fcfg) then
                    return
                end
                
                -- if the file is the unityfile, ignore it
                if p.workspace.getrelative(prj, fcfg.abspath) == "Unity-Build."..(p.project.iscpp(prj) and "cpp" or "c") then
                    return
                end

                -- if the file is the precompiled header source, it can't be added either.
                if cfg.pchsource == fcfg.abspath and not cfg.flags.NoPCH then
                    return
                end
                
                -- if the file is in the unityexclude directive, ignore it
                if table.contains(excludeList, fcfg.abspath) then
                    printf("Excluding %s", p.workspace.getrelative(prj.workspace, fcfg.abspath))
                    return
                end
            end

            -- add to the list.
            table.insert(fileList, file.abspath)

            -- mark as ExcludeFromBuild
            for cfg in p.project.eachconfig(prj) do
                local fcfg = p.fileconfig.getconfig(file, cfg)
                fcfg.flags.ExcludeFromBuild = true
            end
        end)

        if (#fileList >= 0) then
            local unityName = write_unity_file(prj, fileList)
            for cfg in p.project.eachconfig(prj) do
                addFile(cfg, unityName)
            end
        else
            error("No files found for unity build", 0)
        end


        -- Alpha sort the indices, so I will get consistent results in
        -- the exported project files.

        table.sort(files, function(a,b)
            return a.vpath < b.vpath
        end)

        return files
    end

    -- Override the bakeFiles process, when files are being listed
    p.override(p.oven, "bakeFiles", function(base, prj)
        local files = base(prj)

        if prj.unity and type(prj.unity) == 'string' and prj.unity == 'on' then
            files = generate_unity_files(prj, files)
        end

        return files
    end)
end

return p.modules.unity