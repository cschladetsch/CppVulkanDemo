#pragma once

#include "Vulkan.hpp"

VTB_BEGIN

// Provides a `heap` that always increases in size, so that objects
// can be referenced by pointer across method calls without using the runtime heap.
// This is only suitable for PODs.
struct BuilderBase
{
protected:
    BuilderBase(size_t size = heapSize_) : heap_(new char[size]), offset_(0) { }
    ~BuilderBase() { delete[] heap_; }

protected:
    template <class Struct>
    Struct &Alloc()
    {
        static_assert(std::is_pod<Struct>::value, "A must be a POD type.");

        // we can't realloc because there may be external pointers to objects in
        // our `heap`.
        if (offset_ + sizeof(Struct) > heapSize_)
            throw new std::bad_alloc();
        auto top = offset_;

        // TODO: alignment(?)
        offset_ += sizeof(Struct);

        return *reinterpret_cast<Struct *>(heap_ + top);
    }

private:
    static const size_t heapSize_ = 8000;
    char *heap_;
    size_t offset_;
};

VTB_END
