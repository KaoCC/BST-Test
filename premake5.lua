

if os.is("windows") then
    defines{ "WIN32" }
end

if os.is("linux") then
	flags {"C++14"}
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

        includedirs {"Utility/OpenCL/include", "Testbed/opencl_memory/"}
        files {"Utility/OpenCL/**.cpp", "Utility/OpenCL/**.hpp", "Testbed/opencl_memory/**.cpp", "Testbed/opencl_memory/**.hpp"}

        debugdir "Testbed/opencl_memory/"

    project "RegularOpenCL"
        kind "ConsoleApp"

        includedirs {"Utility/OpenCL/include", "Testbed/opencl_regular/"}
        files{"Utility/OpenCL/**.cpp", "Utility/OpenCL/**.hpp", "Testbed/opencl_regular/**.cpp", "Testbed/opencl_regular/**.hpp", "Testbed/opencl_regular/**.cl"}
        
        --usingdirs "Testbed/opencl_regular/"
        --runpathdirs "Testbed/opencl_regular/"
        --resincludedirs "Testbed/opencl_regular/"
        debugdir "Testbed/opencl_regular/"

    
    project "IndirectOpenCL"
        kind "ConsoleApp"

        includedirs {"Utility/OpenCL/include", "Testbed/opencl_indirect/"}
        files{"Utility/OpenCL/**.cpp", "Utility/OpenCL/**.hpp", "Testbed/opencl_indirect/**.cpp", "Testbed/opencl_indirect/**.hpp", "Testbed/opencl_indirect/**.cl"}

        debugdir "Testbed/opencl_indirect/"


    project "BinarySearchTreeOpenCL"
        kind "ConsoleApp"

        includedirs {"Utility/OpenCL/include", "Testbed/opencl_bst/"}
        files{"Utility/OpenCL/**.cpp", "Utility/OpenCL/**.hpp", "Testbed/opencl_bst/**.cpp", "Testbed/opencl_bst/**.hpp", "Testbed/opencl_bst/**.cl"}

        debugdir "Testbed/opencl_bst/"



    project "CoalescingOpenCL"
        kind "ConsoleApp"

        includedirs {"Utility/OpenCL/include", "Testbed/opencl_indirect/"}
        files{"Utility/OpenCL/**.cpp", "Utility/OpenCL/**.hpp", "Testbed/opencl_coalescing/**.cpp", "Testbed/opencl_coalescing/**.hpp", "Testbed/opencl_coalescing/**.cl"}

        debugdir "Testbed/opencl_coalescing/"



    project "LocalityOpenCL"
        kind "ConsoleApp"

        includedirs {"Utility/OpenCL/include", "Testbed/opencl_locality/"}
        files{"Utility/OpenCL/**.cpp", "Utility/OpenCL/**.hpp", "Testbed/opencl_locality/**.cpp", "Testbed/opencl_locality/**.hpp", "Testbed/opencl_locality/**.cl"}

        debugdir "Testbed/opencl_locality/"