#pragma once

#include <ReLib.h>

extern "C"
{
    void foo();
    extern int bar;
}

std::array<std::pair<const char*, void*>, 2> g_exports = {
    std::make_pair("foo", nullptr),
    std::make_pair("bar", nullptr)
};

class TestModule : public ReLibModule<TestModule, g_exports.size()>
{
public:
    static void Foo()
    {
        // Execute might throw, but we don't bother catching the exception here for brevity
        GetInstance().Execute<0, void>();
    }

    static int GetBar()
    {
        // decltype is a relatively new operator. decltype(bar) resolves to int
        // Note that this function does not protect against retrieving nullptr
        return *GetInstance().GetVar<1, decltype(bar)>();
    }

    TestModule() : ReLibModule(g_exports) { }

protected:
    virtual const char* GetPath() const override
    {
#ifdef DEBUG
    return "./test.so";
#else
    return "./test.so";
#endif
    }

    // virtual std::array<const char*, g_exports.size()>& GetSymbolNames() const override
    // {
    //     return g_exports;
    // }
};
