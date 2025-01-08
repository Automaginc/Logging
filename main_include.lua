return function()
    externalincludedirs{ "Logging/include", "Logging/rang/include" }
    links { "Logging" }
    filter { "system:linux", "action:gmake2" }
        links { "stdc++exp" }
    filter { "system:macos", "action:gmake2" }
        links { "stdc++exp" }
end
