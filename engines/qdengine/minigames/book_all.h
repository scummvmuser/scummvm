/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef QDENGINE_MINIGAMES_BOOK_ALL_H
#define QDENGINE_MINIGAMES_BOOK_ALL_H

#include "common/debug.h"

#include "qdengine/qd_fwd.h"
#include "qdengine/qdcore/qd_minigame_interface.h"

namespace QDEngine {

const float bookGusenica[95 * 2] = {
	 0.000f,  0.036f,  0.514f,  1.154f,  1.787f,  2.370f,  2.862f,  4.242f,  4.921f,  5.979f,
	 6.434f,  6.820f,  7.228f,  8.297f,  8.963f,  9.170f,  9.825f, 10.698f, 11.401f, 12.169f,
	12.868f, 13.416f, 14.107f, 15.232f, 16.679f, 17.048f, 17.618f, 18.119f, 19.437f, 19.882f,
	20.492f, 21.583f, 22.646f, 23.566f, 25.343f, 25.521f, 25.758f, 26.371f, 26.561f, 27.310f,
	27.692f, 28.216f, 28.685f, 29.035f, 29.596f, 30.499f, 31.886f, 32.309f, 32.441f, 33.276f,
	33.689f, 34.260f, 34.769f, 35.776f, 36.569f, 37.678f, 38.885f, 39.007f, 39.608f, 40.160f,
	41.874f, 42.118f, 42.637f, 42.775f, 43.555f, 43.949f, 44.187f, 44.761f, 45.475f, 45.861f,
	47.240f, 47.428f, 47.639f, 48.227f, 48.746f, 49.456f, 49.690f, 50.298f, 50.961f, 51.173f,
	51.770f, 52.395f, 52.937f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,

	 0.000f,  0.665f,  1.136f,  1.633f,  2.600f,  2.973f,  3.558f,  3.996f,  4.657f,  6.140f,
	 7.132f,  8.223f,  9.114f,  9.288f, 10.437f, 10.624f, 11.307f, 11.479f, 11.913f, 12.505f,
	13.162f, 13.287f, 14.580f, 15.192f, 16.004f, 16.552f, 17.128f, 17.805f, 19.214f, 19.860f,
	20.571f, 22.766f, 23.256f, 24.098f, 24.888f, 26.421f, 26.786f, 26.893f, 27.351f, 27.804f,
	28.530f, 29.882f, 30.415f, 31.506f, 31.614f, 32.150f, 33.647f, 34.333f, 34.811f, 35.659f,
	35.809f, 36.192f, 36.612f, 37.062f, 37.756f, 39.359f, 40.266f, 41.407f, 41.828f, 41.901f,
	42.447f, 43.290f, 43.980f, 45.047f, 46.263f, 46.407f, 47.836f, 48.311f, 49.430f, 49.752f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
};

const float bookGusenicaCZ[95 * 2] = {
	 0.000f,  0.075f,  0.372f,  1.112f,  1.399f,  1.851f,  1.975f,  2.232f,  3.738f,  3.987f,
	 4.133f,  4.479f,  5.108f,  5.316f,  5.754f,  6.463f,  7.273f,  7.406f,  7.636f,  8.092f,
	 8.797f,  9.319f,  9.833f, 10.238f, 10.968f, 11.075f, 11.876f, 12.620f, 14.243f, 14.438f,
	15.089f, 15.530f, 16.004f, 16.088f, 17.014f, 17.328f, 17.962f, 18.477f, 18.561f, 19.695f,
	19.965f, 20.253f, 20.585f, 20.939f, 21.223f, 21.577f, 22.161f, 22.369f, 22.950f, 24.185f,
	24.474f, 24.797f, 25.107f, 25.678f, 26.258f, 27.228f, 27.990f, 28.158f, 29.028f, 29.134f,
	29.688f, 30.675f, 31.189f, 31.627f, 31.769f, 32.035f, 32.455f, 32.741f, 33.277f, 33.503f,
	34.048f, 34.442f, 34.836f, 35.009f, 35.629f, 36.094f, 36.856f, 36.993f, 37.596f, 37.914f,
	38.561f, 39.106f, 39.612f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,

	 0.000f,  0.042f,  0.500f,  0.667f,  1.460f,  1.627f,  1.960f,  2.336f,  3.587f,  3.712f,
	 4.004f,  4.713f,  5.339f,  6.215f,  6.381f,  7.674f,  8.091f,  8.425f,  8.925f,  9.676f,
	10.052f, 10.302f, 11.220f, 11.595f, 12.137f, 12.221f, 12.721f, 13.430f, 13.597f, 13.722f,
	14.097f, 15.390f, 15.766f, 16.558f, 16.975f, 17.226f, 18.352f, 19.269f, 19.603f, 20.646f,
	21.355f, 21.897f, 22.105f, 23.065f, 23.607f, 23.732f, 24.316f, 25.817f, 26.276f, 26.568f,
	26.944f, 27.402f, 27.569f, 28.403f, 28.695f, 29.154f, 29.279f, 30.447f, 31.615f, 32.032f,
	33.075f, 33.283f, 33.784f, 34.660f, 34.910f, 35.369f, 35.661f, 36.036f, 36.995f, 37.329f,
	37.996f, 38.455f, 38.580f, 38.997f, 39.581f, 39.998f, 40.290f, 40.874f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
};

const float bookLes[95 * 2] = {
	 0.000f,  0.046f,  0.577f,  0.809f,  1.623f,  1.985f,  2.635f,  3.536f,  3.642f,  4.560f,
	 5.071f,  6.558f,  7.007f,  7.876f,  8.998f,  9.548f, 10.387f, 10.471f, 11.054f, 12.062f,
	12.081f, 12.462f, 12.647f, 12.900f, 13.987f, 14.265f, 14.809f, 15.558f, 16.332f, 17.643f,
	18.000f, 19.285f, 19.512f, 19.867f, 20.158f, 20.517f, 20.822f, 21.240f, 21.741f, 23.193f,
	23.704f, 24.338f, 25.117f, 26.058f, 26.845f, 26.989f, 27.455f, 28.551f, 28.859f, 29.181f,
	30.094f, 30.833f, 31.519f, 32.497f, 33.339f, 34.638f, 34.723f, 35.053f, 35.474f, 36.280f,
	37.317f, 38.190f, 38.846f, 39.988f, 40.098f, 40.768f, 41.784f, 42.443f, 42.580f, 43.299f,
	44.911f, 45.276f, 45.679f, 45.935f, 46.393f, 47.825f, 48.345f, 49.492f, 49.723f, 49.915f,
	50.109f, 50.629f, 51.285f, 51.536f, 52.749f, 52.903f, 53.364f, 54.450f, 54.539f, 54.998f,
	55.853f, 56.888f, 57.261f, 58.080f, 58.723f,

	 0.000f,  0.221f,  0.891f,  1.479f,  2.604f,  2.712f,  3.110f,  3.786f,  3.917f,  4.529f,
	 5.952f,  6.322f,  6.555f,  6.676f,  7.046f,  7.185f,  7.567f,  8.643f,  8.802f,  9.154f,
	 9.889f, 10.886f, 11.069f, 11.385f, 12.181f, 12.403f, 12.578f, 12.732f, 13.013f, 14.520f,
	14.680f, 15.121f, 15.278f, 16.030f, 16.396f, 16.897f, 18.072f, 18.506f, 18.687f, 19.255f,
	19.370f, 20.322f, 20.484f, 20.640f, 20.981f, 21.615f, 21.820f, 22.347f, 22.901f, 23.924f,
	24.430f, 24.810f, 25.187f, 26.132f, 26.958f, 27.109f, 28.126f, 28.286f, 28.851f, 29.557f,
	31.111f, 31.308f, 31.858f, 32.155f, 32.680f, 33.127f, 33.368f, 34.789f, 35.089f, 35.577f,
	35.664f, 36.126f, 36.316f, 36.537f, 36.897f, 37.138f, 38.507f, 38.776f, 39.316f, 39.511f,
	39.876f, 40.096f, 41.040f, 41.315f, 41.513f, 41.688f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
};

const float bookLesCZ[95 * 2] = {
	 0.000f,  0.200f,  0.459f,  0.959f,  1.210f,  1.627f,  2.628f,  2.836f,  3.045f,  3.837f,
	 3.962f,  4.254f,  4.493f,  4.796f,  4.880f,  5.422f,  6.673f,  7.299f,  7.466f,  8.133f,
	 8.800f,  9.927f, 10.719f, 11.303f, 11.637f, 12.512f, 13.388f, 13.680f, 13.972f, 14.306f,
	15.432f, 15.557f, 15.807f, 16.308f, 16.683f, 16.892f, 17.768f, 18.560f, 19.186f, 20.145f,
	20.437f, 20.854f, 21.146f, 21.980f, 22.189f, 22.856f, 23.732f, 24.399f, 25.108f, 25.400f,
	25.609f, 26.109f, 26.568f, 27.277f, 28.320f, 28.487f, 29.029f, 29.196f, 29.363f, 29.863f,
	30.405f, 30.614f, 32.074f, 32.616f, 32.950f, 33.825f, 34.117f, 34.618f, 35.285f, 36.536f,
	36.745f, 37.204f, 37.704f, 38.288f, 38.789f, 39.039f, 40.374f, 40.874f, 41.250f, 42.084f,
	43.126f, 43.460f, 44.586f, 45.045f, 45.545f, 45.879f, 46.588f, 46.880f, 47.130f, 47.547f,
	48.382f, 49.883f, 50.467f, 51.009f,  0.000f,

	 0.000f,  0.027f,  0.686f,  1.208f,  1.510f,  1.702f,  2.471f,  2.677f,  3.267f,  3.405f,
	 3.940f,  4.517f,  5.986f,  6.521f,  6.700f,  6.988f,  7.153f,  7.304f,  7.880f,  8.306f,
	 8.773f,  9.830f, 10.530f, 11.038f, 11.381f, 12.205f, 12.576f, 13.056f, 14.429f, 14.709f,
	15.121f, 15.725f, 16.000f, 16.892f, 17.249f, 17.977f, 19.212f, 19.487f, 19.665f, 19.748f,
	20.022f, 21.464f, 22.027f, 22.317f, 22.740f, 22.800f, 23.468f, 23.899f, 24.284f, 25.176f,
	25.862f, 26.027f, 26.727f, 26.906f, 27.441f, 27.565f, 28.045f, 28.210f, 29.185f, 29.473f,
	29.803f, 30.750f, 30.956f, 31.532f, 32.191f, 32.521f, 33.605f, 33.984f, 34.450f, 34.752f,
	34.945f, 35.315f, 35.453f, 35.617f, 35.837f, 36.647f, 37.004f, 37.594f, 37.883f, 38.075f,
	38.638f, 38.996f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
};

const float bookBuhta[95 * 2] = {
	 0.000f,  0.284f,  0.750f,  1.287f,  1.965f,  2.410f,  2.975f,  3.819f,  4.682f,  6.184f,
	 6.735f,  7.182f,  7.918f,  8.786f, 10.161f, 11.182f, 11.970f, 12.297f, 13.910f, 14.518f,
	15.989f, 16.419f, 17.332f, 18.960f, 19.471f, 20.762f, 21.473f, 22.749f, 23.452f, 24.547f,
	26.140f, 26.339f, 26.598f, 27.266f, 28.012f, 28.609f, 29.459f, 29.746f, 30.216f, 31.683f,
	32.613f, 34.180f, 34.987f, 35.505f, 36.508f, 37.457f, 39.046f, 39.387f, 40.471f, 40.896f,
	41.958f, 42.591f, 42.847f, 43.120f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,

	 0.000f,  0.811f,  1.190f,  1.754f,  2.475f,  3.367f,  5.026f,  5.580f,  6.202f,  6.455f,
	 7.050f,  8.709f,  9.114f,  9.551f, 10.241f, 11.304f, 11.936f, 13.122f, 13.897f, 15.227f,
	16.105f, 16.782f, 17.773f, 19.608f, 20.655f, 21.015f, 21.520f, 22.579f, 24.167f, 24.942f,
	25.727f, 27.149f, 28.267f, 28.736f, 29.372f, 30.030f, 30.841f, 31.751f, 32.170f, 32.743f,
	33.234f, 34.740f, 35.880f, 36.294f, 37.218f, 38.147f, 38.543f, 39.309f, 40.896f, 41.469f,
	42.046f, 42.461f, 42.944f, 43.079f, 44.093f, 45.324f, 46.062f, 47.153f, 47.798f, 48.970f,
	49.612f, 50.230f, 51.343f, 51.780f, 52.083f, 52.326f, 52.728f, 53.143f, 54.139f, 55.112f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
};

const float bookBuhtaCZ[95 * 2] = {
	 0.000f,  0.133f,  0.434f,  0.767f,  1.134f,  1.802f,  2.169f,  2.369f,  2.636f,  2.736f,
	 3.036f,  3.770f,  4.004f,  4.605f,  4.771f,  5.305f,  5.706f,  6.106f,  7.808f,  8.141f,
	 8.275f,  8.575f,  8.976f, 10.077f, 10.177f, 10.477f, 10.878f, 11.278f, 11.745f, 12.746f,
	13.046f, 13.180f, 13.714f, 13.947f, 15.282f, 16.350f, 17.084f, 17.284f, 18.151f, 18.952f,
	19.653f, 19.920f, 20.320f, 21.622f, 22.022f, 22.089f, 22.522f, 22.956f, 23.790f, 23.891f,
	24.725f, 24.791f, 25.459f, 25.626f, 26.760f, 27.127f, 27.761f, 28.495f, 28.662f, 29.129f,
	29.296f, 30.163f, 30.464f, 31.265f, 32.399f, 33.133f, 33.367f, 33.934f, 34.134f, 34.334f,
	34.601f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,

	 0.000f,  0.200f,  0.667f,  1.134f,  1.635f,  2.402f,  2.469f,  2.803f,  3.136f,  3.303f,
	 3.704f,  4.171f,  4.204f,  4.671f,  4.838f,  5.205f,  6.240f,  6.373f,  6.507f,  6.607f,
	 7.207f,  8.242f,  8.442f,  8.709f,  9.042f,  9.509f, 10.310f, 11.111f, 11.311f, 11.778f,
	12.112f, 12.246f, 13.113f, 14.781f, 15.315f, 15.582f, 16.250f, 16.717f, 16.884f, 17.384f,
	18.519f, 18.752f, 19.419f, 19.553f, 20.087f, 20.520f, 21.088f, 21.722f, 22.623f, 23.924f,
	24.591f, 24.992f, 25.525f, 25.859f, 25.993f, 27.227f, 27.794f, 28.262f, 28.862f, 29.196f,
	29.897f, 29.997f, 30.731f, 32.699f, 33.200f, 33.934f, 34.701f, 35.068f, 35.235f, 35.903f,
	36.370f, 36.770f, 37.971f, 38.338f, 38.672f, 39.006f, 39.539f, 39.673f, 40.707f, 40.974f,
	41.742f, 42.442f, 43.176f, 43.343f, 43.877f, 44.544f,  0.000f,  0.000f,  0.000f,  0.000f,
	 0.000f,  0.000f,  0.000f,  0.000f,  0.000f,
};

class qdBookAllMiniGame : public qdMiniGameInterface {
public:
	qdBookAllMiniGame(Common::String dll, Common::Language language) : _dll(dll), _language(language) {}
	~qdBookAllMiniGame() { };

	//! Инициализация игры.
	bool init(const qdEngineInterface *engine_interface) {
		debugC(1, kDebugMinigames, "BookAll::init()");

		_engine = engine_interface;
		_scene = engine_interface->current_scene_interface();
		if (!_scene)
			return 0;

		_artObject = _scene->object_interface("art");
		_startReading = _scene->object_interface("$start_reading");
		_recordPlayer = _scene->object_interface("\xe3\xf0\xe0\xec\xee\xf4\xee\xed"); // "грамофон"

		_pageDurations[0] = 0.0;
		_totalPageArts[0] = 0;

		if (_dll == "DLL\\Book_gusenica.dll") {
			if (_language == Common::CS_CZE) {
				_artTimeStamps = bookGusenicaCZ;
				_pageDurations[1] = 40.607f;
				_pageDurations[2] = 41.854f;

				_totalPageArts[1] = 82;
				_totalPageArts[2] = 77;
			} else {
				_artTimeStamps = bookGusenica;
				_pageDurations[1] = 54.300f;
				_pageDurations[2] = 50.400f;

				_totalPageArts[1] = 82;
				_totalPageArts[2] = 69;
			}
		} else if (_dll == "DLL\\Book_les.dll") {
			if (_language == Common::CS_CZE) {
				_artTimeStamps = bookLesCZ;
				_pageDurations[1] = 51.84f;
				_pageDurations[2] = 39.832f;

				_totalPageArts[1] = 93;
				_totalPageArts[2] = 81;
			} else {
				_artTimeStamps = bookLes;
				_pageDurations[1] = 59.809f;
				_pageDurations[2] = 42.30f;

				_totalPageArts[1] = 94;
				_totalPageArts[2] = 85;
			}
		} else if (_dll == "DLL\\Book_buhta.dll") {
			if (_language == Common::CS_CZE) {
				_artTimeStamps = bookBuhtaCZ;
				_pageDurations[1] = 35.424f;
				_pageDurations[2] = 45.128f;

				_totalPageArts[1] = 70;
				_totalPageArts[2] = 85;
			} else {
				_artTimeStamps = bookBuhta;
				_pageDurations[1] = 43.913f;
				_pageDurations[2] = 55.698f;

				_totalPageArts[1] = 53;
				_totalPageArts[2] = 69;
			}
		}

#if 0
		const int pageSize = 86;
		float artTimeStamps[pageSize * 3];
		memset(artTimeStamps, 0, sizeof(artTimeStamps));

		for (int i = 0; i < pageSize; i++) {
			debugN("%2.3f, ", artTimeStamps[pageSize + i]);

			if ((i + 1) % 10 == 0)
				debug("");
		}
		debug("");

		for (int i = 0; i < pageSize; i++) {
			debugN("%2.3f, ", artTimeStamps[pageSize * 2 + i]);

			if ((i + 1) % 10 == 0)
				debug("");
		}
		debug("");
#endif

		_pageNum = 0;
		_playbackOn = 0;

		return true;
	}

	//! Обсчёт логики игры, параметр - время, которое должно пройти в игре (в секундах).
	bool quant(float dt) {
		debugC(3, kDebugMinigames, "BookAll::quant(%f). _playbackOn: %d _playbackOn: %d _currentPageArt: %d _totalPageArts: %d _time: %f",
				dt, _playbackOn, _playbackOn, _currentPageArt, _totalPageArts[_pageNum], _time);

		if (!_playbackOn) {
			if (_startReading->is_state_active("page1")) {
				debugC(1, kDebugMinigames, "BookAll::quant(). Resetting to PAGE1");
				_pageNum = 1;
				_startReading->set_state("reading_page1");
				_currentPageArt = 1;
				_time = 0.0;
				_playbackOn = true;
			} else if (_startReading->is_state_active("page2")) {
				debugC(1, kDebugMinigames, "BookAll::quant(). Resetting to PAGE2");
				_pageNum = 2;
				_startReading->set_state("reading_page2");
				_currentPageArt = 1;
				_time = 0.0;
				_playbackOn = true;
			}
		}

		if (_playbackOn && _recordPlayer->is_state_active("\xf1\xf2\xe0\xf2\xe8\xea")) { // "статик"
			debugC(1, kDebugMinigames, "BookAll::quant(). Stopping playback externally");
			_playbackOn = false;
			_startReading->set_state("no");
		}

		if (_playbackOn) {
			if (_currentPageArt > _totalPageArts[_pageNum]) {
				_time = _time + dt;
				if (_pageDurations[_pageNum] < (double)_time) {
					debugC(1, kDebugMinigames, "BookAll::quant(). Stopping playback by time");
					_startReading->set_state("stopping");
				}
			} else {
				_time = _time + dt;
				if (_artTimeStamps[95 * (_pageNum - 1) + _currentPageArt] <= (double)_time) {
					_artObject->set_state(Common::String::format("page%i_art_%02i", _pageNum, _currentPageArt).c_str());
					++_currentPageArt;

					debugC(1, kDebugMinigames, "BookAll::quant(). Switching pageArt to %d", _currentPageArt);
				}
			}
		}

		return true;
	}

	//! Деинициализация игры.
	bool finit() {
		debugC(1, kDebugMinigames, "BookAll::finit()");

		if (_scene)  {
			_engine->release_scene_interface(_scene);
			_scene = 0;
		}

 		return true;
	}

	/// Инициализация миниигры, вызывается при старте и перезапуске игры.
	bool new_game(const qdEngineInterface *engine_interface) {
		return true;
	}
	/// Сохранение данных, вызывается при сохранении сцены, на которую повешена миниигра.
	int save_game(const qdEngineInterface *engine_interface, const qdMinigameSceneInterface *scene_interface, char *buffer, int buffer_size) {
		return 0;
	}
	/// Загрузка данных, вызывается при загрузке сцены, на которую повешена миниигра.
	int load_game(const qdEngineInterface *engine_interface, const qdMinigameSceneInterface *scene_interface, const char *buffer, int buffer_size) {
		return 0;
	}

	/// Версия интерфейса игры, трогать не надо.
	enum { INTERFACE_VERSION = 112 };
	int version() const {
		return INTERFACE_VERSION;
	}

private:
	const float *_artTimeStamps = nullptr;
	float _pageDurations[3] = { 0.0, 0.0, 0.0 };

	const qdEngineInterface *_engine = nullptr;
	qdMinigameSceneInterface *_scene = nullptr;

	qdMinigameObjectInterface *_artObject = nullptr;
	qdMinigameObjectInterface *_startReading = nullptr;
	qdMinigameObjectInterface *_recordPlayer = nullptr;

	int _pageNum = 0;
	bool _playbackOn = false;
	int _currentPageArt = 1;
	int _totalPageArts[3] = { 0, 0, 0 };
	float _time = 0.0f;

	Common::String _dll;
	Common::Language _language;
};

} // namespace QDEngine

#endif // QDENGINE_MINIGAMES_BOOK_LES_H
