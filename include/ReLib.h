#pragma once

#include <array>
#include <utility>
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

    ReLibModule(std::array<std::pair<const char*, void*>, NumSymbols>& oSymbols) : m_oSymbols(oSymbols) { }
    // Should return the path to the library on disk
    virtual const char* GetPath() const = 0;

    // Should return a reference to an array of C-strings of size NumSymbols
    // Used when loading or reloading the library to lookup the address of
    // all exported symbols
    // virtual std::array<const char*, NumSymbols>& GetSymbolNames() const = 0;

    // template <typename RetValue, typename... Args>
    // RetValue Execute(const char* strName, Args... args)
    // {
    //     // Lookup the function address
    //     auto symbol = m_oSymbols.find(strName);
    //     if (symbol != m_oSymbols.end())
    //     {
    //         // Cast the address to the appropriate function type and call it,
    //         // forwarding all arguments
    //         return reinterpret_cast<RetValue(*)(Args...)>(symbol->second)(args...);
    //     }
    //     else
    //     {
    //         throw std::runtime_error(std::string("Function not found: ") + strName);
    //     }
    // }

    // template <typename T> T* GetVar(const char* strName)
    // {
    //     auto symbol = m_oSymbols.find(strName);
    //     if (symbol != m_oSymbols.end())
    //     {
    //         return static_cast<T*>(symbol->second);
    //     }
    //     else
    //     {
    //         // We didn't find the variable. Return an empty pointer
    //         return nullptr;
    //     }
    // }

    template <size_t uiIndex, typename RetValue, typename... Args>
    RetValue Execute(Args... args)
    {
        static_assert(uiIndex >= 0 && uiIndex < NumSymbols, "Out of bounds symbol index");
        // Lookup the function address
        auto symbol = m_oSymbols[uiIndex];
        // Cast the address to the appropriate function type and call it,
        // forwarding all arguments
        return reinterpret_cast<RetValue(*)(Args...)>(symbol.second)(args...);
    }

    template <size_t uiIndex, typename T> T* GetVar()
    {
        static_assert(uiIndex >= 0 && uiIndex < NumSymbols, "Out of bounds symbol index");
        auto symbol = m_oSymbols[uiIndex];
        return static_cast<T*>(symbol.second);
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
        // m_oSymbols.clear();
        // std::fill( m_oSymbols.begin(), m_oSymbols.end(), 0);
        // std::fill_n(m_oSymbols, NumSymbols, 0);
        // m_oSymbols.fill(std::make_pair("", nullptr));
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
        // m_oSymbols.clear();
        // std::fill( m_oSymbols.begin(), m_oSymbols.end(), 0);
        // std::fill_n(m_oSymbols, NumSymbols, 0);
        // m_oSymbols.fill(std::make_pair("", nullptr));
    }

    void LoadSymbols()
    {
        for (auto&& symbol : m_oSymbols)
        {
        #if _WIN32
            symbol.second = GetProcAddress(m_pLibHandle, symbol.first);
        #elif __linux__
            symbol.second = dlsym(m_pLibHandle, symbol.first);
        #endif
        }
    }

    #if _WIN32
        HANDLE m_pLibHandle;
    #elif __linux__
        void* m_pLibHandle;
    #endif
    // std::unordered_map<std::string, void*> m_oSymbols;
    std::array<std::pair<const char*, void*>, NumSymbols>& m_oSymbols;
};