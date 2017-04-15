

if os.is("windows") then
    defines{ "WIN32" }
end

workspace "HeteroBench"

    location "Generated"

    language "C++"

    configurations {"Debug", "Release"}
    platforms {"x64", "x32"}

    filter { "platforms:x64" }
        architecture "x86_64"
    filter { "platforms:x32" }
        architecture "x86"
    
    filter { }


    filter { "configurations:Debug" }
        symbols "On"

    filter { "configurations:Release" }
        -- Release should be optimized
        optimize "On"

    -- Reset the filter for other settings
    filter { }


    targetdir ("Build/Bin/%{prj.name}/%{cfg.buildcfg}/%{cfg.platform}")
    objdir ("Build/Obj/%{prj.name}/%{cfg.buildcfg}/%{cfg.platform}")


    dofile ("./OpenCLSearch.lua" )




    -- add build cases below:

    project "MemoryOpenCL"
        kind "ConsoleApp"

        includedirs {"opencl_common/include", "opencl_memory/include"}
        files {"opencl_common/**.cpp","opencl_common/**.hpp","opencl_memory/**.cpp", "opencl_memory/**.hpp"}


