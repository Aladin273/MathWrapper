#include "JuliaWrapper.h"

JuliaWrapper::JuliaWrapper()
{
    std::cout << "\nJulie loading..." << std::endl;;

    jl_init();

    for (auto& set : m_settings)
        jl_eval_string(set.c_str());
}

JuliaWrapper::~JuliaWrapper()
{
    jl_atexit_hook(0);
}

void JuliaWrapper::setDirectory(const std::string& dir)
{
    m_temp = dir;
}

std::string JuliaWrapper::execute(const std::string& command)
{ 
    jl_value_t* result = jl_eval_string(command.c_str());
    
    if (!jl_exception_occurred())
    {
        if (jl_typeis(result, jl_string_type))
            return jl_string_ptr(result);
        else if (jl_typeis(result, jl_float64_type))
            return std::to_string(jl_unbox_float64(result));
        else if (jl_typeis(result, jl_int64_type))
            return std::to_string(jl_unbox_int64(result));
        else
            return "true";
    }
    
    return "";
}

std::string JuliaWrapper::executeFile(const std::string& filename)
{
    jl_value_t* result = jl_eval_string(("include(\"" + m_temp + filename + "\")").c_str());

    if (!jl_exception_occurred())
    {
        if (jl_typeis(result, jl_string_type))
            return jl_string_ptr(result);
        else if (jl_typeis(result, jl_float64_type))
            return std::to_string(jl_unbox_float64(result));
        else if (jl_typeis(result, jl_int64_type))
            return std::to_string(jl_unbox_int64(result));
        else
            return "true";
    }

    return "";
}

std::string JuliaWrapper::executeWeave(const std::string& command)
{
    std::fstream filestream;
    
    filestream.open(m_temp + m_tempInput, std::ios::out);
    
    if (filestream.is_open())
    {
        filestream << m_settings[3] << "\n";
        filestream << m_settings[4] << "\n\n";
        filestream << command;
        filestream.close();
    
        jl_eval_string(("weave(\"" + m_temp + m_tempInput + "\")").c_str());
    
        if (!jl_exception_occurred())
        {
            return m_temp + m_tempOutput;
        }
    }
    
    return "";
}

std::string JuliaWrapper::executeFileWeave(const std::string& filename)
{
    if (filename.find(m_typeInput))
    {
        std::string command = "weave(\"" + m_temp + filename + "\")";
    
        jl_eval_string(command.c_str());
    
        if (!jl_exception_occurred())
        {
            command = filename;
            command.pop_back();
            command.pop_back();
            command.pop_back();
            command += m_typeOutput;
    
            return m_temp + command;
        }
    }
    
    return "";
}
