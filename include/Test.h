#pragma once

#include <ReLib.h>

extern "C"
{
    void foo();
    extern int bar;
}

std::array<const char*, 2> g_exports = { "foo", "bar" };

class TestModule : public ReLibModule<TestModule, g_exports.size()>
{
public:
    static void Foo()
    {
        // Execute might throw, but we don't bother catching the exception here for brevity
        GetInstance().Execute<void>("fo");
    }

    static int GetBar()
    {
        // decltype is a relatively new operator. decltype(bar) resolves to int
        // Note that this function does not protect against retrieving nullptr
        return *GetInstance().GetVar<decltype(bar)>("bar");
    }

protected:
    virtual const char* GetPath() const override
    {
#ifdef DEBUG
    return "./test.so";
#else
    return "./test.so";
#endif
    }

    virtual std::array<const char*, g_exports.size()>& GetSymbolNames() const override
    {
        return g_exports;
    }
};
