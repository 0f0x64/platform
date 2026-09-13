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
// ПОВЕРХНОСТЬ 0°
// Правая сторона ствола
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
		// ПОВЕРХНОСТЬ 60°
		// Передняя правая сторона
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
		AddPointToLine({ 60437, 40660, 56797 });
		AddPointToLine({ 60804, 42000, 56959 });

		NewLine();
		AddPointToLine({ 60883, 42300, 56995 });
		AddPointToLine({ 61219, 43600, 57142 });
		AddPointToLine({ 61538, 44900, 57281 });
		AddPointToLine({ 61839, 46200, 57412 });
		AddPointToLine({ 62120, 47500, 57534 });

		NewLine();
		AddPointToLine({ 62182, 47800, 57561 });
		AddPointToLine({ 62445, 49140, 57675 });
		AddPointToLine({ 62684, 50480, 57778 });
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
		AddPointToLine({ 62353, 74500, 57762 });

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
		AddPointToLine({ 58167, 90000, 55997 });


		// ============================================================
		// ПОВЕРХНОСТЬ 120°
		// Передняя левая сторона
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
		AddPointToLine({ 56137, 40660, 56797 });
		AddPointToLine({ 56504, 42000, 56959 });

		NewLine();
		AddPointToLine({ 56583, 42300, 56995 });
		AddPointToLine({ 56919, 43600, 57142 });
		AddPointToLine({ 57238, 44900, 57281 });
		AddPointToLine({ 57539, 46200, 57412 });
		AddPointToLine({ 57820, 47500, 57534 });

		NewLine();
		AddPointToLine({ 57882, 47800, 57561 });
		AddPointToLine({ 58145, 49140, 57675 });
		AddPointToLine({ 58384, 50480, 57778 });
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
		AddPointToLine({ 58053, 74500, 57762 });

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
		AddPointToLine({ 53867, 90000, 55997 });


		// ============================================================
		// ПОВЕРХНОСТЬ 180°
		// Левая сторона ствола
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
		// ПОВЕРХНОСТЬ 240°
		// Задняя левая сторона
		// ============================================================

		NewLine();
		AddPointToLine({ 52850, 30000, 47819 });
		AddPointToLine({ 53254, 31250, 48017 });
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
		AddPointToLine({ 59238, 65320, 50749 });
		AddPointToLine({ 59143, 66660, 50718 });
		AddPointToLine({ 59021, 68000, 50676 });

		NewLine();
		AddPointToLine({ 58990, 68300, 50665 });
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
		// ПОВЕРХНОСТЬ 300°
		// Задняя правая сторона
		// ============================================================

		NewLine();
		AddPointToLine({ 57150, 30000, 47819 });
		AddPointToLine({ 57554, 31250, 48017 });
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
		AddPointToLine({ 63538, 65320, 50749 });
		AddPointToLine({ 63443, 66660, 50718 });
		AddPointToLine({ 63321, 68000, 50676 });

		NewLine();
		AddPointToLine({ 63290, 68300, 50665 });
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
// ДОПОЛНИТЕЛЬНАЯ ПОВЕРХНОСТЬ 20°
// Между 0° и 60°
// ============================================================

		NewLine();
		AddPointToLine({ 59041, 30000, 53014 });
		AddPointToLine({ 59445, 31250, 53211 });
		AddPointToLine({ 59847, 32500, 53405 });
		AddPointToLine({ 60246, 33750, 53594 });
		AddPointToLine({ 60641, 35000, 53779 });

		NewLine();
		AddPointToLine({ 60735, 35300, 53822 });
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
		AddPointToLine({ 64575, 50480, 55526 });
		AddPointToLine({ 64790, 51820, 55619 });
		AddPointToLine({ 64979, 53160, 55702 });
		AddPointToLine({ 65142, 54500, 55774 });

		NewLine();
		AddPointToLine({ 65175, 54800, 55788 });
		AddPointToLine({ 65393, 57280, 55888 });
		AddPointToLine({ 65466, 58520, 55923 });
		AddPointToLine({ 65514, 59760, 55949 });
		AddPointToLine({ 65538, 61000, 55965 });

		NewLine();
		AddPointToLine({ 65540, 61300, 55967 });
		AddPointToLine({ 65495, 63980, 55963 });
		AddPointToLine({ 65429, 65320, 55943 });
		AddPointToLine({ 65334, 66660, 55913 });
		AddPointToLine({ 65212, 68000, 55870 });

		NewLine();
		AddPointToLine({ 65181, 68300, 55859 });
		AddPointToLine({ 64872, 70780, 55747 });
		AddPointToLine({ 64684, 72020, 55677 });
		AddPointToLine({ 64474, 73260, 55598 });
		AddPointToLine({ 64244, 74500, 55510 });

		NewLine();
		AddPointToLine({ 64185, 74800, 55487 });
		AddPointToLine({ 63612, 77480, 55262 });
		AddPointToLine({ 63294, 78820, 55134 });
		AddPointToLine({ 62957, 80160, 54998 });
		AddPointToLine({ 62603, 81500, 54852 });

		NewLine();
		AddPointToLine({ 62521, 81800, 54818 });
		AddPointToLine({ 61583, 85080, 54422 });
		AddPointToLine({ 61087, 86720, 54206 });
		AddPointToLine({ 60578, 88360, 53980 });
		AddPointToLine({ 60058, 90000, 53745 });


		// ============================================================
		// ДОПОЛНИТЕЛЬНАЯ ПОВЕРХНОСТЬ 40°
		// Между 0° и 60°
		// ============================================================

		NewLine();
		AddPointToLine({ 58294, 30000, 54307 });
		AddPointToLine({ 58698, 31250, 54504 });
		AddPointToLine({ 59100, 32500, 54698 });
		AddPointToLine({ 59499, 33750, 54887 });
		AddPointToLine({ 59894, 35000, 55072 });

		NewLine();
		AddPointToLine({ 59988, 35300, 55115 });
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
		AddPointToLine({ 63828, 50480, 56819 });
		AddPointToLine({ 64043, 51820, 56912 });
		AddPointToLine({ 64232, 53160, 56995 });
		AddPointToLine({ 64395, 54500, 57067 });

		NewLine();
		AddPointToLine({ 64428, 54800, 57081 });
		AddPointToLine({ 64646, 57280, 57181 });
		AddPointToLine({ 64719, 58520, 57216 });
		AddPointToLine({ 64767, 59760, 57242 });
		AddPointToLine({ 64791, 61000, 57258 });

		NewLine();
		AddPointToLine({ 64793, 61300, 57260 });
		AddPointToLine({ 64748, 63980, 57256 });
		AddPointToLine({ 64682, 65320, 57236 });
		AddPointToLine({ 64587, 66660, 57206 });
		AddPointToLine({ 64465, 68000, 57163 });

		NewLine();
		AddPointToLine({ 64434, 68300, 57152 });
		AddPointToLine({ 64125, 70780, 57040 });
		AddPointToLine({ 63937, 72020, 56970 });
		AddPointToLine({ 63727, 73260, 56891 });
		AddPointToLine({ 63497, 74500, 56803 });

		NewLine();
		AddPointToLine({ 63438, 74800, 56780 });
		AddPointToLine({ 62865, 77480, 56555 });
		AddPointToLine({ 62547, 78820, 56427 });
		AddPointToLine({ 62210, 80160, 56291 });
		AddPointToLine({ 61856, 81500, 56145 });

		NewLine();
		AddPointToLine({ 61774, 81800, 56111 });
		AddPointToLine({ 60836, 85080, 55715 });
		AddPointToLine({ 60340, 86720, 55499 });
		AddPointToLine({ 59831, 88360, 55273 });
		AddPointToLine({ 59311, 90000, 55038 });


		// ============================================================
		// ДОПОЛНИТЕЛЬНАЯ ПОВЕРХНОСТЬ 80°
		// Между 60° и 120°
		// ============================================================

		NewLine();
		AddPointToLine({ 55747, 30000, 55778 });
		AddPointToLine({ 56151, 31250, 55975 });
		AddPointToLine({ 56553, 32500, 56169 });
		AddPointToLine({ 56952, 33750, 56358 });
		AddPointToLine({ 57347, 35000, 56543 });

		NewLine();
		AddPointToLine({ 57441, 35300, 56586 });
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
		AddPointToLine({ 61281, 50480, 58290 });
		AddPointToLine({ 61496, 51820, 58383 });
		AddPointToLine({ 61685, 53160, 58466 });
		AddPointToLine({ 61848, 54500, 58538 });

		NewLine();
		AddPointToLine({ 61881, 54800, 58552 });
		AddPointToLine({ 62099, 57280, 58652 });
		AddPointToLine({ 62172, 58520, 58687 });
		AddPointToLine({ 62220, 59760, 58713 });
		AddPointToLine({ 62244, 61000, 58729 });

		NewLine();
		AddPointToLine({ 62246, 61300, 58731 });
		AddPointToLine({ 62201, 63980, 58727 });
		AddPointToLine({ 62135, 65320, 58707 });
		AddPointToLine({ 62040, 66660, 58677 });
		AddPointToLine({ 61918, 68000, 58634 });

		NewLine();
		AddPointToLine({ 61887, 68300, 58623 });
		AddPointToLine({ 61578, 70780, 58511 });
		AddPointToLine({ 61390, 72020, 58441 });
		AddPointToLine({ 61180, 73260, 58362 });
		AddPointToLine({ 60950, 74500, 58274 });

		NewLine();
		AddPointToLine({ 60891, 74800, 58251 });
		AddPointToLine({ 60318, 77480, 58026 });
		AddPointToLine({ 60000, 78820, 57898 });
		AddPointToLine({ 59663, 80160, 57762 });
		AddPointToLine({ 59309, 81500, 57616 });

		NewLine();
		AddPointToLine({ 59227, 81800, 57582 });
		AddPointToLine({ 58289, 85080, 57186 });
		AddPointToLine({ 57793, 86720, 56970 });
		AddPointToLine({ 57284, 88360, 56744 });
		AddPointToLine({ 56764, 90000, 56509 });


		// ============================================================
		// ДОПОЛНИТЕЛЬНАЯ ПОВЕРХНОСТЬ 100°
		// Между 60° и 120°
		// ============================================================

		NewLine();
		AddPointToLine({ 54253, 30000, 55778 });
		AddPointToLine({ 54657, 31250, 55975 });
		AddPointToLine({ 55059, 32500, 56169 });
		AddPointToLine({ 55458, 33750, 56358 });
		AddPointToLine({ 55853, 35000, 56543 });

		NewLine();
		AddPointToLine({ 55947, 35300, 56586 });
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
		AddPointToLine({ 59787, 50480, 58290 });
		AddPointToLine({ 60002, 51820, 58383 });
		AddPointToLine({ 60191, 53160, 58466 });
		AddPointToLine({ 60354, 54500, 58538 });

		NewLine();
		AddPointToLine({ 60387, 54800, 58552 });
		AddPointToLine({ 60605, 57280, 58652 });
		AddPointToLine({ 60678, 58520, 58687 });
		AddPointToLine({ 60726, 59760, 58713 });
		AddPointToLine({ 60750, 61000, 58729 });

		NewLine();
		AddPointToLine({ 60752, 61300, 58731 });
		AddPointToLine({ 60707, 63980, 58727 });
		AddPointToLine({ 60641, 65320, 58707 });
		AddPointToLine({ 60546, 66660, 58677 });
		AddPointToLine({ 60424, 68000, 58634 });

		NewLine();
		AddPointToLine({ 60393, 68300, 58623 });
		AddPointToLine({ 60084, 70780, 58511 });
		AddPointToLine({ 59896, 72020, 58441 });
		AddPointToLine({ 59686, 73260, 58362 });
		AddPointToLine({ 59456, 74500, 58274 });

		NewLine();
		AddPointToLine({ 59397, 74800, 58251 });
		AddPointToLine({ 58824, 77480, 58026 });
		AddPointToLine({ 58506, 78820, 57898 });
		AddPointToLine({ 58169, 80160, 57762 });
		AddPointToLine({ 57815, 81500, 57616 });

		NewLine();
		AddPointToLine({ 57733, 81800, 57582 });
		AddPointToLine({ 56795, 85080, 57186 });
		AddPointToLine({ 56299, 86720, 56970 });
		AddPointToLine({ 55790, 88360, 56744 });
		AddPointToLine({ 55270, 90000, 56509 });


		// ============================================================
		// ДОПОЛНИТЕЛЬНАЯ ПОВЕРХНОСТЬ 140°
		// Между 120° и 180°
		// ============================================================

		NewLine();
		AddPointToLine({ 50959, 30000, 53014 });
		AddPointToLine({ 51363, 31250, 53211 });
		AddPointToLine({ 51765, 32500, 53405 });
		AddPointToLine({ 52164, 33750, 53594 });
		AddPointToLine({ 52559, 35000, 53779 });

		NewLine();
		AddPointToLine({ 52653, 35300, 53822 });
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
		AddPointToLine({ 56493, 50480, 55526 });
		AddPointToLine({ 56708, 51820, 55619 });
		AddPointToLine({ 56897, 53160, 55702 });
		AddPointToLine({ 57060, 54500, 55774 });

		NewLine();
		AddPointToLine({ 57093, 54800, 55788 });
		AddPointToLine({ 57311, 57280, 55888 });
		AddPointToLine({ 57384, 58520, 55923 });
		AddPointToLine({ 57432, 59760, 55949 });
		AddPointToLine({ 57456, 61000, 55965 });

		NewLine();
		AddPointToLine({ 57458, 61300, 55967 });
		AddPointToLine({ 57413, 63980, 55963 });
		AddPointToLine({ 57347, 65320, 55943 });
		AddPointToLine({ 57252, 66660, 55913 });
		AddPointToLine({ 57130, 68000, 55870 });

		NewLine();
		AddPointToLine({ 57099, 68300, 55859 });
		AddPointToLine({ 56790, 70780, 55747 });
		AddPointToLine({ 56602, 72020, 55677 });
		AddPointToLine({ 56392, 73260, 55598 });
		AddPointToLine({ 56162, 74500, 55510 });

		NewLine();
		AddPointToLine({ 56103, 74800, 55487 });
		AddPointToLine({ 55530, 77480, 55262 });
		AddPointToLine({ 55212, 78820, 55134 });
		AddPointToLine({ 54875, 80160, 54998 });
		AddPointToLine({ 54521, 81500, 54852 });

		NewLine();
		AddPointToLine({ 54439, 81800, 54818 });
		AddPointToLine({ 53501, 85080, 54422 });
		AddPointToLine({ 53005, 86720, 54206 });
		AddPointToLine({ 52496, 88360, 53980 });
		AddPointToLine({ 51976, 90000, 53745 });


		// ============================================================
		// ДОПОЛНИТЕЛЬНАЯ ПОВЕРХНОСТЬ 160°
		// Между 120° и 180°
		// ============================================================

		NewLine();
		AddPointToLine({ 50157, 30000, 53014 });
		AddPointToLine({ 50561, 31250, 53211 });
		AddPointToLine({ 50963, 32500, 53405 });
		AddPointToLine({ 51362, 33750, 53594 });
		AddPointToLine({ 51757, 35000, 53779 });

		NewLine();
		AddPointToLine({ 51851, 35300, 53822 });
		AddPointToLine({ 52670, 37980, 54197 });
		AddPointToLine({ 53064, 39320, 54375 });
		AddPointToLine({ 53444, 40660, 54545 });
		AddPointToLine({ 53811, 42000, 54707 });

		NewLine();
		AddPointToLine({ 53890, 42300, 54742 });
		AddPointToLine({ 54226, 43600, 54889 });
		AddPointToLine({ 54545, 44900, 55029 });
		AddPointToLine({ 54846, 46200, 55159 });
		AddPointToLine({ 55127, 47500, 55281 });

		NewLine();
		AddPointToLine({ 55189, 47800, 55308 });
		AddPointToLine({ 55691, 50480, 55526 });
		AddPointToLine({ 55906, 51820, 55619 });
		AddPointToLine({ 56095, 53160, 55702 });
		AddPointToLine({ 56258, 54500, 55774 });

		NewLine();
		AddPointToLine({ 56291, 54800, 55788 });
		AddPointToLine({ 56509, 57280, 55888 });
		AddPointToLine({ 56582, 58520, 55923 });
		AddPointToLine({ 56630, 59760, 55949 });
		AddPointToLine({ 56654, 61000, 55965 });

		NewLine();
		AddPointToLine({ 56656, 61300, 55967 });
		AddPointToLine({ 56611, 63980, 55963 });
		AddPointToLine({ 56545, 65320, 55943 });
		AddPointToLine({ 56450, 66660, 55913 });
		AddPointToLine({ 56328, 68000, 55870 });

		NewLine();
		AddPointToLine({ 56297, 68300, 55859 });
		AddPointToLine({ 55988, 70780, 55747 });
		AddPointToLine({ 55800, 72020, 55677 });
		AddPointToLine({ 55590, 73260, 55598 });
		AddPointToLine({ 55360, 74500, 55510 });

		NewLine();
		AddPointToLine({ 55301, 74800, 55487 });
		AddPointToLine({ 54728, 77480, 55262 });
		AddPointToLine({ 54410, 78820, 55134 });
		AddPointToLine({ 54073, 80160, 54998 });
		AddPointToLine({ 53719, 81500, 54852 });

		NewLine();
		AddPointToLine({ 53637, 81800, 54818 });
		AddPointToLine({ 52699, 85080, 54422 });
		AddPointToLine({ 52203, 86720, 54206 });
		AddPointToLine({ 51694, 88360, 53980 });
		AddPointToLine({ 51174, 90000, 53745 });


		// ============================================================
		// ДОПОЛНИТЕЛЬНАЯ ПОВЕРХНОСТЬ 200°
		// Между 180° и 240°
		// ============================================================

		NewLine();
		AddPointToLine({ 50157, 30000, 50072 });
		AddPointToLine({ 50561, 31250, 50269 });
		AddPointToLine({ 50963, 32500, 50463 });
		AddPointToLine({ 51362, 33750, 50652 });
		AddPointToLine({ 51757, 35000, 50837 });

		NewLine();
		AddPointToLine({ 51851, 35300, 50880 });
		AddPointToLine({ 52670, 37980, 51255 });
		AddPointToLine({ 53064, 39320, 51433 });
		AddPointToLine({ 53444, 40660, 51603 });
		AddPointToLine({ 53811, 42000, 51765 });

		NewLine();
		AddPointToLine({ 53890, 42300, 51800 });
		AddPointToLine({ 54226, 43600, 51947 });
		AddPointToLine({ 54545, 44900, 52087 });
		AddPointToLine({ 54846, 46200, 52217 });
		AddPointToLine({ 55127, 47500, 52339 });

		NewLine();
		AddPointToLine({ 55189, 47800, 52366 });
		AddPointToLine({ 55691, 50480, 52584 });
		AddPointToLine({ 55906, 51820, 52677 });
		AddPointToLine({ 56095, 53160, 52760 });
		AddPointToLine({ 56258, 54500, 52832 });

		NewLine();
		AddPointToLine({ 56291, 54800, 52846 });
		AddPointToLine({ 56509, 57280, 52946 });
		AddPointToLine({ 56582, 58520, 52981 });
		AddPointToLine({ 56630, 59760, 53007 });
		AddPointToLine({ 56654, 61000, 53023 });

		NewLine();
		AddPointToLine({ 56656, 61300, 53025 });
		AddPointToLine({ 56611, 63980, 53021 });
		AddPointToLine({ 56545, 65320, 53001 });
		AddPointToLine({ 56450, 66660, 52971 });
		AddPointToLine({ 56328, 68000, 52928 });

		NewLine();
		AddPointToLine({ 56297, 68300, 52917 });
		AddPointToLine({ 55988, 70780, 52805 });
		AddPointToLine({ 55800, 72020, 52735 });
		AddPointToLine({ 55590, 73260, 52656 });
		AddPointToLine({ 55360, 74500, 52568 });

		NewLine();
		AddPointToLine({ 55301, 74800, 52545 });
		AddPointToLine({ 54728, 77480, 52320 });
		AddPointToLine({ 54410, 78820, 52192 });
		AddPointToLine({ 54073, 80160, 52056 });
		AddPointToLine({ 53719, 81500, 51910 });

		NewLine();
		AddPointToLine({ 53637, 81800, 51876 });
		AddPointToLine({ 52699, 85080, 51480 });
		AddPointToLine({ 52203, 86720, 51264 });
		AddPointToLine({ 51694, 88360, 51038 });
		AddPointToLine({ 51174, 90000, 50803 });


		// ============================================================
		// ДОПОЛНИТЕЛЬНАЯ ПОВЕРХНОСТЬ 220°
		// Между 180° и 240°
		// ============================================================

		NewLine();
		AddPointToLine({ 50959, 30000, 48779 });
		AddPointToLine({ 51363, 31250, 48976 });
		AddPointToLine({ 51765, 32500, 49170 });
		AddPointToLine({ 52164, 33750, 49359 });
		AddPointToLine({ 52559, 35000, 49544 });

		NewLine();
		AddPointToLine({ 52653, 35300, 49587 });
		AddPointToLine({ 53472, 37980, 49962 });
		AddPointToLine({ 53866, 39320, 50140 });
		AddPointToLine({ 54246, 40660, 50310 });
		AddPointToLine({ 54613, 42000, 50472 });

		NewLine();
		AddPointToLine({ 54692, 42300, 50507 });
		AddPointToLine({ 55028, 43600, 50654 });
		AddPointToLine({ 55347, 44900, 50794 });
		AddPointToLine({ 55648, 46200, 50924 });
		AddPointToLine({ 55929, 47500, 51046 });

		NewLine();
		AddPointToLine({ 55991, 47800, 51073 });
		AddPointToLine({ 56493, 50480, 51291 });
		AddPointToLine({ 56708, 51820, 51384 });
		AddPointToLine({ 56897, 53160, 51467 });
		AddPointToLine({ 57060, 54500, 51539 });

		NewLine();
		AddPointToLine({ 57093, 54800, 51553 });
		AddPointToLine({ 57311, 57280, 51653 });
		AddPointToLine({ 57384, 58520, 51688 });
		AddPointToLine({ 57432, 59760, 51714 });
		AddPointToLine({ 57456, 61000, 51730 });

		NewLine();
		AddPointToLine({ 57458, 61300, 51732 });
		AddPointToLine({ 57413, 63980, 51728 });
		AddPointToLine({ 57347, 65320, 51708 });
		AddPointToLine({ 57252, 66660, 51678 });
		AddPointToLine({ 57130, 68000, 51635 });

		NewLine();
		AddPointToLine({ 57099, 68300, 51624 });
		AddPointToLine({ 56790, 70780, 51512 });
		AddPointToLine({ 56602, 72020, 51442 });
		AddPointToLine({ 56392, 73260, 51363 });
		AddPointToLine({ 56162, 74500, 51275 });

		NewLine();
		AddPointToLine({ 56103, 74800, 51252 });
		AddPointToLine({ 55530, 77480, 51027 });
		AddPointToLine({ 55212, 78820, 50899 });
		AddPointToLine({ 54875, 80160, 50763 });
		AddPointToLine({ 54521, 81500, 50617 });

		NewLine();
		AddPointToLine({ 54439, 81800, 50583 });
		AddPointToLine({ 53501, 85080, 50187 });
		AddPointToLine({ 53005, 86720, 49971 });
		AddPointToLine({ 52496, 88360, 49745 });
		AddPointToLine({ 51976, 90000, 49510 });


		// ============================================================
		// ДОПОЛНИТЕЛЬНАЯ ПОВЕРХНОСТЬ 260°
		// Между 240° и 300°
		// ============================================================

		NewLine();
		AddPointToLine({ 54253, 30000, 47308 });
		AddPointToLine({ 54657, 31250, 47505 });
		AddPointToLine({ 55059, 32500, 47699 });
		AddPointToLine({ 55458, 33750, 47888 });
		AddPointToLine({ 55853, 35000, 48073 });

		NewLine();
		AddPointToLine({ 55947, 35300, 48116 });
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
		AddPointToLine({ 59787, 50480, 49820 });
		AddPointToLine({ 60002, 51820, 49913 });
		AddPointToLine({ 60191, 53160, 49996 });
		AddPointToLine({ 60354, 54500, 50068 });

		NewLine();
		AddPointToLine({ 60387, 54800, 50082 });
		AddPointToLine({ 60605, 57280, 50182 });
		AddPointToLine({ 60678, 58520, 50217 });
		AddPointToLine({ 60726, 59760, 50243 });
		AddPointToLine({ 60750, 61000, 50259 });

		NewLine();
		AddPointToLine({ 60752, 61300, 50261 });
		AddPointToLine({ 60707, 63980, 50257 });
		AddPointToLine({ 60641, 65320, 50237 });
		AddPointToLine({ 60546, 66660, 50207 });
		AddPointToLine({ 60424, 68000, 50164 });

		NewLine();
		AddPointToLine({ 60393, 68300, 50153 });
		AddPointToLine({ 60084, 70780, 50041 });
		AddPointToLine({ 59896, 72020, 49971 });
		AddPointToLine({ 59686, 73260, 49892 });
		AddPointToLine({ 59456, 74500, 49804 });

		NewLine();
		AddPointToLine({ 59397, 74800, 49781 });
		AddPointToLine({ 58824, 77480, 49556 });
		AddPointToLine({ 58506, 78820, 49428 });
		AddPointToLine({ 58169, 80160, 49292 });
		AddPointToLine({ 57815, 81500, 49146 });

		NewLine();
		AddPointToLine({ 57733, 81800, 49112 });
		AddPointToLine({ 56795, 85080, 48716 });
		AddPointToLine({ 56299, 86720, 48500 });
		AddPointToLine({ 55790, 88360, 48274 });
		AddPointToLine({ 55270, 90000, 48039 });


		// ============================================================
		// ДОПОЛНИТЕЛЬНАЯ ПОВЕРХНОСТЬ 280°
		// Между 240° и 300°
		// ============================================================

		NewLine();
		AddPointToLine({ 55747, 30000, 47308 });
		AddPointToLine({ 56151, 31250, 47505 });
		AddPointToLine({ 56553, 32500, 47699 });
		AddPointToLine({ 56952, 33750, 47888 });
		AddPointToLine({ 57347, 35000, 48073 });

		NewLine();
		AddPointToLine({ 57441, 35300, 48116 });
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
		AddPointToLine({ 61281, 50480, 49820 });
		AddPointToLine({ 61496, 51820, 49913 });
		AddPointToLine({ 61685, 53160, 49996 });
		AddPointToLine({ 61848, 54500, 50068 });

		NewLine();
		AddPointToLine({ 61881, 54800, 50082 });
		AddPointToLine({ 62099, 57280, 50182 });
		AddPointToLine({ 62172, 58520, 50217 });
		AddPointToLine({ 62220, 59760, 50243 });
		AddPointToLine({ 62244, 61000, 50259 });

		NewLine();
		AddPointToLine({ 62246, 61300, 50261 });
		AddPointToLine({ 62201, 63980, 50257 });
		AddPointToLine({ 62135, 65320, 50237 });
		AddPointToLine({ 62040, 66660, 50207 });
		AddPointToLine({ 61918, 68000, 50164 });

		NewLine();
		AddPointToLine({ 61887, 68300, 50153 });
		AddPointToLine({ 61578, 70780, 50041 });
		AddPointToLine({ 61390, 72020, 49971 });
		AddPointToLine({ 61180, 73260, 49892 });
		AddPointToLine({ 60950, 74500, 49804 });

		NewLine();
		AddPointToLine({ 60891, 74800, 49781 });
		AddPointToLine({ 60318, 77480, 49556 });
		AddPointToLine({ 60000, 78820, 49428 });
		AddPointToLine({ 59663, 80160, 49292 });
		AddPointToLine({ 59309, 81500, 49146 });

		NewLine();
		AddPointToLine({ 59227, 81800, 49112 });
		AddPointToLine({ 58289, 85080, 48716 });
		AddPointToLine({ 57793, 86720, 48500 });
		AddPointToLine({ 57284, 88360, 48274 });
		AddPointToLine({ 56764, 90000, 48039 });


		// ============================================================
		// ДОПОЛНИТЕЛЬНАЯ ПОВЕРХНОСТЬ 320°
		// Между 300° и 0°
		// ============================================================

		NewLine();
		AddPointToLine({ 58294, 30000, 48779 });
		AddPointToLine({ 58698, 31250, 48976 });
		AddPointToLine({ 59100, 32500, 49170 });
		AddPointToLine({ 59499, 33750, 49359 });
		AddPointToLine({ 59894, 35000, 49544 });

		NewLine();
		AddPointToLine({ 59988, 35300, 49587 });
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
		AddPointToLine({ 63828, 50480, 51291 });
		AddPointToLine({ 64043, 51820, 51384 });
		AddPointToLine({ 64232, 53160, 51467 });
		AddPointToLine({ 64395, 54500, 51539 });

		NewLine();
		AddPointToLine({ 64428, 54800, 51553 });
		AddPointToLine({ 64646, 57280, 51653 });
		AddPointToLine({ 64719, 58520, 51688 });
		AddPointToLine({ 64767, 59760, 51714 });
		AddPointToLine({ 64791, 61000, 51730 });

		NewLine();
		AddPointToLine({ 64793, 61300, 51732 });
		AddPointToLine({ 64748, 63980, 51728 });
		AddPointToLine({ 64682, 65320, 51708 });
		AddPointToLine({ 64587, 66660, 51678 });
		AddPointToLine({ 64465, 68000, 51635 });

		NewLine();
		AddPointToLine({ 64434, 68300, 51624 });
		AddPointToLine({ 64125, 70780, 51512 });
		AddPointToLine({ 63937, 72020, 51442 });
		AddPointToLine({ 63727, 73260, 51363 });
		AddPointToLine({ 63497, 74500, 51275 });

		NewLine();
		AddPointToLine({ 63438, 74800, 51252 });
		AddPointToLine({ 62865, 77480, 51027 });
		AddPointToLine({ 62547, 78820, 50899 });
		AddPointToLine({ 62210, 80160, 50763 });
		AddPointToLine({ 61856, 81500, 50617 });

		NewLine();
		AddPointToLine({ 61774, 81800, 50583 });
		AddPointToLine({ 60836, 85080, 50187 });
		AddPointToLine({ 60340, 86720, 49971 });
		AddPointToLine({ 59831, 88360, 49745 });
		AddPointToLine({ 59311, 90000, 49510 });


		// ============================================================
		// ДОПОЛНИТЕЛЬНАЯ ПОВЕРХНОСТЬ 340°
		// Между 300° и 0°
		// ============================================================

		NewLine();
		AddPointToLine({ 59041, 30000, 50072 });
		AddPointToLine({ 59445, 31250, 50269 });
		AddPointToLine({ 59847, 32500, 50463 });
		AddPointToLine({ 60246, 33750, 50652 });
		AddPointToLine({ 60641, 35000, 50837 });

		NewLine();
		AddPointToLine({ 60735, 35300, 50880 });
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
		AddPointToLine({ 64575, 50480, 52584 });
		AddPointToLine({ 64790, 51820, 52677 });
		AddPointToLine({ 64979, 53160, 52760 });
		AddPointToLine({ 65142, 54500, 52832 });

		NewLine();
		AddPointToLine({ 65175, 54800, 52846 });
		AddPointToLine({ 65393, 57280, 52946 });
		AddPointToLine({ 65466, 58520, 52981 });
		AddPointToLine({ 65514, 59760, 53007 });
		AddPointToLine({ 65538, 61000, 53023 });

		NewLine();
		AddPointToLine({ 65540, 61300, 53025 });
		AddPointToLine({ 65495, 63980, 53021 });
		AddPointToLine({ 65429, 65320, 53001 });
		AddPointToLine({ 65334, 66660, 52971 });
		AddPointToLine({ 65212, 68000, 52928 });

		NewLine();
		AddPointToLine({ 65181, 68300, 52917 });
		AddPointToLine({ 64872, 70780, 52805 });
		AddPointToLine({ 64684, 72020, 52735 });
		AddPointToLine({ 64474, 73260, 52656 });
		AddPointToLine({ 64244, 74500, 52568 });

		NewLine();
		AddPointToLine({ 64185, 74800, 52545 });
		AddPointToLine({ 63612, 77480, 52320 });
		AddPointToLine({ 63294, 78820, 52192 });
		AddPointToLine({ 62957, 80160, 52056 });
		AddPointToLine({ 62603, 81500, 51910 });

		NewLine();
		AddPointToLine({ 62521, 81800, 51876 });
		AddPointToLine({ 61583, 85080, 51480 });
		AddPointToLine({ 61087, 86720, 51264 });
		AddPointToLine({ 60578, 88360, 51038 });
		AddPointToLine({ 60058, 90000, 50803 });


		// ============================================================
		// КОРНИ — ПРОДОЛЖЕНИЕ ТРУБЫ НАРУЖУ
		// ============================================================

		// Корень влево
		NewLine();
		AddPointToLine({ 48000, 30000, 48000 });
		AddPointToLine({ 44500, 29500, 46500 });
		AddPointToLine({ 41000, 30000, 44000 });
		AddPointToLine({ 37500, 29000, 45500 });
		AddPointToLine({ 34000, 30000, 43000 });
		AddPointToLine({ 30500, 29200, 45000 });
		AddPointToLine({ 27000, 30000, 43000 });


		// Корень вправо
		NewLine();
		AddPointToLine({ 60000, 30000, 48500 });
		AddPointToLine({ 63500, 29500, 50000 });
		AddPointToLine({ 67000, 30000, 52500 });
		AddPointToLine({ 70500, 29000, 50500 });
		AddPointToLine({ 74000, 30000, 53000 });
		AddPointToLine({ 77500, 29200, 51000 });
		AddPointToLine({ 81000, 30000, 53500 });


		// Корень назад
		NewLine();
		AddPointToLine({ 52000, 30000, 45500 });
		AddPointToLine({ 50000, 29200, 42000 });
		AddPointToLine({ 47500, 30000, 39000 });
		AddPointToLine({ 45500, 29000, 36000 });
		AddPointToLine({ 43000, 30000, 33500 });


		// Корень вперед
		NewLine();
		AddPointToLine({ 57000, 30000, 54500 });
		AddPointToLine({ 59500, 29200, 57500 });
		AddPointToLine({ 62000, 30000, 61000 });
		AddPointToLine({ 65000, 29000, 64000 });
		AddPointToLine({ 68000, 30000, 66500 });


		// ============================================================
		// БОЛЬШАЯ ЛЕВАЯ ВЕТВЬ
		// ============================================================

		NewLine();
		AddPointToLine({ 47000, 55000, 54000 });
		AddPointToLine({ 43500, 56000, 52500 });
		AddPointToLine({ 40000, 57500, 53500 });
		AddPointToLine({ 36500, 59000, 56000 });
		AddPointToLine({ 33000, 58500, 54500 });
		AddPointToLine({ 29500, 60000, 52000 });
		AddPointToLine({ 26000, 59000, 53500 });
		AddPointToLine({ 22500, 61000, 51500 });


		// Жила вокруг левой ветви
		NewLine();
		AddPointToLine({ 46500, 54500, 55500 });
		AddPointToLine({ 43000, 55500, 55000 });
		AddPointToLine({ 39500, 57000, 56500 });
		AddPointToLine({ 36000, 58000, 58500 });
		AddPointToLine({ 32500, 57500, 57500 });
		AddPointToLine({ 29000, 59000, 55500 });
		AddPointToLine({ 25500, 58500, 57000 });


		// ============================================================
		// ВЕТКИ ЛЕВОЙ КРОНЫ
		// ============================================================

		NewLine();
		AddPointToLine({ 36500, 59000, 56000 });
		AddPointToLine({ 33500, 61500, 57500 });
		AddPointToLine({ 30500, 63500, 56000 });
		AddPointToLine({ 27500, 65000, 58000 });
		AddPointToLine({ 24500, 67000, 56500 });
		AddPointToLine({ 21500, 66500, 58500 });

		NewLine();
		AddPointToLine({ 33000, 58500, 54500 });
		AddPointToLine({ 30500, 56000, 52000 });
		AddPointToLine({ 28000, 54000, 53500 });
		AddPointToLine({ 25000, 52500, 51500 });
		AddPointToLine({ 22000, 53500, 53000 });

		NewLine();
		AddPointToLine({ 30000, 60000, 52000 });
		AddPointToLine({ 27500, 61500, 50000 });
		AddPointToLine({ 25000, 60500, 48000 });
		AddPointToLine({ 22500, 62000, 46500 });
		AddPointToLine({ 19500, 61000, 48000 });


		// ============================================================
		// БОЛЬШАЯ ПРАВАЯ ВЕТВЬ
		// ============================================================

		NewLine();
		AddPointToLine({ 59000, 60000, 55500 });
		AddPointToLine({ 62500, 61000, 58000 });
		AddPointToLine({ 66000, 62500, 59500 });
		AddPointToLine({ 70000, 61500, 58000 });
		AddPointToLine({ 74000, 63500, 60000 });
		AddPointToLine({ 78000, 62500, 58000 });
		AddPointToLine({ 82000, 64500, 59500 });
		AddPointToLine({ 85500, 63500, 58000 });


		// Вторая жила правой ветви
		NewLine();
		AddPointToLine({ 60000, 59500, 53500 });
		AddPointToLine({ 63500, 60500, 56000 });
		AddPointToLine({ 67000, 61500, 57500 });
		AddPointToLine({ 71000, 60500, 56000 });
		AddPointToLine({ 75000, 62500, 58000 });
		AddPointToLine({ 79000, 61500, 56000 });
		AddPointToLine({ 83000, 63500, 57500 });


		// ============================================================
		// ВЕТКИ ПРАВОЙ КРОНЫ
		// ============================================================

		NewLine();
		AddPointToLine({ 70000, 61500, 58000 });
		AddPointToLine({ 72500, 64000, 60000 });
		AddPointToLine({ 75000, 66000, 58500 });
		AddPointToLine({ 78000, 67500, 60500 });
		AddPointToLine({ 81000, 69000, 59000 });
		AddPointToLine({ 84000, 68500, 61000 });

		NewLine();
		AddPointToLine({ 74000, 63500, 60000 });
		AddPointToLine({ 76000, 61000, 62500 });
		AddPointToLine({ 78500, 59500, 61000 });
		AddPointToLine({ 81500, 60500, 63000 });
		AddPointToLine({ 84500, 59000, 61500 });
		AddPointToLine({ 87500, 60500, 63500 });

		NewLine();
		AddPointToLine({ 78000, 62500, 58000 });
		AddPointToLine({ 80500, 65000, 56000 });
		AddPointToLine({ 83000, 66500, 57500 });
		AddPointToLine({ 85500, 68000, 55500 });
		AddPointToLine({ 88500, 67000, 57000 });


		// ============================================================
		// ЦЕНТРАЛЬНАЯ ВЕТВЬ ВВЕРХ
		// ============================================================

		NewLine();
		AddPointToLine({ 52000, 70000, 55000 });
		AddPointToLine({ 50500, 73500, 53500 });
		AddPointToLine({ 51500, 77000, 52000 });
		AddPointToLine({ 54000, 80500, 53500 });
		AddPointToLine({ 56500, 84000, 55500 });
		AddPointToLine({ 57500, 87500, 54000 });
		AddPointToLine({ 56500, 91000, 51500 });
		AddPointToLine({ 54500, 94500, 53000 });


		// Вторая линия центральной верхушки
		NewLine();
		AddPointToLine({ 54000, 70000, 52000 });
		AddPointToLine({ 55000, 73500, 50000 });
		AddPointToLine({ 56500, 77000, 51000 });
		AddPointToLine({ 58500, 80500, 53500 });
		AddPointToLine({ 60000, 84000, 56000 });
		AddPointToLine({ 59500, 87500, 54500 });
		AddPointToLine({ 58000, 91000, 52000 });
		AddPointToLine({ 56000, 94500, 54000 });


		// ============================================================
		// ВЕРХНЯЯ ЛЕВАЯ ВЕТВЬ
		// ============================================================

		NewLine();
		AddPointToLine({ 51000, 76000, 57500 });
		AddPointToLine({ 47500, 77500, 55500 });
		AddPointToLine({ 44000, 79000, 53500 });
		AddPointToLine({ 40500, 80500, 55000 });
		AddPointToLine({ 37000, 79500, 53000 });
		AddPointToLine({ 33500, 81500, 55000 });
		AddPointToLine({ 30000, 80500, 53500 });


		// Ветка от неё
		NewLine();
		AddPointToLine({ 40500, 80500, 55000 });
		AddPointToLine({ 38000, 83000, 57000 });
		AddPointToLine({ 35000, 84500, 55500 });
		AddPointToLine({ 32000, 86500, 57500 });
		AddPointToLine({ 29000, 85500, 56000 });


		// Ещё одна
		NewLine();
		AddPointToLine({ 37000, 79500, 53000 });
		AddPointToLine({ 34500, 77500, 51000 });
		AddPointToLine({ 32000, 76500, 52500 });
		AddPointToLine({ 29000, 77500, 50500 });
		AddPointToLine({ 26000, 76500, 52000 });


		// ============================================================
		// ВЕРХНЯЯ ПРАВАЯ ВЕТВЬ
		// ============================================================

		NewLine();
		AddPointToLine({ 57000, 80000, 54500 });
		AddPointToLine({ 60500, 81500, 56000 });
		AddPointToLine({ 64000, 83000, 58000 });
		AddPointToLine({ 67500, 82000, 56500 });
		AddPointToLine({ 71000, 84000, 58500 });
		AddPointToLine({ 74500, 83000, 56500 });
		AddPointToLine({ 78000, 85000, 58500 });
		AddPointToLine({ 81500, 84000, 57000 });


		// Ветка от правой
		NewLine();
		AddPointToLine({ 67500, 82000, 56500 });
		AddPointToLine({ 70000, 79500, 59000 });
		AddPointToLine({ 73000, 78000, 57500 });
		AddPointToLine({ 76000, 79000, 60000 });
		AddPointToLine({ 79000, 77500, 58000 });
		AddPointToLine({ 82000, 78500, 59500 });


		// Ещё одна правая
		NewLine();
		AddPointToLine({ 74500, 83000, 56500 });
		AddPointToLine({ 77000, 85500, 55000 });
		AddPointToLine({ 80000, 87000, 57000 });
		AddPointToLine({ 83000, 86500, 55000 });
		AddPointToLine({ 86000, 88000, 56500 });


		// ============================================================
		// ВЕРХУШКА — РАЗВЕТВЛЕНИЕ
		// ============================================================

		// Влево
		NewLine();
		AddPointToLine({ 56000, 91000, 51500 });
		AddPointToLine({ 53000, 92500, 50000 });
		AddPointToLine({ 50000, 94000, 51000 });
		AddPointToLine({ 47000, 95500, 49500 });
		AddPointToLine({ 44000, 95000, 51000 });
		AddPointToLine({ 41000, 97000, 49500 });

		// Вправо
		NewLine();
		AddPointToLine({ 57500, 91000, 54000 });
		AddPointToLine({ 60500, 92500, 55500 });
		AddPointToLine({ 63500, 94000, 54000 });
		AddPointToLine({ 66500, 93000, 56500 });
		AddPointToLine({ 69500, 95000, 55000 });
		AddPointToLine({ 72500, 94000, 57000 });


		// ============================================================
		// ТОНКИЕ ВЕТКИ НА ВЕРХУШКЕ
		// ============================================================

		NewLine();
		AddPointToLine({ 47000, 95500, 49500 });
		AddPointToLine({ 44500, 97500, 48000 });
		AddPointToLine({ 42000, 99000, 49500 });
		AddPointToLine({ 39500, 98500, 47500 });

		NewLine();
		AddPointToLine({ 50000, 94000, 51000 });
		AddPointToLine({ 48500, 96500, 53000 });
		AddPointToLine({ 46500, 98000, 51500 });
		AddPointToLine({ 44500, 99500, 53000 });

		NewLine();
		AddPointToLine({ 66500, 93000, 56500 });
		AddPointToLine({ 69000, 95500, 58500 });
		AddPointToLine({ 71500, 97000, 57000 });
		AddPointToLine({ 74000, 96500, 59000 });

		NewLine();
		AddPointToLine({ 60500, 92500, 55500 });
		AddPointToLine({ 62000, 95000, 53000 });
		AddPointToLine({ 64500, 97000, 54500 });
		AddPointToLine({ 67000, 98500, 52500 });


		// ============================================================
		// ОЧЕНЬ ТОНКИЕ КОНЦЫ
		// ============================================================

		NewLine();
		AddPointToLine({ 41000, 97000, 49500 });
		AddPointToLine({ 38500, 98500, 51000 });
		AddPointToLine({ 36000, 98000, 49500 });

		NewLine();
		AddPointToLine({ 44500, 99500, 53000 });
		AddPointToLine({ 42000, 101000, 54500 });
		AddPointToLine({ 39500, 100000, 53000 });

		NewLine();
		AddPointToLine({ 74000, 96500, 59000 });
		AddPointToLine({ 76500, 98500, 57500 });
		AddPointToLine({ 79000, 98000, 59000 });

		NewLine();
		AddPointToLine({ 67000, 98500, 52500 });
		AddPointToLine({ 69500, 100500, 54000 });
		AddPointToLine({ 72000, 100000, 52500 });





		// ============================================================
		// ЕЩЁ НЕСКОЛЬКО КОРОТКИХ ЖИЛ — КОРНИ
		// ============================================================

		NewLine();
		AddPointToLine({ 49000, 30500, 47500 });
		AddPointToLine({ 45500, 29500, 45000 });
		AddPointToLine({ 42000, 30500, 42000 });
		AddPointToLine({ 39000, 29500, 40000 });
		AddPointToLine({ 36000, 30500, 39000 });

		NewLine();
		AddPointToLine({ 58500, 30500, 48000 });
		AddPointToLine({ 62000, 29500, 49000 });
		AddPointToLine({ 65500, 30500, 51500 });
		AddPointToLine({ 69000, 29500, 50000 });
		AddPointToLine({ 72500, 30500, 52000 });

		NewLine();
		AddPointToLine({ 51000, 30500, 52500 });
		AddPointToLine({ 48000, 29500, 55000 });
		AddPointToLine({ 45000, 30500, 57500 });
		AddPointToLine({ 42000, 29500, 59000 });

		NewLine();
		AddPointToLine({ 57000, 30500, 53000 });
		AddPointToLine({ 60000, 29500, 55500 });
		AddPointToLine({ 63000, 30500, 58000 });
		AddPointToLine({ 66000, 29500, 60000 });
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
