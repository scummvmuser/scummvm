/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the AUTHORS
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "engines/stark/gfx/openglssurface.h"

#include "engines/stark/gfx/opengls.h"
#include "engines/stark/gfx/texture.h"

#include "graphics/opengles2/shader.h"

namespace Stark {
namespace Gfx {

OpenGLSSurfaceRenderer::OpenGLSSurfaceRenderer(OpenGLSDriver *gfx) :
	SurfaceRenderer(),
	_gfx(gfx) {
	_shader = _gfx->createSurfaceShaderInstance();
}

OpenGLSSurfaceRenderer::~OpenGLSSurfaceRenderer() {
	delete _shader;
}

void OpenGLSSurfaceRenderer::render(const Texture *texture, const Common::Point &dest) {
	// Source texture rectangle
	const float tLeft = 0.0;
	const float tWidth = 1.0;
	const float tTop = 0.0;
	const float tHeight = 1.0;

	// Destination rectangle
	const float sLeft = dest.x;
	const float sTop = dest.y;
	const float sWidth = texture->width();
	const float sHeight = texture->height();

	_gfx->start2DMode();

	_shader->use();
	_shader->setUniform("textured", true);
	_shader->setUniform("color", Math::Vector4d(1.0f, 1.0f, 1.0f, 1.0f));
	_shader->setUniform("verOffsetXY", normalizeOriginalCoordinates(sLeft, sTop));
	if (_noScalingOverride) {
		_shader->setUniform("verSizeWH", normalizeCurrentCoordinates(sWidth, sHeight));
	} else {
		_shader->setUniform("verSizeWH", normalizeOriginalCoordinates(sWidth, sHeight));
	}
	_shader->setUniform("texOffsetXY", Math::Vector2d(tLeft, tTop));
	_shader->setUniform("texSizeWH", Math::Vector2d(tWidth, tHeight));

	texture->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	_gfx->end2DMode();
}

Math::Vector2d OpenGLSSurfaceRenderer::normalizeOriginalCoordinates(float x, float y) const {
	Common::Rect viewport = _gfx->getUnscaledViewport();
	return Math::Vector2d(x / (float) viewport.width(), y / (float) viewport.height());
}

Math::Vector2d OpenGLSSurfaceRenderer::normalizeCurrentCoordinates(float x, float y) const {
	Common::Rect viewport = _gfx->getViewport();
	return Math::Vector2d(x / (float) viewport.width(), y / (float) viewport.height());
}

} // End of namespace Gfx
} // End of namespace Stark
