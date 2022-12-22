local p = premake

if not p.modules.clean then
    p.modules.clean = {} 

    newaction {
        trigger = "clean",
        description = "Cleans the entire solution using the chosen config",
        onProject = function(prj)
            printf("Cleaning project '%s'", prj.name)
            
            for cfg in p.project.eachconfig(prj) do
                if string.lower(cfg.name) == _OPTIONS["config"] then
                    local bindir = cfg.buildtarget.abspath
                    local objdir = cfg.objdir
                    
                    if (os.isfile(bindir)) then
                        local res, msg = os.remove(bindir)
                        
                        if not res then
                            error("Failed to remove directory "..bindir..": "..msg, 0)
                        else
                            printf("Removing %s", bindir)
                        end
                    end
                    
                    if (os.isdir(objdir)) then
                        local res, msg = os.rmdir(objdir)
                        
                        if not res then
                            error("Failed to remove directory "..objdir..": "..msg, 0)
                        else
                            printf("Removing %s", objdir)
                        end
                    end
                end
            end
        end
    }
end

return p.modules.clean