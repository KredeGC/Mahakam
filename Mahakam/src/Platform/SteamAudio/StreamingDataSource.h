#pragma once

#include <miniaudio/miniaudio.h>

namespace Mahakam
{
    struct StreamDataSource
    {
        ma_data_source_base base;
    };

    static ma_result streaming_data_source_read(ma_data_source* pDataSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead)
    {
        // Read data here. Output in the same format returned by my_data_source_get_data_format().
    }

    static ma_result streaming_data_source_seek(ma_data_source* pDataSource, ma_uint64 frameIndex)
    {
        // Seek to a specific PCM frame here. Return MA_NOT_IMPLEMENTED if seeking is not supported.
    }

    static ma_result streaming_data_source_get_data_format(ma_data_source* pDataSource, ma_format* pFormat, ma_uint32* pChannels, ma_uint32* pSampleRate, ma_channel* pChannelMap, size_t channelMapCap)
    {
        // Return the format of the data here.
    }

    static ma_result streaming_data_source_get_cursor(ma_data_source* pDataSource, ma_uint64* pCursor)
    {
        // Retrieve the current position of the cursor here. Return MA_NOT_IMPLEMENTED and set *pCursor to 0 if there is no notion of a cursor.
    }

    static ma_result streaming_data_source_get_length(ma_data_source* pDataSource, ma_uint64* pLength)
    {
        // Retrieve the length in PCM frames here. Return MA_NOT_IMPLEMENTED and set *pLength to 0 if there is no notion of a length or if the length is unknown.
    }

    static ma_data_source_vtable g_streaming_data_source_vtable =
    {
        streaming_data_source_read,
        streaming_data_source_seek,
        streaming_data_source_get_data_format,
        streaming_data_source_get_cursor,
        streaming_data_source_get_length
    };

    ma_result streaming_data_source_init(StreamDataSource* pMyDataSource)
    {
        // TODO: To make streaming work properly, a lot of work is needed
        // 1. The data source must be able to read from the AssetDatabase, at an arbitrary point
        // 2. The data source should only read chunks of data


        ma_result result;
        ma_data_source_config baseConfig;

        baseConfig = ma_data_source_config_init();
        baseConfig.vtable = &g_streaming_data_source_vtable;

        result = ma_data_source_init(&baseConfig, &pMyDataSource->base);
        if (result != MA_SUCCESS) {
            return result;
        }

        // ... do the initialization of your custom data source here ...

        return MA_SUCCESS;
    }

    void streaming_data_source_uninit(StreamDataSource* pMyDataSource)
    {
        // ... do the uninitialization of your custom data source here ...

        // You must uninitialize the base data source.
        ma_data_source_uninit(&pMyDataSource->base);
    }
}