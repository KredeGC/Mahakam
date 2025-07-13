#pragma once

#include "SerializeTraits.h"

#include <cstdint>
#include <cstdio>
#include <filesystem>

namespace Mahakam::Serialization
{
    class FileReader
    {
    public:
        static constexpr bool writing = false;
        static constexpr bool reading = true;

    public:
        FileReader(const std::filesystem::path& filepath) :
            m_File(std::fopen(filepath.string().c_str(), "rb")) {}

        ~FileReader()
        {
            if (m_File)
                fclose(m_File);
        }

        bool close() noexcept
        {
            int status = fclose(m_File);

            m_File = nullptr;

            return status == 0;
        }

        template<typename T>
        [[nodiscard]] bool serialize_bytes(T* bytes, uint32_t num_elements) noexcept
        {
            return std::fread(bytes, sizeof(T), num_elements, m_File) == num_elements;
        }

        /**
         * @brief Reads from the buffer, using the given @p Trait.
         * @note The Trait type in this function must always be explicitly declared
         * @tparam Trait A template specialization of serialize_trait<>
         * @tparam ...Args The types of the arguments to pass to the serialize function
         * @param ...args The arguments to pass to the serialize function
         * @return Whether successful or not
        */
        template<typename Trait, typename... Args, typename = utility::has_serialize_t<Trait, FileReader, Args...>>
        [[nodiscard]] bool serialize(Args&&... args) noexcept(utility::is_serialize_noexcept_v<Trait, FileReader, Args...>)
        {
            return SerializeTraits<Trait>::serialize(*this, std::forward<Args>(args)...);
        }

        /**
         * @brief Reads from the buffer, by trying to deduce the trait.
         * @note The Trait type in this function is always implicit and will be deduced from the first argument if possible.
         * If the trait cannot be deduced it will not compile.
         * @tparam Trait The type of the first argument, which will be used to deduce the trait specialization
         * @tparam ...Args The types of the arguments to pass to the serialize function
         * @param arg The first argument to pass to the serialize function
         * @param ...args The rest of the arguments to pass to the serialize function
         * @return Whether successful or not
        */
        template<typename... Args, typename Trait, typename = utility::has_deduce_serialize_t<Trait, FileReader, Args...>>
        [[nodiscard]] bool serialize(Trait&& arg, Args&&... args) noexcept(utility::is_deduce_serialize_noexcept_v<Trait, FileReader, Args...>)
        {
            return SerializeTraits<utility::deduce_trait_t<Trait, FileReader, Args...>>::serialize(*this, std::forward<Trait>(arg), std::forward<Args>(args)...);
        }

    private:
        std::FILE* m_File;
    };

    class FileWriter
    {
    public:
        static constexpr bool writing = true;
        static constexpr bool reading = false;

    public:
        FileWriter(const std::filesystem::path& filepath) :
            m_File(std::fopen(filepath.string().c_str(), "wb")) {}

        ~FileWriter()
        {
            if (m_File)
                fclose(m_File);
        }

        bool close() noexcept
        {
            int status = fclose(m_File);

            m_File = nullptr;

            return status == 0;
        }

        template<typename T>
        [[nodiscard]] bool serialize_bytes(const T* bytes, uint32_t num_elements) noexcept
        {
            return std::fwrite(bytes, sizeof(T), num_elements, m_File) == num_elements;
        }

        /**
         * @brief Writes to the buffer, using the given @p Trait.
         * @note The Trait type in this function must always be explicitly declared
         * @tparam Trait A template specialization of serialize_trait<>
         * @tparam ...Args The types of the arguments to pass to the serialize function
         * @param ...args The arguments to pass to the serialize function
         * @return Whether successful or not
        */
        template<typename Trait, typename... Args, typename = utility::has_serialize_t<Trait, FileWriter, Args...>>
        [[nodiscard]] bool serialize(Args&&... args) noexcept(utility::is_serialize_noexcept_v<Trait, FileWriter, Args...>)
        {
            return SerializeTraits<Trait>::serialize(*this, std::forward<Args>(args)...);
        }

        /**
         * @brief Writes to the buffer, by trying to deduce the trait.
         * @note The Trait type in this function is always implicit and will be deduced from the first argument if possible.
         * If the trait cannot be deduced it will not compile.
         * @tparam Trait The type of the first argument, which will be used to deduce the trait specialization
         * @tparam ...Args The types of the arguments to pass to the serialize function
         * @param arg The first argument to pass to the serialize function
         * @param ...args The rest of the arguments to pass to the serialize function
         * @return Whether successful or not
        */
        template<typename... Args, typename Trait, typename = utility::has_deduce_serialize_t<Trait, FileWriter, Args...>>
        [[nodiscard]] bool serialize(Trait&& arg, Args&&... args) noexcept(utility::is_deduce_serialize_noexcept_v<Trait, FileWriter, Args...>)
        {
            return SerializeTraits<utility::deduce_trait_t<Trait, FileWriter, Args...>>::serialize(*this, std::forward<Trait>(arg), std::forward<Args>(args)...);
        }

    private:
        std::FILE* m_File;
    };
}