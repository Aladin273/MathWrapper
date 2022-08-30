#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <uv.h>
#include <windows.h>
#include <julia.h>

class JuliaWrapper
{
public:
    JuliaWrapper();
    ~JuliaWrapper();

    void setDirectory(const std::string& dir);

    std::string execute(const std::string& command);
    std::string executeFile(const std::string& filename);

    std::string executeWeave(const std::string& command);
    std::string executeFileWeave(const std::string& filename);

private:
    std::vector<std::string> m_settings
    {
        "import Pkg",
        "Pkg.add(\"Plots\")",
        "Pkg.add(\"Weave\")",
        "using Plots",
        "using Weave",
        "using Roots",
    };

    std::string m_temp = "";
    std::string m_tempInput = "output.jl";
    std::string m_tempOutput = "output.html";

    std::string m_typeInput = ".jl";
    std::string m_typeOutput = ".html";
};

