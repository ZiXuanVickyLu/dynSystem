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

#include "Object.h"

namespace gl {

	class Texture2D : public Object
	{
		GL_OBJECT(Texture2D)
	public:
		Texture2D();

		virtual void create() override;
		virtual void release() override;

		virtual void bind();
		virtual void unbind();

		virtual void bind(int slot);

		virtual void dump(void* pixels);

		virtual void resize(int w, int h);
		virtual void load(int w, int h, void* data);

		// OpenGL 4.4+, clear texture
		virtual void clear(void* value);

		void genMipmap();

	public:
		unsigned int target = -1;

		unsigned int internalFormat;
		unsigned int format;
		unsigned int type;

		unsigned int minFilter;
		unsigned int maxFilter;

	};

}

