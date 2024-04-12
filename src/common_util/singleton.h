#pragma once

#include <mutex>

#undef CUTL_COPY_AND_ASSIGN
#define CUTL_COPY_AND_ASSIGN(classname)    \
    classname(const classname &) = delete; \
    classname &operator=(const classname &) = delete;

#undef CUTL_SINGLETON_PTR
#define CUTL_SINGLETON_PTR(classname)                                            \
public:                                                                          \
    static classname *get_instance(bool create_if_needed = true)                 \
    {                                                                            \
        static classname *obj = nullptr;                                         \
        if (!obj && create_if_needed)                                            \
        {                                                                        \
            static std::once_flag flag;                                          \
            std::call_once(flag, [&] { obj = new (std::nothrow) classname(); }); \
        }                                                                        \
        return obj;                                                              \
    }                                                                            \
    ~classname();                                                                \
                                                                                 \
private:                                                                         \
    classname();                                                                 \
    CUTL_COPY_AND_ASSIGN(classname)

#undef CUTL_SINGLETON_PTR_DEFAULT_CTOR
#define CUTL_SINGLETON_PTR_DEFAULT_CTOR(classname)                               \
public:                                                                          \
    static classname *get_instance(bool create_if_needed = true)                 \
    {                                                                            \
        static classname *obj = nullptr;                                         \
        if (!obj && create_if_needed)                                            \
        {                                                                        \
            static std::once_flag flag;                                          \
            std::call_once(flag, [&] { obj = new (std::nothrow) classname(); }); \
        }                                                                        \
        return obj;                                                              \
    }                                                                            \
    ~classname();                                                                \
                                                                                 \
private:                                                                         \
    classname() = default;                                                       \
    CUTL_COPY_AND_ASSIGN(classname)

#undef CUTL_SINGLETON_REF
#define CUTL_SINGLETON_REF(classname)                            \
public:                                                          \
    static classname &get_instance(bool create_if_needed = true) \
    {                                                            \
        static classname obj;                                    \
        return obj;                                              \
    }                                                            \
    ~classname();                                                \
                                                                 \
private:                                                         \
    classname();                                                 \
    CUTL_COPY_AND_ASSIGN(classname)

#undef CUTL_SINGLETON_REF_DEFAULT_CTOR
#define CUTL_SINGLETON_REF_DEFAULT_CTOR(classname)               \
public:                                                          \
    static classname &get_instance(bool create_if_needed = true) \
    {                                                            \
        static classname obj;                                    \
        return obj;                                              \
    }                                                            \
    ~classname();                                                \
                                                                 \
private:                                                         \
    classname() = default;                                       \
    CUTL_COPY_AND_ASSIGN(classname)
