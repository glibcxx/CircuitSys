#pragma once

#include <atomic>
#include <memory>

template <class T>
class WeakPtr;

template <typename T>
class SharedCounter
{
public:
    T               *ptr = nullptr;
    std::atomic<int> share_count = 0;
    std::atomic<int> weak_count = 0;

    SharedCounter(T *p) :
        ptr(p) {}

    ~SharedCounter() {}

    void addSharedRef() { ++this->share_count; }

    void addWeakRef() { ++this->weak_count; }

    bool releaseSharedRef()
    {
        --this->share_count;
        if (this->share_count > 0)
            return false;
        if (this->ptr)
        {
            auto tmp = this->ptr;
            this->ptr = nullptr;
            if (tmp)
                tmp->~T();
        }
        return this->weak_count <= 0;
    }

    bool releaseWeakRef()
    {
        --this->weak_count;
        return this->weak_count <= 0 && !this->ptr;
    }
};

template <typename T>
class SharedPtr
{
public:
    SharedCounter<T> *pc = nullptr;

    SharedPtr() {}

    SharedPtr(std::nullptr_t) {}

    SharedPtr(T *rawPtr)
    {
        if (rawPtr)
        {
            this->pc = new SharedCounter<T>{rawPtr};
            this->pc->addSharedRef();
        }
    }

    SharedPtr(const SharedPtr &sp) :
        pc(sp.pc)
    {
        if (this->pc)
            this->pc->addSharedRef();
    }

    template <typename TDerived, std::enable_if_t<std::is_base_of_v<T, TDerived>, std::nullptr_t> = nullptr>
    SharedPtr(const SharedPtr<TDerived> &sp) :
        pc(reinterpret_cast<SharedCounter<T> *>(sp.pc))
    {
        if (this->pc)
            this->pc->addSharedRef();
    }

    ~SharedPtr() { this->reset(); }

    T *get() const { return this->pc ? this->pc->ptr : nullptr; }

    operator T *() const { return this->pc ? this->pc->ptr : nullptr; }

    T &operator*() const { return this->pc->ptr; }

    T *operator->() const { return this->pc->ptr; }

    SharedPtr &operator=(const SharedPtr &other)
    {
        SharedPtr tmp{other};
        std::swap(this->pc, tmp.pc);
        return *this;
    }

    void reset()
    {
        if (this->pc)
        {
            if (this->pc->releaseSharedRef() && this->pc)
                delete this->pc;
            this->pc = nullptr;
        }
    }

    WeakPtr<T> createWeakPtr() { return WeakPtr<T>{*this}; }

    template <typename... Args>
    static SharedPtr make(Args &&...args)
    {
        T *rawPtr = new T{std::forward<Args>(args)...};
        return rawPtr;
    }
};

template <class T>
class WeakPtr
{
    SharedCounter<T> *pc;

public:
    WeakPtr() :
        pc(nullptr) {}

    WeakPtr(std::nullptr_t) :
        pc(nullptr) {}

    WeakPtr(const SharedPtr<T> &sp)
    {
        this->pc = sp.pc;
        if (this->pc)
            this->pc->addWeakRef();
    }

    WeakPtr(const WeakPtr<T> &wp)
    {
        this->pc = wp.pc;
        if (this->pc)
            this->pc->addWeakRef();
    }

    ~WeakPtr()
    {
        this->reset();
    }

    bool isNull() const { return !this->pc || this->pc->ptr == nullptr; }

    bool operator==(T *p) { return this->pc ? this->pc->ptr == p : p == nullptr; }

    T *get() const { return this->pc->ptr; }

    operator T *() const { return this->pc->ptr; }

    T &operator*() const { return *this->pc->ptr; }

    T *operator->() const { return this->pc->ptr; }

    T &operator[](int idx) const;

    void reset()
    {
        if (!this->pc)
            return;
        if (this->pc->releaseWeakRef() && this->pc)
            delete this->pc;

        this->pc = nullptr;
    }

    static const WeakPtr &null()
    {
        static WeakPtr<T> wnull;
        return wnull;
    }
};
