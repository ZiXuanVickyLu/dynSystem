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

#include "GLVisualModule.h"
#include "gl/VertexArray.h"
#include "../Engine/OpenGL/backend/Vulkan/VulkanBuffer.h"
#include "gl/Shader.h"

#include <DeclarePort.h>
#include <Topology/TriangleSet.h>

namespace dyno
{
	class SurfaceVisualModule : public GLVisualModule
	{
		DECLARE_CLASS(SurfaceVisualModule)
	public:
		SurfaceVisualModule();
		~SurfaceVisualModule();

	public:
		virtual std::string caption() override;

		DECLARE_ENUM(EColorMode,
			CM_Object = 0,
			CM_Vertex = 1);

		DEF_ENUM(EColorMode, ColorMode, EColorMode::CM_Object, "Color Mode");

		DEF_VAR(bool, UseVertexNormal, false, "")

		//DEF_INSTANCE_IN(TriangleSet, TriangleSet, "");

		DEF_ARRAY_IN(Vec3f, Position, DeviceType::GPU, "");
		
		DArray<Vector<uint,3>> m_triangles;

	protected:
		virtual void paintGL(GLRenderPass mode) override;
		virtual void updateGL() override;
		virtual bool initializeGL() override;

		//TODO:
		virtual void destroyGL() {};

	protected:

		gl::Program*			mShaderProgram;

		gl::VertexArray	mVAO;

		gl::VulkanBuffer* 		mIndexBuffer;
		gl::VulkanBuffer*		mVertexBuffer;
		gl::VulkanBuffer*		mNormalBuffer;
		gl::VulkanBuffer*		mColorBuffer;

		unsigned int	mDrawCount = 0;

		// for instanced rendering
		gl::VulkanBuffer*		mInstanceBuffer;
		unsigned int	mInstanceCount = 0;
	};
};