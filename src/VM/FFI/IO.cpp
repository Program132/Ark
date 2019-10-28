#include <Ark/VM/FFI.hpp>

#include <iostream>
#include <filesystem>

#include <Ark/Utils.hpp>

#define FFI_Function(name) Value name(const std::vector<Value>& n)

namespace Ark::internal::FFI::IO
{
    FFI_Function(print)
    {
        for (Value::Iterator it=n.begin(); it != n.end(); ++it)
            std::cout << (*it);
        std::cout << std::endl;

        return nil;
    }

    FFI_Function(input)
    {
        if (n.size() == 1)
        {
            if (n[0].valueType() != ValueType::String)
                throw Ark::TypeError("Argument of input must be of type String");
            std::cout << n[0].string();
        }

        std::string line = "";
        std::getline(std::cin, line);

        return Value(line);
    }

    FFI_Function(writeFile)
    {
        // filename, content
        if (n.size() == 2)
        {
            if (n[0].valueType() != ValueType::String)
                throw Ark::TypeError("First argument of writeFile (filename) should be a String");
            
            std::ofstream f(n[0].string());
            if (f.is_open())
            {
                f << n[1];
                f.close();
            }
            else
                throw std::runtime_error("Couldn't write to file \"" + n[0].string() + "\"");
        }
        // filename, mode (a or w), content
        else if (n.size() == 3)
        {
            if (n[0].valueType() != ValueType::String)
                throw Ark::TypeError("First argument of writeFile (filename) should be a String");
            if (n[1].valueType() != ValueType::String)
                throw Ark::TypeError("Second argument of writeFile (mode) should be a String");
            
            auto mode = n[1].string();
            if (mode != "w" && mode != "a")
                throw std::runtime_error("Second argument of writeFile (mode) is incorrect, available modes are \"a\" and \"w\"");
            
            auto ios_mode = std::ios::out | std::ios::trunc;
            if (mode == "a")
                ios_mode = std::ios::out | std::ios::app;
            
            std::ofstream f(n[0].string(), ios_mode);
            if (f.is_open())
            {
                f << n[2];
                f.close();
            }
            else
                throw std::runtime_error("Couldn't write to file \"" + n[0].string() + "\"");
        }
        else
            throw std::runtime_error("Got too many argument for writeFile: need a filename, an optional mode and a content");
        return nil;
    }

    FFI_Function(readFile)
    {
        if (n.size() != 1)
            throw std::runtime_error("readFile need 1 argument: filename");
        if (n[0].valueType() != ValueType::String)
            throw Ark::TypeError("Argument of readFile should be a String");
        
        auto filename = n[0].string();
        if (!Ark::Utils::fileExists(filename))
            throw std::runtime_error("Couldn't read file \"" + filename + "\": it doesn't exist");

        return Value(Ark::Utils::readFile(filename));
    }

    FFI_Function(fileExists)
    {
        if (n.size() != 1)
            throw std::runtime_error("fileExists? can take only 1 argument, a filename (String)");
        if (n[0].valueType() != ValueType::String)
            throw Ark::TypeError("Argument of fileExists? must be of type String");
        
        return Value(Ark::Utils::fileExists(n[0].string()) ? NFT::True : NFT::False);
    }

    FFI_Function(listFiles)
    {
        if (n.size() != 1)
            throw std::runtime_error("listFiles can take only 1 argument, a filename (String)");
        if (n[0].valueType() != ValueType::String)
            throw Ark::TypeError("Argument of listFiles must be of type String");
        
        std::vector<Value> r;
        for (const auto& entry: std::filesystem::directory_iterator(n[0].string()))
            r.push_back(Value(entry.path().string()));
        
        return Value(std::move(r));
    }

    FFI_Function(isDirectory)
    {
        if (n.size() != 1)
            throw std::runtime_error("isDir? can take only 1 argument, a filename (String)");
        if (n[0].valueType() != ValueType::String)
            throw Ark::TypeError("Argument of isDir? must be of type String");
        
        return (std::filesystem::is_directory(std::filesystem::path(n[0].string()))) ? trueSym : falseSym;
    }

    FFI_Function(makeDir)
    {
        if (n.size() != 1)
            throw std::runtime_error("makeDir can take only 1 argument, a filename (String)");
        if (n[0].valueType() != ValueType::String)
            throw Ark::TypeError("Argument of makeDir must be of type String");
        
        std::filesystem::create_directories(std::filesystem::path(n[0].string()));
        return nil;
    }

    FFI_Function(removeFiles)
    {
        if (n.size() == 0)
            throw std::runtime_error("removeFiles takes at least one argument");
        
        for (Value::Iterator it=n.begin(); it != n.end(); ++it)
        {
            if (it->valueType() != ValueType::String)
                throw Ark::TypeError("Arguments of removeFiles should be of type String");
            std::filesystem::remove_all(std::filesystem::path(it->string()));
        }

        return nil;
    }
}