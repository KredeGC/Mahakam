#include "Mahakam/mhpch.h"
#include "FileStream.h"

#include <cstdio>

namespace Mahakam::Serialization
{
    FileReader::FileReader(const std::filesystem::path& filepath) :
        m_File(std::fopen(filepath.string().c_str(), "rb")) {
    }

    FileReader::FileReader(FileReader&& other) noexcept :
        m_File(other.m_File)
    {
        other.m_File = nullptr;
    }

    FileReader& FileReader::operator=(FileReader&& rhs) noexcept
    {
        m_File = rhs.m_File;
        rhs.m_File = nullptr;
        return *this;
    }

    FileReader::~FileReader()
    {
        if (m_File)
            std::fclose(static_cast<std::FILE*>(m_File));
    }

    bool FileReader::close() noexcept
    {
        int status = std::fclose(static_cast<std::FILE*>(m_File));

        m_File = nullptr;

        return status == 0;
    }

    size_t FileReader::tell() noexcept
    {
        return std::ftell(static_cast<std::FILE*>(m_File));
    }

    void FileReader::seek(size_t offset) noexcept
    {
        std::fseek(static_cast<std::FILE*>(m_File), static_cast<long>(offset), SEEK_SET);
    }

    [[nodiscard]] bool FileReader::serialize_bytes(void* bytes, uint32_t num_elements, size_t element_size) noexcept
    {
        return std::fread(bytes, element_size, num_elements, static_cast<std::FILE*>(m_File)) == num_elements;
    }

    FileWriter::FileWriter(const std::filesystem::path& filepath) :
        m_File(std::fopen(filepath.string().c_str(), "wb"))
    {}

    FileWriter::FileWriter(FileWriter&& other) noexcept :
        m_File(other.m_File)
    {
        other.m_File = nullptr;
    }

    FileWriter& FileWriter::operator=(FileWriter&& rhs) noexcept
    {
        m_File = rhs.m_File;
        rhs.m_File = nullptr;
        return *this;
    }

    FileWriter::~FileWriter()
    {
        if (m_File)
            std::fclose(static_cast<std::FILE*>(m_File));
    }

    bool FileWriter::close() noexcept
    {
        int status = std::fclose(static_cast<std::FILE*>(m_File));

        m_File = nullptr;

        return status == 0;
    }

    [[nodiscard]] bool FileWriter::serialize_bytes(const void* bytes, size_t num_elements, size_t element_size) noexcept
    {
        return std::fwrite(bytes, element_size, num_elements, static_cast<std::FILE*>(m_File)) == num_elements;
    }
}