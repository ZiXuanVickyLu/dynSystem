/**
 * Copyright 2017-2021 Xiaowei He
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
#include "Module.h"
#include "Vector.h"
#include "Quat.h"

namespace dyno
{
	class VisualModule : public Module
	{
	public:
		VisualModule();
		virtual ~VisualModule();

		void setVisible(bool bVisible);
		bool isVisible() { return this->varVisible()->getData(); }

		void rotate(float angle, float x, float y, float z);
		void translate(float x, float y, float z);
		void scale(float x, float y, float z);

		std::string getModuleType() override { return "VisualModule"; }

		virtual void updateGraphicsContext() {};

	protected:
		void updateImpl() final;

	private:
		DEF_VAR(bool, Visible, true, "A toggle to control the viability");

		Quat<float> m_rotation;
		Vec3f m_scale;
		Vec3f m_translation;
	};
}

