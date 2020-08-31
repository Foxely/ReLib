#pragma once

#include <array>
#include <unordered_map>
#include <string>

#if _WIN32
#include <windows.h>
#elif __linux__
#include <dlfcn.h>
#endif

template <typename E, size_t NumSymbols>
class ReLibModule
{
public:
    static void LoadLibrary() { GetInstance().Load(); }
    static void ReloadLibrary() { GetInstance().Reload(); }
    static void UnloadLibrary() { GetInstance().Unload(); }

protected:
    static E& GetInstance()
    {
        static E instance;
        return instance;
    }

protected:

    // Should return the path to the library on disk
    virtual const char* GetPath() const = 0;

    // Should return a reference to an array of C-strings of size NumSymbols
    // Used when loading or reloading the library to lookup the address of
    // all exported symbols
    virtual std::array<const char*, NumSymbols>& GetSymbolNames() const = 0;

    template <typename RetValue, typename... Args>
    RetValue Execute(const char* strName, Args... args)
    {
        // Lookup the function address
        auto symbol = m_oSymbols.find(strName);
        if (symbol != m_oSymbols.end())
        {
            // Cast the address to the appropriate function type and call it,
            // forwarding all arguments
            return reinterpret_cast<RetValue(*)(Args...)>(symbol->second)(args...);
        }
        else
        {
            throw std::runtime_error(std::string("Function not found: ") + strName);
        }
    }

    template <typename T> T* GetVar(const char* strName)
    {
        auto symbol = m_oSymbols.find(strName);
        if (symbol != m_oSymbols.end())
        {
            return static_cast<T*>(symbol->second);
        }
        else
        {
            // We didn't find the variable. Return an empty pointer
            return nullptr;
        }
    }

private:
    void Load()
    {
    #if _WIN32
        custom.custom_dll = LoadLibraryA(custom_dll_path);
    #elif __linux__
        m_pLibHandle = dlopen(GetPath(), RTLD_NOW);
    #endif
        LoadSymbols();
    }

    void Reload()
    {
        dlclose(m_pLibHandle);
        m_oSymbols.clear();
        Load();
    }

    void Unload()
    {
        if(m_pLibHandle)
    #if _WIN32
        FreeLibrary(m_pLibHandle);
    #elif __linux__
        dlclose(m_pLibHandle);
    #endif
        m_oSymbols.clear();
    }

    void LoadSymbols()
    {
        for (const char* symbol : GetSymbolNames())
        {
        #if _WIN32
            m_oSymbols[symbol] = GetProcAddress(m_pLibHandle, symbol);
        #elif __linux__
            m_oSymbols[symbol] = dlsym(m_pLibHandle, symbol);
        #endif
        }
    }

    #if _WIN32
        HANDLE m_pLibHandle;
    #elif __linux__
        void* m_pLibHandle;
    #endif
    std::unordered_map<std::string, void*> m_oSymbols;
};