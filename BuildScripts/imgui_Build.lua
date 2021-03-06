------------------------------------------------------------------------------------------------------------------------
--
--  imgui_Build.lua
--
--  Running the build process for imgui.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
print("--", "Generating project for imgui.")

group("ThirdParty")
project("imgui")
language("C++")
cppdialect("C++17")
kind("StaticLib")

targetdir(ENGINE_BIN_OUTPUT_DIR)

includedirs({
    THIRD_PARTY_SRC_DIR.."/imgui/src"
})

files({
    THIRD_PARTY_SRC_DIR.."/imgui/*.h",
    THIRD_PARTY_SRC_DIR.."/imgui/*.cpp"
})
