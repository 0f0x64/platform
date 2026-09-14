float distance(const float4& p1, const float4& p2) {
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	float dz = p2.z - p1.z;

	return sqrt(dx * dx + dy * dy + dz * dz);
}

float4 lerp3(const float4& a, const float4& b, float t) {
	return float4{
		a.x + t * (b.x - a.x),
		a.y + t * (b.y - a.y),
		a.z + t * (b.z - a.z),
		a.w // Сохраняем оригинальное значение w из первой точки
	};
}

float frac(float x) {
	return x - floor(x);
}

float4 normalize(const float4& v) {
	// Считаем длину вектора по формуле Пифагора
	float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

	// Защита от деления на ноль (если вектор нулевой)
	if (length < 0.00001f) {
		return float4{ 0.0f, 0.0f, 0.0f, v.w };
	}

	// Возвращаем нормализованный вектор
	return float4{
		v.x / length,
		v.y / length,
		v.z / length,
		v.w // Поле w оставляем оригинальным
	};
}

float4 cross(const float4& a, const float4& b) {
	return float4{
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x,
		0.0f // Для векторов направления w обычно равен 0
	};
}

namespace Object {

	cmd(Show,
		texture geometry,
		texture normals,
		int8u quality,
		int pos_x,
		int pos_y,
		int pos_z,
		int glow
	)
	{
		reflect;

#if EditMode //dynamic limits
		auto r = max(Textures::Texture[(int)in.geometry].size.x, Textures::Texture[(int)in.geometry].size.y);
		auto mipMaps = Textures::Texture[(int)in.geometry].mipMaps;
		cmdParamDesc[cmdCounter - 1].param[2]._min = 0;
		cmdParamDesc[cmdCounter - 1].param[2]._max = max((mipMaps ? (UINT)(_log2(r)) : 0) - 2, 0);
#endif

		int denom = (int)pow(2, (float)in.quality);
		float q = intToFloatDenom;

		int gX = Textures::Texture[(int)in.geometry].size.x / denom;
		int gY = Textures::Texture[(int)in.geometry].size.y / denom;

		vs::objViewer = {

			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(in.pos_x / q, in.pos_y / q, in.pos_z / q)),
				.gX = gX,
				.gY = gY,
				.glow_p = (float)in.glow,
			},

			.textures = {
				.positions = in.geometry,
				.normals = in.normals
				},

			.samplers = {
				.sam1Filter = filter::linear,
				.sam1AddressU = addr::wrap,
				.sam1AddressV = addr::clamp
			}
		};

		vs::objViewer.set();

		ps::basic =
		{
			.params = {
				#if EditMode
					.hilight = cmdCounter - 1 == hilightedCmd ? 1.f : 0.f
				#else 
					.hilight = 0.f
				#endif
				}

		};

		ps::basicLow =
		{
			.params = {
				#if EditMode
					.hilight = cmdCounter - 1 == hilightedCmd ? 1.f : 0.f
				#else 
					.hilight = 0.f
				#endif
				}

		};

		if (in.glow == 1)
		{
			ps::basic.set();
		}
		else
		{
			ps::basicLow.set();
		}


		//Drawer::NullDrawer({(int)gX*(int)gY,1});
		if (in.glow == 0)
		{
			Drawer::NullDrawer({ 1, (int)gX * (int)gY / 10394 });
		}
		else
		{
			Drawer::NullDrawer({ 1, (int)gX * (int)gY });
		}


	}



	enum class pMode { point, glow };
	enum class triMode { on, off };

	void psModeSet(pMode mode)
	{
		switch (mode)
		{
		case pMode::point:
		{
			ps::basic = { .params = {.hilight = 0.f } };
			ps::basic.set();
			break;
		}
		case pMode::glow:
		{
			ps::basicLow = { .params = {.hilight = 0.f } };
			ps::basicLow.set();
			break;
		}
		}
	}

	void psModeSet2(pMode mode)
	{
		switch (mode)
		{
		case pMode::point:
		{
			ps::basic2 = { .params = {.hilight = 0.f } };
			ps::basic2.set();
			break;
		}
		case pMode::glow:
		{
			ps::basicLow2 = { .params = {.hilight = 0.f } };
			ps::basicLow2.set();
			break;
		}
		}
	}

	void PillarsHand(int count, int skipper, pMode mode)
	{
		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);

		vs::pillarsHand = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
			},
		};

		vs::pillarsHand.set();

		Drawer::NullDrawer({ 1, (int)gX * (int)gY });
	}

	cmd(InsideNebula, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::insideNebula = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::insideNebula.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });


	}

	cmd(Blob, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::blob = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::blob.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });


	}

	cmd(Pearl, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::pearl = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::pearl.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });


	}

	cmd(LeoStar, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::leo = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::leo.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });


	}

	cmd(CapStar, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::capriStar = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::capriStar.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });


	}



	cmd(Tau, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::Tau = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1),
				//.mesh = {
					//#include "girl.h"
					//#include "girl_rand.h"
					//#include "girl_mini.h"
				//}
			},
		};

		vs::Tau.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });


	}

#if EditMode

	dx11::ConstBuf::sbObject HeroMesh;
	dx11::ConstBuf::sbObject BossMesh;
	dx11::ConstBuf::sbObject* MeshPtr = NULL;
	XMMATRIX heroOnRails;
	XMMATRIX heroWorld;

	void ShowMesh(dx11::ConstBuf::sbObject* obj, int count, int skipper, pMode mode, int r, int g, int b, triMode tMode, int xPos, int yPos, int zPos, int brightness, int tickness, int zoom, int onLineOfs, int jumpCharge)
	{

		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);
		float zm = zoom / 100. + 1;

		vs::girl = {
			.params =
			{
				.model = heroWorld,
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
				.base_color = float4(r / 100.,g / 100.,b / 100.,1),
				.modelPos = float4(xPos / 10000.,yPos / 10000.,zPos / 10000.,0),
				.triCount = float4(obj->triangleCount,0,0,0),
				.brightness = float4(brightness,0,0,0),
				.tickness = float4(tickness,0,0,0),
				.zoom = float4(zm,zm,zm,1),
				.onLineOfs = (float)onLineOfs / 1000.f,
				.jumpCharge = (float)jumpCharge / 100.f,
			},
		};

		if (tMode == triMode::on)
		{
			vs::girl.params.mode = 2;
		}

		vs::girl.set();

		obj->BindSB(0);
		obj->BindSB(1);


		if (tMode == triMode::on)
		{
			//dx11::Shaders::resetShader(dx11::Shaders::basic);
			//dx11::Shaders::resetShader(dx11::Shaders::basic);
			Drawer::NullDrawerTri({ count, 1 });
		}
		else
		{
			Drawer::NullDrawer({ 1,(int)gX * (int)gY });
		}


	}

	cmd(Mesh, int quality, int xPos, int yPos, int zPos, int brightness, int tickness, switcher stencil, int zoom, int onLineOfs, int jumpCharge)
	{
		reflect;

		int count = 500000;

		DepthBuf::Mode({ depthmode::on });
		BlendMode::Set({
			.mode = blendmode::off,
			.op = blendop::add
			});

		Culling::Set({ cullmode::off });
		if (in.stencil == switcher::on)
		{
			ShowMesh(MeshPtr, (int)MeshPtr->triangleCount, 1, pMode::point, 0, 0, 0, triMode::on, in.xPos, in.yPos, in.zPos, in.brightness, in.tickness, in.zoom, in.onLineOfs, in.jumpCharge);
		}

		Culling::Set({ cullmode::off });
		DepthBuf::Mode({ depthmode::readonly });
		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});

		ShowMesh(MeshPtr, count, 1, pMode::point, 100, 252, 1400, triMode::off, in.xPos, in.yPos, in.zPos, in.brightness, in.tickness, in.zoom, in.onLineOfs, in.jumpCharge);
	}

#endif

	cmd(ScorpBall, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::scorpBall = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::scorpBall.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });


	}

	cmd(Nebula2, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::Nebula2 = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::Nebula2.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });


	}

	cmd(vrg, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		XMMATRIX invViewMatrix = XMMatrixInverse(nullptr, XMMatrixTranspose(ConstBuf::camera.view[0]));
		// Позиция камеры находится в 4-й строке инвертированной матрицы (вектор смещения)
		XMVECTOR cameraPos = invViewMatrix.r[3];
		// Если нужно получить отдельные float:
		XMFLOAT3 eye;
		XMStoreFloat3(&eye, cameraPos);

		XMVECTOR cameraLookAtVec = XMVector3Normalize(invViewMatrix.r[2]);

		// Сохраняем в структуру XMFLOAT3 для передачи в Shader Constants / Constant Buffer
		XMFLOAT3 cameraForward;
		XMStoreFloat3(&cameraForward, cameraLookAtVec);

		// 1. Извлекаем и нормализуем вектор Right (1-я строка инвертированной матрицы)
		XMVECTOR cameraRightVec = XMVector3Normalize(invViewMatrix.r[0]);

		// 2. Извлекаем и нормализуем вектор Up (2-я строка инвертированной матрицы)
		XMVECTOR cameraUpVec = XMVector3Normalize(invViewMatrix.r[1]);

		// Сохраняем в структуры XMFLOAT3 для передачи в ваш Constant Buffer
		XMFLOAT3 cameraRight;
		XMStoreFloat3(&cameraRight, cameraRightVec);

		XMFLOAT3 cameraUp;
		XMStoreFloat3(&cameraUp, cameraUpVec);

		vs::Virgo = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1),
				.eye = float4(eye.x,eye.y,eye.z,0),
				.forward = float4(cameraForward.x,cameraForward.y,cameraForward.z,0),
				.up = float4(cameraUp.x,cameraUp.y,cameraUp.z,0),
				.right = float4(cameraRight.x,cameraRight.y,cameraRight.z,0),
			},
		};

		vs::Virgo.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });


	}

	const int smoothPointMAX = 3500;

	struct starline {
		float4 baseColor;
		float4 basePoint[4000];
		float4 point[smoothPointMAX];
		float4 upVector[smoothPointMAX];
		int basePointCount = 0;
		int pointCount = 0;
	};

	struct {
		starline line[2000];
		int lineCount = 0;
	} starLineList;

	int currentLine = -1;
	int currentPoint = 0;

	cmd(SetLineCount, int lineCount)
	{
		reflect;
		starLineList.lineCount = in.lineCount;
	}

	const int denom = 100;

	cmd(SetPointPosInLine, int line, int point, int x, int y, int z, int a)
	{
		reflect;
		starLineList.line[in.line].basePoint[in.point] = float4(in.x / (float)denom, in.y / (float)denom, in.z / (float)denom, in.a);
	}

	cmd(AddPointToLine, int x, int y, int z, int a = 0)
	{
		reflect;
		starLineList.line[currentLine].basePoint[currentPoint++] = float4(in.x / (float)denom, in.y / (float)denom, in.z / (float)denom, in.a);
		starLineList.line[currentLine].basePointCount = currentPoint;
	}

	void AddPoint(float4 p)
	{
		starLineList.line[currentLine].basePoint[currentPoint++] = p;
		starLineList.line[currentLine].basePointCount = currentPoint;
	}





	void smoothStarline(starline& line) {
		line.pointCount = 0; // Сбрасываем старый результат сглаживания

		float totalLength = 0;
		for (int i = 0; i < line.basePointCount - 1; i++)
		{
			totalLength += distance(line.basePoint[i], line.basePoint[i + 1]);
		}

		int stepsPerSegment = totalLength / 50.;
		//if (stepsPerSegment < 2) stepsPerSegment = 2;

		// Если исходных точек недостаточно для сглаживания или шаг некорректен
		if (line.basePointCount < 2 || stepsPerSegment <= 0) {
			// Просто копируем исходные точки в результирующий массив
			int limit = (line.basePointCount > smoothPointMAX) ? smoothPointMAX : line.basePointCount;
			for (int i = 0; i < limit; ++i) {
				line.point[i] = line.basePoint[i];
			}
			line.pointCount = limit;
			return;
		}

		// Проходим по сегментам между исходными точками basePoint
		for (int i = 0; i < line.basePointCount - 1; ++i) {
			// Формируем 4 опорные точки для Кэтмулла-Рома (с виртуальным продлением на краях)
			float4 p0 = (i == 0) ? line.basePoint[i] : line.basePoint[i - 1];
			float4 p1 = line.basePoint[i];
			float4 p2 = line.basePoint[i + 1];
			float4 p3 = (i == line.basePointCount - 2) ? line.basePoint[i + 1] : line.basePoint[i + 2];

			// Генерируем промежуточные точки внутри текущего сегмента
			for (int step = 0; step < stepsPerSegment; ++step) {
				// Защита от переполнения жестко ограниченного массива point[100]
				if (line.pointCount >= smoothPointMAX) {
					return;
				}

				float t = (float)step / (float)stepsPerSegment;
				line.point[line.pointCount] = catmullRom(p0, p1, p2, p3, t);
				line.pointCount++;
			}
		}

		// Добавляем финальную опорную точку, чтобы линия завершилась корректно
		if (line.pointCount < smoothPointMAX) {
			line.point[line.pointCount] = line.basePoint[line.basePointCount - 1];
			line.pointCount++;
		}
	}



	void Starline(starline& line, int stepsPerSegment) {
		line.pointCount = 0; // Сбрасываем старый результат сглаживания

		// Проходим по сегментам между исходными точками basePoint
		for (int i = 0; i < line.basePointCount - 1; ++i) {

			// Генерируем промежуточные точки внутри текущего сегмента
			for (int step = 0; step < stepsPerSegment; ++step) {
				// Защита от переполнения жестко ограниченного массива point[100]
				if (line.pointCount >= smoothPointMAX) {
					return;
				}

				float t = (float)step / (float)stepsPerSegment;
				line.point[line.pointCount] = lerp3(line.basePoint[i], line.basePoint[i + 1], t);
				line.pointCount++;
			}
		}

		// Добавляем финальную опорную точку, чтобы линия завершилась корректно
		if (line.pointCount < smoothPointMAX) {
			line.point[line.pointCount] = line.basePoint[line.basePointCount - 1];
			line.pointCount++;
		}
	}


	// Функция плавной интерполяции (Smoothstep / Fade)
	inline float perlin_fade(float t) {
		return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
	}

	// Линейная интерполяция
	inline float perlin_lerp(float t, float a, float b) {
		return a + t * (b - a);
	}

	// Вычисление скалярного произведения с градиентным вектором
	inline float perlin_grad(int hash, float x, float y, float z) {
		int h = hash & 15;
		float u = h < 8 ? x : y;
		float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}

	// Вспомогательная функция для получения одного скалярного значения шума
	float GetSinglePerlinNoise3D(float x, float y, float z) {
		// Таблица перестановок Перлина (повторена дважды, чтобы избежать выхода за границы при +1)
		static const int p[512] = {
			151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
			190,6,148,247,120,234,75,0,26,56,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,
			125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,
			105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,
			135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,
			82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,
			153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
			251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,
			157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,
			66,215,61,156,180,
			// Повторение массива
			151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
			190,6,148,247,120,234,75,0,26,56,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,
			125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,
			105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,
			135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,
			82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,
			153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
			251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,
			157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,
			66,215,61,156,180
		};

		int X = static_cast<int>(std::floor(x)) & 255;
		int Y = static_cast<int>(std::floor(y)) & 255;
		int Z = static_cast<int>(std::floor(z)) & 255;

		x -= std::floor(x);
		y -= std::floor(y);
		z -= std::floor(z);

		float u = perlin_fade(x);
		float v = perlin_fade(y);
		float w = perlin_fade(z);

		int A = p[X] + Y;
		int AA = p[A] + Z;
		int AB = p[A + 1] + Z;
		int B = p[X + 1] + Y;
		int BA = p[B] + Z;
		int BB = p[B + 1] + Z;

		return perlin_lerp(w, perlin_lerp(v, perlin_lerp(u, perlin_grad(p[AA], x, y, z),
			perlin_grad(p[BA], x - 1, y, z)),
			perlin_lerp(u, perlin_grad(p[AB], x, y - 1, z),
				perlin_grad(p[BB], x - 1, y - 1, z))),
			perlin_lerp(v, perlin_lerp(u, perlin_grad(p[AA + 1], x, y, z - 1),
				perlin_grad(p[BA + 1], x - 1, y, z - 1)),
				perlin_lerp(u, perlin_grad(p[AB + 1], x, y - 1, z - 1),
					perlin_grad(p[BB + 1], x - 1, y - 1, z - 1))));
	}

	// Целевая функция, принимающая три аргумента и возвращающая XMVECTOR
	XMVECTOR GetPerlinNoiseVector3(float x, float y, float z) {
		// Смещаем координаты для каждого канала, чтобы значения X, Y и Z не дублировали друг друга
		float nx = GetSinglePerlinNoise3D(x, y, z);
		float ny = GetSinglePerlinNoise3D(x + 31.415f, y + 58.271f, z + 93.123f);
		float nz = GetSinglePerlinNoise3D(x + 115.53f, y + 213.91f, z + 351.67f);

		// Возвращаем упакованный в SIMD-регистр вектор (компонента W = 0.0f)
		return XMVectorSet(nx, ny, nz, 0.0f);
	}

	void NewLine()
	{
		currentLine++;
		currentPoint = 0;
		starLineList.lineCount = currentLine + 1;
	}

	cmd(NewStar, int x, int y, int z, int rad, int r, int g, int b, int brightness)
	{
		reflect;

		currentLine++;
		currentPoint = 0;
		starLineList.lineCount = currentLine + 1;

		starLineList.line[currentLine].baseColor = float4(in.r / 100.f, in.g / 100.f, in.b / 100.f, in.brightness / 100.f);
		starLineList.line[currentLine].basePoint[0] = float4(in.x / (float)denom, in.y / (float)denom, in.z / (float)denom, in.rad / (float)denom);
		starLineList.line[currentLine].basePointCount = 1;

		starLineList.line[currentLine].point[0] = starLineList.line[currentLine].basePoint[0];
		starLineList.line[currentLine].pointCount = 1;
	}


	float4 gemini[] = {
		// Pollux Line (Right Branch)
		{  0.95f,  0.72f,  0.0f,  0.0f }, // Pollux
		{  0.58f,  0.41f,  0.0f,  1.0f }, // Wasat
		{  0.21f,  0.12f,  0.0f,  2.0f }, // Mebsuta
		{ -0.25f, -0.28f,  0.0f,  3.0f }, // Mekbuda
		{ -0.68f, -0.65f,  0.0f,  4.0f }, // Alhena
		{ -0.92f, -0.85f,  0.0f,  5.0f }, // Alzirr

		// Castor Line (Left Branch)
		{  0.88f,  0.91f,  0.0f,  6.0f }, // Castor
		{  0.45f,  0.62f,  0.0f,  7.0f }, // Kappa Gem
		{  0.12f,  0.35f,  0.0f,  8.0f }, // Upsilon Gem
		{ -0.18f,  0.08f,  0.0f,  9.0f }, // Propus
		{ -0.52f, -0.22f,  0.0f, 10.0f }, // Tejat Posterior
		{ -0.75f, -0.45f,  0.0f, 11.0f }, // Tejat Prior

		// Connecting stars
		{  0.32f,  0.18f,  0.0f, 12.0f }, // Lambda Gem
		{  0.05f,  0.55f,  0.0f, 13.0f }, // Tau Gem
		{ -0.15f,  0.42f,  0.0f, 14.0f }, // Theta Gem
		{ -0.42f,  0.22f,  0.0f, 15.0f }, // Nu Gem
		{ -0.85f, -0.12f,  0.0f, 16.0f }  // 1 Gem
	};

	void genSegment(float4 start, float4 end)
	{
		NewLine();
		int seg = distance(start, end) * 20.;
		if (seg == 0) seg += 2;
		for (int k = 0; k <= seg; k++)
		{
			float4 p = lerp3(start, end, k / (float)seg);

			float rs = .02 * sin((k / (float)seg) * PI);
			p.x += getRandFloat() * rs;
			p.y += getRandFloat() * rs;
			p.z += getRandFloat() * rs;

			// Масштабируем координаты точек для игрового мира
			float scale = 600;
			p.x *= scale;
			p.y *= scale;
			p.z *= scale;

			AddPoint(p);
		}
	}

	float2 GetJitteredSphereAngle(int j, int raysCount, float jitterAmount) {
		// Золотое сечение для идеального распределения по спирали
		const float phi = (1.0f + std::sqrt(5.0f)) / 2.0f;
		const float angleIncrement = XM_2PI / phi;

		// 1. Базовая регулярная точка на сфере (распределение Фибоначчи)
		// Предотвращаем деление на ноль, если raysCount == 1
		float t = (raysCount > 1) ? (float)j / (raysCount - 1) : 0.5f;
		float baseZ = 1.0f - t * 2.0f;
		float basePitch = std::acos(baseZ);
		float baseYaw = j * angleIncrement;

		// 2. Генерация случайного смещения (джиттеринга) вокруг базовой точки
		float randDir = ((float)rand() / RAND_MAX) * XM_2PI; // Случайное направление сдвига
		float randRadius = ((float)rand() / RAND_MAX) * jitterAmount; // Случайный радиус сдвига

		// 3. Смещение координат
		float finalYaw = baseYaw + randRadius * std::cos(randDir);
		float finalPitch = basePitch + randRadius * std::sin(randDir);

		// Коррекция углов, чтобы они не выходили за математические границы
		finalPitch = std::fmax(0.0f, std::fmin(XM_PI, finalPitch));
		finalYaw = std::fmod(finalYaw, XM_2PI);
		if (finalYaw < 0.0f) finalYaw += XM_2PI;

		return { finalYaw, finalPitch };
	}

	void initPatches(float pathTime)
	{
		// init maze
		currentLine = -1;
		// 
		//-----------------------------------------
		//-----------start user space--------------

		int starsCount = sizeof(gemini) / sizeof(float4);

		srand(100);

		/*for (int i = 0; i < starsCount; i++)
		{
			gemini[i].z = getRandFloat();
		}

		genSegment(gemini[0], gemini[1]);
		genSegment(gemini[1], gemini[2]);
		genSegment(gemini[2], gemini[3]);
		genSegment(gemini[3], gemini[4]);
		genSegment(gemini[4], gemini[5]);
		genSegment(gemini[6], gemini[7]);
		genSegment(gemini[7], gemini[8]);
		genSegment(gemini[8], gemini[8]);
		genSegment(gemini[9], gemini[10]);
		genSegment(gemini[10], gemini[11]);
		genSegment(gemini[0], gemini[6]);
		genSegment(gemini[7], gemini[12]);
		genSegment(gemini[12], gemini[13]);
		genSegment(gemini[13], gemini[14]);
		genSegment(gemini[14], gemini[15]);
		genSegment(gemini[10], gemini[16]);
		*/


		NewLine();
		AddPointToLine({ 0,-800,-1600 });
		AddPointToLine({ 3500,-11100,0 });
		AddPointToLine({ 46400,0,0 });
		AddPointToLine({ 20000,7900,0 });
		AddPointToLine({ 10300,11000,0 });
		AddPointToLine({ 0,600,0 });



		// ============================================================
// ДРЕВО В СТИЛЕ ELDEN RING
//
// Y = ВЫСОТА
//
// Ствол = деформированная труба.
// Линии НЕ проходят через центр ствола.
// Они расположены НА НАРУЖНОЙ ПОВЕРХНОСТИ.
//
// Радиус основной трубы примерно 4500.
// Центральная ось немного изгибается по X/Z.
// ============================================================


// ============================================================
// ЦЕНТРАЛЬНАЯ ОСЬ — НЕ РИСУЕМ
//
// Примерная ось:
//
// Y 30000 -> X 55000 Z 50000
// Y 45000 -> X 50000 Z 52000
// Y 60000 -> X 54000 Z 55000
// Y 75000 -> X 50000 Z 58000
// Y 90000 -> X 56000 Z 54000
//
// Вокруг этой оси располагаются наружные жилы.
// ============================================================


// ============================================================
// КРИВОЙ СТВОЛ — СЕГМЕНТИРОВАННАЯ ТРУБА
//
// Y — направление подъёма
//
// Ствол изгибается:
// низ     -> X ~ 55000
// середина -> X уходит вправо
// верх     -> снова возвращается влево
//
// Каждая линия находится на поверхности трубы.
// Радиус трубы примерно 4300.
//
// Каждый отдельный кусок:
// - минимум 5 точек
// - длина разная
// - между кусками ~300 координат
// ============================================================


// ============================================================
// ДЕРЕВО — РОВНАЯ НАРУЖНАЯ ПОВЕРХНОСТЬ
// 36 продольных жил, шаг 10°
// Все линии находятся на одном радиусе 4300 от центральной оси.
// Y = высота. Линии распределены равномерно по окружности.
// ============================================================

// ============================================================
// ПОВЕРХНОСТЬ 0°
// ============================================================

		NewLine();
		AddPointToLine({ 59300, 30000, 51543 });
		AddPointToLine({ 59704, 31250, 51740 });
		AddPointToLine({ 60106, 32500, 51934 });
		AddPointToLine({ 60505, 33750, 52123 });
		AddPointToLine({ 60900, 35000, 52308 });

		NewLine();
		AddPointToLine({ 60994, 35300, 52351 });
		AddPointToLine({ 61408, 36640, 52542 });
		AddPointToLine({ 61813, 37980, 52726 });
		AddPointToLine({ 62207, 39320, 52904 });
		AddPointToLine({ 62587, 40660, 53074 });
		AddPointToLine({ 62954, 42000, 53236 });

		NewLine();
		AddPointToLine({ 63033, 42300, 53271 });
		AddPointToLine({ 63369, 43600, 53418 });
		AddPointToLine({ 63688, 44900, 53558 });
		AddPointToLine({ 63989, 46200, 53688 });
		AddPointToLine({ 64270, 47500, 53810 });

		NewLine();
		AddPointToLine({ 64332, 47800, 53837 });
		AddPointToLine({ 64595, 49140, 53951 });
		AddPointToLine({ 64834, 50480, 54055 });
		AddPointToLine({ 65049, 51820, 54148 });
		AddPointToLine({ 65238, 53160, 54231 });
		AddPointToLine({ 65401, 54500, 54303 });

		NewLine();
		AddPointToLine({ 65434, 54800, 54317 });
		AddPointToLine({ 65555, 56040, 54372 });
		AddPointToLine({ 65652, 57280, 54417 });
		AddPointToLine({ 65725, 58520, 54452 });
		AddPointToLine({ 65773, 59760, 54478 });
		AddPointToLine({ 65797, 61000, 54494 });

		NewLine();
		AddPointToLine({ 65799, 61300, 54496 });
		AddPointToLine({ 65791, 62640, 54500 });
		AddPointToLine({ 65754, 63980, 54492 });
		AddPointToLine({ 65688, 65320, 54472 });
		AddPointToLine({ 65593, 66660, 54442 });
		AddPointToLine({ 65471, 68000, 54399 });

		NewLine();
		AddPointToLine({ 65440, 68300, 54388 });
		AddPointToLine({ 65297, 69540, 54337 });
		AddPointToLine({ 65131, 70780, 54276 });
		AddPointToLine({ 64943, 72020, 54206 });
		AddPointToLine({ 64733, 73260, 54127 });
		AddPointToLine({ 64503, 74500, 54039 });

		NewLine();
		AddPointToLine({ 64444, 74800, 54016 });
		AddPointToLine({ 64168, 76140, 53908 });
		AddPointToLine({ 63871, 77480, 53791 });
		AddPointToLine({ 63553, 78820, 53663 });
		AddPointToLine({ 63216, 80160, 53527 });
		AddPointToLine({ 62862, 81500, 53381 });

		NewLine();
		AddPointToLine({ 62780, 81800, 53347 });
		AddPointToLine({ 62321, 83440, 53155 });
		AddPointToLine({ 61842, 85080, 52951 });
		AddPointToLine({ 61346, 86720, 52735 });
		AddPointToLine({ 60837, 88360, 52509 });
		AddPointToLine({ 60317, 90000, 52274 });


		// ============================================================
		// ПОВЕРХНОСТЬ 10°
		// ============================================================

		NewLine();
		AddPointToLine({ 59235, 30000, 52290 });
		AddPointToLine({ 59639, 31250, 52487 });
		AddPointToLine({ 60041, 32500, 52681 });
		AddPointToLine({ 60440, 33750, 52870 });
		AddPointToLine({ 60835, 35000, 53055 });

		NewLine();
		AddPointToLine({ 60929, 35300, 53098 });
		AddPointToLine({ 61343, 36640, 53289 });
		AddPointToLine({ 61748, 37980, 53473 });
		AddPointToLine({ 62142, 39320, 53651 });
		AddPointToLine({ 62522, 40660, 53821 });
		AddPointToLine({ 62889, 42000, 53983 });

		NewLine();
		AddPointToLine({ 62968, 42300, 54018 });
		AddPointToLine({ 63304, 43600, 54165 });
		AddPointToLine({ 63623, 44900, 54305 });
		AddPointToLine({ 63924, 46200, 54435 });
		AddPointToLine({ 64205, 47500, 54557 });

		NewLine();
		AddPointToLine({ 64267, 47800, 54584 });
		AddPointToLine({ 64530, 49140, 54698 });
		AddPointToLine({ 64769, 50480, 54802 });
		AddPointToLine({ 64984, 51820, 54895 });
		AddPointToLine({ 65173, 53160, 54978 });
		AddPointToLine({ 65336, 54500, 55050 });

		NewLine();
		AddPointToLine({ 65369, 54800, 55064 });
		AddPointToLine({ 65490, 56040, 55119 });
		AddPointToLine({ 65587, 57280, 55164 });
		AddPointToLine({ 65660, 58520, 55199 });
		AddPointToLine({ 65708, 59760, 55225 });
		AddPointToLine({ 65732, 61000, 55241 });

		NewLine();
		AddPointToLine({ 65734, 61300, 55243 });
		AddPointToLine({ 65726, 62640, 55247 });
		AddPointToLine({ 65689, 63980, 55239 });
		AddPointToLine({ 65623, 65320, 55219 });
		AddPointToLine({ 65528, 66660, 55189 });
		AddPointToLine({ 65406, 68000, 55146 });

		NewLine();
		AddPointToLine({ 65375, 68300, 55135 });
		AddPointToLine({ 65232, 69540, 55084 });
		AddPointToLine({ 65066, 70780, 55023 });
		AddPointToLine({ 64878, 72020, 54953 });
		AddPointToLine({ 64668, 73260, 54874 });
		AddPointToLine({ 64438, 74500, 54786 });

		NewLine();
		AddPointToLine({ 64379, 74800, 54763 });
		AddPointToLine({ 64103, 76140, 54655 });
		AddPointToLine({ 63806, 77480, 54538 });
		AddPointToLine({ 63488, 78820, 54410 });
		AddPointToLine({ 63151, 80160, 54274 });
		AddPointToLine({ 62797, 81500, 54128 });

		NewLine();
		AddPointToLine({ 62715, 81800, 54094 });
		AddPointToLine({ 62256, 83440, 53902 });
		AddPointToLine({ 61777, 85080, 53698 });
		AddPointToLine({ 61281, 86720, 53482 });
		AddPointToLine({ 60772, 88360, 53256 });
		AddPointToLine({ 60252, 90000, 53021 });


		// ============================================================
		// ПОВЕРХНОСТЬ 20°
		// ============================================================

		NewLine();
		AddPointToLine({ 59041, 30000, 53014 });
		AddPointToLine({ 59445, 31250, 53211 });
		AddPointToLine({ 59847, 32500, 53405 });
		AddPointToLine({ 60246, 33750, 53594 });
		AddPointToLine({ 60641, 35000, 53779 });

		NewLine();
		AddPointToLine({ 60735, 35300, 53822 });
		AddPointToLine({ 61149, 36640, 54013 });
		AddPointToLine({ 61554, 37980, 54197 });
		AddPointToLine({ 61948, 39320, 54375 });
		AddPointToLine({ 62328, 40660, 54545 });
		AddPointToLine({ 62695, 42000, 54707 });

		NewLine();
		AddPointToLine({ 62774, 42300, 54742 });
		AddPointToLine({ 63110, 43600, 54889 });
		AddPointToLine({ 63429, 44900, 55029 });
		AddPointToLine({ 63730, 46200, 55159 });
		AddPointToLine({ 64011, 47500, 55281 });

		NewLine();
		AddPointToLine({ 64073, 47800, 55308 });
		AddPointToLine({ 64336, 49140, 55422 });
		AddPointToLine({ 64575, 50480, 55526 });
		AddPointToLine({ 64790, 51820, 55619 });
		AddPointToLine({ 64979, 53160, 55702 });
		AddPointToLine({ 65142, 54500, 55774 });

		NewLine();
		AddPointToLine({ 65175, 54800, 55788 });
		AddPointToLine({ 65296, 56040, 55843 });
		AddPointToLine({ 65393, 57280, 55888 });
		AddPointToLine({ 65466, 58520, 55923 });
		AddPointToLine({ 65514, 59760, 55949 });
		AddPointToLine({ 65538, 61000, 55965 });

		NewLine();
		AddPointToLine({ 65540, 61300, 55967 });
		AddPointToLine({ 65532, 62640, 55971 });
		AddPointToLine({ 65495, 63980, 55963 });
		AddPointToLine({ 65429, 65320, 55943 });
		AddPointToLine({ 65334, 66660, 55913 });
		AddPointToLine({ 65212, 68000, 55870 });

		NewLine();
		AddPointToLine({ 65181, 68300, 55859 });
		AddPointToLine({ 65038, 69540, 55808 });
		AddPointToLine({ 64872, 70780, 55747 });
		AddPointToLine({ 64684, 72020, 55677 });
		AddPointToLine({ 64474, 73260, 55598 });
		AddPointToLine({ 64244, 74500, 55510 });

		NewLine();
		AddPointToLine({ 64185, 74800, 55487 });
		AddPointToLine({ 63909, 76140, 55379 });
		AddPointToLine({ 63612, 77480, 55262 });
		AddPointToLine({ 63294, 78820, 55134 });
		AddPointToLine({ 62957, 80160, 54998 });
		AddPointToLine({ 62603, 81500, 54852 });

		NewLine();
		AddPointToLine({ 62521, 81800, 54818 });
		AddPointToLine({ 62062, 83440, 54626 });
		AddPointToLine({ 61583, 85080, 54422 });
		AddPointToLine({ 61087, 86720, 54206 });
		AddPointToLine({ 60578, 88360, 53980 });
		AddPointToLine({ 60058, 90000, 53745 });


		// ============================================================
		// ПОВЕРХНОСТЬ 30°
		// ============================================================

		NewLine();
		AddPointToLine({ 58724, 30000, 53693 });
		AddPointToLine({ 59128, 31250, 53890 });
		AddPointToLine({ 59530, 32500, 54084 });
		AddPointToLine({ 59929, 33750, 54273 });
		AddPointToLine({ 60324, 35000, 54458 });

		NewLine();
		AddPointToLine({ 60418, 35300, 54501 });
		AddPointToLine({ 60832, 36640, 54692 });
		AddPointToLine({ 61237, 37980, 54876 });
		AddPointToLine({ 61631, 39320, 55054 });
		AddPointToLine({ 62011, 40660, 55224 });
		AddPointToLine({ 62378, 42000, 55386 });

		NewLine();
		AddPointToLine({ 62457, 42300, 55421 });
		AddPointToLine({ 62793, 43600, 55568 });
		AddPointToLine({ 63112, 44900, 55708 });
		AddPointToLine({ 63413, 46200, 55838 });
		AddPointToLine({ 63694, 47500, 55960 });

		NewLine();
		AddPointToLine({ 63756, 47800, 55987 });
		AddPointToLine({ 64019, 49140, 56101 });
		AddPointToLine({ 64258, 50480, 56205 });
		AddPointToLine({ 64473, 51820, 56298 });
		AddPointToLine({ 64662, 53160, 56381 });
		AddPointToLine({ 64825, 54500, 56453 });

		NewLine();
		AddPointToLine({ 64858, 54800, 56467 });
		AddPointToLine({ 64979, 56040, 56522 });
		AddPointToLine({ 65076, 57280, 56567 });
		AddPointToLine({ 65149, 58520, 56602 });
		AddPointToLine({ 65197, 59760, 56628 });
		AddPointToLine({ 65221, 61000, 56644 });

		NewLine();
		AddPointToLine({ 65223, 61300, 56646 });
		AddPointToLine({ 65215, 62640, 56650 });
		AddPointToLine({ 65178, 63980, 56642 });
		AddPointToLine({ 65112, 65320, 56622 });
		AddPointToLine({ 65017, 66660, 56592 });
		AddPointToLine({ 64895, 68000, 56549 });

		NewLine();
		AddPointToLine({ 64864, 68300, 56538 });
		AddPointToLine({ 64721, 69540, 56487 });
		AddPointToLine({ 64555, 70780, 56426 });
		AddPointToLine({ 64367, 72020, 56356 });
		AddPointToLine({ 64157, 73260, 56277 });
		AddPointToLine({ 63927, 74500, 56189 });

		NewLine();
		AddPointToLine({ 63868, 74800, 56166 });
		AddPointToLine({ 63592, 76140, 56058 });
		AddPointToLine({ 63295, 77480, 55941 });
		AddPointToLine({ 62977, 78820, 55813 });
		AddPointToLine({ 62640, 80160, 55677 });
		AddPointToLine({ 62286, 81500, 55531 });

		NewLine();
		AddPointToLine({ 62204, 81800, 55497 });
		AddPointToLine({ 61745, 83440, 55305 });
		AddPointToLine({ 61266, 85080, 55101 });
		AddPointToLine({ 60770, 86720, 54885 });
		AddPointToLine({ 60261, 88360, 54659 });
		AddPointToLine({ 59741, 90000, 54424 });


		// ============================================================
		// ПОВЕРХНОСТЬ 40°
		// ============================================================

		NewLine();
		AddPointToLine({ 58294, 30000, 54307 });
		AddPointToLine({ 58698, 31250, 54504 });
		AddPointToLine({ 59100, 32500, 54698 });
		AddPointToLine({ 59499, 33750, 54887 });
		AddPointToLine({ 59894, 35000, 55072 });

		NewLine();
		AddPointToLine({ 59988, 35300, 55115 });
		AddPointToLine({ 60402, 36640, 55306 });
		AddPointToLine({ 60807, 37980, 55490 });
		AddPointToLine({ 61201, 39320, 55668 });
		AddPointToLine({ 61581, 40660, 55838 });
		AddPointToLine({ 61948, 42000, 56000 });

		NewLine();
		AddPointToLine({ 62027, 42300, 56035 });
		AddPointToLine({ 62363, 43600, 56182 });
		AddPointToLine({ 62682, 44900, 56322 });
		AddPointToLine({ 62983, 46200, 56452 });
		AddPointToLine({ 63264, 47500, 56574 });

		NewLine();
		AddPointToLine({ 63326, 47800, 56601 });
		AddPointToLine({ 63589, 49140, 56715 });
		AddPointToLine({ 63828, 50480, 56819 });
		AddPointToLine({ 64043, 51820, 56912 });
		AddPointToLine({ 64232, 53160, 56995 });
		AddPointToLine({ 64395, 54500, 57067 });

		NewLine();
		AddPointToLine({ 64428, 54800, 57081 });
		AddPointToLine({ 64549, 56040, 57136 });
		AddPointToLine({ 64646, 57280, 57181 });
		AddPointToLine({ 64719, 58520, 57216 });
		AddPointToLine({ 64767, 59760, 57242 });
		AddPointToLine({ 64791, 61000, 57258 });

		NewLine();
		AddPointToLine({ 64793, 61300, 57260 });
		AddPointToLine({ 64785, 62640, 57264 });
		AddPointToLine({ 64748, 63980, 57256 });
		AddPointToLine({ 64682, 65320, 57236 });
		AddPointToLine({ 64587, 66660, 57206 });
		AddPointToLine({ 64465, 68000, 57163 });

		NewLine();
		AddPointToLine({ 64434, 68300, 57152 });
		AddPointToLine({ 64291, 69540, 57101 });
		AddPointToLine({ 64125, 70780, 57040 });
		AddPointToLine({ 63937, 72020, 56970 });
		AddPointToLine({ 63727, 73260, 56891 });
		AddPointToLine({ 63497, 74500, 56803 });

		NewLine();
		AddPointToLine({ 63438, 74800, 56780 });
		AddPointToLine({ 63162, 76140, 56672 });
		AddPointToLine({ 62865, 77480, 56555 });
		AddPointToLine({ 62547, 78820, 56427 });
		AddPointToLine({ 62210, 80160, 56291 });
		AddPointToLine({ 61856, 81500, 56145 });

		NewLine();
		AddPointToLine({ 61774, 81800, 56111 });
		AddPointToLine({ 61315, 83440, 55919 });
		AddPointToLine({ 60836, 85080, 55715 });
		AddPointToLine({ 60340, 86720, 55499 });
		AddPointToLine({ 59831, 88360, 55273 });
		AddPointToLine({ 59311, 90000, 55038 });


		// ============================================================
		// ПОВЕРХНОСТЬ 50°
		// ============================================================

		NewLine();
		AddPointToLine({ 57764, 30000, 54837 });
		AddPointToLine({ 58168, 31250, 55034 });
		AddPointToLine({ 58570, 32500, 55228 });
		AddPointToLine({ 58969, 33750, 55417 });
		AddPointToLine({ 59364, 35000, 55602 });

		NewLine();
		AddPointToLine({ 59458, 35300, 55645 });
		AddPointToLine({ 59872, 36640, 55836 });
		AddPointToLine({ 60277, 37980, 56020 });
		AddPointToLine({ 60671, 39320, 56198 });
		AddPointToLine({ 61051, 40660, 56368 });
		AddPointToLine({ 61418, 42000, 56530 });

		NewLine();
		AddPointToLine({ 61497, 42300, 56565 });
		AddPointToLine({ 61833, 43600, 56712 });
		AddPointToLine({ 62152, 44900, 56852 });
		AddPointToLine({ 62453, 46200, 56982 });
		AddPointToLine({ 62734, 47500, 57104 });

		NewLine();
		AddPointToLine({ 62796, 47800, 57131 });
		AddPointToLine({ 63059, 49140, 57245 });
		AddPointToLine({ 63298, 50480, 57349 });
		AddPointToLine({ 63513, 51820, 57442 });
		AddPointToLine({ 63702, 53160, 57525 });
		AddPointToLine({ 63865, 54500, 57597 });

		NewLine();
		AddPointToLine({ 63898, 54800, 57611 });
		AddPointToLine({ 64019, 56040, 57666 });
		AddPointToLine({ 64116, 57280, 57711 });
		AddPointToLine({ 64189, 58520, 57746 });
		AddPointToLine({ 64237, 59760, 57772 });
		AddPointToLine({ 64261, 61000, 57788 });

		NewLine();
		AddPointToLine({ 64263, 61300, 57790 });
		AddPointToLine({ 64255, 62640, 57794 });
		AddPointToLine({ 64218, 63980, 57786 });
		AddPointToLine({ 64152, 65320, 57766 });
		AddPointToLine({ 64057, 66660, 57736 });
		AddPointToLine({ 63935, 68000, 57693 });

		NewLine();
		AddPointToLine({ 63904, 68300, 57682 });
		AddPointToLine({ 63761, 69540, 57631 });
		AddPointToLine({ 63595, 70780, 57570 });
		AddPointToLine({ 63407, 72020, 57500 });
		AddPointToLine({ 63197, 73260, 57421 });
		AddPointToLine({ 62967, 74500, 57333 });

		NewLine();
		AddPointToLine({ 62908, 74800, 57310 });
		AddPointToLine({ 62632, 76140, 57202 });
		AddPointToLine({ 62335, 77480, 57085 });
		AddPointToLine({ 62017, 78820, 56957 });
		AddPointToLine({ 61680, 80160, 56821 });
		AddPointToLine({ 61326, 81500, 56675 });

		NewLine();
		AddPointToLine({ 61244, 81800, 56641 });
		AddPointToLine({ 60785, 83440, 56449 });
		AddPointToLine({ 60306, 85080, 56245 });
		AddPointToLine({ 59810, 86720, 56029 });
		AddPointToLine({ 59301, 88360, 55803 });
		AddPointToLine({ 58781, 90000, 55568 });


		// ============================================================
		// ПОВЕРХНОСТЬ 60°
		// ============================================================

		NewLine();
		AddPointToLine({ 57150, 30000, 55267 });
		AddPointToLine({ 57554, 31250, 55464 });
		AddPointToLine({ 57956, 32500, 55658 });
		AddPointToLine({ 58355, 33750, 55847 });
		AddPointToLine({ 58750, 35000, 56032 });

		NewLine();
		AddPointToLine({ 58844, 35300, 56075 });
		AddPointToLine({ 59258, 36640, 56266 });
		AddPointToLine({ 59663, 37980, 56450 });
		AddPointToLine({ 60057, 39320, 56628 });
		AddPointToLine({ 60437, 40660, 56798 });
		AddPointToLine({ 60804, 42000, 56960 });

		NewLine();
		AddPointToLine({ 60883, 42300, 56995 });
		AddPointToLine({ 61219, 43600, 57142 });
		AddPointToLine({ 61538, 44900, 57282 });
		AddPointToLine({ 61839, 46200, 57412 });
		AddPointToLine({ 62120, 47500, 57534 });

		NewLine();
		AddPointToLine({ 62182, 47800, 57561 });
		AddPointToLine({ 62445, 49140, 57675 });
		AddPointToLine({ 62684, 50480, 57779 });
		AddPointToLine({ 62899, 51820, 57872 });
		AddPointToLine({ 63088, 53160, 57955 });
		AddPointToLine({ 63251, 54500, 58027 });

		NewLine();
		AddPointToLine({ 63284, 54800, 58041 });
		AddPointToLine({ 63405, 56040, 58096 });
		AddPointToLine({ 63502, 57280, 58141 });
		AddPointToLine({ 63575, 58520, 58176 });
		AddPointToLine({ 63623, 59760, 58202 });
		AddPointToLine({ 63647, 61000, 58218 });

		NewLine();
		AddPointToLine({ 63649, 61300, 58220 });
		AddPointToLine({ 63641, 62640, 58224 });
		AddPointToLine({ 63604, 63980, 58216 });
		AddPointToLine({ 63538, 65320, 58196 });
		AddPointToLine({ 63443, 66660, 58166 });
		AddPointToLine({ 63321, 68000, 58123 });

		NewLine();
		AddPointToLine({ 63290, 68300, 58112 });
		AddPointToLine({ 63147, 69540, 58061 });
		AddPointToLine({ 62981, 70780, 58000 });
		AddPointToLine({ 62793, 72020, 57930 });
		AddPointToLine({ 62583, 73260, 57851 });
		AddPointToLine({ 62353, 74500, 57763 });

		NewLine();
		AddPointToLine({ 62294, 74800, 57740 });
		AddPointToLine({ 62018, 76140, 57632 });
		AddPointToLine({ 61721, 77480, 57515 });
		AddPointToLine({ 61403, 78820, 57387 });
		AddPointToLine({ 61066, 80160, 57251 });
		AddPointToLine({ 60712, 81500, 57105 });

		NewLine();
		AddPointToLine({ 60630, 81800, 57071 });
		AddPointToLine({ 60171, 83440, 56879 });
		AddPointToLine({ 59692, 85080, 56675 });
		AddPointToLine({ 59196, 86720, 56459 });
		AddPointToLine({ 58687, 88360, 56233 });
		AddPointToLine({ 58167, 90000, 55998 });


		// ============================================================
		// ПОВЕРХНОСТЬ 70°
		// ============================================================

		NewLine();
		AddPointToLine({ 56471, 30000, 55584 });
		AddPointToLine({ 56875, 31250, 55781 });
		AddPointToLine({ 57277, 32500, 55975 });
		AddPointToLine({ 57676, 33750, 56164 });
		AddPointToLine({ 58071, 35000, 56349 });

		NewLine();
		AddPointToLine({ 58165, 35300, 56392 });
		AddPointToLine({ 58579, 36640, 56583 });
		AddPointToLine({ 58984, 37980, 56767 });
		AddPointToLine({ 59378, 39320, 56945 });
		AddPointToLine({ 59758, 40660, 57115 });
		AddPointToLine({ 60125, 42000, 57277 });

		NewLine();
		AddPointToLine({ 60204, 42300, 57312 });
		AddPointToLine({ 60540, 43600, 57459 });
		AddPointToLine({ 60859, 44900, 57599 });
		AddPointToLine({ 61160, 46200, 57729 });
		AddPointToLine({ 61441, 47500, 57851 });

		NewLine();
		AddPointToLine({ 61503, 47800, 57878 });
		AddPointToLine({ 61766, 49140, 57992 });
		AddPointToLine({ 62005, 50480, 58096 });
		AddPointToLine({ 62220, 51820, 58189 });
		AddPointToLine({ 62409, 53160, 58272 });
		AddPointToLine({ 62572, 54500, 58344 });

		NewLine();
		AddPointToLine({ 62605, 54800, 58358 });
		AddPointToLine({ 62726, 56040, 58413 });
		AddPointToLine({ 62823, 57280, 58458 });
		AddPointToLine({ 62896, 58520, 58493 });
		AddPointToLine({ 62944, 59760, 58519 });
		AddPointToLine({ 62968, 61000, 58535 });

		NewLine();
		AddPointToLine({ 62970, 61300, 58537 });
		AddPointToLine({ 62962, 62640, 58541 });
		AddPointToLine({ 62925, 63980, 58533 });
		AddPointToLine({ 62859, 65320, 58513 });
		AddPointToLine({ 62764, 66660, 58483 });
		AddPointToLine({ 62642, 68000, 58440 });

		NewLine();
		AddPointToLine({ 62611, 68300, 58429 });
		AddPointToLine({ 62468, 69540, 58378 });
		AddPointToLine({ 62302, 70780, 58317 });
		AddPointToLine({ 62114, 72020, 58247 });
		AddPointToLine({ 61904, 73260, 58168 });
		AddPointToLine({ 61674, 74500, 58080 });

		NewLine();
		AddPointToLine({ 61615, 74800, 58057 });
		AddPointToLine({ 61339, 76140, 57949 });
		AddPointToLine({ 61042, 77480, 57832 });
		AddPointToLine({ 60724, 78820, 57704 });
		AddPointToLine({ 60387, 80160, 57568 });
		AddPointToLine({ 60033, 81500, 57422 });

		NewLine();
		AddPointToLine({ 59951, 81800, 57388 });
		AddPointToLine({ 59492, 83440, 57196 });
		AddPointToLine({ 59013, 85080, 56992 });
		AddPointToLine({ 58517, 86720, 56776 });
		AddPointToLine({ 58008, 88360, 56550 });
		AddPointToLine({ 57488, 90000, 56315 });


		// ============================================================
		// ПОВЕРХНОСТЬ 80°
		// ============================================================

		NewLine();
		AddPointToLine({ 55747, 30000, 55778 });
		AddPointToLine({ 56151, 31250, 55975 });
		AddPointToLine({ 56553, 32500, 56169 });
		AddPointToLine({ 56952, 33750, 56358 });
		AddPointToLine({ 57347, 35000, 56543 });

		NewLine();
		AddPointToLine({ 57441, 35300, 56586 });
		AddPointToLine({ 57855, 36640, 56777 });
		AddPointToLine({ 58260, 37980, 56961 });
		AddPointToLine({ 58654, 39320, 57139 });
		AddPointToLine({ 59034, 40660, 57309 });
		AddPointToLine({ 59401, 42000, 57471 });

		NewLine();
		AddPointToLine({ 59480, 42300, 57506 });
		AddPointToLine({ 59816, 43600, 57653 });
		AddPointToLine({ 60135, 44900, 57793 });
		AddPointToLine({ 60436, 46200, 57923 });
		AddPointToLine({ 60717, 47500, 58045 });

		NewLine();
		AddPointToLine({ 60779, 47800, 58072 });
		AddPointToLine({ 61042, 49140, 58186 });
		AddPointToLine({ 61281, 50480, 58290 });
		AddPointToLine({ 61496, 51820, 58383 });
		AddPointToLine({ 61685, 53160, 58466 });
		AddPointToLine({ 61848, 54500, 58538 });

		NewLine();
		AddPointToLine({ 61881, 54800, 58552 });
		AddPointToLine({ 62002, 56040, 58607 });
		AddPointToLine({ 62099, 57280, 58652 });
		AddPointToLine({ 62172, 58520, 58687 });
		AddPointToLine({ 62220, 59760, 58713 });
		AddPointToLine({ 62244, 61000, 58729 });

		NewLine();
		AddPointToLine({ 62246, 61300, 58731 });
		AddPointToLine({ 62238, 62640, 58735 });
		AddPointToLine({ 62201, 63980, 58727 });
		AddPointToLine({ 62135, 65320, 58707 });
		AddPointToLine({ 62040, 66660, 58677 });
		AddPointToLine({ 61918, 68000, 58634 });

		NewLine();
		AddPointToLine({ 61887, 68300, 58623 });
		AddPointToLine({ 61744, 69540, 58572 });
		AddPointToLine({ 61578, 70780, 58511 });
		AddPointToLine({ 61390, 72020, 58441 });
		AddPointToLine({ 61180, 73260, 58362 });
		AddPointToLine({ 60950, 74500, 58274 });

		NewLine();
		AddPointToLine({ 60891, 74800, 58251 });
		AddPointToLine({ 60615, 76140, 58143 });
		AddPointToLine({ 60318, 77480, 58026 });
		AddPointToLine({ 60000, 78820, 57898 });
		AddPointToLine({ 59663, 80160, 57762 });
		AddPointToLine({ 59309, 81500, 57616 });

		NewLine();
		AddPointToLine({ 59227, 81800, 57582 });
		AddPointToLine({ 58768, 83440, 57390 });
		AddPointToLine({ 58289, 85080, 57186 });
		AddPointToLine({ 57793, 86720, 56970 });
		AddPointToLine({ 57284, 88360, 56744 });
		AddPointToLine({ 56764, 90000, 56509 });


		// ============================================================
		// ПОВЕРХНОСТЬ 90°
		// ============================================================

		NewLine();
		AddPointToLine({ 55000, 30000, 55843 });
		AddPointToLine({ 55404, 31250, 56040 });
		AddPointToLine({ 55806, 32500, 56234 });
		AddPointToLine({ 56205, 33750, 56423 });
		AddPointToLine({ 56600, 35000, 56608 });

		NewLine();
		AddPointToLine({ 56694, 35300, 56651 });
		AddPointToLine({ 57108, 36640, 56842 });
		AddPointToLine({ 57513, 37980, 57026 });
		AddPointToLine({ 57907, 39320, 57204 });
		AddPointToLine({ 58287, 40660, 57374 });
		AddPointToLine({ 58654, 42000, 57536 });

		NewLine();
		AddPointToLine({ 58733, 42300, 57571 });
		AddPointToLine({ 59069, 43600, 57718 });
		AddPointToLine({ 59388, 44900, 57858 });
		AddPointToLine({ 59689, 46200, 57988 });
		AddPointToLine({ 59970, 47500, 58110 });

		NewLine();
		AddPointToLine({ 60032, 47800, 58137 });
		AddPointToLine({ 60295, 49140, 58251 });
		AddPointToLine({ 60534, 50480, 58355 });
		AddPointToLine({ 60749, 51820, 58448 });
		AddPointToLine({ 60938, 53160, 58531 });
		AddPointToLine({ 61101, 54500, 58603 });

		NewLine();
		AddPointToLine({ 61134, 54800, 58617 });
		AddPointToLine({ 61255, 56040, 58672 });
		AddPointToLine({ 61352, 57280, 58717 });
		AddPointToLine({ 61425, 58520, 58752 });
		AddPointToLine({ 61473, 59760, 58778 });
		AddPointToLine({ 61497, 61000, 58794 });

		NewLine();
		AddPointToLine({ 61499, 61300, 58796 });
		AddPointToLine({ 61491, 62640, 58800 });
		AddPointToLine({ 61454, 63980, 58792 });
		AddPointToLine({ 61388, 65320, 58772 });
		AddPointToLine({ 61293, 66660, 58742 });
		AddPointToLine({ 61171, 68000, 58699 });

		NewLine();
		AddPointToLine({ 61140, 68300, 58688 });
		AddPointToLine({ 60997, 69540, 58637 });
		AddPointToLine({ 60831, 70780, 58576 });
		AddPointToLine({ 60643, 72020, 58506 });
		AddPointToLine({ 60433, 73260, 58427 });
		AddPointToLine({ 60203, 74500, 58339 });

		NewLine();
		AddPointToLine({ 60144, 74800, 58316 });
		AddPointToLine({ 59868, 76140, 58208 });
		AddPointToLine({ 59571, 77480, 58091 });
		AddPointToLine({ 59253, 78820, 57963 });
		AddPointToLine({ 58916, 80160, 57827 });
		AddPointToLine({ 58562, 81500, 57681 });

		NewLine();
		AddPointToLine({ 58480, 81800, 57647 });
		AddPointToLine({ 58021, 83440, 57455 });
		AddPointToLine({ 57542, 85080, 57251 });
		AddPointToLine({ 57046, 86720, 57035 });
		AddPointToLine({ 56537, 88360, 56809 });
		AddPointToLine({ 56017, 90000, 56574 });


		// ============================================================
		// ПОВЕРХНОСТЬ 100°
		// ============================================================

		NewLine();
		AddPointToLine({ 54253, 30000, 55778 });
		AddPointToLine({ 54657, 31250, 55975 });
		AddPointToLine({ 55059, 32500, 56169 });
		AddPointToLine({ 55458, 33750, 56358 });
		AddPointToLine({ 55853, 35000, 56543 });

		NewLine();
		AddPointToLine({ 55947, 35300, 56586 });
		AddPointToLine({ 56361, 36640, 56777 });
		AddPointToLine({ 56766, 37980, 56961 });
		AddPointToLine({ 57160, 39320, 57139 });
		AddPointToLine({ 57540, 40660, 57309 });
		AddPointToLine({ 57907, 42000, 57471 });

		NewLine();
		AddPointToLine({ 57986, 42300, 57506 });
		AddPointToLine({ 58322, 43600, 57653 });
		AddPointToLine({ 58641, 44900, 57793 });
		AddPointToLine({ 58942, 46200, 57923 });
		AddPointToLine({ 59223, 47500, 58045 });

		NewLine();
		AddPointToLine({ 59285, 47800, 58072 });
		AddPointToLine({ 59548, 49140, 58186 });
		AddPointToLine({ 59787, 50480, 58290 });
		AddPointToLine({ 60002, 51820, 58383 });
		AddPointToLine({ 60191, 53160, 58466 });
		AddPointToLine({ 60354, 54500, 58538 });

		NewLine();
		AddPointToLine({ 60387, 54800, 58552 });
		AddPointToLine({ 60508, 56040, 58607 });
		AddPointToLine({ 60605, 57280, 58652 });
		AddPointToLine({ 60678, 58520, 58687 });
		AddPointToLine({ 60726, 59760, 58713 });
		AddPointToLine({ 60750, 61000, 58729 });

		NewLine();
		AddPointToLine({ 60752, 61300, 58731 });
		AddPointToLine({ 60744, 62640, 58735 });
		AddPointToLine({ 60707, 63980, 58727 });
		AddPointToLine({ 60641, 65320, 58707 });
		AddPointToLine({ 60546, 66660, 58677 });
		AddPointToLine({ 60424, 68000, 58634 });

		NewLine();
		AddPointToLine({ 60393, 68300, 58623 });
		AddPointToLine({ 60250, 69540, 58572 });
		AddPointToLine({ 60084, 70780, 58511 });
		AddPointToLine({ 59896, 72020, 58441 });
		AddPointToLine({ 59686, 73260, 58362 });
		AddPointToLine({ 59456, 74500, 58274 });

		NewLine();
		AddPointToLine({ 59397, 74800, 58251 });
		AddPointToLine({ 59121, 76140, 58143 });
		AddPointToLine({ 58824, 77480, 58026 });
		AddPointToLine({ 58506, 78820, 57898 });
		AddPointToLine({ 58169, 80160, 57762 });
		AddPointToLine({ 57815, 81500, 57616 });

		NewLine();
		AddPointToLine({ 57733, 81800, 57582 });
		AddPointToLine({ 57274, 83440, 57390 });
		AddPointToLine({ 56795, 85080, 57186 });
		AddPointToLine({ 56299, 86720, 56970 });
		AddPointToLine({ 55790, 88360, 56744 });
		AddPointToLine({ 55270, 90000, 56509 });


		// ============================================================
		// ПОВЕРХНОСТЬ 110°
		// ============================================================

		NewLine();
		AddPointToLine({ 53529, 30000, 55584 });
		AddPointToLine({ 53933, 31250, 55781 });
		AddPointToLine({ 54335, 32500, 55975 });
		AddPointToLine({ 54734, 33750, 56164 });
		AddPointToLine({ 55129, 35000, 56349 });

		NewLine();
		AddPointToLine({ 55223, 35300, 56392 });
		AddPointToLine({ 55637, 36640, 56583 });
		AddPointToLine({ 56042, 37980, 56767 });
		AddPointToLine({ 56436, 39320, 56945 });
		AddPointToLine({ 56816, 40660, 57115 });
		AddPointToLine({ 57183, 42000, 57277 });

		NewLine();
		AddPointToLine({ 57262, 42300, 57312 });
		AddPointToLine({ 57598, 43600, 57459 });
		AddPointToLine({ 57917, 44900, 57599 });
		AddPointToLine({ 58218, 46200, 57729 });
		AddPointToLine({ 58499, 47500, 57851 });

		NewLine();
		AddPointToLine({ 58561, 47800, 57878 });
		AddPointToLine({ 58824, 49140, 57992 });
		AddPointToLine({ 59063, 50480, 58096 });
		AddPointToLine({ 59278, 51820, 58189 });
		AddPointToLine({ 59467, 53160, 58272 });
		AddPointToLine({ 59630, 54500, 58344 });

		NewLine();
		AddPointToLine({ 59663, 54800, 58358 });
		AddPointToLine({ 59784, 56040, 58413 });
		AddPointToLine({ 59881, 57280, 58458 });
		AddPointToLine({ 59954, 58520, 58493 });
		AddPointToLine({ 60002, 59760, 58519 });
		AddPointToLine({ 60026, 61000, 58535 });

		NewLine();
		AddPointToLine({ 60028, 61300, 58537 });
		AddPointToLine({ 60020, 62640, 58541 });
		AddPointToLine({ 59983, 63980, 58533 });
		AddPointToLine({ 59917, 65320, 58513 });
		AddPointToLine({ 59822, 66660, 58483 });
		AddPointToLine({ 59700, 68000, 58440 });

		NewLine();
		AddPointToLine({ 59669, 68300, 58429 });
		AddPointToLine({ 59526, 69540, 58378 });
		AddPointToLine({ 59360, 70780, 58317 });
		AddPointToLine({ 59172, 72020, 58247 });
		AddPointToLine({ 58962, 73260, 58168 });
		AddPointToLine({ 58732, 74500, 58080 });

		NewLine();
		AddPointToLine({ 58673, 74800, 58057 });
		AddPointToLine({ 58397, 76140, 57949 });
		AddPointToLine({ 58100, 77480, 57832 });
		AddPointToLine({ 57782, 78820, 57704 });
		AddPointToLine({ 57445, 80160, 57568 });
		AddPointToLine({ 57091, 81500, 57422 });

		NewLine();
		AddPointToLine({ 57009, 81800, 57388 });
		AddPointToLine({ 56550, 83440, 57196 });
		AddPointToLine({ 56071, 85080, 56992 });
		AddPointToLine({ 55575, 86720, 56776 });
		AddPointToLine({ 55066, 88360, 56550 });
		AddPointToLine({ 54546, 90000, 56315 });


		// ============================================================
		// ПОВЕРХНОСТЬ 120°
		// ============================================================

		NewLine();
		AddPointToLine({ 52850, 30000, 55267 });
		AddPointToLine({ 53254, 31250, 55464 });
		AddPointToLine({ 53656, 32500, 55658 });
		AddPointToLine({ 54055, 33750, 55847 });
		AddPointToLine({ 54450, 35000, 56032 });

		NewLine();
		AddPointToLine({ 54544, 35300, 56075 });
		AddPointToLine({ 54958, 36640, 56266 });
		AddPointToLine({ 55363, 37980, 56450 });
		AddPointToLine({ 55757, 39320, 56628 });
		AddPointToLine({ 56137, 40660, 56798 });
		AddPointToLine({ 56504, 42000, 56960 });

		NewLine();
		AddPointToLine({ 56583, 42300, 56995 });
		AddPointToLine({ 56919, 43600, 57142 });
		AddPointToLine({ 57238, 44900, 57282 });
		AddPointToLine({ 57539, 46200, 57412 });
		AddPointToLine({ 57820, 47500, 57534 });

		NewLine();
		AddPointToLine({ 57882, 47800, 57561 });
		AddPointToLine({ 58145, 49140, 57675 });
		AddPointToLine({ 58384, 50480, 57779 });
		AddPointToLine({ 58599, 51820, 57872 });
		AddPointToLine({ 58788, 53160, 57955 });
		AddPointToLine({ 58951, 54500, 58027 });

		NewLine();
		AddPointToLine({ 58984, 54800, 58041 });
		AddPointToLine({ 59105, 56040, 58096 });
		AddPointToLine({ 59202, 57280, 58141 });
		AddPointToLine({ 59275, 58520, 58176 });
		AddPointToLine({ 59323, 59760, 58202 });
		AddPointToLine({ 59347, 61000, 58218 });

		NewLine();
		AddPointToLine({ 59349, 61300, 58220 });
		AddPointToLine({ 59341, 62640, 58224 });
		AddPointToLine({ 59304, 63980, 58216 });
		AddPointToLine({ 59238, 65320, 58196 });
		AddPointToLine({ 59143, 66660, 58166 });
		AddPointToLine({ 59021, 68000, 58123 });

		NewLine();
		AddPointToLine({ 58990, 68300, 58112 });
		AddPointToLine({ 58847, 69540, 58061 });
		AddPointToLine({ 58681, 70780, 58000 });
		AddPointToLine({ 58493, 72020, 57930 });
		AddPointToLine({ 58283, 73260, 57851 });
		AddPointToLine({ 58053, 74500, 57763 });

		NewLine();
		AddPointToLine({ 57994, 74800, 57740 });
		AddPointToLine({ 57718, 76140, 57632 });
		AddPointToLine({ 57421, 77480, 57515 });
		AddPointToLine({ 57103, 78820, 57387 });
		AddPointToLine({ 56766, 80160, 57251 });
		AddPointToLine({ 56412, 81500, 57105 });

		NewLine();
		AddPointToLine({ 56330, 81800, 57071 });
		AddPointToLine({ 55871, 83440, 56879 });
		AddPointToLine({ 55392, 85080, 56675 });
		AddPointToLine({ 54896, 86720, 56459 });
		AddPointToLine({ 54387, 88360, 56233 });
		AddPointToLine({ 53867, 90000, 55998 });


		// ============================================================
		// ПОВЕРХНОСТЬ 130°
		// ============================================================

		NewLine();
		AddPointToLine({ 52236, 30000, 54837 });
		AddPointToLine({ 52640, 31250, 55034 });
		AddPointToLine({ 53042, 32500, 55228 });
		AddPointToLine({ 53441, 33750, 55417 });
		AddPointToLine({ 53836, 35000, 55602 });

		NewLine();
		AddPointToLine({ 53930, 35300, 55645 });
		AddPointToLine({ 54344, 36640, 55836 });
		AddPointToLine({ 54749, 37980, 56020 });
		AddPointToLine({ 55143, 39320, 56198 });
		AddPointToLine({ 55523, 40660, 56368 });
		AddPointToLine({ 55890, 42000, 56530 });

		NewLine();
		AddPointToLine({ 55969, 42300, 56565 });
		AddPointToLine({ 56305, 43600, 56712 });
		AddPointToLine({ 56624, 44900, 56852 });
		AddPointToLine({ 56925, 46200, 56982 });
		AddPointToLine({ 57206, 47500, 57104 });

		NewLine();
		AddPointToLine({ 57268, 47800, 57131 });
		AddPointToLine({ 57531, 49140, 57245 });
		AddPointToLine({ 57770, 50480, 57349 });
		AddPointToLine({ 57985, 51820, 57442 });
		AddPointToLine({ 58174, 53160, 57525 });
		AddPointToLine({ 58337, 54500, 57597 });

		NewLine();
		AddPointToLine({ 58370, 54800, 57611 });
		AddPointToLine({ 58491, 56040, 57666 });
		AddPointToLine({ 58588, 57280, 57711 });
		AddPointToLine({ 58661, 58520, 57746 });
		AddPointToLine({ 58709, 59760, 57772 });
		AddPointToLine({ 58733, 61000, 57788 });

		NewLine();
		AddPointToLine({ 58735, 61300, 57790 });
		AddPointToLine({ 58727, 62640, 57794 });
		AddPointToLine({ 58690, 63980, 57786 });
		AddPointToLine({ 58624, 65320, 57766 });
		AddPointToLine({ 58529, 66660, 57736 });
		AddPointToLine({ 58407, 68000, 57693 });

		NewLine();
		AddPointToLine({ 58376, 68300, 57682 });
		AddPointToLine({ 58233, 69540, 57631 });
		AddPointToLine({ 58067, 70780, 57570 });
		AddPointToLine({ 57879, 72020, 57500 });
		AddPointToLine({ 57669, 73260, 57421 });
		AddPointToLine({ 57439, 74500, 57333 });

		NewLine();
		AddPointToLine({ 57380, 74800, 57310 });
		AddPointToLine({ 57104, 76140, 57202 });
		AddPointToLine({ 56807, 77480, 57085 });
		AddPointToLine({ 56489, 78820, 56957 });
		AddPointToLine({ 56152, 80160, 56821 });
		AddPointToLine({ 55798, 81500, 56675 });

		NewLine();
		AddPointToLine({ 55716, 81800, 56641 });
		AddPointToLine({ 55257, 83440, 56449 });
		AddPointToLine({ 54778, 85080, 56245 });
		AddPointToLine({ 54282, 86720, 56029 });
		AddPointToLine({ 53773, 88360, 55803 });
		AddPointToLine({ 53253, 90000, 55568 });


		// ============================================================
		// ПОВЕРХНОСТЬ 140°
		// ============================================================

		NewLine();
		AddPointToLine({ 51706, 30000, 54307 });
		AddPointToLine({ 52110, 31250, 54504 });
		AddPointToLine({ 52512, 32500, 54698 });
		AddPointToLine({ 52911, 33750, 54887 });
		AddPointToLine({ 53306, 35000, 55072 });

		NewLine();
		AddPointToLine({ 53400, 35300, 55115 });
		AddPointToLine({ 53814, 36640, 55306 });
		AddPointToLine({ 54219, 37980, 55490 });
		AddPointToLine({ 54613, 39320, 55668 });
		AddPointToLine({ 54993, 40660, 55838 });
		AddPointToLine({ 55360, 42000, 56000 });

		NewLine();
		AddPointToLine({ 55439, 42300, 56035 });
		AddPointToLine({ 55775, 43600, 56182 });
		AddPointToLine({ 56094, 44900, 56322 });
		AddPointToLine({ 56395, 46200, 56452 });
		AddPointToLine({ 56676, 47500, 56574 });

		NewLine();
		AddPointToLine({ 56738, 47800, 56601 });
		AddPointToLine({ 57001, 49140, 56715 });
		AddPointToLine({ 57240, 50480, 56819 });
		AddPointToLine({ 57455, 51820, 56912 });
		AddPointToLine({ 57644, 53160, 56995 });
		AddPointToLine({ 57807, 54500, 57067 });

		NewLine();
		AddPointToLine({ 57840, 54800, 57081 });
		AddPointToLine({ 57961, 56040, 57136 });
		AddPointToLine({ 58058, 57280, 57181 });
		AddPointToLine({ 58131, 58520, 57216 });
		AddPointToLine({ 58179, 59760, 57242 });
		AddPointToLine({ 58203, 61000, 57258 });

		NewLine();
		AddPointToLine({ 58205, 61300, 57260 });
		AddPointToLine({ 58197, 62640, 57264 });
		AddPointToLine({ 58160, 63980, 57256 });
		AddPointToLine({ 58094, 65320, 57236 });
		AddPointToLine({ 57999, 66660, 57206 });
		AddPointToLine({ 57877, 68000, 57163 });

		NewLine();
		AddPointToLine({ 57846, 68300, 57152 });
		AddPointToLine({ 57703, 69540, 57101 });
		AddPointToLine({ 57537, 70780, 57040 });
		AddPointToLine({ 57349, 72020, 56970 });
		AddPointToLine({ 57139, 73260, 56891 });
		AddPointToLine({ 56909, 74500, 56803 });

		NewLine();
		AddPointToLine({ 56850, 74800, 56780 });
		AddPointToLine({ 56574, 76140, 56672 });
		AddPointToLine({ 56277, 77480, 56555 });
		AddPointToLine({ 55959, 78820, 56427 });
		AddPointToLine({ 55622, 80160, 56291 });
		AddPointToLine({ 55268, 81500, 56145 });

		NewLine();
		AddPointToLine({ 55186, 81800, 56111 });
		AddPointToLine({ 54727, 83440, 55919 });
		AddPointToLine({ 54248, 85080, 55715 });
		AddPointToLine({ 53752, 86720, 55499 });
		AddPointToLine({ 53243, 88360, 55273 });
		AddPointToLine({ 52723, 90000, 55038 });


		// ============================================================
		// ПОВЕРХНОСТЬ 150°
		// ============================================================

		NewLine();
		AddPointToLine({ 51276, 30000, 53693 });
		AddPointToLine({ 51680, 31250, 53890 });
		AddPointToLine({ 52082, 32500, 54084 });
		AddPointToLine({ 52481, 33750, 54273 });
		AddPointToLine({ 52876, 35000, 54458 });

		NewLine();
		AddPointToLine({ 52970, 35300, 54501 });
		AddPointToLine({ 53384, 36640, 54692 });
		AddPointToLine({ 53789, 37980, 54876 });
		AddPointToLine({ 54183, 39320, 55054 });
		AddPointToLine({ 54563, 40660, 55224 });
		AddPointToLine({ 54930, 42000, 55386 });

		NewLine();
		AddPointToLine({ 55009, 42300, 55421 });
		AddPointToLine({ 55345, 43600, 55568 });
		AddPointToLine({ 55664, 44900, 55708 });
		AddPointToLine({ 55965, 46200, 55838 });
		AddPointToLine({ 56246, 47500, 55960 });

		NewLine();
		AddPointToLine({ 56308, 47800, 55987 });
		AddPointToLine({ 56571, 49140, 56101 });
		AddPointToLine({ 56810, 50480, 56205 });
		AddPointToLine({ 57025, 51820, 56298 });
		AddPointToLine({ 57214, 53160, 56381 });
		AddPointToLine({ 57377, 54500, 56453 });

		NewLine();
		AddPointToLine({ 57410, 54800, 56467 });
		AddPointToLine({ 57531, 56040, 56522 });
		AddPointToLine({ 57628, 57280, 56567 });
		AddPointToLine({ 57701, 58520, 56602 });
		AddPointToLine({ 57749, 59760, 56628 });
		AddPointToLine({ 57773, 61000, 56644 });

		NewLine();
		AddPointToLine({ 57775, 61300, 56646 });
		AddPointToLine({ 57767, 62640, 56650 });
		AddPointToLine({ 57730, 63980, 56642 });
		AddPointToLine({ 57664, 65320, 56622 });
		AddPointToLine({ 57569, 66660, 56592 });
		AddPointToLine({ 57447, 68000, 56549 });

		NewLine();
		AddPointToLine({ 57416, 68300, 56538 });
		AddPointToLine({ 57273, 69540, 56487 });
		AddPointToLine({ 57107, 70780, 56426 });
		AddPointToLine({ 56919, 72020, 56356 });
		AddPointToLine({ 56709, 73260, 56277 });
		AddPointToLine({ 56479, 74500, 56189 });

		NewLine();
		AddPointToLine({ 56420, 74800, 56166 });
		AddPointToLine({ 56144, 76140, 56058 });
		AddPointToLine({ 55847, 77480, 55941 });
		AddPointToLine({ 55529, 78820, 55813 });
		AddPointToLine({ 55192, 80160, 55677 });
		AddPointToLine({ 54838, 81500, 55531 });

		NewLine();
		AddPointToLine({ 54756, 81800, 55497 });
		AddPointToLine({ 54297, 83440, 55305 });
		AddPointToLine({ 53818, 85080, 55101 });
		AddPointToLine({ 53322, 86720, 54885 });
		AddPointToLine({ 52813, 88360, 54659 });
		AddPointToLine({ 52293, 90000, 54424 });


		// ============================================================
		// ПОВЕРХНОСТЬ 160°
		// ============================================================

		NewLine();
		AddPointToLine({ 50959, 30000, 53014 });
		AddPointToLine({ 51363, 31250, 53211 });
		AddPointToLine({ 51765, 32500, 53405 });
		AddPointToLine({ 52164, 33750, 53594 });
		AddPointToLine({ 52559, 35000, 53779 });

		NewLine();
		AddPointToLine({ 52653, 35300, 53822 });
		AddPointToLine({ 53067, 36640, 54013 });
		AddPointToLine({ 53472, 37980, 54197 });
		AddPointToLine({ 53866, 39320, 54375 });
		AddPointToLine({ 54246, 40660, 54545 });
		AddPointToLine({ 54613, 42000, 54707 });

		NewLine();
		AddPointToLine({ 54692, 42300, 54742 });
		AddPointToLine({ 55028, 43600, 54889 });
		AddPointToLine({ 55347, 44900, 55029 });
		AddPointToLine({ 55648, 46200, 55159 });
		AddPointToLine({ 55929, 47500, 55281 });

		NewLine();
		AddPointToLine({ 55991, 47800, 55308 });
		AddPointToLine({ 56254, 49140, 55422 });
		AddPointToLine({ 56493, 50480, 55526 });
		AddPointToLine({ 56708, 51820, 55619 });
		AddPointToLine({ 56897, 53160, 55702 });
		AddPointToLine({ 57060, 54500, 55774 });

		NewLine();
		AddPointToLine({ 57093, 54800, 55788 });
		AddPointToLine({ 57214, 56040, 55843 });
		AddPointToLine({ 57311, 57280, 55888 });
		AddPointToLine({ 57384, 58520, 55923 });
		AddPointToLine({ 57432, 59760, 55949 });
		AddPointToLine({ 57456, 61000, 55965 });

		NewLine();
		AddPointToLine({ 57458, 61300, 55967 });
		AddPointToLine({ 57450, 62640, 55971 });
		AddPointToLine({ 57413, 63980, 55963 });
		AddPointToLine({ 57347, 65320, 55943 });
		AddPointToLine({ 57252, 66660, 55913 });
		AddPointToLine({ 57130, 68000, 55870 });

		NewLine();
		AddPointToLine({ 57099, 68300, 55859 });
		AddPointToLine({ 56956, 69540, 55808 });
		AddPointToLine({ 56790, 70780, 55747 });
		AddPointToLine({ 56602, 72020, 55677 });
		AddPointToLine({ 56392, 73260, 55598 });
		AddPointToLine({ 56162, 74500, 55510 });

		NewLine();
		AddPointToLine({ 56103, 74800, 55487 });
		AddPointToLine({ 55827, 76140, 55379 });
		AddPointToLine({ 55530, 77480, 55262 });
		AddPointToLine({ 55212, 78820, 55134 });
		AddPointToLine({ 54875, 80160, 54998 });
		AddPointToLine({ 54521, 81500, 54852 });

		NewLine();
		AddPointToLine({ 54439, 81800, 54818 });
		AddPointToLine({ 53980, 83440, 54626 });
		AddPointToLine({ 53501, 85080, 54422 });
		AddPointToLine({ 53005, 86720, 54206 });
		AddPointToLine({ 52496, 88360, 53980 });
		AddPointToLine({ 51976, 90000, 53745 });


		// ============================================================
		// ПОВЕРХНОСТЬ 170°
		// ============================================================

		NewLine();
		AddPointToLine({ 50765, 30000, 52290 });
		AddPointToLine({ 51169, 31250, 52487 });
		AddPointToLine({ 51571, 32500, 52681 });
		AddPointToLine({ 51970, 33750, 52870 });
		AddPointToLine({ 52365, 35000, 53055 });

		NewLine();
		AddPointToLine({ 52459, 35300, 53098 });
		AddPointToLine({ 52873, 36640, 53289 });
		AddPointToLine({ 53278, 37980, 53473 });
		AddPointToLine({ 53672, 39320, 53651 });
		AddPointToLine({ 54052, 40660, 53821 });
		AddPointToLine({ 54419, 42000, 53983 });

		NewLine();
		AddPointToLine({ 54498, 42300, 54018 });
		AddPointToLine({ 54834, 43600, 54165 });
		AddPointToLine({ 55153, 44900, 54305 });
		AddPointToLine({ 55454, 46200, 54435 });
		AddPointToLine({ 55735, 47500, 54557 });

		NewLine();
		AddPointToLine({ 55797, 47800, 54584 });
		AddPointToLine({ 56060, 49140, 54698 });
		AddPointToLine({ 56299, 50480, 54802 });
		AddPointToLine({ 56514, 51820, 54895 });
		AddPointToLine({ 56703, 53160, 54978 });
		AddPointToLine({ 56866, 54500, 55050 });

		NewLine();
		AddPointToLine({ 56899, 54800, 55064 });
		AddPointToLine({ 57020, 56040, 55119 });
		AddPointToLine({ 57117, 57280, 55164 });
		AddPointToLine({ 57190, 58520, 55199 });
		AddPointToLine({ 57238, 59760, 55225 });
		AddPointToLine({ 57262, 61000, 55241 });

		NewLine();
		AddPointToLine({ 57264, 61300, 55243 });
		AddPointToLine({ 57256, 62640, 55247 });
		AddPointToLine({ 57219, 63980, 55239 });
		AddPointToLine({ 57153, 65320, 55219 });
		AddPointToLine({ 57058, 66660, 55189 });
		AddPointToLine({ 56936, 68000, 55146 });

		NewLine();
		AddPointToLine({ 56905, 68300, 55135 });
		AddPointToLine({ 56762, 69540, 55084 });
		AddPointToLine({ 56596, 70780, 55023 });
		AddPointToLine({ 56408, 72020, 54953 });
		AddPointToLine({ 56198, 73260, 54874 });
		AddPointToLine({ 55968, 74500, 54786 });

		NewLine();
		AddPointToLine({ 55909, 74800, 54763 });
		AddPointToLine({ 55633, 76140, 54655 });
		AddPointToLine({ 55336, 77480, 54538 });
		AddPointToLine({ 55018, 78820, 54410 });
		AddPointToLine({ 54681, 80160, 54274 });
		AddPointToLine({ 54327, 81500, 54128 });

		NewLine();
		AddPointToLine({ 54245, 81800, 54094 });
		AddPointToLine({ 53786, 83440, 53902 });
		AddPointToLine({ 53307, 85080, 53698 });
		AddPointToLine({ 52811, 86720, 53482 });
		AddPointToLine({ 52302, 88360, 53256 });
		AddPointToLine({ 51782, 90000, 53021 });


		// ============================================================
		// ПОВЕРХНОСТЬ 180°
		// ============================================================

		NewLine();
		AddPointToLine({ 50700, 30000, 51543 });
		AddPointToLine({ 51104, 31250, 51740 });
		AddPointToLine({ 51506, 32500, 51934 });
		AddPointToLine({ 51905, 33750, 52123 });
		AddPointToLine({ 52300, 35000, 52308 });

		NewLine();
		AddPointToLine({ 52394, 35300, 52351 });
		AddPointToLine({ 52808, 36640, 52542 });
		AddPointToLine({ 53213, 37980, 52726 });
		AddPointToLine({ 53607, 39320, 52904 });
		AddPointToLine({ 53987, 40660, 53074 });
		AddPointToLine({ 54354, 42000, 53236 });

		NewLine();
		AddPointToLine({ 54433, 42300, 53271 });
		AddPointToLine({ 54769, 43600, 53418 });
		AddPointToLine({ 55088, 44900, 53558 });
		AddPointToLine({ 55389, 46200, 53688 });
		AddPointToLine({ 55670, 47500, 53810 });

		NewLine();
		AddPointToLine({ 55732, 47800, 53837 });
		AddPointToLine({ 55995, 49140, 53951 });
		AddPointToLine({ 56234, 50480, 54055 });
		AddPointToLine({ 56449, 51820, 54148 });
		AddPointToLine({ 56638, 53160, 54231 });
		AddPointToLine({ 56801, 54500, 54303 });

		NewLine();
		AddPointToLine({ 56834, 54800, 54317 });
		AddPointToLine({ 56955, 56040, 54372 });
		AddPointToLine({ 57052, 57280, 54417 });
		AddPointToLine({ 57125, 58520, 54452 });
		AddPointToLine({ 57173, 59760, 54478 });
		AddPointToLine({ 57197, 61000, 54494 });

		NewLine();
		AddPointToLine({ 57199, 61300, 54496 });
		AddPointToLine({ 57191, 62640, 54500 });
		AddPointToLine({ 57154, 63980, 54492 });
		AddPointToLine({ 57088, 65320, 54472 });
		AddPointToLine({ 56993, 66660, 54442 });
		AddPointToLine({ 56871, 68000, 54399 });

		NewLine();
		AddPointToLine({ 56840, 68300, 54388 });
		AddPointToLine({ 56697, 69540, 54337 });
		AddPointToLine({ 56531, 70780, 54276 });
		AddPointToLine({ 56343, 72020, 54206 });
		AddPointToLine({ 56133, 73260, 54127 });
		AddPointToLine({ 55903, 74500, 54039 });

		NewLine();
		AddPointToLine({ 55844, 74800, 54016 });
		AddPointToLine({ 55568, 76140, 53908 });
		AddPointToLine({ 55271, 77480, 53791 });
		AddPointToLine({ 54953, 78820, 53663 });
		AddPointToLine({ 54616, 80160, 53527 });
		AddPointToLine({ 54262, 81500, 53381 });

		NewLine();
		AddPointToLine({ 54180, 81800, 53347 });
		AddPointToLine({ 53721, 83440, 53155 });
		AddPointToLine({ 53242, 85080, 52951 });
		AddPointToLine({ 52746, 86720, 52735 });
		AddPointToLine({ 52237, 88360, 52509 });
		AddPointToLine({ 51717, 90000, 52274 });


		// ============================================================
		// ПОВЕРХНОСТЬ 190°
		// ============================================================

		NewLine();
		AddPointToLine({ 50765, 30000, 50796 });
		AddPointToLine({ 51169, 31250, 50993 });
		AddPointToLine({ 51571, 32500, 51187 });
		AddPointToLine({ 51970, 33750, 51376 });
		AddPointToLine({ 52365, 35000, 51561 });

		NewLine();
		AddPointToLine({ 52459, 35300, 51604 });
		AddPointToLine({ 52873, 36640, 51795 });
		AddPointToLine({ 53278, 37980, 51979 });
		AddPointToLine({ 53672, 39320, 52157 });
		AddPointToLine({ 54052, 40660, 52327 });
		AddPointToLine({ 54419, 42000, 52489 });

		NewLine();
		AddPointToLine({ 54498, 42300, 52524 });
		AddPointToLine({ 54834, 43600, 52671 });
		AddPointToLine({ 55153, 44900, 52811 });
		AddPointToLine({ 55454, 46200, 52941 });
		AddPointToLine({ 55735, 47500, 53063 });

		NewLine();
		AddPointToLine({ 55797, 47800, 53090 });
		AddPointToLine({ 56060, 49140, 53204 });
		AddPointToLine({ 56299, 50480, 53308 });
		AddPointToLine({ 56514, 51820, 53401 });
		AddPointToLine({ 56703, 53160, 53484 });
		AddPointToLine({ 56866, 54500, 53556 });

		NewLine();
		AddPointToLine({ 56899, 54800, 53570 });
		AddPointToLine({ 57020, 56040, 53625 });
		AddPointToLine({ 57117, 57280, 53670 });
		AddPointToLine({ 57190, 58520, 53705 });
		AddPointToLine({ 57238, 59760, 53731 });
		AddPointToLine({ 57262, 61000, 53747 });

		NewLine();
		AddPointToLine({ 57264, 61300, 53749 });
		AddPointToLine({ 57256, 62640, 53753 });
		AddPointToLine({ 57219, 63980, 53745 });
		AddPointToLine({ 57153, 65320, 53725 });
		AddPointToLine({ 57058, 66660, 53695 });
		AddPointToLine({ 56936, 68000, 53652 });

		NewLine();
		AddPointToLine({ 56905, 68300, 53641 });
		AddPointToLine({ 56762, 69540, 53590 });
		AddPointToLine({ 56596, 70780, 53529 });
		AddPointToLine({ 56408, 72020, 53459 });
		AddPointToLine({ 56198, 73260, 53380 });
		AddPointToLine({ 55968, 74500, 53292 });

		NewLine();
		AddPointToLine({ 55909, 74800, 53269 });
		AddPointToLine({ 55633, 76140, 53161 });
		AddPointToLine({ 55336, 77480, 53044 });
		AddPointToLine({ 55018, 78820, 52916 });
		AddPointToLine({ 54681, 80160, 52780 });
		AddPointToLine({ 54327, 81500, 52634 });

		NewLine();
		AddPointToLine({ 54245, 81800, 52600 });
		AddPointToLine({ 53786, 83440, 52408 });
		AddPointToLine({ 53307, 85080, 52204 });
		AddPointToLine({ 52811, 86720, 51988 });
		AddPointToLine({ 52302, 88360, 51762 });
		AddPointToLine({ 51782, 90000, 51527 });


		// ============================================================
		// ПОВЕРХНОСТЬ 200°
		// ============================================================

		NewLine();
		AddPointToLine({ 50959, 30000, 50072 });
		AddPointToLine({ 51363, 31250, 50269 });
		AddPointToLine({ 51765, 32500, 50463 });
		AddPointToLine({ 52164, 33750, 50652 });
		AddPointToLine({ 52559, 35000, 50837 });

		NewLine();
		AddPointToLine({ 52653, 35300, 50880 });
		AddPointToLine({ 53067, 36640, 51071 });
		AddPointToLine({ 53472, 37980, 51255 });
		AddPointToLine({ 53866, 39320, 51433 });
		AddPointToLine({ 54246, 40660, 51603 });
		AddPointToLine({ 54613, 42000, 51765 });

		NewLine();
		AddPointToLine({ 54692, 42300, 51800 });
		AddPointToLine({ 55028, 43600, 51947 });
		AddPointToLine({ 55347, 44900, 52087 });
		AddPointToLine({ 55648, 46200, 52217 });
		AddPointToLine({ 55929, 47500, 52339 });

		NewLine();
		AddPointToLine({ 55991, 47800, 52366 });
		AddPointToLine({ 56254, 49140, 52480 });
		AddPointToLine({ 56493, 50480, 52584 });
		AddPointToLine({ 56708, 51820, 52677 });
		AddPointToLine({ 56897, 53160, 52760 });
		AddPointToLine({ 57060, 54500, 52832 });

		NewLine();
		AddPointToLine({ 57093, 54800, 52846 });
		AddPointToLine({ 57214, 56040, 52901 });
		AddPointToLine({ 57311, 57280, 52946 });
		AddPointToLine({ 57384, 58520, 52981 });
		AddPointToLine({ 57432, 59760, 53007 });
		AddPointToLine({ 57456, 61000, 53023 });

		NewLine();
		AddPointToLine({ 57458, 61300, 53025 });
		AddPointToLine({ 57450, 62640, 53029 });
		AddPointToLine({ 57413, 63980, 53021 });
		AddPointToLine({ 57347, 65320, 53001 });
		AddPointToLine({ 57252, 66660, 52971 });
		AddPointToLine({ 57130, 68000, 52928 });

		NewLine();
		AddPointToLine({ 57099, 68300, 52917 });
		AddPointToLine({ 56956, 69540, 52866 });
		AddPointToLine({ 56790, 70780, 52805 });
		AddPointToLine({ 56602, 72020, 52735 });
		AddPointToLine({ 56392, 73260, 52656 });
		AddPointToLine({ 56162, 74500, 52568 });

		NewLine();
		AddPointToLine({ 56103, 74800, 52545 });
		AddPointToLine({ 55827, 76140, 52437 });
		AddPointToLine({ 55530, 77480, 52320 });
		AddPointToLine({ 55212, 78820, 52192 });
		AddPointToLine({ 54875, 80160, 52056 });
		AddPointToLine({ 54521, 81500, 51910 });

		NewLine();
		AddPointToLine({ 54439, 81800, 51876 });
		AddPointToLine({ 53980, 83440, 51684 });
		AddPointToLine({ 53501, 85080, 51480 });
		AddPointToLine({ 53005, 86720, 51264 });
		AddPointToLine({ 52496, 88360, 51038 });
		AddPointToLine({ 51976, 90000, 50803 });


		// ============================================================
		// ПОВЕРХНОСТЬ 210°
		// ============================================================

		NewLine();
		AddPointToLine({ 51276, 30000, 49393 });
		AddPointToLine({ 51680, 31250, 49590 });
		AddPointToLine({ 52082, 32500, 49784 });
		AddPointToLine({ 52481, 33750, 49973 });
		AddPointToLine({ 52876, 35000, 50158 });

		NewLine();
		AddPointToLine({ 52970, 35300, 50201 });
		AddPointToLine({ 53384, 36640, 50392 });
		AddPointToLine({ 53789, 37980, 50576 });
		AddPointToLine({ 54183, 39320, 50754 });
		AddPointToLine({ 54563, 40660, 50924 });
		AddPointToLine({ 54930, 42000, 51086 });

		NewLine();
		AddPointToLine({ 55009, 42300, 51121 });
		AddPointToLine({ 55345, 43600, 51268 });
		AddPointToLine({ 55664, 44900, 51408 });
		AddPointToLine({ 55965, 46200, 51538 });
		AddPointToLine({ 56246, 47500, 51660 });

		NewLine();
		AddPointToLine({ 56308, 47800, 51687 });
		AddPointToLine({ 56571, 49140, 51801 });
		AddPointToLine({ 56810, 50480, 51905 });
		AddPointToLine({ 57025, 51820, 51998 });
		AddPointToLine({ 57214, 53160, 52081 });
		AddPointToLine({ 57377, 54500, 52153 });

		NewLine();
		AddPointToLine({ 57410, 54800, 52167 });
		AddPointToLine({ 57531, 56040, 52222 });
		AddPointToLine({ 57628, 57280, 52267 });
		AddPointToLine({ 57701, 58520, 52302 });
		AddPointToLine({ 57749, 59760, 52328 });
		AddPointToLine({ 57773, 61000, 52344 });

		NewLine();
		AddPointToLine({ 57775, 61300, 52346 });
		AddPointToLine({ 57767, 62640, 52350 });
		AddPointToLine({ 57730, 63980, 52342 });
		AddPointToLine({ 57664, 65320, 52322 });
		AddPointToLine({ 57569, 66660, 52292 });
		AddPointToLine({ 57447, 68000, 52249 });

		NewLine();
		AddPointToLine({ 57416, 68300, 52238 });
		AddPointToLine({ 57273, 69540, 52187 });
		AddPointToLine({ 57107, 70780, 52126 });
		AddPointToLine({ 56919, 72020, 52056 });
		AddPointToLine({ 56709, 73260, 51977 });
		AddPointToLine({ 56479, 74500, 51889 });

		NewLine();
		AddPointToLine({ 56420, 74800, 51866 });
		AddPointToLine({ 56144, 76140, 51758 });
		AddPointToLine({ 55847, 77480, 51641 });
		AddPointToLine({ 55529, 78820, 51513 });
		AddPointToLine({ 55192, 80160, 51377 });
		AddPointToLine({ 54838, 81500, 51231 });

		NewLine();
		AddPointToLine({ 54756, 81800, 51197 });
		AddPointToLine({ 54297, 83440, 51005 });
		AddPointToLine({ 53818, 85080, 50801 });
		AddPointToLine({ 53322, 86720, 50585 });
		AddPointToLine({ 52813, 88360, 50359 });
		AddPointToLine({ 52293, 90000, 50124 });


		// ============================================================
		// ПОВЕРХНОСТЬ 220°
		// ============================================================

		NewLine();
		AddPointToLine({ 51706, 30000, 48779 });
		AddPointToLine({ 52110, 31250, 48976 });
		AddPointToLine({ 52512, 32500, 49170 });
		AddPointToLine({ 52911, 33750, 49359 });
		AddPointToLine({ 53306, 35000, 49544 });

		NewLine();
		AddPointToLine({ 53400, 35300, 49587 });
		AddPointToLine({ 53814, 36640, 49778 });
		AddPointToLine({ 54219, 37980, 49962 });
		AddPointToLine({ 54613, 39320, 50140 });
		AddPointToLine({ 54993, 40660, 50310 });
		AddPointToLine({ 55360, 42000, 50472 });

		NewLine();
		AddPointToLine({ 55439, 42300, 50507 });
		AddPointToLine({ 55775, 43600, 50654 });
		AddPointToLine({ 56094, 44900, 50794 });
		AddPointToLine({ 56395, 46200, 50924 });
		AddPointToLine({ 56676, 47500, 51046 });

		NewLine();
		AddPointToLine({ 56738, 47800, 51073 });
		AddPointToLine({ 57001, 49140, 51187 });
		AddPointToLine({ 57240, 50480, 51291 });
		AddPointToLine({ 57455, 51820, 51384 });
		AddPointToLine({ 57644, 53160, 51467 });
		AddPointToLine({ 57807, 54500, 51539 });

		NewLine();
		AddPointToLine({ 57840, 54800, 51553 });
		AddPointToLine({ 57961, 56040, 51608 });
		AddPointToLine({ 58058, 57280, 51653 });
		AddPointToLine({ 58131, 58520, 51688 });
		AddPointToLine({ 58179, 59760, 51714 });
		AddPointToLine({ 58203, 61000, 51730 });

		NewLine();
		AddPointToLine({ 58205, 61300, 51732 });
		AddPointToLine({ 58197, 62640, 51736 });
		AddPointToLine({ 58160, 63980, 51728 });
		AddPointToLine({ 58094, 65320, 51708 });
		AddPointToLine({ 57999, 66660, 51678 });
		AddPointToLine({ 57877, 68000, 51635 });

		NewLine();
		AddPointToLine({ 57846, 68300, 51624 });
		AddPointToLine({ 57703, 69540, 51573 });
		AddPointToLine({ 57537, 70780, 51512 });
		AddPointToLine({ 57349, 72020, 51442 });
		AddPointToLine({ 57139, 73260, 51363 });
		AddPointToLine({ 56909, 74500, 51275 });

		NewLine();
		AddPointToLine({ 56850, 74800, 51252 });
		AddPointToLine({ 56574, 76140, 51144 });
		AddPointToLine({ 56277, 77480, 51027 });
		AddPointToLine({ 55959, 78820, 50899 });
		AddPointToLine({ 55622, 80160, 50763 });
		AddPointToLine({ 55268, 81500, 50617 });

		NewLine();
		AddPointToLine({ 55186, 81800, 50583 });
		AddPointToLine({ 54727, 83440, 50391 });
		AddPointToLine({ 54248, 85080, 50187 });
		AddPointToLine({ 53752, 86720, 49971 });
		AddPointToLine({ 53243, 88360, 49745 });
		AddPointToLine({ 52723, 90000, 49510 });


		// ============================================================
		// ПОВЕРХНОСТЬ 230°
		// ============================================================

		NewLine();
		AddPointToLine({ 52236, 30000, 48249 });
		AddPointToLine({ 52640, 31250, 48446 });
		AddPointToLine({ 53042, 32500, 48640 });
		AddPointToLine({ 53441, 33750, 48829 });
		AddPointToLine({ 53836, 35000, 49014 });

		NewLine();
		AddPointToLine({ 53930, 35300, 49057 });
		AddPointToLine({ 54344, 36640, 49248 });
		AddPointToLine({ 54749, 37980, 49432 });
		AddPointToLine({ 55143, 39320, 49610 });
		AddPointToLine({ 55523, 40660, 49780 });
		AddPointToLine({ 55890, 42000, 49942 });

		NewLine();
		AddPointToLine({ 55969, 42300, 49977 });
		AddPointToLine({ 56305, 43600, 50124 });
		AddPointToLine({ 56624, 44900, 50264 });
		AddPointToLine({ 56925, 46200, 50394 });
		AddPointToLine({ 57206, 47500, 50516 });

		NewLine();
		AddPointToLine({ 57268, 47800, 50543 });
		AddPointToLine({ 57531, 49140, 50657 });
		AddPointToLine({ 57770, 50480, 50761 });
		AddPointToLine({ 57985, 51820, 50854 });
		AddPointToLine({ 58174, 53160, 50937 });
		AddPointToLine({ 58337, 54500, 51009 });

		NewLine();
		AddPointToLine({ 58370, 54800, 51023 });
		AddPointToLine({ 58491, 56040, 51078 });
		AddPointToLine({ 58588, 57280, 51123 });
		AddPointToLine({ 58661, 58520, 51158 });
		AddPointToLine({ 58709, 59760, 51184 });
		AddPointToLine({ 58733, 61000, 51200 });

		NewLine();
		AddPointToLine({ 58735, 61300, 51202 });
		AddPointToLine({ 58727, 62640, 51206 });
		AddPointToLine({ 58690, 63980, 51198 });
		AddPointToLine({ 58624, 65320, 51178 });
		AddPointToLine({ 58529, 66660, 51148 });
		AddPointToLine({ 58407, 68000, 51105 });

		NewLine();
		AddPointToLine({ 58376, 68300, 51094 });
		AddPointToLine({ 58233, 69540, 51043 });
		AddPointToLine({ 58067, 70780, 50982 });
		AddPointToLine({ 57879, 72020, 50912 });
		AddPointToLine({ 57669, 73260, 50833 });
		AddPointToLine({ 57439, 74500, 50745 });

		NewLine();
		AddPointToLine({ 57380, 74800, 50722 });
		AddPointToLine({ 57104, 76140, 50614 });
		AddPointToLine({ 56807, 77480, 50497 });
		AddPointToLine({ 56489, 78820, 50369 });
		AddPointToLine({ 56152, 80160, 50233 });
		AddPointToLine({ 55798, 81500, 50087 });

		NewLine();
		AddPointToLine({ 55716, 81800, 50053 });
		AddPointToLine({ 55257, 83440, 49861 });
		AddPointToLine({ 54778, 85080, 49657 });
		AddPointToLine({ 54282, 86720, 49441 });
		AddPointToLine({ 53773, 88360, 49215 });
		AddPointToLine({ 53253, 90000, 48980 });


		// ============================================================
		// ПОВЕРХНОСТЬ 240°
		// ============================================================

		NewLine();
		AddPointToLine({ 52850, 30000, 47819 });
		AddPointToLine({ 53254, 31250, 48016 });
		AddPointToLine({ 53656, 32500, 48210 });
		AddPointToLine({ 54055, 33750, 48399 });
		AddPointToLine({ 54450, 35000, 48584 });

		NewLine();
		AddPointToLine({ 54544, 35300, 48627 });
		AddPointToLine({ 54958, 36640, 48818 });
		AddPointToLine({ 55363, 37980, 49002 });
		AddPointToLine({ 55757, 39320, 49180 });
		AddPointToLine({ 56137, 40660, 49350 });
		AddPointToLine({ 56504, 42000, 49512 });

		NewLine();
		AddPointToLine({ 56583, 42300, 49547 });
		AddPointToLine({ 56919, 43600, 49694 });
		AddPointToLine({ 57238, 44900, 49834 });
		AddPointToLine({ 57539, 46200, 49964 });
		AddPointToLine({ 57820, 47500, 50086 });

		NewLine();
		AddPointToLine({ 57882, 47800, 50113 });
		AddPointToLine({ 58145, 49140, 50227 });
		AddPointToLine({ 58384, 50480, 50331 });
		AddPointToLine({ 58599, 51820, 50424 });
		AddPointToLine({ 58788, 53160, 50507 });
		AddPointToLine({ 58951, 54500, 50579 });

		NewLine();
		AddPointToLine({ 58984, 54800, 50593 });
		AddPointToLine({ 59105, 56040, 50648 });
		AddPointToLine({ 59202, 57280, 50693 });
		AddPointToLine({ 59275, 58520, 50728 });
		AddPointToLine({ 59323, 59760, 50754 });
		AddPointToLine({ 59347, 61000, 50770 });

		NewLine();
		AddPointToLine({ 59349, 61300, 50772 });
		AddPointToLine({ 59341, 62640, 50776 });
		AddPointToLine({ 59304, 63980, 50768 });
		AddPointToLine({ 59238, 65320, 50748 });
		AddPointToLine({ 59143, 66660, 50718 });
		AddPointToLine({ 59021, 68000, 50675 });

		NewLine();
		AddPointToLine({ 58990, 68300, 50664 });
		AddPointToLine({ 58847, 69540, 50613 });
		AddPointToLine({ 58681, 70780, 50552 });
		AddPointToLine({ 58493, 72020, 50482 });
		AddPointToLine({ 58283, 73260, 50403 });
		AddPointToLine({ 58053, 74500, 50315 });

		NewLine();
		AddPointToLine({ 57994, 74800, 50292 });
		AddPointToLine({ 57718, 76140, 50184 });
		AddPointToLine({ 57421, 77480, 50067 });
		AddPointToLine({ 57103, 78820, 49939 });
		AddPointToLine({ 56766, 80160, 49803 });
		AddPointToLine({ 56412, 81500, 49657 });

		NewLine();
		AddPointToLine({ 56330, 81800, 49623 });
		AddPointToLine({ 55871, 83440, 49431 });
		AddPointToLine({ 55392, 85080, 49227 });
		AddPointToLine({ 54896, 86720, 49011 });
		AddPointToLine({ 54387, 88360, 48785 });
		AddPointToLine({ 53867, 90000, 48550 });


		// ============================================================
		// ПОВЕРХНОСТЬ 250°
		// ============================================================

		NewLine();
		AddPointToLine({ 53529, 30000, 47502 });
		AddPointToLine({ 53933, 31250, 47699 });
		AddPointToLine({ 54335, 32500, 47893 });
		AddPointToLine({ 54734, 33750, 48082 });
		AddPointToLine({ 55129, 35000, 48267 });

		NewLine();
		AddPointToLine({ 55223, 35300, 48310 });
		AddPointToLine({ 55637, 36640, 48501 });
		AddPointToLine({ 56042, 37980, 48685 });
		AddPointToLine({ 56436, 39320, 48863 });
		AddPointToLine({ 56816, 40660, 49033 });
		AddPointToLine({ 57183, 42000, 49195 });

		NewLine();
		AddPointToLine({ 57262, 42300, 49230 });
		AddPointToLine({ 57598, 43600, 49377 });
		AddPointToLine({ 57917, 44900, 49517 });
		AddPointToLine({ 58218, 46200, 49647 });
		AddPointToLine({ 58499, 47500, 49769 });

		NewLine();
		AddPointToLine({ 58561, 47800, 49796 });
		AddPointToLine({ 58824, 49140, 49910 });
		AddPointToLine({ 59063, 50480, 50014 });
		AddPointToLine({ 59278, 51820, 50107 });
		AddPointToLine({ 59467, 53160, 50190 });
		AddPointToLine({ 59630, 54500, 50262 });

		NewLine();
		AddPointToLine({ 59663, 54800, 50276 });
		AddPointToLine({ 59784, 56040, 50331 });
		AddPointToLine({ 59881, 57280, 50376 });
		AddPointToLine({ 59954, 58520, 50411 });
		AddPointToLine({ 60002, 59760, 50437 });
		AddPointToLine({ 60026, 61000, 50453 });

		NewLine();
		AddPointToLine({ 60028, 61300, 50455 });
		AddPointToLine({ 60020, 62640, 50459 });
		AddPointToLine({ 59983, 63980, 50451 });
		AddPointToLine({ 59917, 65320, 50431 });
		AddPointToLine({ 59822, 66660, 50401 });
		AddPointToLine({ 59700, 68000, 50358 });

		NewLine();
		AddPointToLine({ 59669, 68300, 50347 });
		AddPointToLine({ 59526, 69540, 50296 });
		AddPointToLine({ 59360, 70780, 50235 });
		AddPointToLine({ 59172, 72020, 50165 });
		AddPointToLine({ 58962, 73260, 50086 });
		AddPointToLine({ 58732, 74500, 49998 });

		NewLine();
		AddPointToLine({ 58673, 74800, 49975 });
		AddPointToLine({ 58397, 76140, 49867 });
		AddPointToLine({ 58100, 77480, 49750 });
		AddPointToLine({ 57782, 78820, 49622 });
		AddPointToLine({ 57445, 80160, 49486 });
		AddPointToLine({ 57091, 81500, 49340 });

		NewLine();
		AddPointToLine({ 57009, 81800, 49306 });
		AddPointToLine({ 56550, 83440, 49114 });
		AddPointToLine({ 56071, 85080, 48910 });
		AddPointToLine({ 55575, 86720, 48694 });
		AddPointToLine({ 55066, 88360, 48468 });
		AddPointToLine({ 54546, 90000, 48233 });


		// ============================================================
		// ПОВЕРХНОСТЬ 260°
		// ============================================================

		NewLine();
		AddPointToLine({ 54253, 30000, 47308 });
		AddPointToLine({ 54657, 31250, 47505 });
		AddPointToLine({ 55059, 32500, 47699 });
		AddPointToLine({ 55458, 33750, 47888 });
		AddPointToLine({ 55853, 35000, 48073 });

		NewLine();
		AddPointToLine({ 55947, 35300, 48116 });
		AddPointToLine({ 56361, 36640, 48307 });
		AddPointToLine({ 56766, 37980, 48491 });
		AddPointToLine({ 57160, 39320, 48669 });
		AddPointToLine({ 57540, 40660, 48839 });
		AddPointToLine({ 57907, 42000, 49001 });

		NewLine();
		AddPointToLine({ 57986, 42300, 49036 });
		AddPointToLine({ 58322, 43600, 49183 });
		AddPointToLine({ 58641, 44900, 49323 });
		AddPointToLine({ 58942, 46200, 49453 });
		AddPointToLine({ 59223, 47500, 49575 });

		NewLine();
		AddPointToLine({ 59285, 47800, 49602 });
		AddPointToLine({ 59548, 49140, 49716 });
		AddPointToLine({ 59787, 50480, 49820 });
		AddPointToLine({ 60002, 51820, 49913 });
		AddPointToLine({ 60191, 53160, 49996 });
		AddPointToLine({ 60354, 54500, 50068 });

		NewLine();
		AddPointToLine({ 60387, 54800, 50082 });
		AddPointToLine({ 60508, 56040, 50137 });
		AddPointToLine({ 60605, 57280, 50182 });
		AddPointToLine({ 60678, 58520, 50217 });
		AddPointToLine({ 60726, 59760, 50243 });
		AddPointToLine({ 60750, 61000, 50259 });

		NewLine();
		AddPointToLine({ 60752, 61300, 50261 });
		AddPointToLine({ 60744, 62640, 50265 });
		AddPointToLine({ 60707, 63980, 50257 });
		AddPointToLine({ 60641, 65320, 50237 });
		AddPointToLine({ 60546, 66660, 50207 });
		AddPointToLine({ 60424, 68000, 50164 });

		NewLine();
		AddPointToLine({ 60393, 68300, 50153 });
		AddPointToLine({ 60250, 69540, 50102 });
		AddPointToLine({ 60084, 70780, 50041 });
		AddPointToLine({ 59896, 72020, 49971 });
		AddPointToLine({ 59686, 73260, 49892 });
		AddPointToLine({ 59456, 74500, 49804 });

		NewLine();
		AddPointToLine({ 59397, 74800, 49781 });
		AddPointToLine({ 59121, 76140, 49673 });
		AddPointToLine({ 58824, 77480, 49556 });
		AddPointToLine({ 58506, 78820, 49428 });
		AddPointToLine({ 58169, 80160, 49292 });
		AddPointToLine({ 57815, 81500, 49146 });

		NewLine();
		AddPointToLine({ 57733, 81800, 49112 });
		AddPointToLine({ 57274, 83440, 48920 });
		AddPointToLine({ 56795, 85080, 48716 });
		AddPointToLine({ 56299, 86720, 48500 });
		AddPointToLine({ 55790, 88360, 48274 });
		AddPointToLine({ 55270, 90000, 48039 });


		// ============================================================
		// ПОВЕРХНОСТЬ 270°
		// ============================================================

		NewLine();
		AddPointToLine({ 55000, 30000, 47243 });
		AddPointToLine({ 55404, 31250, 47440 });
		AddPointToLine({ 55806, 32500, 47634 });
		AddPointToLine({ 56205, 33750, 47823 });
		AddPointToLine({ 56600, 35000, 48008 });

		NewLine();
		AddPointToLine({ 56694, 35300, 48051 });
		AddPointToLine({ 57108, 36640, 48242 });
		AddPointToLine({ 57513, 37980, 48426 });
		AddPointToLine({ 57907, 39320, 48604 });
		AddPointToLine({ 58287, 40660, 48774 });
		AddPointToLine({ 58654, 42000, 48936 });

		NewLine();
		AddPointToLine({ 58733, 42300, 48971 });
		AddPointToLine({ 59069, 43600, 49118 });
		AddPointToLine({ 59388, 44900, 49258 });
		AddPointToLine({ 59689, 46200, 49388 });
		AddPointToLine({ 59970, 47500, 49510 });

		NewLine();
		AddPointToLine({ 60032, 47800, 49537 });
		AddPointToLine({ 60295, 49140, 49651 });
		AddPointToLine({ 60534, 50480, 49755 });
		AddPointToLine({ 60749, 51820, 49848 });
		AddPointToLine({ 60938, 53160, 49931 });
		AddPointToLine({ 61101, 54500, 50003 });

		NewLine();
		AddPointToLine({ 61134, 54800, 50017 });
		AddPointToLine({ 61255, 56040, 50072 });
		AddPointToLine({ 61352, 57280, 50117 });
		AddPointToLine({ 61425, 58520, 50152 });
		AddPointToLine({ 61473, 59760, 50178 });
		AddPointToLine({ 61497, 61000, 50194 });

		NewLine();
		AddPointToLine({ 61499, 61300, 50196 });
		AddPointToLine({ 61491, 62640, 50200 });
		AddPointToLine({ 61454, 63980, 50192 });
		AddPointToLine({ 61388, 65320, 50172 });
		AddPointToLine({ 61293, 66660, 50142 });
		AddPointToLine({ 61171, 68000, 50099 });

		NewLine();
		AddPointToLine({ 61140, 68300, 50088 });
		AddPointToLine({ 60997, 69540, 50037 });
		AddPointToLine({ 60831, 70780, 49976 });
		AddPointToLine({ 60643, 72020, 49906 });
		AddPointToLine({ 60433, 73260, 49827 });
		AddPointToLine({ 60203, 74500, 49739 });

		NewLine();
		AddPointToLine({ 60144, 74800, 49716 });
		AddPointToLine({ 59868, 76140, 49608 });
		AddPointToLine({ 59571, 77480, 49491 });
		AddPointToLine({ 59253, 78820, 49363 });
		AddPointToLine({ 58916, 80160, 49227 });
		AddPointToLine({ 58562, 81500, 49081 });

		NewLine();
		AddPointToLine({ 58480, 81800, 49047 });
		AddPointToLine({ 58021, 83440, 48855 });
		AddPointToLine({ 57542, 85080, 48651 });
		AddPointToLine({ 57046, 86720, 48435 });
		AddPointToLine({ 56537, 88360, 48209 });
		AddPointToLine({ 56017, 90000, 47974 });


		// ============================================================
		// ПОВЕРХНОСТЬ 280°
		// ============================================================

		NewLine();
		AddPointToLine({ 55747, 30000, 47308 });
		AddPointToLine({ 56151, 31250, 47505 });
		AddPointToLine({ 56553, 32500, 47699 });
		AddPointToLine({ 56952, 33750, 47888 });
		AddPointToLine({ 57347, 35000, 48073 });

		NewLine();
		AddPointToLine({ 57441, 35300, 48116 });
		AddPointToLine({ 57855, 36640, 48307 });
		AddPointToLine({ 58260, 37980, 48491 });
		AddPointToLine({ 58654, 39320, 48669 });
		AddPointToLine({ 59034, 40660, 48839 });
		AddPointToLine({ 59401, 42000, 49001 });

		NewLine();
		AddPointToLine({ 59480, 42300, 49036 });
		AddPointToLine({ 59816, 43600, 49183 });
		AddPointToLine({ 60135, 44900, 49323 });
		AddPointToLine({ 60436, 46200, 49453 });
		AddPointToLine({ 60717, 47500, 49575 });

		NewLine();
		AddPointToLine({ 60779, 47800, 49602 });
		AddPointToLine({ 61042, 49140, 49716 });
		AddPointToLine({ 61281, 50480, 49820 });
		AddPointToLine({ 61496, 51820, 49913 });
		AddPointToLine({ 61685, 53160, 49996 });
		AddPointToLine({ 61848, 54500, 50068 });

		NewLine();
		AddPointToLine({ 61881, 54800, 50082 });
		AddPointToLine({ 62002, 56040, 50137 });
		AddPointToLine({ 62099, 57280, 50182 });
		AddPointToLine({ 62172, 58520, 50217 });
		AddPointToLine({ 62220, 59760, 50243 });
		AddPointToLine({ 62244, 61000, 50259 });

		NewLine();
		AddPointToLine({ 62246, 61300, 50261 });
		AddPointToLine({ 62238, 62640, 50265 });
		AddPointToLine({ 62201, 63980, 50257 });
		AddPointToLine({ 62135, 65320, 50237 });
		AddPointToLine({ 62040, 66660, 50207 });
		AddPointToLine({ 61918, 68000, 50164 });

		NewLine();
		AddPointToLine({ 61887, 68300, 50153 });
		AddPointToLine({ 61744, 69540, 50102 });
		AddPointToLine({ 61578, 70780, 50041 });
		AddPointToLine({ 61390, 72020, 49971 });
		AddPointToLine({ 61180, 73260, 49892 });
		AddPointToLine({ 60950, 74500, 49804 });

		NewLine();
		AddPointToLine({ 60891, 74800, 49781 });
		AddPointToLine({ 60615, 76140, 49673 });
		AddPointToLine({ 60318, 77480, 49556 });
		AddPointToLine({ 60000, 78820, 49428 });
		AddPointToLine({ 59663, 80160, 49292 });
		AddPointToLine({ 59309, 81500, 49146 });

		NewLine();
		AddPointToLine({ 59227, 81800, 49112 });
		AddPointToLine({ 58768, 83440, 48920 });
		AddPointToLine({ 58289, 85080, 48716 });
		AddPointToLine({ 57793, 86720, 48500 });
		AddPointToLine({ 57284, 88360, 48274 });
		AddPointToLine({ 56764, 90000, 48039 });


		// ============================================================
		// ПОВЕРХНОСТЬ 290°
		// ============================================================

		NewLine();
		AddPointToLine({ 56471, 30000, 47502 });
		AddPointToLine({ 56875, 31250, 47699 });
		AddPointToLine({ 57277, 32500, 47893 });
		AddPointToLine({ 57676, 33750, 48082 });
		AddPointToLine({ 58071, 35000, 48267 });

		NewLine();
		AddPointToLine({ 58165, 35300, 48310 });
		AddPointToLine({ 58579, 36640, 48501 });
		AddPointToLine({ 58984, 37980, 48685 });
		AddPointToLine({ 59378, 39320, 48863 });
		AddPointToLine({ 59758, 40660, 49033 });
		AddPointToLine({ 60125, 42000, 49195 });

		NewLine();
		AddPointToLine({ 60204, 42300, 49230 });
		AddPointToLine({ 60540, 43600, 49377 });
		AddPointToLine({ 60859, 44900, 49517 });
		AddPointToLine({ 61160, 46200, 49647 });
		AddPointToLine({ 61441, 47500, 49769 });

		NewLine();
		AddPointToLine({ 61503, 47800, 49796 });
		AddPointToLine({ 61766, 49140, 49910 });
		AddPointToLine({ 62005, 50480, 50014 });
		AddPointToLine({ 62220, 51820, 50107 });
		AddPointToLine({ 62409, 53160, 50190 });
		AddPointToLine({ 62572, 54500, 50262 });

		NewLine();
		AddPointToLine({ 62605, 54800, 50276 });
		AddPointToLine({ 62726, 56040, 50331 });
		AddPointToLine({ 62823, 57280, 50376 });
		AddPointToLine({ 62896, 58520, 50411 });
		AddPointToLine({ 62944, 59760, 50437 });
		AddPointToLine({ 62968, 61000, 50453 });

		NewLine();
		AddPointToLine({ 62970, 61300, 50455 });
		AddPointToLine({ 62962, 62640, 50459 });
		AddPointToLine({ 62925, 63980, 50451 });
		AddPointToLine({ 62859, 65320, 50431 });
		AddPointToLine({ 62764, 66660, 50401 });
		AddPointToLine({ 62642, 68000, 50358 });

		NewLine();
		AddPointToLine({ 62611, 68300, 50347 });
		AddPointToLine({ 62468, 69540, 50296 });
		AddPointToLine({ 62302, 70780, 50235 });
		AddPointToLine({ 62114, 72020, 50165 });
		AddPointToLine({ 61904, 73260, 50086 });
		AddPointToLine({ 61674, 74500, 49998 });

		NewLine();
		AddPointToLine({ 61615, 74800, 49975 });
		AddPointToLine({ 61339, 76140, 49867 });
		AddPointToLine({ 61042, 77480, 49750 });
		AddPointToLine({ 60724, 78820, 49622 });
		AddPointToLine({ 60387, 80160, 49486 });
		AddPointToLine({ 60033, 81500, 49340 });

		NewLine();
		AddPointToLine({ 59951, 81800, 49306 });
		AddPointToLine({ 59492, 83440, 49114 });
		AddPointToLine({ 59013, 85080, 48910 });
		AddPointToLine({ 58517, 86720, 48694 });
		AddPointToLine({ 58008, 88360, 48468 });
		AddPointToLine({ 57488, 90000, 48233 });


		// ============================================================
		// ПОВЕРХНОСТЬ 300°
		// ============================================================

		NewLine();
		AddPointToLine({ 57150, 30000, 47819 });
		AddPointToLine({ 57554, 31250, 48016 });
		AddPointToLine({ 57956, 32500, 48210 });
		AddPointToLine({ 58355, 33750, 48399 });
		AddPointToLine({ 58750, 35000, 48584 });

		NewLine();
		AddPointToLine({ 58844, 35300, 48627 });
		AddPointToLine({ 59258, 36640, 48818 });
		AddPointToLine({ 59663, 37980, 49002 });
		AddPointToLine({ 60057, 39320, 49180 });
		AddPointToLine({ 60437, 40660, 49350 });
		AddPointToLine({ 60804, 42000, 49512 });

		NewLine();
		AddPointToLine({ 60883, 42300, 49547 });
		AddPointToLine({ 61219, 43600, 49694 });
		AddPointToLine({ 61538, 44900, 49834 });
		AddPointToLine({ 61839, 46200, 49964 });
		AddPointToLine({ 62120, 47500, 50086 });

		NewLine();
		AddPointToLine({ 62182, 47800, 50113 });
		AddPointToLine({ 62445, 49140, 50227 });
		AddPointToLine({ 62684, 50480, 50331 });
		AddPointToLine({ 62899, 51820, 50424 });
		AddPointToLine({ 63088, 53160, 50507 });
		AddPointToLine({ 63251, 54500, 50579 });

		NewLine();
		AddPointToLine({ 63284, 54800, 50593 });
		AddPointToLine({ 63405, 56040, 50648 });
		AddPointToLine({ 63502, 57280, 50693 });
		AddPointToLine({ 63575, 58520, 50728 });
		AddPointToLine({ 63623, 59760, 50754 });
		AddPointToLine({ 63647, 61000, 50770 });

		NewLine();
		AddPointToLine({ 63649, 61300, 50772 });
		AddPointToLine({ 63641, 62640, 50776 });
		AddPointToLine({ 63604, 63980, 50768 });
		AddPointToLine({ 63538, 65320, 50748 });
		AddPointToLine({ 63443, 66660, 50718 });
		AddPointToLine({ 63321, 68000, 50675 });

		NewLine();
		AddPointToLine({ 63290, 68300, 50664 });
		AddPointToLine({ 63147, 69540, 50613 });
		AddPointToLine({ 62981, 70780, 50552 });
		AddPointToLine({ 62793, 72020, 50482 });
		AddPointToLine({ 62583, 73260, 50403 });
		AddPointToLine({ 62353, 74500, 50315 });

		NewLine();
		AddPointToLine({ 62294, 74800, 50292 });
		AddPointToLine({ 62018, 76140, 50184 });
		AddPointToLine({ 61721, 77480, 50067 });
		AddPointToLine({ 61403, 78820, 49939 });
		AddPointToLine({ 61066, 80160, 49803 });
		AddPointToLine({ 60712, 81500, 49657 });

		NewLine();
		AddPointToLine({ 60630, 81800, 49623 });
		AddPointToLine({ 60171, 83440, 49431 });
		AddPointToLine({ 59692, 85080, 49227 });
		AddPointToLine({ 59196, 86720, 49011 });
		AddPointToLine({ 58687, 88360, 48785 });
		AddPointToLine({ 58167, 90000, 48550 });


		// ============================================================
		// ПОВЕРХНОСТЬ 310°
		// ============================================================

		NewLine();
		AddPointToLine({ 57764, 30000, 48249 });
		AddPointToLine({ 58168, 31250, 48446 });
		AddPointToLine({ 58570, 32500, 48640 });
		AddPointToLine({ 58969, 33750, 48829 });
		AddPointToLine({ 59364, 35000, 49014 });

		NewLine();
		AddPointToLine({ 59458, 35300, 49057 });
		AddPointToLine({ 59872, 36640, 49248 });
		AddPointToLine({ 60277, 37980, 49432 });
		AddPointToLine({ 60671, 39320, 49610 });
		AddPointToLine({ 61051, 40660, 49780 });
		AddPointToLine({ 61418, 42000, 49942 });

		NewLine();
		AddPointToLine({ 61497, 42300, 49977 });
		AddPointToLine({ 61833, 43600, 50124 });
		AddPointToLine({ 62152, 44900, 50264 });
		AddPointToLine({ 62453, 46200, 50394 });
		AddPointToLine({ 62734, 47500, 50516 });

		NewLine();
		AddPointToLine({ 62796, 47800, 50543 });
		AddPointToLine({ 63059, 49140, 50657 });
		AddPointToLine({ 63298, 50480, 50761 });
		AddPointToLine({ 63513, 51820, 50854 });
		AddPointToLine({ 63702, 53160, 50937 });
		AddPointToLine({ 63865, 54500, 51009 });

		NewLine();
		AddPointToLine({ 63898, 54800, 51023 });
		AddPointToLine({ 64019, 56040, 51078 });
		AddPointToLine({ 64116, 57280, 51123 });
		AddPointToLine({ 64189, 58520, 51158 });
		AddPointToLine({ 64237, 59760, 51184 });
		AddPointToLine({ 64261, 61000, 51200 });

		NewLine();
		AddPointToLine({ 64263, 61300, 51202 });
		AddPointToLine({ 64255, 62640, 51206 });
		AddPointToLine({ 64218, 63980, 51198 });
		AddPointToLine({ 64152, 65320, 51178 });
		AddPointToLine({ 64057, 66660, 51148 });
		AddPointToLine({ 63935, 68000, 51105 });

		NewLine();
		AddPointToLine({ 63904, 68300, 51094 });
		AddPointToLine({ 63761, 69540, 51043 });
		AddPointToLine({ 63595, 70780, 50982 });
		AddPointToLine({ 63407, 72020, 50912 });
		AddPointToLine({ 63197, 73260, 50833 });
		AddPointToLine({ 62967, 74500, 50745 });

		NewLine();
		AddPointToLine({ 62908, 74800, 50722 });
		AddPointToLine({ 62632, 76140, 50614 });
		AddPointToLine({ 62335, 77480, 50497 });
		AddPointToLine({ 62017, 78820, 50369 });
		AddPointToLine({ 61680, 80160, 50233 });
		AddPointToLine({ 61326, 81500, 50087 });

		NewLine();
		AddPointToLine({ 61244, 81800, 50053 });
		AddPointToLine({ 60785, 83440, 49861 });
		AddPointToLine({ 60306, 85080, 49657 });
		AddPointToLine({ 59810, 86720, 49441 });
		AddPointToLine({ 59301, 88360, 49215 });
		AddPointToLine({ 58781, 90000, 48980 });


		// ============================================================
		// ПОВЕРХНОСТЬ 320°
		// ============================================================

		NewLine();
		AddPointToLine({ 58294, 30000, 48779 });
		AddPointToLine({ 58698, 31250, 48976 });
		AddPointToLine({ 59100, 32500, 49170 });
		AddPointToLine({ 59499, 33750, 49359 });
		AddPointToLine({ 59894, 35000, 49544 });

		NewLine();
		AddPointToLine({ 59988, 35300, 49587 });
		AddPointToLine({ 60402, 36640, 49778 });
		AddPointToLine({ 60807, 37980, 49962 });
		AddPointToLine({ 61201, 39320, 50140 });
		AddPointToLine({ 61581, 40660, 50310 });
		AddPointToLine({ 61948, 42000, 50472 });

		NewLine();
		AddPointToLine({ 62027, 42300, 50507 });
		AddPointToLine({ 62363, 43600, 50654 });
		AddPointToLine({ 62682, 44900, 50794 });
		AddPointToLine({ 62983, 46200, 50924 });
		AddPointToLine({ 63264, 47500, 51046 });

		NewLine();
		AddPointToLine({ 63326, 47800, 51073 });
		AddPointToLine({ 63589, 49140, 51187 });
		AddPointToLine({ 63828, 50480, 51291 });
		AddPointToLine({ 64043, 51820, 51384 });
		AddPointToLine({ 64232, 53160, 51467 });
		AddPointToLine({ 64395, 54500, 51539 });

		NewLine();
		AddPointToLine({ 64428, 54800, 51553 });
		AddPointToLine({ 64549, 56040, 51608 });
		AddPointToLine({ 64646, 57280, 51653 });
		AddPointToLine({ 64719, 58520, 51688 });
		AddPointToLine({ 64767, 59760, 51714 });
		AddPointToLine({ 64791, 61000, 51730 });

		NewLine();
		AddPointToLine({ 64793, 61300, 51732 });
		AddPointToLine({ 64785, 62640, 51736 });
		AddPointToLine({ 64748, 63980, 51728 });
		AddPointToLine({ 64682, 65320, 51708 });
		AddPointToLine({ 64587, 66660, 51678 });
		AddPointToLine({ 64465, 68000, 51635 });

		NewLine();
		AddPointToLine({ 64434, 68300, 51624 });
		AddPointToLine({ 64291, 69540, 51573 });
		AddPointToLine({ 64125, 70780, 51512 });
		AddPointToLine({ 63937, 72020, 51442 });
		AddPointToLine({ 63727, 73260, 51363 });
		AddPointToLine({ 63497, 74500, 51275 });

		NewLine();
		AddPointToLine({ 63438, 74800, 51252 });
		AddPointToLine({ 63162, 76140, 51144 });
		AddPointToLine({ 62865, 77480, 51027 });
		AddPointToLine({ 62547, 78820, 50899 });
		AddPointToLine({ 62210, 80160, 50763 });
		AddPointToLine({ 61856, 81500, 50617 });

		NewLine();
		AddPointToLine({ 61774, 81800, 50583 });
		AddPointToLine({ 61315, 83440, 50391 });
		AddPointToLine({ 60836, 85080, 50187 });
		AddPointToLine({ 60340, 86720, 49971 });
		AddPointToLine({ 59831, 88360, 49745 });
		AddPointToLine({ 59311, 90000, 49510 });


		// ============================================================
		// ПОВЕРХНОСТЬ 330°
		// ============================================================

		NewLine();
		AddPointToLine({ 58724, 30000, 49393 });
		AddPointToLine({ 59128, 31250, 49590 });
		AddPointToLine({ 59530, 32500, 49784 });
		AddPointToLine({ 59929, 33750, 49973 });
		AddPointToLine({ 60324, 35000, 50158 });

		NewLine();
		AddPointToLine({ 60418, 35300, 50201 });
		AddPointToLine({ 60832, 36640, 50392 });
		AddPointToLine({ 61237, 37980, 50576 });
		AddPointToLine({ 61631, 39320, 50754 });
		AddPointToLine({ 62011, 40660, 50924 });
		AddPointToLine({ 62378, 42000, 51086 });

		NewLine();
		AddPointToLine({ 62457, 42300, 51121 });
		AddPointToLine({ 62793, 43600, 51268 });
		AddPointToLine({ 63112, 44900, 51408 });
		AddPointToLine({ 63413, 46200, 51538 });
		AddPointToLine({ 63694, 47500, 51660 });

		NewLine();
		AddPointToLine({ 63756, 47800, 51687 });
		AddPointToLine({ 64019, 49140, 51801 });
		AddPointToLine({ 64258, 50480, 51905 });
		AddPointToLine({ 64473, 51820, 51998 });
		AddPointToLine({ 64662, 53160, 52081 });
		AddPointToLine({ 64825, 54500, 52153 });

		NewLine();
		AddPointToLine({ 64858, 54800, 52167 });
		AddPointToLine({ 64979, 56040, 52222 });
		AddPointToLine({ 65076, 57280, 52267 });
		AddPointToLine({ 65149, 58520, 52302 });
		AddPointToLine({ 65197, 59760, 52328 });
		AddPointToLine({ 65221, 61000, 52344 });

		NewLine();
		AddPointToLine({ 65223, 61300, 52346 });
		AddPointToLine({ 65215, 62640, 52350 });
		AddPointToLine({ 65178, 63980, 52342 });
		AddPointToLine({ 65112, 65320, 52322 });
		AddPointToLine({ 65017, 66660, 52292 });
		AddPointToLine({ 64895, 68000, 52249 });

		NewLine();
		AddPointToLine({ 64864, 68300, 52238 });
		AddPointToLine({ 64721, 69540, 52187 });
		AddPointToLine({ 64555, 70780, 52126 });
		AddPointToLine({ 64367, 72020, 52056 });
		AddPointToLine({ 64157, 73260, 51977 });
		AddPointToLine({ 63927, 74500, 51889 });

		NewLine();
		AddPointToLine({ 63868, 74800, 51866 });
		AddPointToLine({ 63592, 76140, 51758 });
		AddPointToLine({ 63295, 77480, 51641 });
		AddPointToLine({ 62977, 78820, 51513 });
		AddPointToLine({ 62640, 80160, 51377 });
		AddPointToLine({ 62286, 81500, 51231 });

		NewLine();
		AddPointToLine({ 62204, 81800, 51197 });
		AddPointToLine({ 61745, 83440, 51005 });
		AddPointToLine({ 61266, 85080, 50801 });
		AddPointToLine({ 60770, 86720, 50585 });
		AddPointToLine({ 60261, 88360, 50359 });
		AddPointToLine({ 59741, 90000, 50124 });


		// ============================================================
		// ПОВЕРХНОСТЬ 340°
		// ============================================================

		NewLine();
		AddPointToLine({ 59041, 30000, 50072 });
		AddPointToLine({ 59445, 31250, 50269 });
		AddPointToLine({ 59847, 32500, 50463 });
		AddPointToLine({ 60246, 33750, 50652 });
		AddPointToLine({ 60641, 35000, 50837 });

		NewLine();
		AddPointToLine({ 60735, 35300, 50880 });
		AddPointToLine({ 61149, 36640, 51071 });
		AddPointToLine({ 61554, 37980, 51255 });
		AddPointToLine({ 61948, 39320, 51433 });
		AddPointToLine({ 62328, 40660, 51603 });
		AddPointToLine({ 62695, 42000, 51765 });

		NewLine();
		AddPointToLine({ 62774, 42300, 51800 });
		AddPointToLine({ 63110, 43600, 51947 });
		AddPointToLine({ 63429, 44900, 52087 });
		AddPointToLine({ 63730, 46200, 52217 });
		AddPointToLine({ 64011, 47500, 52339 });

		NewLine();
		AddPointToLine({ 64073, 47800, 52366 });
		AddPointToLine({ 64336, 49140, 52480 });
		AddPointToLine({ 64575, 50480, 52584 });
		AddPointToLine({ 64790, 51820, 52677 });
		AddPointToLine({ 64979, 53160, 52760 });
		AddPointToLine({ 65142, 54500, 52832 });

		NewLine();
		AddPointToLine({ 65175, 54800, 52846 });
		AddPointToLine({ 65296, 56040, 52901 });
		AddPointToLine({ 65393, 57280, 52946 });
		AddPointToLine({ 65466, 58520, 52981 });
		AddPointToLine({ 65514, 59760, 53007 });
		AddPointToLine({ 65538, 61000, 53023 });

		NewLine();
		AddPointToLine({ 65540, 61300, 53025 });
		AddPointToLine({ 65532, 62640, 53029 });
		AddPointToLine({ 65495, 63980, 53021 });
		AddPointToLine({ 65429, 65320, 53001 });
		AddPointToLine({ 65334, 66660, 52971 });
		AddPointToLine({ 65212, 68000, 52928 });

		NewLine();
		AddPointToLine({ 65181, 68300, 52917 });
		AddPointToLine({ 65038, 69540, 52866 });
		AddPointToLine({ 64872, 70780, 52805 });
		AddPointToLine({ 64684, 72020, 52735 });
		AddPointToLine({ 64474, 73260, 52656 });
		AddPointToLine({ 64244, 74500, 52568 });

		NewLine();
		AddPointToLine({ 64185, 74800, 52545 });
		AddPointToLine({ 63909, 76140, 52437 });
		AddPointToLine({ 63612, 77480, 52320 });
		AddPointToLine({ 63294, 78820, 52192 });
		AddPointToLine({ 62957, 80160, 52056 });
		AddPointToLine({ 62603, 81500, 51910 });

		NewLine();
		AddPointToLine({ 62521, 81800, 51876 });
		AddPointToLine({ 62062, 83440, 51684 });
		AddPointToLine({ 61583, 85080, 51480 });
		AddPointToLine({ 61087, 86720, 51264 });
		AddPointToLine({ 60578, 88360, 51038 });
		AddPointToLine({ 60058, 90000, 50803 });


		// ============================================================
		// ПОВЕРХНОСТЬ 350°
		// ============================================================

		NewLine();
		AddPointToLine({ 59235, 30000, 50796 });
		AddPointToLine({ 59639, 31250, 50993 });
		AddPointToLine({ 60041, 32500, 51187 });
		AddPointToLine({ 60440, 33750, 51376 });
		AddPointToLine({ 60835, 35000, 51561 });

		NewLine();
		AddPointToLine({ 60929, 35300, 51604 });
		AddPointToLine({ 61343, 36640, 51795 });
		AddPointToLine({ 61748, 37980, 51979 });
		AddPointToLine({ 62142, 39320, 52157 });
		AddPointToLine({ 62522, 40660, 52327 });
		AddPointToLine({ 62889, 42000, 52489 });

		NewLine();
		AddPointToLine({ 62968, 42300, 52524 });
		AddPointToLine({ 63304, 43600, 52671 });
		AddPointToLine({ 63623, 44900, 52811 });
		AddPointToLine({ 63924, 46200, 52941 });
		AddPointToLine({ 64205, 47500, 53063 });

		NewLine();
		AddPointToLine({ 64267, 47800, 53090 });
		AddPointToLine({ 64530, 49140, 53204 });
		AddPointToLine({ 64769, 50480, 53308 });
		AddPointToLine({ 64984, 51820, 53401 });
		AddPointToLine({ 65173, 53160, 53484 });
		AddPointToLine({ 65336, 54500, 53556 });

		NewLine();
		AddPointToLine({ 65369, 54800, 53570 });
		AddPointToLine({ 65490, 56040, 53625 });
		AddPointToLine({ 65587, 57280, 53670 });
		AddPointToLine({ 65660, 58520, 53705 });
		AddPointToLine({ 65708, 59760, 53731 });
		AddPointToLine({ 65732, 61000, 53747 });

		NewLine();
		AddPointToLine({ 65734, 61300, 53749 });
		AddPointToLine({ 65726, 62640, 53753 });
		AddPointToLine({ 65689, 63980, 53745 });
		AddPointToLine({ 65623, 65320, 53725 });
		AddPointToLine({ 65528, 66660, 53695 });
		AddPointToLine({ 65406, 68000, 53652 });

		NewLine();
		AddPointToLine({ 65375, 68300, 53641 });
		AddPointToLine({ 65232, 69540, 53590 });
		AddPointToLine({ 65066, 70780, 53529 });
		AddPointToLine({ 64878, 72020, 53459 });
		AddPointToLine({ 64668, 73260, 53380 });
		AddPointToLine({ 64438, 74500, 53292 });

		NewLine();
		AddPointToLine({ 64379, 74800, 53269 });
		AddPointToLine({ 64103, 76140, 53161 });
		AddPointToLine({ 63806, 77480, 53044 });
		AddPointToLine({ 63488, 78820, 52916 });
		AddPointToLine({ 63151, 80160, 52780 });
		AddPointToLine({ 62797, 81500, 52634 });

		NewLine();
		AddPointToLine({ 62715, 81800, 52600 });
		AddPointToLine({ 62256, 83440, 52408 });
		AddPointToLine({ 61777, 85080, 52204 });
		AddPointToLine({ 61281, 86720, 51988 });
		AddPointToLine({ 60772, 88360, 51762 });
		AddPointToLine({ 60252, 90000, 51527 });


		// ============================================================
// КОРНИ — НЕБОЛЬШИЕ ИЗОГНУТЫЕ ТРУБЫ
// 4 корня, по 6 продольных жил на каждой трубе
// Радиус корней: 750
// Каждая жила имеет 5 точек
// ============================================================

// Корень ЛЕВЫЙ, жила 0°
		NewLine();
		AddPointToLine({ 53795, 30000, 50811 });
		AddPointToLine({ 50335, 29400, 49329 });
		AddPointToLine({ 46868, 30200, 47346 });
		AddPointToLine({ 42391, 28900, 44860 });
		AddPointToLine({ 37835, 29700, 41829 });
		AddPointToLine({ 33000, 29200, 41000 });

		// Корень ЛЕВЫЙ, жила 60°
		NewLine();
		AddPointToLine({ 53648, 30357, 51155 });
		AddPointToLine({ 50168, 29757, 49665 });
		AddPointToLine({ 46684, 30557, 47673 });
		AddPointToLine({ 42196, 29257, 45180 });
		AddPointToLine({ 37668, 30057, 42165 });
		AddPointToLine({ 33000, 29200, 41000 });

		// Корень ЛЕВЫЙ, жила 120°
		NewLine();
		AddPointToLine({ 53352, 30357, 51845 });
		AddPointToLine({ 49832, 29757, 50335 });
		AddPointToLine({ 46316, 30557, 48327 });
		AddPointToLine({ 41804, 29257, 45820 });
		AddPointToLine({ 37332, 30057, 42835 });
		AddPointToLine({ 33000, 29200, 41000 });

		// Корень ЛЕВЫЙ, жила 180°
		NewLine();
		AddPointToLine({ 53205, 30000, 52189 });
		AddPointToLine({ 49665, 29400, 50671 });
		AddPointToLine({ 46132, 30200, 48654 });
		AddPointToLine({ 41609, 28900, 46140 });
		AddPointToLine({ 37165, 29700, 43171 });
		AddPointToLine({ 33000, 29200, 41000 });

		// Корень ЛЕВЫЙ, жила 240°
		NewLine();
		AddPointToLine({ 53352, 29643, 51845 });
		AddPointToLine({ 49832, 29043, 50335 });
		AddPointToLine({ 46316, 29843, 48327 });
		AddPointToLine({ 41804, 28543, 45820 });
		AddPointToLine({ 37332, 29343, 42835 });
		AddPointToLine({ 33000, 29200, 41000 });

		// Корень ЛЕВЫЙ, жила 300°
		NewLine();
		AddPointToLine({ 53648, 29643, 51155 });
		AddPointToLine({ 50168, 29043, 49665 });
		AddPointToLine({ 46684, 29843, 47673 });
		AddPointToLine({ 42196, 28543, 45180 });
		AddPointToLine({ 37668, 29343, 42165 });
		AddPointToLine({ 33000, 29200, 41000 });

		// Корень ПРАВЫЙ, жила 0°
		NewLine();
		AddPointToLine({ 57205, 30000, 52189 });
		AddPointToLine({ 60628, 29500, 53651 });
		AddPointToLine({ 64394, 30200, 56242 });
		AddPointToLine({ 67901, 28800, 54743 });
		AddPointToLine({ 71953, 29700, 57249 });
		AddPointToLine({ 76000, 29100, 54500 });

		// Корень ПРАВЫЙ, жила 60°
		NewLine();
		AddPointToLine({ 57352, 30357, 51845 });
		AddPointToLine({ 60814, 29857, 53326 });
		AddPointToLine({ 64447, 30557, 55871 });
		AddPointToLine({ 67950, 29157, 54372 });
		AddPointToLine({ 71977, 30057, 56874 });
		AddPointToLine({ 76000, 29100, 54500 });

		// Корень ПРАВЫЙ, жила 120°
		NewLine();
		AddPointToLine({ 57648, 30357, 51155 });
		AddPointToLine({ 61186, 29857, 52674 });
		AddPointToLine({ 64553, 30557, 55129 });
		AddPointToLine({ 68050, 29157, 53628 });
		AddPointToLine({ 72023, 30057, 56126 });
		AddPointToLine({ 76000, 29100, 54500 });

		// Корень ПРАВЫЙ, жила 180°
		NewLine();
		AddPointToLine({ 57795, 30000, 50811 });
		AddPointToLine({ 61372, 29500, 52349 });
		AddPointToLine({ 64606, 30200, 54758 });
		AddPointToLine({ 68099, 28800, 53257 });
		AddPointToLine({ 72047, 29700, 55751 });
		AddPointToLine({ 76000, 29100, 54500 });

		// Корень ПРАВЫЙ, жила 240°
		NewLine();
		AddPointToLine({ 57648, 29643, 51155 });
		AddPointToLine({ 61186, 29143, 52674 });
		AddPointToLine({ 64553, 29843, 55129 });
		AddPointToLine({ 68050, 28443, 53628 });
		AddPointToLine({ 72023, 29343, 56126 });
		AddPointToLine({ 76000, 29100, 54500 });

		// Корень ПРАВЫЙ, жила 300°
		NewLine();
		AddPointToLine({ 57352, 29643, 51845 });
		AddPointToLine({ 60814, 29143, 53326 });
		AddPointToLine({ 64447, 29843, 55871 });
		AddPointToLine({ 67950, 28443, 54372 });
		AddPointToLine({ 71977, 29343, 56874 });
		AddPointToLine({ 76000, 29100, 54500 });


		// Корень НАЗАД, жила 0°
		NewLine();
		AddPointToLine({ 54151, 30000, 48628 });
		AddPointToLine({ 52151, 29400, 45128 });
		AddPointToLine({ 50131, 30200, 41594 });
		AddPointToLine({ 47610, 28900, 38064 });
		AddPointToLine({ 45094, 29700, 34543 });
		AddPointToLine({ 42000, 29200, 32000 });

		// Корень НАЗАД, жила 60°
		NewLine();
		AddPointToLine({ 53826, 30357, 48814 });
		AddPointToLine({ 51826, 29757, 45314 });
		AddPointToLine({ 49815, 30557, 41797 });
		AddPointToLine({ 47305, 29257, 38282 });
		AddPointToLine({ 44797, 30057, 34771 });
		AddPointToLine({ 42000, 29200, 32000 });

		// Корень НАЗАД, жила 120°
		NewLine();
		AddPointToLine({ 53174, 30357, 49186 });
		AddPointToLine({ 51174, 29757, 45686 });
		AddPointToLine({ 49185, 30557, 42203 });
		AddPointToLine({ 46695, 29257, 38718 });
		AddPointToLine({ 44203, 30057, 35229 });
		AddPointToLine({ 42000, 29200, 32000 });

		// Корень НАЗАД, жила 180°
		NewLine();
		AddPointToLine({ 52849, 30000, 49372 });
		AddPointToLine({ 50849, 29400, 45872 });
		AddPointToLine({ 48869, 30200, 42406 });
		AddPointToLine({ 46390, 28900, 38936 });
		AddPointToLine({ 43906, 29700, 35457 });
		AddPointToLine({ 42000, 29200, 32000 });

		// Корень НАЗАД, жила 240°
		NewLine();
		AddPointToLine({ 53174, 29643, 49186 });
		AddPointToLine({ 51174, 29043, 45686 });
		AddPointToLine({ 49185, 29843, 42203 });
		AddPointToLine({ 46695, 28543, 38718 });
		AddPointToLine({ 44203, 29343, 35229 });
		AddPointToLine({ 42000, 29200, 32000 });

		// Корень НАЗАД, жила 300°
		NewLine();
		AddPointToLine({ 53826, 29643, 48814 });
		AddPointToLine({ 51826, 29043, 45314 });
		AddPointToLine({ 49815, 29843, 41797 });
		AddPointToLine({ 47305, 28543, 38282 });
		AddPointToLine({ 44797, 29343, 34771 });
		AddPointToLine({ 42000, 29200, 32000 });

		// Корень ВПЕРЕД, жила 0°
		NewLine();
		AddPointToLine({ 56890, 30000, 54436 });
		AddPointToLine({ 59390, 29400, 57936 });
		AddPointToLine({ 61927, 30200, 61484 });
		AddPointToLine({ 64949, 28900, 64509 });
		AddPointToLine({ 67970, 29700, 68030 });
		AddPointToLine({ 71500, 29200, 70000 });

		// Корень ВПЕРЕД, жила 60°
		NewLine();
		AddPointToLine({ 57195, 30357, 54218 });
		AddPointToLine({ 59695, 29757, 57718 });
		AddPointToLine({ 62214, 30557, 61242 });
		AddPointToLine({ 65224, 29257, 64254 });
		AddPointToLine({ 68235, 30057, 67765 });
		AddPointToLine({ 71500, 29200, 70000 });

		// Корень ВПЕРЕД, жила 120°
		NewLine();
		AddPointToLine({ 57805, 30357, 53782 });
		AddPointToLine({ 60305, 29757, 57282 });
		AddPointToLine({ 62786, 30557, 60758 });
		AddPointToLine({ 65776, 29257, 63746 });
		AddPointToLine({ 68765, 30057, 67235 });
		AddPointToLine({ 71500, 29200, 70000 });

		// Корень ВПЕРЕД, жила 180°
		NewLine();
		AddPointToLine({ 58110, 30000, 53564 });
		AddPointToLine({ 60610, 29400, 57064 });
		AddPointToLine({ 63073, 30200, 60516 });
		AddPointToLine({ 66051, 28900, 63491 });
		AddPointToLine({ 69030, 29700, 66970 });
		AddPointToLine({ 71500, 29200, 70000 });

		// Корень ВПЕРЕД, жила 240°
		NewLine();
		AddPointToLine({ 57805, 29643, 53782 });
		AddPointToLine({ 60305, 29043, 57282 });
		AddPointToLine({ 62786, 29843, 60758 });
		AddPointToLine({ 65776, 28543, 63746 });
		AddPointToLine({ 68765, 29343, 67235 });
		AddPointToLine({ 71500, 29200, 70000 });

		// Корень ВПЕРЕД, жила 300°
		NewLine();
		AddPointToLine({ 57195, 29643, 54218 });
		AddPointToLine({ 59695, 29043, 57718 });
		AddPointToLine({ 62214, 29843, 61242 });
		AddPointToLine({ 65224, 28543, 64254 });
		AddPointToLine({ 68235, 29343, 67765 });
		AddPointToLine({ 71500, 29200, 70000 });


		// ============================================================
// НОВЫЕ ВЕТВИ ДЕРЕВА — ВО ВСЕ СТОРОНЫ
// Каждая ветвь = труба из 6 жил
// Радиус трубы около 750
// Ветви выходят непосредственно с поверхности ствола
// ============================================================

// ============================================================
// ВЕТВЬ 01 — ПРАВАЯ, С ПОДЪЕМОМ ВВЕРХ
// ============================================================

		NewLine();
		AddPointToLine({ 65797, 61000, 54494 });
		AddPointToLine({ 68000, 62500, 55200 });
		AddPointToLine({ 70500, 64500, 56000 });
		AddPointToLine({ 73500, 66000, 54800 });
		AddPointToLine({ 76500, 68500, 56500 });
		AddPointToLine({ 80000, 70000, 55000 });

		NewLine();
		AddPointToLine({ 65900, 61650, 55100 });
		AddPointToLine({ 68100, 63150, 55800 });
		AddPointToLine({ 70600, 65150, 56600 });
		AddPointToLine({ 73600, 66650, 55400 });
		AddPointToLine({ 76600, 69150, 57100 });
		AddPointToLine({ 80100, 70650, 55600 });

		NewLine();
		AddPointToLine({ 65900, 61650, 53850 });
		AddPointToLine({ 68100, 63150, 54450 });
		AddPointToLine({ 70600, 65150, 55150 });
		AddPointToLine({ 73600, 66650, 53350 });
		AddPointToLine({ 76600, 69150, 55050 });
		AddPointToLine({ 80100, 70650, 53550 });

		NewLine();
		AddPointToLine({ 65797, 61000, 53744 });
		AddPointToLine({ 68000, 62500, 54450 });
		AddPointToLine({ 70500, 64500, 55250 });
		AddPointToLine({ 73500, 66000, 54050 });
		AddPointToLine({ 76500, 68500, 55750 });
		AddPointToLine({ 80000, 70000, 54250 });

		NewLine();
		AddPointToLine({ 65694, 60350, 53850 });
		AddPointToLine({ 67900, 61850, 54550 });
		AddPointToLine({ 70400, 63850, 55350 });
		AddPointToLine({ 73400, 65350, 54150 });
		AddPointToLine({ 76400, 67850, 55850 });
		AddPointToLine({ 79900, 69350, 54350 });

		NewLine();
		AddPointToLine({ 65694, 60350, 55100 });
		AddPointToLine({ 67900, 61850, 55800 });
		AddPointToLine({ 70400, 63850, 56600 });
		AddPointToLine({ 73400, 65350, 55400 });
		AddPointToLine({ 76400, 67850, 57100 });
		AddPointToLine({ 79900, 69350, 55600 });


		// ============================================================
		// ВЕТВЬ 02 — ЛЕВАЯ, С ПОДЪЕМОМ
		// ============================================================

		NewLine();
		AddPointToLine({ 56871, 68000, 54399 });
		AddPointToLine({ 54500, 69500, 53500 });
		AddPointToLine({ 52000, 71500, 52800 });
		AddPointToLine({ 49000, 73500, 54000 });
		AddPointToLine({ 45500, 75000, 52000 });
		AddPointToLine({ 42000, 77500, 53500 });

		NewLine();
		AddPointToLine({ 56974, 68650, 55100 });
		AddPointToLine({ 54600, 70150, 54200 });
		AddPointToLine({ 52100, 72150, 53500 });
		AddPointToLine({ 49100, 74150, 54700 });
		AddPointToLine({ 45600, 75650, 52700 });
		AddPointToLine({ 42100, 78150, 54200 });

		NewLine();
		AddPointToLine({ 56974, 68650, 53700 });
		AddPointToLine({ 54600, 70150, 52800 });
		AddPointToLine({ 52100, 72150, 52100 });
		AddPointToLine({ 49100, 74150, 53300 });
		AddPointToLine({ 45600, 75650, 51300 });
		AddPointToLine({ 42100, 78150, 52800 });

		NewLine();
		AddPointToLine({ 56871, 68000, 53649 });
		AddPointToLine({ 54500, 69500, 52750 });
		AddPointToLine({ 52000, 71500, 52050 });
		AddPointToLine({ 49000, 73500, 53250 });
		AddPointToLine({ 45500, 75000, 51250 });
		AddPointToLine({ 42000, 77500, 52750 });

		NewLine();
		AddPointToLine({ 56768, 67350, 53700 });
		AddPointToLine({ 54400, 68850, 52800 });
		AddPointToLine({ 51900, 70850, 52100 });
		AddPointToLine({ 48900, 72850, 53300 });
		AddPointToLine({ 45400, 74350, 51300 });
		AddPointToLine({ 41900, 76850, 52800 });

		NewLine();
		AddPointToLine({ 56768, 67350, 55100 });
		AddPointToLine({ 54400, 68850, 54200 });
		AddPointToLine({ 51900, 70850, 53500 });
		AddPointToLine({ 48900, 72850, 54700 });
		AddPointToLine({ 45400, 74350, 52700 });
		AddPointToLine({ 41900, 76850, 54200 });


		// ============================================================
		// ВЕТВЬ 03 — ПРЯМО ВПЕРЕД ПО Z
		// ============================================================

		NewLine();
		AddPointToLine({ 61171, 68000, 58699 });
		AddPointToLine({ 61500, 69500, 61500 });
		AddPointToLine({ 61000, 71500, 64500 });
		AddPointToLine({ 62000, 73500, 67500 });
		AddPointToLine({ 60500, 75500, 70500 });
		AddPointToLine({ 61500, 78000, 73500 });

		NewLine();
		AddPointToLine({ 61800, 68000, 58900 });
		AddPointToLine({ 62200, 69500, 61700 });
		AddPointToLine({ 61700, 71500, 64700 });
		AddPointToLine({ 62700, 73500, 67700 });
		AddPointToLine({ 61200, 75500, 70700 });
		AddPointToLine({ 62200, 78000, 73700 });

		NewLine();
		AddPointToLine({ 60500, 68000, 58900 });
		AddPointToLine({ 60800, 69500, 61700 });
		AddPointToLine({ 60300, 71500, 64700 });
		AddPointToLine({ 61300, 73500, 67700 });
		AddPointToLine({ 59800, 75500, 70700 });
		AddPointToLine({ 60800, 78000, 73700 });

		NewLine();
		AddPointToLine({ 61171, 68000, 57949 });
		AddPointToLine({ 61500, 69500, 60750 });
		AddPointToLine({ 61000, 71500, 63750 });
		AddPointToLine({ 62000, 73500, 66750 });
		AddPointToLine({ 60500, 75500, 69750 });
		AddPointToLine({ 61500, 78000, 72750 });

		NewLine();
		AddPointToLine({ 61842, 68000, 57900 });
		AddPointToLine({ 62200, 69500, 60700 });
		AddPointToLine({ 61700, 71500, 63700 });
		AddPointToLine({ 62700, 73500, 66700 });
		AddPointToLine({ 61200, 75500, 69700 });
		AddPointToLine({ 62200, 78000, 72700 });

		NewLine();
		AddPointToLine({ 60500, 68000, 57900 });
		AddPointToLine({ 60800, 69500, 60700 });
		AddPointToLine({ 60300, 71500, 63700 });
		AddPointToLine({ 61300, 73500, 66700 });
		AddPointToLine({ 59800, 75500, 69700 });
		AddPointToLine({ 60800, 78000, 72700 });


		// ============================================================
		// ВЕТВЬ 04 — НАЗАД ПО Z
		// ============================================================

		NewLine();
		AddPointToLine({ 61497, 61000, 50194 });
		AddPointToLine({ 62000, 62500, 47500 });
		AddPointToLine({ 61500, 64500, 44500 });
		AddPointToLine({ 62500, 66500, 42000 });
		AddPointToLine({ 61000, 68500, 39000 });
		AddPointToLine({ 62000, 71000, 36000 });

		NewLine();
		AddPointToLine({ 62247, 61000, 50294 });
		AddPointToLine({ 62750, 62500, 47600 });
		AddPointToLine({ 62250, 64500, 44600 });
		AddPointToLine({ 63250, 66500, 42100 });
		AddPointToLine({ 61750, 68500, 39100 });
		AddPointToLine({ 62750, 71000, 36100 });

		NewLine();
		AddPointToLine({ 60747, 61000, 50294 });
		AddPointToLine({ 61250, 62500, 47600 });
		AddPointToLine({ 60750, 64500, 44600 });
		AddPointToLine({ 61750, 66500, 42100 });
		AddPointToLine({ 60250, 68500, 39100 });
		AddPointToLine({ 61250, 71000, 36100 });

		NewLine();
		AddPointToLine({ 61497, 61000, 49444 });
		AddPointToLine({ 62000, 62500, 46750 });
		AddPointToLine({ 61500, 64500, 43750 });
		AddPointToLine({ 62500, 66500, 41250 });
		AddPointToLine({ 61000, 68500, 38250 });
		AddPointToLine({ 62000, 71000, 35250 });

		NewLine();
		AddPointToLine({ 60747, 61000, 49444 });
		AddPointToLine({ 61250, 62500, 46750 });
		AddPointToLine({ 60750, 64500, 43750 });
		AddPointToLine({ 61750, 66500, 41250 });
		AddPointToLine({ 60250, 68500, 38250 });
		AddPointToLine({ 61250, 71000, 35250 });

		NewLine();
		AddPointToLine({ 62247, 61000, 49444 });
		AddPointToLine({ 62750, 62500, 46750 });
		AddPointToLine({ 62250, 64500, 43750 });
		AddPointToLine({ 63250, 66500, 41250 });
		AddPointToLine({ 61750, 68500, 38250 });
		AddPointToLine({ 62750, 71000, 35250 });


		// ============================================================
		// ВЕТВЬ 05 — ДИАГОНАЛЬ ВПРАВО + Z
		// ============================================================

		NewLine();
		AddPointToLine({ 64791, 74500, 56803 });
		AddPointToLine({ 67000, 75500, 59000 });
		AddPointToLine({ 69500, 77500, 61500 });
		AddPointToLine({ 72500, 79000, 64000 });
		AddPointToLine({ 75500, 81000, 66000 });
		AddPointToLine({ 79000, 82500, 68500 });

		NewLine();
		AddPointToLine({ 65400, 75000, 57000 });
		AddPointToLine({ 67600, 76000, 59700 });
		AddPointToLine({ 70100, 78000, 62200 });
		AddPointToLine({ 73100, 79500, 64700 });
		AddPointToLine({ 76100, 81500, 66700 });
		AddPointToLine({ 79600, 83000, 69200 });

		NewLine();
		AddPointToLine({ 64182, 75000, 57000 });
		AddPointToLine({ 66400, 76000, 58300 });
		AddPointToLine({ 68900, 78000, 60800 });
		AddPointToLine({ 71900, 79500, 63300 });
		AddPointToLine({ 74900, 81500, 65300 });
		AddPointToLine({ 78400, 83000, 67800 });

		NewLine();
		AddPointToLine({ 64791, 74500, 56053 });
		AddPointToLine({ 67000, 75500, 58250 });
		AddPointToLine({ 69500, 77500, 60750 });
		AddPointToLine({ 72500, 79000, 63250 });
		AddPointToLine({ 75500, 81000, 65250 });
		AddPointToLine({ 79000, 82500, 67750 });

		NewLine();
		AddPointToLine({ 64182, 74000, 56100 });
		AddPointToLine({ 66400, 75000, 58400 });
		AddPointToLine({ 68900, 77000, 60900 });
		AddPointToLine({ 71900, 78500, 63400 });
		AddPointToLine({ 74900, 80500, 65400 });
		AddPointToLine({ 78400, 82000, 67900 });

		NewLine();
		AddPointToLine({ 65400, 74000, 56100 });
		AddPointToLine({ 67600, 75000, 58400 });
		AddPointToLine({ 70100, 77000, 60900 });
		AddPointToLine({ 73100, 78500, 63400 });
		AddPointToLine({ 76100, 80500, 65400 });
		AddPointToLine({ 79600, 82000, 67900 });


		// ============================================================
		// ВЕТВЬ 06 — ДИАГОНАЛЬ ВЛЕВО + Z
		// ============================================================

		NewLine();
		AddPointToLine({ 57877, 74500, 56803 });
		AddPointToLine({ 55500, 76000, 59000 });
		AddPointToLine({ 53000, 78000, 61500 });
		AddPointToLine({ 50000, 79500, 64000 });
		AddPointToLine({ 47000, 81500, 66000 });
		AddPointToLine({ 43500, 83000, 68500 });

		NewLine();
		AddPointToLine({ 58480, 75000, 57000 });
		AddPointToLine({ 56100, 76500, 59700 });
		AddPointToLine({ 53600, 78500, 62200 });
		AddPointToLine({ 50600, 80000, 64700 });
		AddPointToLine({ 47600, 82000, 66700 });
		AddPointToLine({ 44100, 83500, 69200 });

		NewLine();
		AddPointToLine({ 57274, 75000, 57000 });
		AddPointToLine({ 54900, 76500, 58300 });
		AddPointToLine({ 52400, 78500, 60800 });
		AddPointToLine({ 49400, 80000, 63300 });
		AddPointToLine({ 46400, 82000, 65300 });
		AddPointToLine({ 42900, 83500, 67800 });

		NewLine();
		AddPointToLine({ 57877, 74500, 56053 });
		AddPointToLine({ 55500, 76000, 58250 });
		AddPointToLine({ 53000, 78000, 60750 });
		AddPointToLine({ 50000, 79500, 63250 });
		AddPointToLine({ 47000, 81500, 65250 });
		AddPointToLine({ 43500, 83000, 67750 });

		NewLine();
		AddPointToLine({ 57274, 74000, 56100 });
		AddPointToLine({ 54900, 75500, 58400 });
		AddPointToLine({ 52400, 77500, 60900 });
		AddPointToLine({ 49400, 79000, 63400 });
		AddPointToLine({ 46400, 81000, 65400 });
		AddPointToLine({ 42900, 82500, 67900 });

		NewLine();
		AddPointToLine({ 58480, 74000, 56100 });
		AddPointToLine({ 56100, 75500, 58400 });
		AddPointToLine({ 53600, 77500, 60900 });
		AddPointToLine({ 50600, 79000, 63400 });
		AddPointToLine({ 47600, 81000, 65400 });
		AddPointToLine({ 44100, 82500, 67900 });


		// ============================================================
		// ВЕТВЬ 07 — ДИАГОНАЛЬ ВПРАВО -Z
		// ============================================================

		NewLine();
		AddPointToLine({ 61856, 81500, 56145 });
		AddPointToLine({ 64500, 82500, 54000 });
		AddPointToLine({ 67500, 84500, 51500 });
		AddPointToLine({ 70500, 86000, 49000 });
		AddPointToLine({ 73500, 88000, 47000 });
		AddPointToLine({ 77000, 89500, 44500 });

		NewLine();
		AddPointToLine({ 62459, 82000, 56800 });
		AddPointToLine({ 65100, 83000, 54700 });
		AddPointToLine({ 68100, 85000, 52200 });
		AddPointToLine({ 71100, 86500, 49700 });
		AddPointToLine({ 74100, 88500, 47700 });
		AddPointToLine({ 77600, 90000, 45200 });

		NewLine();
		AddPointToLine({ 61253, 82000, 56800 });
		AddPointToLine({ 63900, 83000, 53300 });
		AddPointToLine({ 66900, 85000, 50800 });
		AddPointToLine({ 69900, 86500, 48300 });
		AddPointToLine({ 72900, 88500, 46300 });
		AddPointToLine({ 76400, 90000, 43800 });

		NewLine();
		AddPointToLine({ 61856, 81500, 55295 });
		AddPointToLine({ 64500, 82500, 53250 });
		AddPointToLine({ 67500, 84500, 50750 });
		AddPointToLine({ 70500, 86000, 48250 });
		AddPointToLine({ 73500, 88000, 46250 });
		AddPointToLine({ 77000, 89500, 43750 });

		NewLine();
		AddPointToLine({ 61253, 81000, 55300 });
		AddPointToLine({ 63900, 82000, 53300 });
		AddPointToLine({ 66900, 84000, 50800 });
		AddPointToLine({ 69900, 85500, 48300 });
		AddPointToLine({ 72900, 87500, 46300 });
		AddPointToLine({ 76400, 89000, 43800 });

		NewLine();
		AddPointToLine({ 62459, 81000, 55300 });
		AddPointToLine({ 65100, 82000, 53300 });
		AddPointToLine({ 68100, 84000, 50800 });
		AddPointToLine({ 71100, 85500, 48300 });
		AddPointToLine({ 74100, 87500, 46300 });
		AddPointToLine({ 77600, 89000, 43800 });


		// ============================================================
		// ВЕТВЬ 08 — ДИАГОНАЛЬ ВЛЕВО -Z
		// ============================================================

		NewLine();
		AddPointToLine({ 55268, 81500, 50617 });
		AddPointToLine({ 53000, 82500, 48500 });
		AddPointToLine({ 50500, 84500, 46000 });
		AddPointToLine({ 47500, 86000, 43500 });
		AddPointToLine({ 44500, 88000, 41500 });
		AddPointToLine({ 41000, 89500, 39000 });

		NewLine();
		AddPointToLine({ 55871, 82000, 51270 });
		AddPointToLine({ 53600, 83000, 49150 });
		AddPointToLine({ 51100, 85000, 46650 });
		AddPointToLine({ 48100, 86500, 44150 });
		AddPointToLine({ 45100, 88500, 42150 });
		AddPointToLine({ 41600, 90000, 39650 });

		NewLine();
		AddPointToLine({ 54665, 82000, 51270 });
		AddPointToLine({ 52400, 83000, 47850 });
		AddPointToLine({ 49900, 85000, 45350 });
		AddPointToLine({ 46900, 86500, 42850 });
		AddPointToLine({ 43900, 88500, 40850 });
		AddPointToLine({ 40400, 90000, 38350 });

		NewLine();
		AddPointToLine({ 55268, 81500, 49867 });
		AddPointToLine({ 53000, 82500, 47750 });
		AddPointToLine({ 50500, 84500, 45250 });
		AddPointToLine({ 47500, 86000, 42750 });
		AddPointToLine({ 44500, 88000, 40750 });
		AddPointToLine({ 41000, 89500, 38250 });

		NewLine();
		AddPointToLine({ 54665, 81000, 49900 });
		AddPointToLine({ 52400, 82000, 47800 });
		AddPointToLine({ 49900, 84000, 45300 });
		AddPointToLine({ 46900, 85500, 42800 });
		AddPointToLine({ 43900, 87500, 40800 });
		AddPointToLine({ 40400, 89000, 38300 });

		NewLine();
		AddPointToLine({ 55871, 81000, 49900 });
		AddPointToLine({ 53600, 82000, 47800 });
		AddPointToLine({ 51100, 84000, 45300 });
		AddPointToLine({ 48100, 85500, 42800 });
		AddPointToLine({ 45100, 87500, 40800 });
		AddPointToLine({ 41600, 89000, 38300 });


		// ============================================================
		// ВЕТВЬ 09 — ВВЕРХ И ВПРАВО
		// ============================================================

		NewLine();
		AddPointToLine({ 64503, 74500, 54039 });
		AddPointToLine({ 66500, 76500, 55500 });
		AddPointToLine({ 69000, 79000, 56500 });
		AddPointToLine({ 71500, 81500, 55000 });
		AddPointToLine({ 74000, 84500, 57000 });
		AddPointToLine({ 76500, 87500, 55500 });
		AddPointToLine({ 78500, 90500, 57500 });

		NewLine();
		AddPointToLine({ 65050, 75000, 54700 });
		AddPointToLine({ 67050, 77000, 56200 });
		AddPointToLine({ 69550, 79500, 57200 });
		AddPointToLine({ 72050, 82000, 55700 });
		AddPointToLine({ 74550, 85000, 57700 });
		AddPointToLine({ 77050, 88000, 56200 });
		AddPointToLine({ 79050, 91000, 58200 });

		NewLine();
		AddPointToLine({ 63956, 75000, 54700 });
		AddPointToLine({ 65950, 77000, 54800 });
		AddPointToLine({ 68450, 79500, 55800 });
		AddPointToLine({ 70950, 82000, 54300 });
		AddPointToLine({ 73450, 85000, 56300 });
		AddPointToLine({ 75950, 88000, 54800 });
		AddPointToLine({ 77950, 91000, 56800 });

		NewLine();
		AddPointToLine({ 64503, 74500, 53289 });
		AddPointToLine({ 66500, 76500, 54750 });
		AddPointToLine({ 69000, 79000, 55750 });
		AddPointToLine({ 71500, 81500, 54250 });
		AddPointToLine({ 74000, 84500, 56250 });
		AddPointToLine({ 76500, 87500, 54750 });
		AddPointToLine({ 78500, 90500, 56750 });

		NewLine();
		AddPointToLine({ 63956, 74000, 53300 });
		AddPointToLine({ 65950, 76000, 54800 });
		AddPointToLine({ 68450, 78500, 55800 });
		AddPointToLine({ 70950, 81000, 54300 });
		AddPointToLine({ 73450, 84000, 56300 });
		AddPointToLine({ 75950, 87000, 54800 });
		AddPointToLine({ 77950, 90000, 56800 });

		NewLine();
		AddPointToLine({ 65050, 74000, 53300 });
		AddPointToLine({ 67050, 76000, 54800 });
		AddPointToLine({ 69550, 78500, 55800 });
		AddPointToLine({ 72050, 81000, 54300 });
		AddPointToLine({ 74550, 84000, 56300 });
		AddPointToLine({ 77050, 87000, 54800 });
		AddPointToLine({ 79050, 90000, 56800 });


		// ============================================================
		// ВЕТВЬ 10 — ВВЕРХ И ВЛЕВО
		// ============================================================

		NewLine();
		AddPointToLine({ 55903, 74500, 54039 });
		AddPointToLine({ 54000, 76500, 52500 });
		AddPointToLine({ 51500, 79000, 53500 });
		AddPointToLine({ 49000, 81500, 52000 });
		AddPointToLine({ 46500, 84500, 54000 });
		AddPointToLine({ 44000, 87500, 52500 });
		AddPointToLine({ 41500, 90500, 54500 });

		NewLine();
		AddPointToLine({ 56450, 75000, 54700 });
		AddPointToLine({ 54550, 77000, 53200 });
		AddPointToLine({ 52050, 79500, 54200 });
		AddPointToLine({ 49550, 82000, 52700 });
		AddPointToLine({ 47050, 85000, 54700 });
		AddPointToLine({ 44550, 88000, 53200 });
		AddPointToLine({ 42050, 91000, 55200 });

		NewLine();
		AddPointToLine({ 55356, 75000, 54700 });
		AddPointToLine({ 53450, 77000, 51800 });
		AddPointToLine({ 50950, 79500, 52800 });
		AddPointToLine({ 48450, 82000, 51300 });
		AddPointToLine({ 45950, 85000, 53300 });
		AddPointToLine({ 43450, 88000, 51800 });
		AddPointToLine({ 40950, 91000, 53800 });

		NewLine();
		AddPointToLine({ 55903, 74500, 53289 });
		AddPointToLine({ 54000, 76500, 51750 });
		AddPointToLine({ 51500, 79000, 52750 });
		AddPointToLine({ 49000, 81500, 51250 });
		AddPointToLine({ 46500, 84500, 53250 });
		AddPointToLine({ 44000, 87500, 51750 });
		AddPointToLine({ 41500, 90500, 53750 });

		NewLine();
		AddPointToLine({ 55356, 74000, 53300 });
		AddPointToLine({ 53450, 76000, 51800 });
		AddPointToLine({ 50950, 78500, 52800 });
		AddPointToLine({ 48450, 81000, 51300 });
		AddPointToLine({ 45950, 84000, 53300 });
		AddPointToLine({ 43450, 87000, 51800 });
		AddPointToLine({ 40950, 90000, 53800 });

		NewLine();
		AddPointToLine({ 56450, 74000, 53300 });
		AddPointToLine({ 54550, 76000, 51800 });
		AddPointToLine({ 52050, 78500, 52800 });
		AddPointToLine({ 49550, 81000, 51300 });
		AddPointToLine({ 47050, 84000, 53300 });
		AddPointToLine({ 44550, 87000, 51800 });
		AddPointToLine({ 42050, 90000, 53800 });

		// ============================================================
// НОВЫЕ ВЕТВИ ДЕРЕВА — ВО ВСЕ СТОРОНЫ
// Каждая ветвь = труба из 6 жил
// Радиус трубы около 750
// Ветви выходят непосредственно с поверхности ствола
// ============================================================

// ============================================================
// ВЕТВЬ 01 — ПРАВАЯ, С ПОДЪЕМОМ ВВЕРХ
// ============================================================

		NewLine();
		AddPointToLine({ 65797, 61000, 54494 });
		AddPointToLine({ 68000, 62500, 55200 });
		AddPointToLine({ 70500, 64500, 56000 });
		AddPointToLine({ 73500, 66000, 54800 });
		AddPointToLine({ 76500, 68500, 56500 });
		AddPointToLine({ 80000, 70000, 55000 });

		NewLine();
		AddPointToLine({ 65900, 61650, 55100 });
		AddPointToLine({ 68100, 63150, 55800 });
		AddPointToLine({ 70600, 65150, 56600 });
		AddPointToLine({ 73600, 66650, 55400 });
		AddPointToLine({ 76600, 69150, 57100 });
		AddPointToLine({ 80100, 70650, 55600 });

		NewLine();
		AddPointToLine({ 65900, 61650, 53850 });
		AddPointToLine({ 68100, 63150, 54450 });
		AddPointToLine({ 70600, 65150, 55150 });
		AddPointToLine({ 73600, 66650, 53350 });
		AddPointToLine({ 76600, 69150, 55050 });
		AddPointToLine({ 80100, 70650, 53550 });

		NewLine();
		AddPointToLine({ 65797, 61000, 53744 });
		AddPointToLine({ 68000, 62500, 54450 });
		AddPointToLine({ 70500, 64500, 55250 });
		AddPointToLine({ 73500, 66000, 54050 });
		AddPointToLine({ 76500, 68500, 55750 });
		AddPointToLine({ 80000, 70000, 54250 });

		NewLine();
		AddPointToLine({ 65694, 60350, 53850 });
		AddPointToLine({ 67900, 61850, 54550 });
		AddPointToLine({ 70400, 63850, 55350 });
		AddPointToLine({ 73400, 65350, 54150 });
		AddPointToLine({ 76400, 67850, 55850 });
		AddPointToLine({ 79900, 69350, 54350 });

		NewLine();
		AddPointToLine({ 65694, 60350, 55100 });
		AddPointToLine({ 67900, 61850, 55800 });
		AddPointToLine({ 70400, 63850, 56600 });
		AddPointToLine({ 73400, 65350, 55400 });
		AddPointToLine({ 76400, 67850, 57100 });
		AddPointToLine({ 79900, 69350, 55600 });


		// ============================================================
		// ВЕТВЬ 02 — ЛЕВАЯ, С ПОДЪЕМОМ
		// ============================================================

		NewLine();
		AddPointToLine({ 56871, 68000, 54399 });
		AddPointToLine({ 54500, 69500, 53500 });
		AddPointToLine({ 52000, 71500, 52800 });
		AddPointToLine({ 49000, 73500, 54000 });
		AddPointToLine({ 45500, 75000, 52000 });
		AddPointToLine({ 42000, 77500, 53500 });

		NewLine();
		AddPointToLine({ 56974, 68650, 55100 });
		AddPointToLine({ 54600, 70150, 54200 });
		AddPointToLine({ 52100, 72150, 53500 });
		AddPointToLine({ 49100, 74150, 54700 });
		AddPointToLine({ 45600, 75650, 52700 });
		AddPointToLine({ 42100, 78150, 54200 });

		NewLine();
		AddPointToLine({ 56974, 68650, 53700 });
		AddPointToLine({ 54600, 70150, 52800 });
		AddPointToLine({ 52100, 72150, 52100 });
		AddPointToLine({ 49100, 74150, 53300 });
		AddPointToLine({ 45600, 75650, 51300 });
		AddPointToLine({ 42100, 78150, 52800 });

		NewLine();
		AddPointToLine({ 56871, 68000, 53649 });
		AddPointToLine({ 54500, 69500, 52750 });
		AddPointToLine({ 52000, 71500, 52050 });
		AddPointToLine({ 49000, 73500, 53250 });
		AddPointToLine({ 45500, 75000, 51250 });
		AddPointToLine({ 42000, 77500, 52750 });

		NewLine();
		AddPointToLine({ 56768, 67350, 53700 });
		AddPointToLine({ 54400, 68850, 52800 });
		AddPointToLine({ 51900, 70850, 52100 });
		AddPointToLine({ 48900, 72850, 53300 });
		AddPointToLine({ 45400, 74350, 51300 });
		AddPointToLine({ 41900, 76850, 52800 });

		NewLine();
		AddPointToLine({ 56768, 67350, 55100 });
		AddPointToLine({ 54400, 68850, 54200 });
		AddPointToLine({ 51900, 70850, 53500 });
		AddPointToLine({ 48900, 72850, 54700 });
		AddPointToLine({ 45400, 74350, 52700 });
		AddPointToLine({ 41900, 76850, 54200 });


		// ============================================================
		// ВЕТВЬ 03 — ПРЯМО ВПЕРЕД ПО Z
		// ============================================================

		NewLine();
		AddPointToLine({ 61171, 68000, 58699 });
		AddPointToLine({ 61500, 69500, 61500 });
		AddPointToLine({ 61000, 71500, 64500 });
		AddPointToLine({ 62000, 73500, 67500 });
		AddPointToLine({ 60500, 75500, 70500 });
		AddPointToLine({ 61500, 78000, 73500 });

		NewLine();
		AddPointToLine({ 61800, 68000, 58900 });
		AddPointToLine({ 62200, 69500, 61700 });
		AddPointToLine({ 61700, 71500, 64700 });
		AddPointToLine({ 62700, 73500, 67700 });
		AddPointToLine({ 61200, 75500, 70700 });
		AddPointToLine({ 62200, 78000, 73700 });

		NewLine();
		AddPointToLine({ 60500, 68000, 58900 });
		AddPointToLine({ 60800, 69500, 61700 });
		AddPointToLine({ 60300, 71500, 64700 });
		AddPointToLine({ 61300, 73500, 67700 });
		AddPointToLine({ 59800, 75500, 70700 });
		AddPointToLine({ 60800, 78000, 73700 });

		NewLine();
		AddPointToLine({ 61171, 68000, 57949 });
		AddPointToLine({ 61500, 69500, 60750 });
		AddPointToLine({ 61000, 71500, 63750 });
		AddPointToLine({ 62000, 73500, 66750 });
		AddPointToLine({ 60500, 75500, 69750 });
		AddPointToLine({ 61500, 78000, 72750 });

		NewLine();
		AddPointToLine({ 61842, 68000, 57900 });
		AddPointToLine({ 62200, 69500, 60700 });
		AddPointToLine({ 61700, 71500, 63700 });
		AddPointToLine({ 62700, 73500, 66700 });
		AddPointToLine({ 61200, 75500, 69700 });
		AddPointToLine({ 62200, 78000, 72700 });

		NewLine();
		AddPointToLine({ 60500, 68000, 57900 });
		AddPointToLine({ 60800, 69500, 60700 });
		AddPointToLine({ 60300, 71500, 63700 });
		AddPointToLine({ 61300, 73500, 66700 });
		AddPointToLine({ 59800, 75500, 69700 });
		AddPointToLine({ 60800, 78000, 72700 });


		// ============================================================
		// ВЕТВЬ 04 — НАЗАД ПО Z
		// ============================================================

		NewLine();
		AddPointToLine({ 61497, 61000, 50194 });
		AddPointToLine({ 62000, 62500, 47500 });
		AddPointToLine({ 61500, 64500, 44500 });
		AddPointToLine({ 62500, 66500, 42000 });
		AddPointToLine({ 61000, 68500, 39000 });
		AddPointToLine({ 62000, 71000, 36000 });

		NewLine();
		AddPointToLine({ 62247, 61000, 50294 });
		AddPointToLine({ 62750, 62500, 47600 });
		AddPointToLine({ 62250, 64500, 44600 });
		AddPointToLine({ 63250, 66500, 42100 });
		AddPointToLine({ 61750, 68500, 39100 });
		AddPointToLine({ 62750, 71000, 36100 });

		NewLine();
		AddPointToLine({ 60747, 61000, 50294 });
		AddPointToLine({ 61250, 62500, 47600 });
		AddPointToLine({ 60750, 64500, 44600 });
		AddPointToLine({ 61750, 66500, 42100 });
		AddPointToLine({ 60250, 68500, 39100 });
		AddPointToLine({ 61250, 71000, 36100 });

		NewLine();
		AddPointToLine({ 61497, 61000, 49444 });
		AddPointToLine({ 62000, 62500, 46750 });
		AddPointToLine({ 61500, 64500, 43750 });
		AddPointToLine({ 62500, 66500, 41250 });
		AddPointToLine({ 61000, 68500, 38250 });
		AddPointToLine({ 62000, 71000, 35250 });

		NewLine();
		AddPointToLine({ 60747, 61000, 49444 });
		AddPointToLine({ 61250, 62500, 46750 });
		AddPointToLine({ 60750, 64500, 43750 });
		AddPointToLine({ 61750, 66500, 41250 });
		AddPointToLine({ 60250, 68500, 38250 });
		AddPointToLine({ 61250, 71000, 35250 });

		NewLine();
		AddPointToLine({ 62247, 61000, 49444 });
		AddPointToLine({ 62750, 62500, 46750 });
		AddPointToLine({ 62250, 64500, 43750 });
		AddPointToLine({ 63250, 66500, 41250 });
		AddPointToLine({ 61750, 68500, 38250 });
		AddPointToLine({ 62750, 71000, 35250 });


		// ============================================================
		// ВЕТВЬ 05 — ДИАГОНАЛЬ ВПРАВО + Z
		// ============================================================

		NewLine();
		AddPointToLine({ 64791, 74500, 56803 });
		AddPointToLine({ 67000, 75500, 59000 });
		AddPointToLine({ 69500, 77500, 61500 });
		AddPointToLine({ 72500, 79000, 64000 });
		AddPointToLine({ 75500, 81000, 66000 });
		AddPointToLine({ 79000, 82500, 68500 });

		NewLine();
		AddPointToLine({ 65400, 75000, 57000 });
		AddPointToLine({ 67600, 76000, 59700 });
		AddPointToLine({ 70100, 78000, 62200 });
		AddPointToLine({ 73100, 79500, 64700 });
		AddPointToLine({ 76100, 81500, 66700 });
		AddPointToLine({ 79600, 83000, 69200 });

		NewLine();
		AddPointToLine({ 64182, 75000, 57000 });
		AddPointToLine({ 66400, 76000, 58300 });
		AddPointToLine({ 68900, 78000, 60800 });
		AddPointToLine({ 71900, 79500, 63300 });
		AddPointToLine({ 74900, 81500, 65300 });
		AddPointToLine({ 78400, 83000, 67800 });

		NewLine();
		AddPointToLine({ 64791, 74500, 56053 });
		AddPointToLine({ 67000, 75500, 58250 });
		AddPointToLine({ 69500, 77500, 60750 });
		AddPointToLine({ 72500, 79000, 63250 });
		AddPointToLine({ 75500, 81000, 65250 });
		AddPointToLine({ 79000, 82500, 67750 });

		NewLine();
		AddPointToLine({ 64182, 74000, 56100 });
		AddPointToLine({ 66400, 75000, 58400 });
		AddPointToLine({ 68900, 77000, 60900 });
		AddPointToLine({ 71900, 78500, 63400 });
		AddPointToLine({ 74900, 80500, 65400 });
		AddPointToLine({ 78400, 82000, 67900 });

		NewLine();
		AddPointToLine({ 65400, 74000, 56100 });
		AddPointToLine({ 67600, 75000, 58400 });
		AddPointToLine({ 70100, 77000, 60900 });
		AddPointToLine({ 73100, 78500, 63400 });
		AddPointToLine({ 76100, 80500, 65400 });
		AddPointToLine({ 79600, 82000, 67900 });


		// ============================================================
		// ВЕТВЬ 06 — ДИАГОНАЛЬ ВЛЕВО + Z
		// ============================================================

		NewLine();
		AddPointToLine({ 57877, 74500, 56803 });
		AddPointToLine({ 55500, 76000, 59000 });
		AddPointToLine({ 53000, 78000, 61500 });
		AddPointToLine({ 50000, 79500, 64000 });
		AddPointToLine({ 47000, 81500, 66000 });
		AddPointToLine({ 43500, 83000, 68500 });

		NewLine();
		AddPointToLine({ 58480, 75000, 57000 });
		AddPointToLine({ 56100, 76500, 59700 });
		AddPointToLine({ 53600, 78500, 62200 });
		AddPointToLine({ 50600, 80000, 64700 });
		AddPointToLine({ 47600, 82000, 66700 });
		AddPointToLine({ 44100, 83500, 69200 });

		NewLine();
		AddPointToLine({ 57274, 75000, 57000 });
		AddPointToLine({ 54900, 76500, 58300 });
		AddPointToLine({ 52400, 78500, 60800 });
		AddPointToLine({ 49400, 80000, 63300 });
		AddPointToLine({ 46400, 82000, 65300 });
		AddPointToLine({ 42900, 83500, 67800 });

		NewLine();
		AddPointToLine({ 57877, 74500, 56053 });
		AddPointToLine({ 55500, 76000, 58250 });
		AddPointToLine({ 53000, 78000, 60750 });
		AddPointToLine({ 50000, 79500, 63250 });
		AddPointToLine({ 47000, 81500, 65250 });
		AddPointToLine({ 43500, 83000, 67750 });

		NewLine();
		AddPointToLine({ 57274, 74000, 56100 });
		AddPointToLine({ 54900, 75500, 58400 });
		AddPointToLine({ 52400, 77500, 60900 });
		AddPointToLine({ 49400, 79000, 63400 });
		AddPointToLine({ 46400, 81000, 65400 });
		AddPointToLine({ 42900, 82500, 67900 });

		NewLine();
		AddPointToLine({ 58480, 74000, 56100 });
		AddPointToLine({ 56100, 75500, 58400 });
		AddPointToLine({ 53600, 77500, 60900 });
		AddPointToLine({ 50600, 79000, 63400 });
		AddPointToLine({ 47600, 81000, 65400 });
		AddPointToLine({ 44100, 82500, 67900 });


		// ============================================================
		// ВЕТВЬ 07 — ДИАГОНАЛЬ ВПРАВО -Z
		// ============================================================

		NewLine();
		AddPointToLine({ 61856, 81500, 56145 });
		AddPointToLine({ 64500, 82500, 54000 });
		AddPointToLine({ 67500, 84500, 51500 });
		AddPointToLine({ 70500, 86000, 49000 });
		AddPointToLine({ 73500, 88000, 47000 });
		AddPointToLine({ 77000, 89500, 44500 });

		NewLine();
		AddPointToLine({ 62459, 82000, 56800 });
		AddPointToLine({ 65100, 83000, 54700 });
		AddPointToLine({ 68100, 85000, 52200 });
		AddPointToLine({ 71100, 86500, 49700 });
		AddPointToLine({ 74100, 88500, 47700 });
		AddPointToLine({ 77600, 90000, 45200 });

		NewLine();
		AddPointToLine({ 61253, 82000, 56800 });
		AddPointToLine({ 63900, 83000, 53300 });
		AddPointToLine({ 66900, 85000, 50800 });
		AddPointToLine({ 69900, 86500, 48300 });
		AddPointToLine({ 72900, 88500, 46300 });
		AddPointToLine({ 76400, 90000, 43800 });

		NewLine();
		AddPointToLine({ 61856, 81500, 55295 });
		AddPointToLine({ 64500, 82500, 53250 });
		AddPointToLine({ 67500, 84500, 50750 });
		AddPointToLine({ 70500, 86000, 48250 });
		AddPointToLine({ 73500, 88000, 46250 });
		AddPointToLine({ 77000, 89500, 43750 });

		NewLine();
		AddPointToLine({ 61253, 81000, 55300 });
		AddPointToLine({ 63900, 82000, 53300 });
		AddPointToLine({ 66900, 84000, 50800 });
		AddPointToLine({ 69900, 85500, 48300 });
		AddPointToLine({ 72900, 87500, 46300 });
		AddPointToLine({ 76400, 89000, 43800 });

		NewLine();
		AddPointToLine({ 62459, 81000, 55300 });
		AddPointToLine({ 65100, 82000, 53300 });
		AddPointToLine({ 68100, 84000, 50800 });
		AddPointToLine({ 71100, 85500, 48300 });
		AddPointToLine({ 74100, 87500, 46300 });
		AddPointToLine({ 77600, 89000, 43800 });


		// ============================================================
		// ВЕТВЬ 08 — ДИАГОНАЛЬ ВЛЕВО -Z
		// ============================================================

		NewLine();
		AddPointToLine({ 55268, 81500, 50617 });
		AddPointToLine({ 53000, 82500, 48500 });
		AddPointToLine({ 50500, 84500, 46000 });
		AddPointToLine({ 47500, 86000, 43500 });
		AddPointToLine({ 44500, 88000, 41500 });
		AddPointToLine({ 41000, 89500, 39000 });

		NewLine();
		AddPointToLine({ 55871, 82000, 51270 });
		AddPointToLine({ 53600, 83000, 49150 });
		AddPointToLine({ 51100, 85000, 46650 });
		AddPointToLine({ 48100, 86500, 44150 });
		AddPointToLine({ 45100, 88500, 42150 });
		AddPointToLine({ 41600, 90000, 39650 });

		NewLine();
		AddPointToLine({ 54665, 82000, 51270 });
		AddPointToLine({ 52400, 83000, 47850 });
		AddPointToLine({ 49900, 85000, 45350 });
		AddPointToLine({ 46900, 86500, 42850 });
		AddPointToLine({ 43900, 88500, 40850 });
		AddPointToLine({ 40400, 90000, 38350 });

		NewLine();
		AddPointToLine({ 55268, 81500, 49867 });
		AddPointToLine({ 53000, 82500, 47750 });
		AddPointToLine({ 50500, 84500, 45250 });
		AddPointToLine({ 47500, 86000, 42750 });
		AddPointToLine({ 44500, 88000, 40750 });
		AddPointToLine({ 41000, 89500, 38250 });

		NewLine();
		AddPointToLine({ 54665, 81000, 49900 });
		AddPointToLine({ 52400, 82000, 47800 });
		AddPointToLine({ 49900, 84000, 45300 });
		AddPointToLine({ 46900, 85500, 42800 });
		AddPointToLine({ 43900, 87500, 40800 });
		AddPointToLine({ 40400, 89000, 38300 });

		NewLine();
		AddPointToLine({ 55871, 81000, 49900 });
		AddPointToLine({ 53600, 82000, 47800 });
		AddPointToLine({ 51100, 84000, 45300 });
		AddPointToLine({ 48100, 85500, 42800 });
		AddPointToLine({ 45100, 87500, 40800 });
		AddPointToLine({ 41600, 89000, 38300 });


		// ============================================================
		// ВЕТВЬ 09 — ВВЕРХ И ВПРАВО
		// ============================================================

		NewLine();
		AddPointToLine({ 64503, 74500, 54039 });
		AddPointToLine({ 66500, 76500, 55500 });
		AddPointToLine({ 69000, 79000, 56500 });
		AddPointToLine({ 71500, 81500, 55000 });
		AddPointToLine({ 74000, 84500, 57000 });
		AddPointToLine({ 76500, 87500, 55500 });
		AddPointToLine({ 78500, 90500, 57500 });

		NewLine();
		AddPointToLine({ 65050, 75000, 54700 });
		AddPointToLine({ 67050, 77000, 56200 });
		AddPointToLine({ 69550, 79500, 57200 });
		AddPointToLine({ 72050, 82000, 55700 });
		AddPointToLine({ 74550, 85000, 57700 });
		AddPointToLine({ 77050, 88000, 56200 });
		AddPointToLine({ 79050, 91000, 58200 });

		NewLine();
		AddPointToLine({ 63956, 75000, 54700 });
		AddPointToLine({ 65950, 77000, 54800 });
		AddPointToLine({ 68450, 79500, 55800 });
		AddPointToLine({ 70950, 82000, 54300 });
		AddPointToLine({ 73450, 85000, 56300 });
		AddPointToLine({ 75950, 88000, 54800 });
		AddPointToLine({ 77950, 91000, 56800 });

		NewLine();
		AddPointToLine({ 64503, 74500, 53289 });
		AddPointToLine({ 66500, 76500, 54750 });
		AddPointToLine({ 69000, 79000, 55750 });
		AddPointToLine({ 71500, 81500, 54250 });
		AddPointToLine({ 74000, 84500, 56250 });
		AddPointToLine({ 76500, 87500, 54750 });
		AddPointToLine({ 78500, 90500, 56750 });

		NewLine();
		AddPointToLine({ 63956, 74000, 53300 });
		AddPointToLine({ 65950, 76000, 54800 });
		AddPointToLine({ 68450, 78500, 55800 });
		AddPointToLine({ 70950, 81000, 54300 });
		AddPointToLine({ 73450, 84000, 56300 });
		AddPointToLine({ 75950, 87000, 54800 });
		AddPointToLine({ 77950, 90000, 56800 });

		NewLine();
		AddPointToLine({ 65050, 74000, 53300 });
		AddPointToLine({ 67050, 76000, 54800 });
		AddPointToLine({ 69550, 78500, 55800 });
		AddPointToLine({ 72050, 81000, 54300 });
		AddPointToLine({ 74550, 84000, 56300 });
		AddPointToLine({ 77050, 87000, 54800 });
		AddPointToLine({ 79050, 90000, 56800 });


		// ============================================================
		// ВЕТВЬ 10 — ВВЕРХ И ВЛЕВО
		// ============================================================

		NewLine();
		AddPointToLine({ 55903, 74500, 54039 });
		AddPointToLine({ 54000, 76500, 52500 });
		AddPointToLine({ 51500, 79000, 53500 });
		AddPointToLine({ 49000, 81500, 52000 });
		AddPointToLine({ 46500, 84500, 54000 });
		AddPointToLine({ 44000, 87500, 52500 });
		AddPointToLine({ 41500, 90500, 54500 });

		NewLine();
		AddPointToLine({ 56450, 75000, 54700 });
		AddPointToLine({ 54550, 77000, 53200 });
		AddPointToLine({ 52050, 79500, 54200 });
		AddPointToLine({ 49550, 82000, 52700 });
		AddPointToLine({ 47050, 85000, 54700 });
		AddPointToLine({ 44550, 88000, 53200 });
		AddPointToLine({ 42050, 91000, 55200 });

		NewLine();
		AddPointToLine({ 55356, 75000, 54700 });
		AddPointToLine({ 53450, 77000, 51800 });
		AddPointToLine({ 50950, 79500, 52800 });
		AddPointToLine({ 48450, 82000, 51300 });
		AddPointToLine({ 45950, 85000, 53300 });
		AddPointToLine({ 43450, 88000, 51800 });
		AddPointToLine({ 40950, 91000, 53800 });

		NewLine();
		AddPointToLine({ 55903, 74500, 53289 });
		AddPointToLine({ 54000, 76500, 51750 });
		AddPointToLine({ 51500, 79000, 52750 });
		AddPointToLine({ 49000, 81500, 51250 });
		AddPointToLine({ 46500, 84500, 53250 });
		AddPointToLine({ 44000, 87500, 51750 });
		AddPointToLine({ 41500, 90500, 53750 });

		NewLine();
		AddPointToLine({ 55356, 74000, 53300 });
		AddPointToLine({ 53450, 76000, 51800 });
		AddPointToLine({ 50950, 78500, 52800 });
		AddPointToLine({ 48450, 81000, 51300 });
		AddPointToLine({ 45950, 84000, 53300 });
		AddPointToLine({ 43450, 87000, 51800 });
		AddPointToLine({ 40950, 90000, 53800 });

		NewLine();
		AddPointToLine({ 56450, 74000, 53300 });
		AddPointToLine({ 54550, 76000, 51800 });
		AddPointToLine({ 52050, 78500, 52800 });
		AddPointToLine({ 49550, 81000, 51300 });
		AddPointToLine({ 47050, 84000, 53300 });
		AddPointToLine({ 44550, 87000, 51800 });
		AddPointToLine({ 42050, 90000, 53800 });


		// ============================================================
		// ДОПОЛНИТЕЛЬНЫЕ ОТВЕТВЛЕНИЯ ОТ СУЩЕСТВУЮЩИХ ВЕТОК
		// Каждое ответвление — отдельная труба из 6 жил
		// ============================================================


		// ------------------------------------------------------------
		// ОТВЕТВЛЕНИЕ 01 — от правой ветви, вверх и вправо
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 70500, 64500, 55250 });
		AddPointToLine({ 72500, 66000, 57500 });
		AddPointToLine({ 75000, 68500, 59000 });
		AddPointToLine({ 77500, 70000, 61000 });
		AddPointToLine({ 80000, 72000, 60000 });

		NewLine();
		AddPointToLine({ 71103, 65000, 55900 });
		AddPointToLine({ 73103, 66500, 58200 });
		AddPointToLine({ 75603, 69000, 59700 });
		AddPointToLine({ 78103, 70500, 61700 });
		AddPointToLine({ 80603, 72500, 60700 });

		NewLine();
		AddPointToLine({ 69897, 65000, 55900 });
		AddPointToLine({ 71897, 66500, 58200 });
		AddPointToLine({ 74397, 69000, 59700 });
		AddPointToLine({ 76897, 70500, 61700 });
		AddPointToLine({ 79397, 72500, 60700 });

		NewLine();
		AddPointToLine({ 70500, 64500, 54500 });
		AddPointToLine({ 72500, 66000, 56800 });
		AddPointToLine({ 75000, 68500, 58300 });
		AddPointToLine({ 77500, 70000, 60300 });
		AddPointToLine({ 80000, 72000, 59300 });

		NewLine();
		AddPointToLine({ 69897, 64000, 54500 });
		AddPointToLine({ 71897, 65500, 56800 });
		AddPointToLine({ 74397, 68000, 58300 });
		AddPointToLine({ 76897, 69500, 60300 });
		AddPointToLine({ 79397, 71500, 59300 });

		NewLine();
		AddPointToLine({ 71103, 64000, 55900 });
		AddPointToLine({ 73103, 65500, 58200 });
		AddPointToLine({ 75603, 68000, 59700 });
		AddPointToLine({ 78103, 69500, 61700 });
		AddPointToLine({ 80603, 71500, 60700 });


		// ------------------------------------------------------------
		// ОТВЕТВЛЕНИЕ 02 — от правой ветви, вниз и вправо
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 74000, 84500, 57000 });
		AddPointToLine({ 76000, 86000, 54500 });
		AddPointToLine({ 78500, 88500, 53000 });
		AddPointToLine({ 81500, 90000, 54500 });
		AddPointToLine({ 84500, 92000, 52500 });

		NewLine();
		AddPointToLine({ 74603, 85000, 57700 });
		AddPointToLine({ 76603, 86500, 55200 });
		AddPointToLine({ 79103, 89000, 53700 });
		AddPointToLine({ 82103, 90500, 55200 });
		AddPointToLine({ 85103, 92500, 53200 });

		NewLine();
		AddPointToLine({ 73397, 85000, 57700 });
		AddPointToLine({ 75397, 86500, 55200 });
		AddPointToLine({ 77897, 89000, 53700 });
		AddPointToLine({ 80897, 90500, 55200 });
		AddPointToLine({ 83897, 92500, 53200 });

		NewLine();
		AddPointToLine({ 74000, 84500, 56250 });
		AddPointToLine({ 76000, 86000, 53750 });
		AddPointToLine({ 78500, 88500, 52250 });
		AddPointToLine({ 81500, 90000, 53750 });
		AddPointToLine({ 84500, 92000, 50750 });

		NewLine();
		AddPointToLine({ 73397, 84000, 56250 });
		AddPointToLine({ 75397, 85500, 53750 });
		AddPointToLine({ 77897, 88000, 52250 });
		AddPointToLine({ 80897, 89500, 53750 });
		AddPointToLine({ 83897, 91500, 50750 });

		NewLine();
		AddPointToLine({ 74603, 84000, 57700 });
		AddPointToLine({ 76603, 85500, 55200 });
		AddPointToLine({ 79103, 88000, 53700 });
		AddPointToLine({ 82103, 89500, 55200 });
		AddPointToLine({ 85103, 91500, 53200 });


		// ------------------------------------------------------------
		// ОТВЕТВЛЕНИЕ 03 — от левой ветви, вверх и влево
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 49000, 73500, 54000 });
		AddPointToLine({ 47000, 71500, 56000 });
		AddPointToLine({ 44500, 69500, 54500 });
		AddPointToLine({ 42000, 67500, 56500 });
		AddPointToLine({ 39000, 66000, 55000 });

		NewLine();
		AddPointToLine({ 49603, 74000, 54700 });
		AddPointToLine({ 47603, 72000, 56700 });
		AddPointToLine({ 45103, 70000, 55200 });
		AddPointToLine({ 42603, 68000, 57200 });
		AddPointToLine({ 39603, 66500, 55700 });

		NewLine();
		AddPointToLine({ 48397, 74000, 54700 });
		AddPointToLine({ 46397, 72000, 56700 });
		AddPointToLine({ 43897, 70000, 55200 });
		AddPointToLine({ 41397, 68000, 57200 });
		AddPointToLine({ 38397, 66500, 55700 });

		NewLine();
		AddPointToLine({ 49000, 73500, 52500 });
		AddPointToLine({ 47000, 71500, 54500 });
		AddPointToLine({ 44500, 69500, 53000 });
		AddPointToLine({ 42000, 67500, 55000 });
		AddPointToLine({ 39000, 66000, 53500 });

		NewLine();
		AddPointToLine({ 48397, 73000, 52500 });
		AddPointToLine({ 46397, 71000, 54500 });
		AddPointToLine({ 43897, 69000, 53000 });
		AddPointToLine({ 41397, 67000, 55000 });
		AddPointToLine({ 38397, 65500, 53500 });

		NewLine();
		AddPointToLine({ 49603, 73000, 54700 });
		AddPointToLine({ 47603, 71000, 56700 });
		AddPointToLine({ 45103, 69000, 55200 });
		AddPointToLine({ 42603, 67000, 57200 });
		AddPointToLine({ 39603, 65500, 55700 });


		// ------------------------------------------------------------
		// ОТВЕТВЛЕНИЕ 04 — от левой ветви, вниз и влево
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 45500, 75000, 52000 });
		AddPointToLine({ 43500, 77000, 50000 });
		AddPointToLine({ 41000, 79000, 51500 });
		AddPointToLine({ 38000, 81000, 49500 });
		AddPointToLine({ 35000, 82500, 51000 });

		NewLine();
		AddPointToLine({ 46103, 75500, 52700 });
		AddPointToLine({ 44103, 77500, 50700 });
		AddPointToLine({ 41603, 79500, 52200 });
		AddPointToLine({ 38603, 81500, 50200 });
		AddPointToLine({ 35603, 83000, 51700 });

		NewLine();
		AddPointToLine({ 44897, 75500, 52700 });
		AddPointToLine({ 42897, 77500, 50700 });
		AddPointToLine({ 40397, 79500, 52200 });
		AddPointToLine({ 37397, 81500, 50200 });
		AddPointToLine({ 34397, 83000, 51700 });

		NewLine();
		AddPointToLine({ 45500, 75000, 50500 });
		AddPointToLine({ 43500, 77000, 48500 });
		AddPointToLine({ 41000, 79000, 50000 });
		AddPointToLine({ 38000, 81000, 48000 });
		AddPointToLine({ 35000, 82500, 49500 });

		NewLine();
		AddPointToLine({ 44897, 74500, 50500 });
		AddPointToLine({ 42897, 76500, 48500 });
		AddPointToLine({ 40397, 78500, 50000 });
		AddPointToLine({ 37397, 80500, 48000 });
		AddPointToLine({ 34397, 82000, 49500 });

		NewLine();
		AddPointToLine({ 46103, 74500, 52700 });
		AddPointToLine({ 44103, 76500, 50700 });
		AddPointToLine({ 41603, 78500, 52200 });
		AddPointToLine({ 38603, 80500, 50200 });
		AddPointToLine({ 35603, 82000, 51700 });


		// ------------------------------------------------------------
		// ОТВЕТВЛЕНИЕ 05 — от передней ветви, вверх по Z
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 61000, 71500, 64500 });
		AddPointToLine({ 59000, 70000, 67000 });
		AddPointToLine({ 57000, 68000, 69500 });
		AddPointToLine({ 54500, 66500, 68000 });
		AddPointToLine({ 52000, 65000, 70500 });

		NewLine();
		AddPointToLine({ 61603, 72000, 65200 });
		AddPointToLine({ 59603, 70500, 67700 });
		AddPointToLine({ 57603, 68500, 70200 });
		AddPointToLine({ 55103, 67000, 68700 });
		AddPointToLine({ 52603, 65500, 71200 });

		NewLine();
		AddPointToLine({ 60397, 72000, 65200 });
		AddPointToLine({ 58397, 70500, 67700 });
		AddPointToLine({ 56397, 68500, 70200 });
		AddPointToLine({ 53897, 67000, 68700 });
		AddPointToLine({ 51397, 65500, 71200 });

		NewLine();
		AddPointToLine({ 61000, 71500, 63000 });
		AddPointToLine({ 59000, 70000, 65500 });
		AddPointToLine({ 57000, 68000, 68000 });
		AddPointToLine({ 54500, 66500, 66500 });
		AddPointToLine({ 52000, 65000, 69000 });

		NewLine();
		AddPointToLine({ 60397, 71000, 63000 });
		AddPointToLine({ 58397, 69500, 65500 });
		AddPointToLine({ 56397, 67500, 68000 });
		AddPointToLine({ 53897, 66000, 66500 });
		AddPointToLine({ 51397, 64500, 69000 });

		NewLine();
		AddPointToLine({ 61603, 71000, 65200 });
		AddPointToLine({ 59603, 69500, 67700 });
		AddPointToLine({ 57603, 67500, 70200 });
		AddPointToLine({ 55103, 66000, 68700 });
		AddPointToLine({ 52603, 64500, 71200 });


		// ------------------------------------------------------------
		// ОТВЕТВЛЕНИЕ 06 — от передней ветви, вниз по Z
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 60500, 75500, 70500 });
		AddPointToLine({ 62500, 77500, 73000 });
		AddPointToLine({ 64500, 79500, 75000 });
		AddPointToLine({ 67000, 81000, 73500 });
		AddPointToLine({ 69500, 83000, 76000 });

		NewLine();
		AddPointToLine({ 61103, 76000, 71200 });
		AddPointToLine({ 63103, 78000, 73700 });
		AddPointToLine({ 65103, 80000, 75700 });
		AddPointToLine({ 67603, 81500, 74200 });
		AddPointToLine({ 70103, 83500, 76700 });

		NewLine();
		AddPointToLine({ 59897, 76000, 71200 });
		AddPointToLine({ 61897, 78000, 73700 });
		AddPointToLine({ 63897, 80000, 75700 });
		AddPointToLine({ 66397, 81500, 74200 });
		AddPointToLine({ 68897, 83500, 76700 });

		NewLine();
		AddPointToLine({ 60500, 75500, 69000 });
		AddPointToLine({ 62500, 77500, 71500 });
		AddPointToLine({ 64500, 79500, 73500 });
		AddPointToLine({ 67000, 81000, 72000 });
		AddPointToLine({ 69500, 83000, 74500 });

		NewLine();
		AddPointToLine({ 59897, 75000, 69000 });
		AddPointToLine({ 61897, 77000, 71500 });
		AddPointToLine({ 63897, 79000, 73500 });
		AddPointToLine({ 66397, 80500, 72000 });
		AddPointToLine({ 68897, 82500, 74500 });

		NewLine();
		AddPointToLine({ 61103, 75000, 71200 });
		AddPointToLine({ 63103, 77000, 73700 });
		AddPointToLine({ 65103, 79000, 75700 });
		AddPointToLine({ 67603, 80500, 74200 });
		AddPointToLine({ 70103, 82500, 76700 });


		// ------------------------------------------------------------
		// ОТВЕТВЛЕНИЕ 07 — от диагональной ветви, влево + вверх
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 69500, 77500, 61500 });
		AddPointToLine({ 67500, 75500, 63500 });
		AddPointToLine({ 65000, 73500, 62000 });
		AddPointToLine({ 62500, 71500, 64500 });
		AddPointToLine({ 59500, 70000, 63000 });

		NewLine();
		AddPointToLine({ 70103, 78000, 62200 });
		AddPointToLine({ 68103, 76000, 64200 });
		AddPointToLine({ 65603, 74000, 62700 });
		AddPointToLine({ 63103, 72000, 65200 });
		AddPointToLine({ 60103, 70500, 63700 });

		NewLine();
		AddPointToLine({ 68897, 78000, 62200 });
		AddPointToLine({ 66897, 76000, 64200 });
		AddPointToLine({ 64397, 74000, 62700 });
		AddPointToLine({ 61897, 72000, 65200 });
		AddPointToLine({ 58897, 70500, 63700 });

		NewLine();
		AddPointToLine({ 69500, 77500, 60000 });
		AddPointToLine({ 67500, 75500, 62000 });
		AddPointToLine({ 65000, 73500, 60500 });
		AddPointToLine({ 62500, 71500, 63000 });
		AddPointToLine({ 59500, 70000, 61500 });

		NewLine();
		AddPointToLine({ 68897, 77000, 60000 });
		AddPointToLine({ 66897, 75000, 62000 });
		AddPointToLine({ 64397, 73000, 60500 });
		AddPointToLine({ 61897, 71000, 63000 });
		AddPointToLine({ 58897, 69500, 61500 });

		NewLine();
		AddPointToLine({ 70103, 77000, 62200 });
		AddPointToLine({ 68103, 75000, 64200 });
		AddPointToLine({ 65603, 73000, 62700 });
		AddPointToLine({ 63103, 71000, 65200 });
		AddPointToLine({ 60103, 69500, 63700 });


		// ------------------------------------------------------------
		// ОТВЕТВЛЕНИЕ 08 — от нижней диагонали, вправо + вверх
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 73500, 88000, 47000 });
		AddPointToLine({ 75500, 86000, 45000 });
		AddPointToLine({ 78000, 84500, 46500 });
		AddPointToLine({ 81000, 83000, 44500 });
		AddPointToLine({ 84000, 81500, 46000 });

		NewLine();
		AddPointToLine({ 74103, 88500, 47700 });
		AddPointToLine({ 76103, 86500, 45700 });
		AddPointToLine({ 78603, 85000, 47200 });
		AddPointToLine({ 81603, 83500, 45200 });
		AddPointToLine({ 84603, 82000, 46700 });

		NewLine();
		AddPointToLine({ 72897, 88500, 47700 });
		AddPointToLine({ 74897, 86500, 45700 });
		AddPointToLine({ 77397, 85000, 47200 });
		AddPointToLine({ 80397, 83500, 45200 });
		AddPointToLine({ 83397, 82000, 46700 });

		NewLine();
		AddPointToLine({ 73500, 88000, 45500 });
		AddPointToLine({ 75500, 86000, 43500 });
		AddPointToLine({ 78000, 84500, 45000 });
		AddPointToLine({ 81000, 83000, 43000 });
		AddPointToLine({ 84000, 81500, 44500 });

		NewLine();
		AddPointToLine({ 72897, 87500, 45500 });
		AddPointToLine({ 74897, 85500, 43500 });
		AddPointToLine({ 77397, 84000, 45000 });
		AddPointToLine({ 80397, 82500, 43000 });
		AddPointToLine({ 83397, 81000, 44500 });

		NewLine();
		AddPointToLine({ 74103, 87500, 47700 });
		AddPointToLine({ 76103, 85500, 45700 });
		AddPointToLine({ 78603, 84000, 47200 });
		AddPointToLine({ 81603, 82500, 45200 });
		AddPointToLine({ 84603, 81000, 46700 });

		/*NewLine();
		AddPointToLine({ -39,-34,3 });
		AddPointToLine({ 100,0,10 });
		AddPointToLine({ 200,0,13 });
		AddPointToLine({ 200,110,13 });
		AddPointToLine({ 100,110,3 });
		AddPointToLine({ -32,-4,3 });

		NewLine();
		AddPointToLine({ -44,-5,-5 });
		AddPointToLine({ -11,-32,-4 });
		AddPointToLine({ 22,-16,25 });
		AddPointToLine({ 22,17,13 });
		AddPointToLine({ -3,31,11 });
		AddPointToLine({ -33,16,13 });*/

		// ============================================================
// STARS
// ============================================================

// A — нижняя левая
		NewStar({
			.x = 110000,
			.y = 52000,
			.z = 50000,
			.rad = 6630,
			.r = 0,
			.g = 4,
			.b = 100,
			.brightness = 100
			});


		// B — выше A, левее D
		NewStar({
			.x = 3729,
			.y = 52000,
			.z = 50000,
			.rad = 6630,
			.r = 0,
			.g = 4,
			.b = 0,
			.brightness = 100
			});
		//
		//
		//// D — выше и правее B
		//NewStar({
		//	.x = 51876,
		//	.y = 74141,
		//	.z = 67000,
		//	.rad = 6630,
		//	.r = 0,
		//	.g = 4,
		//	.b = 100,
		//	.brightness = 100
		//	});
		//
		//
		//// E — самая высокая и левая
		//NewStar({
		//	.x = 15000,
		//	.y = 90000,
		//	.z = 95000,
		//	.rad = 6630,
		//	.r = 0,
		//	.g = 4,
		//	.b = 100,
		//	.brightness = 100
		//	});
		//
		//
		//// C — справа и значительно ниже D
		//NewStar({
		//	.x = 85000,
		//	.y = 48000,
		//	.z = 48000,
		//	.rad = 6630,
		//	.r = 0,
		//	.g = 4,
		//	.b = 100,
		//	.brightness = 100
		//	});

		/*
		for (int j = 0; j < 10; j++)
		{
			NewLine();
			int g = 10;
			float r = 1 + j/7.;
			for (int i = 0; i < g; i++)
			{

				float x = sin(2 * PI * i / (float)g);
				float y = cos(2 * PI * i / (float)g);
				x *= r;
				y *= r;
				int _x = x * 40;
				int _y = y * 40;
				AddPointToLine({ _x,_y,0 });
			}
		}*/

		/*
		for (int j = 0; j < 10; j++)
		{
			NewLine();
			int g = 10;
			float r = 1.0f + j / 7.0f;

			// 1. Генерируем случайные углы для полноценного 3D-разворота плоскости
			float alpha = ((float)rand() / RAND_MAX) * XM_2PI; // Вокруг Z
			float beta = ((float)rand() / RAND_MAX) * XM_PI;  // Наклон (Вокруг X)
			float gamma = ((float)rand() / RAND_MAX) * XM_2PI; // Вокруг новой Z

			// 2. Создаем матрицы вращения d3d11 и объединяем их в одну общую матрицу трансформации
			XMMATRIX rotZ1 = XMMatrixRotationZ(alpha);
			XMMATRIX rotX = XMMatrixRotationX(beta);
			XMMATRIX rotZ2 = XMMatrixRotationZ(gamma);

			// В DirectX матрицы перемножаются слева направо: Сначала Z2, потом наклон X, потом Z1
			XMMATRIX finalRotation = rotZ2 * rotX * rotZ1;

			for (int i = 0; i < g; i++)
			{
				// 3. Базовая точка на плоском кольце (Z = 0)
				float angle = XM_2PI * i / (float)g;
				XMVECTOR basePoint = XMVectorSet(r * sinf(angle), r * cosf(angle), 0.0f, 1.0f);

				// 4. Умножаем вектор на матрицу вращения
				XMVECTOR rotatedPoint = XMVector3Transform(basePoint, finalRotation);

				// 5. Извлекаем данные, масштабируем (* 40) и приводим к int
				int _x = (int)(XMVectorGetX(rotatedPoint) * 40.0f);
				int _y = (int)(XMVectorGetY(rotatedPoint) * 40.0f);
				int _z = (int)(XMVectorGetZ(rotatedPoint) * 40.0f);

				AddPointToLine({ _x, _y, _z });
			}
		}
		*/

		/*
		for (int j = 0; j < 10; j++)
		{
			NewLine();

			// g = 12 идеально для гладкой петли Катмулла-Рома
			int g = 12;
			// ТВОЙ ИСХОДНЫЙ МАСШТАБ
			float r = 1.0f + j / 7.0f;

			// Фиксируем сид для каждого протуберанца
			unsigned int starSeed = 101 + j;
			srand(starSeed);

			// Случайные углы 3D-гироскопа, чтобы петли торчали из звезды в разные стороны
			float alpha = ((float)rand() / RAND_MAX) * XM_2PI;
			float beta = ((float)rand() / RAND_MAX) * XM_PI;
			float gamma = ((float)rand() / RAND_MAX) * XM_2PI;

			XMMATRIX rotZ1 = XMMatrixRotationZ(alpha);
			XMMATRIX rotX = XMMatrixRotationX(beta);
			XMMATRIX rotZ2 = XMMatrixRotationZ(gamma);
			XMMATRIX finalRotation = rotZ2 * rotX * rotZ1;

			for (int i = 0; i < g; i++)
			{
				// Пускаем angle по полному кругу, но за счет сдвига это будет петля,
				// растущая ИЗ центра
				float angle = XM_2PI * i / (float)(g - 1);

				// ГЕОМЕТРИЧЕСКИЙ ТРЮК:
				// Строим базовый круг, но сдвигаем его по Y на величину радиуса (+ r).
				// Теперь при угле angle = PI точка (0, -r + r) окажется ровно в координатах (0, 0, 0) - т.е. в центре звезды.
				float x_raw = r * sinf(angle);
				float y_raw = r * cosf(angle) + r; // <-- СДВИГ ОСИ ВЫТАЛКИВАНИЯ
				float z_raw = 0.0f;

				XMVECTOR basePoint = XMVectorSet(x_raw, y_raw, z_raw, 1.0f);

				// Поворачиваем петлю. Теперь она вращается НЕ вокруг своего геометрического центра,
				// а вокруг точки своего основания, которая привязана к ядру звезды.
				XMVECTOR rotatedPoint = XMVector3Transform(basePoint, finalRotation);

				// ТВОЙ ИСХОДНЫЙ МАСШТАБ ДЛЯ ДВИЖКА (* 40)
				int _x = (int)(XMVectorGetX(rotatedPoint) * 40.0f);
				int _y = (int)(XMVectorGetY(rotatedPoint) * 40.0f);
				int _z = (int)(XMVectorGetZ(rotatedPoint) * 40.0f);

				AddPointToLine({ _x, _y, _z });
			}
		}*/

		// Большой внешний цикл — хаотично рассыпаем 100 прямых лазерных штрихов

		/*
		for (int l = 0; l < starLineList.lineCount; l++)
		{
			if (starLineList.line[l].basePointCount != 1) continue;

			int raysCount = 140;
			for (int j = 0; j < raysCount; j++)
			{
				NewLine();

				// Прямая линия из 5 точек. Для идеального лерпа этого достаточно,
				// сплайн Катмулла-Рома прорисует её как ровную световую струну


				// Случайный масштаб (расстояние от центра звезды до начала луча)
				float randScale = (float)rand() / RAND_MAX;
				float r_start = (3.0f + (randScale * 10.0f) / 7.0f) * 0.35f;
				r_start = starLineList.line[l].basePoint[0].w * denom;

				// Длина самого лазерного штриха (в твоих пропорциях, например, небольшая фиксированная длина)
				float rayLength = 4111.2f;
				float r_end = r_start + rayLength;

				int g = 5;

				// Рандомный разворот всей линии целиком по двум осям (Yaw и Pitch)
				float yaw = ((float)rand() / RAND_MAX) * XM_2PI; // Поворот вокруг Y
				float pitch = ((float)rand() / RAND_MAX) * XM_PI;  // Наклон вокруг X

				auto yp = GetJitteredSphereAngle(j, raysCount, 0);
				yaw = yp.x;
				pitch = yp.y;

				//yaw = XM_2PI* j / (float)raysCount;
				//pitch = XM_PI * j / (float)raysCount;
				//pitch = 0;

				XMMATRIX finalRotation = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);

				for (int i = 0; i < g; i++)
				{
					// t строго от 0.0 (начало штриха) до 1.0 (конец штриха)
					float t = (float)i / (float)(g - 1);

					// ЧЕСТНЫЙ ЛЕРП: линия абсолютно прямая и направлена строго вдоль оси Y наружу
					float x_raw = 0.0f;
					float y_raw = r_start + (r_end - r_start) * t;
					float z_raw = 0;

					XMVECTOR basePoint = XMVectorSet(x_raw, y_raw, z_raw, 1.0f);

					// Поворачиваем всю прямую линию одинаково
					XMVECTOR rotatedPoint = XMVector3Transform(basePoint, finalRotation);

					// ТВОЙ ИСХОДНЫЙ МАСШТАБ (* 40)
					int _x = (int)XMVectorGetX(rotatedPoint);
					int _y = (int)XMVectorGetY(rotatedPoint);
					int _z = (int)XMVectorGetZ(rotatedPoint);

					_x += starLineList.line[l].basePoint->x * denom;
					_y += starLineList.line[l].basePoint->y * denom;
					_z += starLineList.line[l].basePoint->z * denom;


						AddPointToLine({ _x, _y, _z });
				}
			}
		}*/

		//------------end user space---------------
		//-----------------------------------------

		for (int j = 0; j < starLineList.lineCount; j++)
		{
			if (starLineList.line[j].basePointCount > 1)
			{
				smoothStarline(starLineList.line[j]);
			}
			//Starline(starLineList.line[j], 3*12. / starLineList.line[j].basePointCount);
		}

		/*pathTime /= 100.;

		for (int j = 0; j < 3; j++)
		{
			ln = j;
			SetPointCountInLine({ ln,7 });
			pt = 0;
			for (int i = 0; i < 7; i++)
			{
				float4 pos;
				float amp = 10 * 10000 * ((i+1) / (j + .3) + 4);
				//pos.x = amp * sin(i * 13 + pathTime);
				//pos.y = amp * cos(i * 14 + pathTime);
				//pos.z = amp * sin(i * 15 + pathTime);

				auto v = GetPerlinNoiseVector3(ln+i * .13 + pathTime, ln+i * .25 + pathTime, ln+i * .37 + pathTime) * amp;
				pos.x = XMVectorGetX(v);
				pos.y = XMVectorGetY(v);
				pos.z = XMVectorGetZ(v);

				SetPointPosInLine({ ln,pt++, (int)pos.x,(int)pos.y,(int)pos.z,0,10000 });

			}

			smoothStarline(starLineList.line[ln], 7);
		}


		//
		*/
	}

	float4 hero_pos;

	cmd(Maze, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		for (int i = 0; i < starLineList.lineCount; i++)
		{
			if (starLineList.line[i].pointCount == 1) continue;//skip all lone stars

			vs::maze = {
				.params = {
					.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
					.gX = gX,
					.gY = gY,
					.mode = (int)in.mode,
					.skipper = in.skipper,
					.heroPosition = hero_pos,
					.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1),
				},
			};

			//vs::maze.params.particlesCount = in.count;
			int count = starLineList.line[i].pointCount * 10000.;
			vs::maze.params.particlesCount = count / in.skipper;
			vs::maze.params.basePointsCount = starLineList.line[i].pointCount;

			for (int j = 0; j < starLineList.line[i].pointCount; j++)
			{
				vs::maze.params.basePoint[j] = starLineList.line[i].point[j];
			}

			vs::maze.set();

			Drawer::NullDrawer({ 1,count / in.skipper });
		}


	}

	int starStencilTarget = 0;

	cmd(AllStars, int count, int skipper, pMode mode, int r, int g, int b, triMode tMode)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		if (in.tMode == triMode::on)
		{
			Culling::Set({ cullmode::front });
			DepthBuf::Mode({ depthmode::off });

			if (starStencilTarget == 2)
			{
				DepthBuf::Mode({ depthmode::off });
			}

			BlendMode::Set({
				.mode = blendmode::on,
				.op = blendop::add
				});
		}
		else
		{
			psModeSet2(in.mode);

			Culling::Set({ cullmode::off });
			DepthBuf::Mode({ depthmode::readonly });
			BlendMode::Set({
				.mode = blendmode::on,
				.op = blendop::add
				});
		}

		for (int i = 0; i < starLineList.lineCount; i++)
		{
			if (starLineList.line[i].pointCount != 1) continue;//skip all paths

			auto sd = starLineList.line[i].point[0];

			int count = 500000;

			int w = sd.w;
			if (in.tMode == triMode::on)
			{
				gX = 64;
				gY = 32;


				if (starStencilTarget == 1)
				{
					//	w *= .975;
				}

				if (starStencilTarget == 2)
				{
					//w *= 1.15;
				}

				auto sd = starLineList.line[i].point[0];
				ps::starTri.params = {
						.PosRad = float4(sd.x,sd.y,sd.z,sd.w)

				};

				ps::starTri.set();

			}

			vs::star = {
				.params = {
					.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
					.gX = gX,
					.gY = gY,
					.mode = (int)in.mode,
					.skipper = 0,
					.base_color = starLineList.line[i].baseColor,
					.PosRad = float4(sd.x,sd.y,sd.z,w),
					.triMode = (int)in.tMode
				},
			};


			vs::star.set();

			if (in.tMode == triMode::on) {
				Drawer::NullDrawerTri({ gX * gY * 2, 1 });
			}
			else
			{
				Drawer::NullDrawer({ 1,in.count / in.skipper });
			}

		}

		psModeSet(in.mode);

		Culling::Set({ cullmode::off });
		DepthBuf::Mode({ depthmode::readonly });
		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});
	}

	cmd(Rocks, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::rocks = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::rocks.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });


	}

	cmd(Transporter, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::transporter = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::transporter.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });


	}

	cmd(Islands, int count; int skipper; pMode mode; int r; int g; int b;)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::islands = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::islands.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });


	}

	cmd(Waterfall, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::waterfall = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1)
			},
		};

		vs::waterfall.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });


	}

	void DoubleStar(int count, int skipper, pMode mode)
	{
		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);

		vs::fish = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
			},
		};

		vs::fish.set();

		Drawer::NullDrawer({ 1, (int)gX * (int)gY });
	}

	void Tree(int count, int skipper, pMode mode)
	{
		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);

		vs::tree = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
			},
		};

		vs::tree.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });
	}

	void Libra_spheres(int count, int skipper, pMode mode)
	{
		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);

		vs::libra_sph = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
			},
		};

		vs::libra_sph.set();

		Drawer::NullDrawer({ 1, (int)gX * (int)gY });
	}

	void Pillars(int count, int skipper, pMode mode)
	{
		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);

		vs::pillars = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
			},
		};

		vs::pillars.set();

		Drawer::NullDrawer({ 1, (int)gX * (int)gY });
	}

	void OuterSpace(int count, int skipper, pMode mode)
	{
		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);

		vs::space = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
			},
		};

		vs::space.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });
	}

	void NeutronStar(int count, int skipper, pMode mode)
	{
		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);

		vs::neitron_star = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)mode,
				.skipper = skipper,
			},
		};

		vs::neitron_star.set();

		Drawer::NullDrawer({ 1, (int)gX * (int)gY });
	}

	cmd(DoubleTwo, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;
		float r = in.r / 100.f;
		float g = in.g / 100.f;
		float b = in.b / 100.f;
		float4 base_color = float4(r, g, b, 1);

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::galaxy_2 = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = base_color
			},
		};

		vs::galaxy_2.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });

	}

	cmd(Galaxy, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;
		float r = in.r / 100.f;
		float g = in.g / 100.f;
		float b = in.b / 100.f;
		float4 base_color = float4(r, g, b, 1);

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		vs::galaxy = {
			.params = {
				.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
				.gX = gX,
				.gY = gY,
				.mode = (int)in.mode,
				.skipper = in.skipper,
				.base_color = base_color
			},
		};

		vs::galaxy.set();

		Drawer::NullDrawer({ 1,(int)gX * (int)gY });

	}

	cmd(Libra, int quality)
	{
		reflect;

		int pillars_cnt = 1000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Tree(pillars_cnt, 1, pMode::point);
		Libra_spheres(256 * 256, 1, pMode::point);
		InsideNebula({ pillars_cnt,1,pMode::point,100,252,400 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

		//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		Galaxy({ galaxy_cnt, 4, pMode::glow ,254,220,41 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Tree(pillars_cnt, 1394 / 2, pMode::glow);
		InsideNebula({ pillars_cnt,1394,pMode::glow,100,202,400 });
		Libra_spheres(256 * 256, 143, pMode::glow);
		OuterSpace(outerSpace_cnt, 64, pMode::glow);


	}

	cmd(Aquarius, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Blob({ pillars_cnt,1,pMode::point,100,252,500 });
		Islands({ pillars_cnt / 2,1,pMode::point,130,112,10 });
		Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

				//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		Blob({ pillars_cnt,194,pMode::glow,100,252,600 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);


	}

	cmd(Crab, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Pearl({ pillars_cnt,1,pMode::point,600,252,100 });
		InsideNebula({ pillars_cnt / 2, 1, pMode::point ,1500,100,00 });
		//Islands({ pillars_cnt / 2,1,pMode::point,130,112,10 });
		//Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

				//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		Pearl({ pillars_cnt,194,pMode::glow,600,252,100 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);


	}

	cmd(LeoBigStar, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		LeoStar({ pillars_cnt,1,pMode::point,600,252,100 });
		//InsideNebula({ pillars_cnt / 2, 1, pMode::point ,1500,100,00 });
		//Islands({ pillars_cnt / 2,1,pMode::point,130,112,10 });
		//Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

				//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		LeoStar({ pillars_cnt,194,pMode::glow,600,252,100 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);


	}

	cmd(Capri, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		CapStar({ pillars_cnt,1,pMode::point,100,252,1400 });
		//InsideNebula({ pillars_cnt / 2, 1, pMode::point ,1500,100,00 });
		//Islands({ pillars_cnt / 2,1,pMode::point,130,112,10 });
		//Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

				//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		CapStar({ pillars_cnt,194,pMode::glow,100,252,1400 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);


	}

	cmd(Taurus, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Tau({ pillars_cnt,1,pMode::point,100,252,1400 });
		//InsideNebula({ pillars_cnt / 2, 1, pMode::point ,1500,100,00 });
		//Islands({ pillars_cnt / 2,1,pMode::point,130,112,10 });
		//Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

				//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		Tau({ pillars_cnt,194,pMode::glow,100,252,1400 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);


	}






	cmd(Girl, int quality, int xPos, int yPos, int zPos, int brightness, int tickness, switcher stencil)
	{
		reflect;

		//Object::Capri({ .quality = 1 });

		int pillars_cnt2 = 2000 * 1000;

		int pillars_cnt = 3725442 / in.quality;
		int outerSpace_cnt = 6853 / in.quality;
		int neutronStar_cnt = 279620 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		starStencilTarget = 0;
		AllStars({ 200000,1,pMode::point,1000,200,10,triMode::on });

		Culling::Set({ cullmode::off });
		DepthBuf::Mode({ depthmode::readonly });
		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});

		OuterSpace(outerSpace_cnt, 1, pMode::point);

		//AllStars({ 200000,1,pMode::point,26,11,2,triMode::off });

		//vrg({ pillars_cnt/2,1,pMode::point,1390,925,111 });
		Maze({ 200000,1,pMode::point,1390,925,111 });


		//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		starStencilTarget = 1;

		//AllStars({ 200000,1,pMode::point,0,0,0,triMode::on });

		Culling::Set({ cullmode::off });
		DepthBuf::Mode({ depthmode::readonly });
		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});
		//AllStars({ 200000,1,pMode::point,26,11,2,triMode::off });

		//vrg({ pillars_cnt,94,pMode::glow,20,30,75 });
		//Maze({ 200000,94,pMode::glow,20,30,75 });

		//AllStars({ 200000,94,pMode::glow,20,30,75,triMode::off });
		//AllStars({ 200000,1,pMode::point,1390,925,111,triMode::on });

		//Galaxy({ galaxy_cnt, 4, pMode::glow ,100,200,300 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		starStencilTarget = 2;
		//AllStars({ 200000,1,pMode::point,7,2,0,triMode::on });

		Culling::Set({ cullmode::off });
		DepthBuf::Mode({ depthmode::readonly });
		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});

		//AllStars({ 200000,1,pMode::glow,1390,925,111,triMode::off });
		//Pillars(pillars_cnt, 10394, pMode::glow);
		OuterSpace(outerSpace_cnt, 64, pMode::glow);

		//AllStars({ 200000,1,pMode::point,26,11,2,triMode::off });
		//------------------
		//hi



	}

	cmd(Scorpio, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });


		ScorpBall({ pillars_cnt / 2,1,pMode::point,1390,925,111 });
		Nebula2({ pillars_cnt,1,pMode::point,1390,925,111 });
		//InsideNebula({ pillars_cnt , 1, pMode::point ,220,130,175 });
		//Islands({ pillars_cnt / 2,1,pMode::point,130,112,10 });
		//Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

				//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//ScorpBall({ pillars_cnt,1,pMode::point,1390,925,111 });
		//ScorpBall({ pillars_cnt,94,pMode::glow,1,10,5 });
		Nebula2({ pillars_cnt,94,pMode::glow,20,30,75 });
		//InsideNebula({ pillars_cnt , 134, pMode::glow ,40,16,10 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);


	}

	cmd(Virgo, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		vrg({ pillars_cnt,1,pMode::point,1390,925,111 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

		//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		vrg({ pillars_cnt,94,pMode::glow,20,30,75 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		OuterSpace(outerSpace_cnt, 64, pMode::glow);


	}

	cmd(Aries, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Rocks({ pillars_cnt,1,pMode::point,1390,925,111 });
		//InsideNebula({ pillars_cnt , 1, pMode::point ,220,130,175 });
		//Islands({ pillars_cnt / 2,1,pMode::point,130,112,10 });
		//Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

				//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		Rocks({ pillars_cnt,194,pMode::glow,20,30,75 });
		//InsideNebula({ pillars_cnt , 134, pMode::glow ,40,16,10 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);


	}

	cmd(Twins, int quality)
	{
		reflect;

		int pillars_cnt = 2000 * 1000;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Transporter({ pillars_cnt,1,pMode::point,1390,925,111 });
		InsideNebula({ pillars_cnt , 1, pMode::point ,220,130,175 });
		//Islands({ pillars_cnt / 2,1,pMode::point,130,112,10 });
		//Waterfall({ pillars_cnt / 4,1,pMode::point,30,352,1100 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//		Galaxy({ galaxy_cnt,14,pMode::point,254,220,41 });

				//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		Transporter({ pillars_cnt,194,pMode::glow,2,3,7 });
		InsideNebula({ pillars_cnt , 134, pMode::glow ,10,26,40 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//Blob({ pillars_cnt,1394,pMode::glow,100,252,600 });
		OuterSpace(outerSpace_cnt, 64, pMode::glow);


	}

	cmd(Fish, int quality)
	{
		reflect;

		int pillars_cnt = 3725470 / in.quality;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;
		int galaxy_cnt2 = 2182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//DoubleStar(pillars_cnt, 1, pMode::point);
		DoubleTwo({ pillars_cnt, 1, pMode::point,90,130,800 });
		//InsideNebula({ pillars_cnt / 2, 1, pMode::point ,100,200,600});
		OuterSpace(outerSpace_cnt, 1, pMode::point);

		//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });
		DoubleTwo({ galaxy_cnt, 25, pMode::glow, 100, 200, 300 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		DoubleTwo({ galaxy_cnt, 2, pMode::glow,20,40,160 });

		//InsideNebula({pillars_cnt / 2, 1394, pMode::glow, 100, 200, 600});
		OuterSpace(outerSpace_cnt, 64, pMode::glow);


	}

	cmd(Zenith, int quality)
	{
		reflect;

		int pillars_cnt = 3725470 / 2 / in.quality;
		int outerSpace_cnt = 6853 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		PillarsHand(pillars_cnt, 1, pMode::point);
		InsideNebula({ pillars_cnt, 1, pMode::point,100,200,600 });
		OuterSpace(outerSpace_cnt, 1, pMode::point);

		//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		PillarsHand(pillars_cnt, 1394 / 2, pMode::glow);
		InsideNebula({ pillars_cnt, 1394, pMode::glow ,100,200,600 });
		//	OuterSpace(outerSpace_cnt, 64, pMode::glow);


	}

	cmd(Saggitarius, int quality)
	{
		reflect;

		int pillars_cnt = 3725470 / in.quality;
		int outerSpace_cnt = 6853 / in.quality;
		int neutronStar_cnt = 279620 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Pillars(pillars_cnt, 1, pMode::point);
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		NeutronStar(neutronStar_cnt, 1, pMode::point);

		Galaxy({ galaxy_cnt, 14, pMode::point ,100,200,300 });

		//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		RenderTarget::Clear({ 0,0,0,0 });


		Galaxy({ galaxy_cnt, 4, pMode::glow ,100,200,300 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Pillars(pillars_cnt, 10394, pMode::glow);
		OuterSpace(outerSpace_cnt, 64, pMode::glow);
		//NeutronStar(1024 * 1024, 1, pMode::glow);
		//Galaxy(182361, 4, pMode::glow);


	}

	cmd(CalcNormals, texture srcGeomerty, texture targetNrml)
	{
		reflect;

		RenderTarget::Set({ in.targetNrml, 0 });

		vs::quad.set();

		ps::genNormals = {

			.textures = {
				.geo = in.srcGeomerty
			},

			.samplers = {
				.sam1Filter = filter::linear,
				.sam1AddressU = addr::wrap,
				.sam1AddressV = addr::wrap
			}
		};

		ps::genNormals.set();

		Drawer::NullDrawer({ 1, 1 });
		RenderTarget::GenerateMips({});



	}

	cmd(Calc, texture targetGeo, texture targetNrml)
	{
		reflect;

		BlendMode::Set({ blendmode::off, blendop::add });
		Culling::Set({ cullmode::off });
		RenderTarget::Set({ in.targetGeo,0 });
		DepthBuf::Mode({ depthmode::off });

		//pos
		vs::quad.set();
		ps::cat.set();
		Drawer::NullDrawer({ 1, 1 });
		RenderTarget::GenerateMips({});

		//normals
		CalcNormals({ in.targetGeo, in.targetNrml });


	}

}
