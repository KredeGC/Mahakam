#pragma once

#include "ShaderDataTypes.h"

#include <vector>

namespace Mahakam
{
	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:
			return 4;
		case ShaderDataType::Float2:
			return 8;
		case ShaderDataType::Float3:
			return 12;
		case ShaderDataType::Float4:
			return 16;
		case ShaderDataType::Mat3:
			return 4 * 3 * 3;
		case ShaderDataType::Mat4:
			return 4 * 4 * 4;
		case ShaderDataType::Int:
			return 4;
		case ShaderDataType::Int2:
			return 8;
		case ShaderDataType::Int3:
			return 12;
		case ShaderDataType::Int4:
			return 16;
		case ShaderDataType::Bool:
			return 1;
		}

		MH_CORE_BREAK("Unknown shader data type!");
		return 0;
	}


	struct BufferElement
	{
		ShaderDataType type = ShaderDataType::None;
		std::string name = "";
		uint32_t size = 0;
		uint32_t offset = 0;
		bool normalized = false;

		BufferElement() {}

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: type(type), name(name), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized) {}

		uint32_t GetComponentCount() const
		{
			switch (type)
			{
			case ShaderDataType::Float:
				return 1;
			case ShaderDataType::Float2:
				return 2;
			case ShaderDataType::Float3:
				return 3;
			case ShaderDataType::Float4:
				return 4;
			case ShaderDataType::Mat3:
				return 3 * 3;
			case ShaderDataType::Mat4:
				return 4 * 4;
			case ShaderDataType::Int:
				return 1;
			case ShaderDataType::Int2:
				return 2;
			case ShaderDataType::Int3:
				return 3;
			case ShaderDataType::Int4:
				return 4;
			case ShaderDataType::Bool:
				return 1;
			}

			MH_CORE_BREAK("Unknown shader data type!");
			return 0;
		}
	};


	class BufferLayout
	{
	private:
		std::vector<BufferElement> elements;
		uint32_t stride = 0;

	public:
		BufferLayout() {}

		BufferLayout(const std::initializer_list<BufferElement>& elements) : elements(elements)
		{
			CalculateOffsetAndStride();
		}

		BufferLayout(const std::vector<BufferElement>& elements) : elements(elements)
		{
			CalculateOffsetAndStride();
		}

		inline const BufferElement& GetElement(int index) const { return elements[index]; }
		inline const std::vector<BufferElement>& GetElements() const { return elements; }
		inline uint32_t GetStride() const { return stride; }

		std::vector<BufferElement>::iterator begin() { return elements.begin(); }
		std::vector<BufferElement>::iterator end() { return elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return elements.end(); }

	private:
		void CalculateOffsetAndStride();
	};


#pragma region VertexBuffer
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const char* data, uint32_t size) = 0;

		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;

		static Ref<VertexBuffer> Create(const char* vertices, uint32_t size);
		static Ref<VertexBuffer> Create(uint32_t size);
	};
#pragma endregion


#pragma region IndexBuffer
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};
#pragma endregion


#pragma region UniformBuffer
	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() {}

		virtual void Bind(int slot = 0, int offset = 0, int size = 0) const = 0;
		virtual void Unbind(int slot = 0) const = 0;

		virtual void SetData(const void* data, uint32_t offset = 0, uint32_t size = 0) = 0;

		static Ref<UniformBuffer> Create(uint32_t size);
	};
#pragma endregion


#pragma region StorageBuffer
	class StorageBuffer
	{
	public:
		virtual ~StorageBuffer() {}

		virtual void Bind(int slot = 0, int offset = 0, int size = 0) const = 0;
		virtual void Unbind(int slot = 0) const = 0;

		virtual uint32_t GetSize() const = 0;

		virtual void SetData(const void* data, uint32_t offset = 0, uint32_t size = 0) = 0;

		static Ref<StorageBuffer> Create(uint32_t size);
	};
#pragma endregion
}