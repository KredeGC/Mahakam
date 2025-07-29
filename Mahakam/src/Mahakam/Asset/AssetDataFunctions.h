#pragma once

#include "Mahakam/Core/Allocator.h"

#include <cstdint>
#include <type_traits>

namespace Mahakam
{
    enum class AssetState
    {
        Failed = 0,
        Loaded,
        Processing,
        Streaming
    };

    using AssetID = uint64_t;

    struct ControlBlock;

    struct AssetDataFunctions
    {
        ControlBlock* (*CreateControlBlock)();
        void (*MoveConstruct)(void*, void*);
        void (*MoveAssign)(void*, void*);
        void (*Delete)(void*);
    };

    struct ControlBlock
    {
        // ID 0 is guaranteed to be invalid
        size_t UseCount;
        AssetID ID;
        AssetState State;
        AssetDataFunctions* Functions;
    };

    template<typename T>
    struct DataBlock
    {
        ControlBlock Control;
        T Data;
    };

    template<typename T>
    AssetDataFunctions* GetAssetDataFunctions()
    {
        auto control = []()
            {
                DataBlock<T>* block = Allocator::Allocate<DataBlock<T>>(1);

                block->Control.UseCount = 0;
                block->Control.ID = 0;
                block->Control.State = AssetState::Processing;
                block->Control.Functions = GetAssetDataFunctions<T>();

                return &block->Control;
            };

        auto constructor = [](void* obj, void* other)
            {
                Allocator::Construct(static_cast<T*>(obj), std::move(*static_cast<T*>(other)));
            };

        auto mover = [](void* from, void* to)
            {
                *static_cast<T*>(to) = std::move(*static_cast<T*>(from));
            };

        auto deleter = [](void* p)
            {
                DataBlock<T>* block = static_cast<DataBlock<T>*>(p);

                if (block->Control.State == AssetState::Loaded || block->Control.State == AssetState::Streaming)
                    Allocator::Deconstruct(&block->Data);

                Allocator::Deallocate(block, 1);
            };

        static AssetDataFunctions static_functions
        {
            control,
            constructor,
            mover,
            deleter
        };

        return &static_functions;
    }
}