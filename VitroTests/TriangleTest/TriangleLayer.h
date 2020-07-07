#pragma once

#include <Vitro.h>

class TriangleLayer : public Vitro::Layer
{
public:
	TriangleLayer()
	{
		Vertices	   = Vitro::VertexBuffer::New(Triangle, sizeof(Vertex), sizeof(Triangle));
		Indices		   = Vitro::IndexBuffer::New(TriangleIndices, ArrayCount(TriangleIndices));
		VertexShader   = Vitro::VertexShader::New(Vitro::File("TriangleVertex.cso"));
		FragmentShader = Vitro::FragmentShader::New(Vitro::File("TriangleFragment.cso"));
	}

	void OnAttach() override
	{
		std::srand(static_cast<uint32_t>(time(nullptr)));

		using namespace Vitro;
		VertexLayout vl {{VertexField::Position, 0, VertexFieldType::Float3},
						 {VertexField::Color, 0, VertexFieldType::Float4}};
		VertexShader->SetVertexLayout(vl);
		VertexShader->Bind();
		FragmentShader->Bind();
	}

	void OnDetach() override
	{}

	void OnTick(Vitro::Tick t) override
	{
		Vertices->Bind(Vitro::VertexTopology::TriangleList);
		Renderer->Submit(Indices);
	}

	void OnEvent(Vitro::Event& e) override
	{
		using namespace Vitro;

		auto random = []() {
			return std::rand() / static_cast<float>(RAND_MAX);
		};
		e.Dispatch<KeyDownEvent>([random, this](KeyDownEvent& e) {
			for(auto& vertex : Triangle)
				vertex.Color = {random(), random(), random(), random()};
			Vertices->Update(Triangle);
			return true;
		});
	}

private:
	struct Vertex
	{
		Vitro::Float3 Position;
		Vitro::Float4 Color;
	};

	Vertex Triangle[3] {
		{{0.0f, 0.5f, 0.5}, {1, 0, 0, 1}}, {{0.5f, -.5f, 0.5}, {0, 1, 0, 1}}, {{-.5f, -.5f, 0.5}, {0, 0, 1, 1}}};

	uint32_t TriangleIndices[3] {0, 1, 2};

	Vitro::Ref<Vitro::VertexBuffer> Vertices;
	Vitro::Ref<Vitro::IndexBuffer> Indices;
	Vitro::Ref<Vitro::VertexShader> VertexShader;
	Vitro::Ref<Vitro::FragmentShader> FragmentShader;
};
