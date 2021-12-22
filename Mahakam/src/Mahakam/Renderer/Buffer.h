#pragma once

#include <vector>

namespace Mahakam
{
	enum class ShaderDataType
	{
		None = 0,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool
	};

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

		MH_CORE_ASSERT(false, "Unknown shader data type!");
		return 0;
	}


	struct BufferElement
	{
		ShaderDataType type;
		std::string name;
		uint32_t size;
		uint32_t offset;
		bool normalized;

		BufferElement() {}

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized) {}

		uint32_t getComponentCount() const
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

			MH_CORE_ASSERT(false, "Unknown shader data type!");
			return 0;
		}
	};


	class BufferLayout
	{
	private:
		std::vector<BufferElement> elements;
		uint32_t stride = 0;

		void calculateOffsetAndStride();

	public:
		BufferLayout() {}

		BufferLayout(const std::initializer_list<BufferElement>& elements) : elements(elements) 
		{
			calculateOffsetAndStride();
		}

		inline const std::vector<BufferElement>& getElements() const { return elements; }
		inline uint32_t getStride() const { return stride; }

		std::vector<BufferElement>::iterator begin() { return elements.begin(); }
		std::vector<BufferElement>::iterator end() { return elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return elements.end(); }
	};


	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void setLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& getLayout() const = 0;

		static Ref<VertexBuffer> create(float* vertices, uint32_t size);
		static Ref<VertexBuffer> create(uint32_t size);
	};


	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual uint32_t getCount() const = 0;

		static Ref<IndexBuffer> create(uint32_t* vertices, uint32_t count);
	};
}