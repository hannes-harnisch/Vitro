#pragma once

#include <Vitro.h>

class CubeLayer : public Vitro::Layer
{
public:
	CubeLayer(int width, int height) :
		Cam({-3, 0, -3}, {3, 0, 3}, Vitro::Perspective(0.4f * 3.14f, width, height, 1.0f, 1000.f))
	{
		Vertices	   = Vitro::VertexBuffer::New(Cube, sizeof(Vertex), sizeof(Cube));
		Indices		   = Vitro::IndexBuffer::New(CubeIndices, ArrayCount(CubeIndices));
		Uniforms	   = Vitro::UniformBuffer::New(&CamUniforms, sizeof(CamUniforms));
		VertexShader   = Vitro::VertexShader::New(Vitro::File("CubeVertex.cso"));
		FragmentShader = Vitro::FragmentShader::New(Vitro::File("CubeFragment.cso"));
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
		using namespace Vitro;

		constexpr int distance = 5;
		constexpr int count	   = 3;
		for(int i = 0; i < count * distance; i += distance)
			for(int j = 0; j < count * distance; j += distance)
				for(int k = 0; k < count * distance; k += distance)
				{
					CamUniforms.MVP = Translate(Cam.GetViewProjection(), {i, j, k});
					Vertices->Bind(VertexTopology::TriangleList);
					Uniforms->Update(&CamUniforms);
					Uniforms->Bind();
					Renderer->Submit(Indices);
				}

		float moveSpeed = 15 * t;

		if(Input::IsDown(KeyCode::A))
			Cam.Translate({-moveSpeed, 0, 0});
		if(Input::IsDown(KeyCode::D))
			Cam.Translate({moveSpeed, 0, 0});

		if(Input::IsDown(KeyCode::Q))
			Cam.Translate({0, -moveSpeed, 0});
		if(Input::IsDown(KeyCode::E))
			Cam.Translate({0, moveSpeed, 0});

		if(Input::IsDown(KeyCode::S))
			Cam.Translate({0, 0, -moveSpeed});
		if(Input::IsDown(KeyCode::W))
			Cam.Translate({0, 0, moveSpeed});

		if(Input::IsDown(KeyCode::R))
			Cam.SetPosition({-3, 0, -3});

		if(Input::IsDown(KeyCode::F))
			Cam.Roll(-t);
		if(Input::IsDown(KeyCode::G))
			Cam.Roll(t);
	}

	void OnEvent(Vitro::Event& e) override
	{
		auto random = []() {
			return std::rand() / static_cast<float>(RAND_MAX);
		};

		using namespace Vitro;
		e.Dispatch<MouseUpEvent>([random, this](MouseUpEvent&) {
			for(auto& vertex : Cube)
				vertex.Color = {random(), random(), random(), random()};
			Vertices->Update(Cube);
			return true;
		});

		e.Dispatch<MouseMoveEvent>([this](MouseMoveEvent& e) {
			Cam.Pitch(Radians(e.GetYDirection()));
			Cam.Yaw(Radians(e.GetXDirection()));
			return true;
		});
	}

private:
	struct Vertex
	{
		Vitro::Float3 Position;
		Vitro::Float4 Color;
	};

	Vertex Cube[8] {{{-1, -1, -1}, {1, 0, 0, 1}}, {{-1, 1, -1}, {1, 1, 0, 1}}, {{1, 1, -1}, {0, 1, 0, 1}},
					{{1, -1, -1}, {0, 0, 1, 1}},  {{-1, -1, 1}, {1, 0, 0, 1}}, {{-1, 1, 1}, {1, 1, 0, 1}},
					{{1, 1, 1}, {0, 1, 0, 1}},	  {{1, -1, 1}, {0, 0, 1, 1}}};

	uint32_t CubeIndices[36] {0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6, 4, 5, 1, 4, 1, 0,
							  3, 2, 6, 3, 6, 7, 1, 5, 6, 1, 6, 2, 4, 0, 3, 4, 3, 7};

	struct CameraUniforms
	{
		Vitro::Float4x4 MVP;
	};
	CameraUniforms CamUniforms;
	Vitro::Camera Cam;

	Vitro::Ref<Vitro::VertexBuffer> Vertices;
	Vitro::Ref<Vitro::IndexBuffer> Indices;
	Vitro::Ref<Vitro::UniformBuffer> Uniforms;
	Vitro::Ref<Vitro::VertexShader> VertexShader;
	Vitro::Ref<Vitro::FragmentShader> FragmentShader;
};
