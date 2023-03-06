/**
 * Copyright 2017-2021 Jian SHI
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#pragma once
#include "Topology/TriangleSet.h"

#include "GLVisualModule.h"
#include "gl/VertexArray.h"
#include "../Engine/OpenGL/backend/Vulkan/VulkanBuffer.h"
#include "gl/Shader.h"

#include <DeclarePort.h>

namespace dyno
{
	class PointVisualModule : public GLVisualModule
	{
		
	public:

		enum ColorMapMode
		{
			PER_OBJECT_SHADER = 0,	// use constant color
			PER_VERTEX_SHADER = 1
		};

		void setColorMapMode(ColorMapMode mode);

		PointVisualModule();
		~PointVisualModule();
		virtual std::string caption() override;

	public:
		//DEF_INSTANCE_IN(PointSet, PointSet, "");

		DEF_ARRAY_IN(Vec3f, Position, DeviceType::GPU, "");

		DEF_VAR(float, PointSize, 0.04f, "Size of rendered particles");

	protected:
		virtual void paintGL(GLRenderPass pass) override;
		virtual void updateGL() override;
		virtual bool initializeGL() override;

		//TODO:
		virtual void destroyGL() {};

	private:


		gl::Program* mShaderProgram;

		gl::VertexArray	mVAO;
		gl::VulkanBuffer* mVertexBuffer;
		gl::VulkanBuffer* mColorBuffer;

		// for instanced rendering
		gl::VulkanBuffer* mInstanceBuffer;
		unsigned int	mNumPoints;

		ColorMapMode	mColorMode = ColorMapMode::PER_OBJECT_SHADER;
	};
};
