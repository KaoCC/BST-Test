

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

        includedirs {"Utility/OpenCL/include"}
        files {"Utility/OpenCL/**.cpp", "Utility/OpenCL/**.hpp", "Testbed/opencl_memory/**.cpp", "Testbed/opencl_memory/**.hpp"}


