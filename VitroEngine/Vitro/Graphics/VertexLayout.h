#pragma once

#include "Vitro/Graphics/VertexField.h"
#include "Vitro/Utility/HeapArray.h"

namespace Vitro
{
	class VertexLayout final
	{
	public:
		VertexLayout(const std::initializer_list<VertexField>& fields);

		const VertexField& operator[](size_t index) const;
		size_t Count() const;
		size_t GetStride() const;

		std::vector<VertexField>::const_iterator begin() const;
		std::vector<VertexField>::const_iterator end() const;

	private:
		std::vector<VertexField> Fields;
		size_t Stride;
	};
}
