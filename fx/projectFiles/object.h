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
			cmdParamDesc[cmdCounter - 1].param[2]._max = max((mipMaps ? (UINT)(_log2(r)) : 0)-2,0);
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
			Drawer::NullDrawer({ 1, (int)gX * (int)gY/ 10394 });
		}
		else
		{
			Drawer::NullDrawer({ 1, (int)gX * (int)gY });
		}

		
	}

	

	enum class pMode { point,glow };
	enum class triMode { on,off };
	
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
				.base_color=float4(in.r/100.,in.g/100.,in.b/100.,1)
			},
		};

		vs::insideNebula.set();

		Drawer::NullDrawer({1,(int)gX*(int)gY});

		
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

	void ShowMesh(dx11::ConstBuf::sbObject* obj, int count, int skipper, pMode mode, int r, int g, int b, triMode tMode, int xPos, int yPos, int zPos, int brightness, int tickness,int zoom, int onLineOfs, int jumpCharge)
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
				.modelPos = float4(xPos/10000.,yPos / 10000.,zPos / 10000.,0),
				.triCount = float4(obj->triangleCount,0,0,0),
				.brightness = float4(brightness,0,0,0),
				.tickness = float4(tickness,0,0,0),
				.zoom = float4(zm,zm,zm,1),
				.onLineOfs = (float)onLineOfs/1000.f,
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

	cmd(Mesh, int quality, int xPos, int yPos, int zPos, int brightness, int tickness, switcher stencil,int zoom, int onLineOfs, int jumpCharge)
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
			ShowMesh(MeshPtr, (int)MeshPtr->triangleCount,1,pMode::point,0,0,0, triMode::on, in.xPos, in.yPos, in.zPos,in.brightness,in.tickness,in.zoom,in.onLineOfs, in.jumpCharge);
		}

		Culling::Set({ cullmode::off });
		DepthBuf::Mode({ depthmode::readonly });
		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});

		ShowMesh(MeshPtr, count, 1, pMode::point, 100, 252, 1400, triMode::off, in.xPos, in.yPos, in.zPos, in.brightness, in.tickness,in.zoom, in.onLineOfs, in.jumpCharge);
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

	cmd(SetPointPosInLine, int line, int point, int x,int y, int z, int a)
	{
		reflect;
		starLineList.line[in.line].basePoint[in.point] = float4(in.x/ (float)denom,in.y/ (float)denom,in.z/ (float)denom,in.a);
	}

	cmd(AddPointToLine, int x, int y, int z, int a=0)
	{
		reflect;
		starLineList.line[currentLine].basePoint[currentPoint++] = float4(in.x / (float)denom, in.y / (float)denom, in.z / (float)denom, in.a);
		starLineList.line[currentLine].basePointCount = currentPoint;
	}

	void AddPoint( float4 p)
	{
		starLineList.line[currentLine].basePoint[currentPoint++] = p;
		starLineList.line[currentLine].basePointCount = currentPoint;
	}

	



	void smoothStarline(starline& line) {
		line.pointCount = 0; // Сбрасываем старый результат сглаживания

		float totalLength = 0;
		for (int i = 0; i < line.basePointCount-1; i++)
		{
			totalLength += distance(line.basePoint[i], line.basePoint[i + 1]);
		}

		int stepsPerSegment = totalLength/50.;
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
				line.point[line.pointCount] = lerp3(line.basePoint[i],line.basePoint[i+1],t);
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
		starLineList.lineCount = currentLine+1;
	}

	cmd(NewStar, int x, int y, int z, int rad, int r, int g, int b, int brightness)
	{
		reflect;

		currentLine++;
		currentPoint = 0;
		starLineList.lineCount = currentLine + 1;

		starLineList.line[currentLine].baseColor = float4(in.r / 100.f, in.g / 100.f, in.b / 100.f, in.brightness / 100.f );
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
		int seg = distance(start,end)*20.;
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
// ДЛИННЫЙ ОБЪЁМНЫЙ МАРШРУТ
// X/Y — структура рисунка без изменений
// Z — плавная волна, линии не лежат в одной плоскости
// ============================================================


// ============================================================
// УЧАСТОК 01
// ============================================================

// КОРОТКАЯ
		NewLine();
		AddPointToLine({ 10000, 50000, 49500 });
		AddPointToLine({ 10500, 49800, 49800 });
		AddPointToLine({ 11000, 49600, 50200 });
		AddPointToLine({ 11500, 49700, 50500 });
		AddPointToLine({ 12000, 50000, 50300 });
		AddPointToLine({ 12500, 50200, 49900 });

		// КОРОТКАЯ С ИЗГИБОМ ВВЕРХ
		NewLine();
		AddPointToLine({ 11200, 50700, 50400 });
		AddPointToLine({ 11700, 50500, 50700 });
		AddPointToLine({ 12200, 50100, 51000 });
		AddPointToLine({ 12700, 49700, 50800 });
		AddPointToLine({ 13200, 49600, 50400 });
		AddPointToLine({ 13700, 49800, 50000 });
		AddPointToLine({ 14200, 50100, 49800 });

		// КОРОТКАЯ С ИЗГИБОМ ВНИЗ
		NewLine();
		AddPointToLine({ 12500, 49300, 49700 });
		AddPointToLine({ 13000, 49500, 50000 });
		AddPointToLine({ 13500, 50000, 50400 });
		AddPointToLine({ 14000, 50400, 50700 });
		AddPointToLine({ 14500, 50600, 50500 });
		AddPointToLine({ 15000, 50500, 50100 });

		// ДЛИННАЯ ВОЛНА
		NewLine();
		AddPointToLine({ 14500, 49900, 50000 });
		AddPointToLine({ 15200, 49700, 50300 });
		AddPointToLine({ 15900, 49400, 50700 });
		AddPointToLine({ 16600, 49300, 51000 });
		AddPointToLine({ 17300, 49500, 50700 });
		AddPointToLine({ 18000, 49900, 50300 });
		AddPointToLine({ 18700, 50200, 49900 });
		AddPointToLine({ 19400, 50300, 50200 });


		// ============================================================
		// УЧАСТОК 02
		// ============================================================

		// ДЛИННАЯ С ИЗГИБОМ ВВЕРХ
		NewLine();
		AddPointToLine({ 16000, 51000, 50200 });
		AddPointToLine({ 16800, 50800, 50500 });
		AddPointToLine({ 17600, 50400, 50900 });
		AddPointToLine({ 18400, 49900, 51200 });
		AddPointToLine({ 19200, 49400, 51000 });
		AddPointToLine({ 20000, 49100, 50600 });
		AddPointToLine({ 20800, 49200, 50100 });
		AddPointToLine({ 21600, 49600, 49800 });
		AddPointToLine({ 22400, 50000, 50100 });

		// КОРОТКАЯ
		NewLine();
		AddPointToLine({ 19000, 51000, 49600 });
		AddPointToLine({ 19500, 50800, 49900 });
		AddPointToLine({ 20000, 50700, 50300 });
		AddPointToLine({ 20500, 50900, 50500 });
		AddPointToLine({ 21000, 51200, 50200 });

		// ДЛИННАЯ С ИЗГИБОМ ВНИЗ
		NewLine();
		AddPointToLine({ 20500, 48700, 50100 });
		AddPointToLine({ 21300, 48900, 50400 });
		AddPointToLine({ 22100, 49300, 50800 });
		AddPointToLine({ 22900, 49900, 51100 });
		AddPointToLine({ 23700, 50500, 50800 });
		AddPointToLine({ 24500, 50800, 50400 });
		AddPointToLine({ 25300, 50700, 50000 });
		AddPointToLine({ 26100, 50300, 50300 });

		// КОРОТКАЯ С ИЗГИБОМ ВВЕРХ
		NewLine();
		AddPointToLine({ 22500, 51500, 50500 });
		AddPointToLine({ 23100, 51200, 50800 });
		AddPointToLine({ 23700, 50700, 51100 });
		AddPointToLine({ 24300, 50300, 50800 });
		AddPointToLine({ 24900, 50200, 50400 });


		// ============================================================
		// УЧАСТОК 03
		// ============================================================

		// ДЛИННАЯ ВОЛНА
		NewLine();
		AddPointToLine({ 25000, 49700, 49800 });
		AddPointToLine({ 25800, 49500, 50100 });
		AddPointToLine({ 26600, 49600, 50500 });
		AddPointToLine({ 27400, 49900, 50800 });
		AddPointToLine({ 28200, 50300, 50500 });
		AddPointToLine({ 29000, 50500, 50100 });
		AddPointToLine({ 29800, 50300, 49700 });
		AddPointToLine({ 30600, 49800, 50000 });

		// КОРОТКАЯ С ИЗГИБОМ ВНИЗ
		NewLine();
		AddPointToLine({ 27000, 48700, 50300 });
		AddPointToLine({ 27600, 48900, 50600 });
		AddPointToLine({ 28200, 49300, 50900 });
		AddPointToLine({ 28800, 49800, 50700 });
		AddPointToLine({ 29400, 50100, 50300 });

		// КОРОТКАЯ С ИЗГИБОМ ВВЕРХ
		NewLine();
		AddPointToLine({ 28500, 51200, 49700 });
		AddPointToLine({ 29100, 50900, 50000 });
		AddPointToLine({ 29700, 50400, 50400 });
		AddPointToLine({ 30300, 49900, 50700 });
		AddPointToLine({ 30900, 49700, 50400 });

		// ДЛИННАЯ С ИЗГИБОМ ВВЕРХ
		NewLine();
		AddPointToLine({ 30000, 50900, 50100 });
		AddPointToLine({ 30800, 50700, 50400 });
		AddPointToLine({ 31600, 50300, 50800 });
		AddPointToLine({ 32400, 49700, 51100 });
		AddPointToLine({ 33200, 49000, 50800 });
		AddPointToLine({ 34000, 48600, 50400 });
		AddPointToLine({ 34800, 48700, 50000 });
		AddPointToLine({ 35600, 49200, 50300 });


		// ============================================================
		// УЧАСТОК 04
		// ============================================================

		// ДЛИННАЯ С ПЕТЛЁЙ
		NewLine();
		AddPointToLine({ 34500, 50000, 50400 });
		AddPointToLine({ 35200, 49700, 50700 });
		AddPointToLine({ 35900, 49300, 51000 });
		AddPointToLine({ 36600, 49000, 50700 });
		AddPointToLine({ 37300, 49200, 50300 });
		AddPointToLine({ 37800, 49700, 49900 });
		AddPointToLine({ 37900, 50300, 49600 });
		AddPointToLine({ 37500, 50800, 50000 });
		AddPointToLine({ 36800, 51000, 50400 });
		AddPointToLine({ 36200, 50700, 50700 });
		AddPointToLine({ 36100, 50200, 50400 });
		AddPointToLine({ 36500, 49800, 50000 });

		// КОРОТКАЯ
		NewLine();
		AddPointToLine({ 36500, 51500, 49800 });
		AddPointToLine({ 37100, 51300, 50100 });
		AddPointToLine({ 37700, 51200, 50500 });
		AddPointToLine({ 38300, 51400, 50800 });
		AddPointToLine({ 38900, 51600, 50500 });

		// ДЛИННАЯ ВОЛНА
		NewLine();
		AddPointToLine({ 38000, 48500, 50200 });
		AddPointToLine({ 38800, 48300, 50500 });
		AddPointToLine({ 39600, 48400, 50900 });
		AddPointToLine({ 40400, 48700, 50700 });
		AddPointToLine({ 41200, 49200, 50300 });
		AddPointToLine({ 42000, 49500, 49900 });
		AddPointToLine({ 42800, 49300, 49600 });
		AddPointToLine({ 43600, 48800, 50000 });

		// КОРОТКАЯ С ИЗГИБОМ ВНИЗ
		NewLine();
		AddPointToLine({ 40500, 50700, 50400 });
		AddPointToLine({ 41100, 50900, 50700 });
		AddPointToLine({ 41700, 51200, 51000 });
		AddPointToLine({ 42300, 51100, 50700 });
		AddPointToLine({ 42900, 50800, 50300 });


		// ============================================================
		// УЧАСТОК 05
		// ============================================================

		// ДЛИННАЯ С ИЗГИБОМ ВВЕРХ
		NewLine();
		AddPointToLine({ 42500, 50000, 50000 });
		AddPointToLine({ 43300, 49700, 50300 });
		AddPointToLine({ 44100, 49200, 50700 });
		AddPointToLine({ 44900, 48600, 51000 });
		AddPointToLine({ 45700, 48200, 50700 });
		AddPointToLine({ 46500, 48300, 50300 });
		AddPointToLine({ 47300, 48700, 49900 });
		AddPointToLine({ 48100, 49200, 50200 });

		// КОРОТКАЯ С ИЗГИБОМ ВНИЗ
		NewLine();
		AddPointToLine({ 45000, 50500, 50500 });
		AddPointToLine({ 45600, 50700, 50800 });
		AddPointToLine({ 46200, 51000, 51100 });
		AddPointToLine({ 46800, 50900, 50800 });
		AddPointToLine({ 47400, 50600, 50400 });

		// ДЛИННАЯ ВОЛНА
		NewLine();
		AddPointToLine({ 47000, 50000, 49800 });
		AddPointToLine({ 47800, 49800, 50100 });
		AddPointToLine({ 48600, 49500, 50500 });
		AddPointToLine({ 49400, 49600, 50800 });
		AddPointToLine({ 50200, 50000, 50500 });
		AddPointToLine({ 51000, 50400, 50100 });
		AddPointToLine({ 51800, 50500, 49800 });
		AddPointToLine({ 52600, 50200, 50100 });

		// КОРОТКАЯ
		NewLine();
		AddPointToLine({ 49000, 48600, 50300 });
		AddPointToLine({ 49600, 48400, 50600 });
		AddPointToLine({ 50200, 48500, 50900 });
		AddPointToLine({ 50800, 48800, 50600 });
		AddPointToLine({ 51400, 49000, 50200 });


		// ============================================================
		// УЧАСТОК 06
		// ============================================================

		// ДЛИННАЯ С ИЗГИБОМ ВНИЗ
		NewLine();
		AddPointToLine({ 51500, 49500, 50100 });
		AddPointToLine({ 52300, 49800, 50400 });
		AddPointToLine({ 53100, 50300, 50800 });
		AddPointToLine({ 53900, 50900, 51100 });
		AddPointToLine({ 54700, 51300, 50800 });
		AddPointToLine({ 55500, 51200, 50400 });
		AddPointToLine({ 56300, 50800, 50000 });
		AddPointToLine({ 57100, 50300, 50300 });

		// КОРОТКАЯ С ИЗГИБОМ ВВЕРХ
		NewLine();
		AddPointToLine({ 53500, 49000, 49700 });
		AddPointToLine({ 54100, 48700, 50000 });
		AddPointToLine({ 54700, 48300, 50400 });
		AddPointToLine({ 55300, 48200, 50700 });
		AddPointToLine({ 55900, 48400, 50400 });

		// ДЛИННАЯ ВОЛНА
		NewLine();
		AddPointToLine({ 55500, 49800, 50200 });
		AddPointToLine({ 56300, 49600, 50500 });
		AddPointToLine({ 57100, 49700, 50900 });
		AddPointToLine({ 57900, 50100, 51200 });
		AddPointToLine({ 58700, 50500, 50900 });
		AddPointToLine({ 59500, 50400, 50500 });
		AddPointToLine({ 60300, 50000, 50100 });
		AddPointToLine({ 61100, 49500, 50400 });

		// КОРОТКАЯ
		NewLine();
		AddPointToLine({ 58000, 51300, 50500 });
		AddPointToLine({ 58600, 51100, 50800 });
		AddPointToLine({ 59200, 50900, 51100 });
		AddPointToLine({ 59800, 51000, 50800 });
		AddPointToLine({ 60400, 51300, 50400 });


		// ============================================================
		// УЧАСТОК 07
		// ============================================================

		// ДЛИННАЯ С ПЕТЛЁЙ
		NewLine();
		AddPointToLine({ 60000, 49200, 49900 });
		AddPointToLine({ 60800, 48800, 50200 });
		AddPointToLine({ 61600, 48400, 50600 });
		AddPointToLine({ 62400, 48500, 50900 });
		AddPointToLine({ 63100, 49000, 50600 });
		AddPointToLine({ 63400, 49600, 50200 });
		AddPointToLine({ 63200, 50200, 49800 });
		AddPointToLine({ 62600, 50500, 49500 });
		AddPointToLine({ 62000, 50300, 49900 });
		AddPointToLine({ 61900, 49800, 50300 });
		AddPointToLine({ 62300, 49400, 50600 });
		AddPointToLine({ 63000, 49300, 50300 });

		// КОРОТКАЯ С ИЗГИБОМ ВНИЗ
		NewLine();
		AddPointToLine({ 62500, 51300, 50400 });
		AddPointToLine({ 63100, 51100, 50700 });
		AddPointToLine({ 63700, 50800, 51000 });
		AddPointToLine({ 64300, 50700, 50700 });
		AddPointToLine({ 64900, 51000, 50300 });

		// ДЛИННАЯ С ИЗГИБОМ ВВЕРХ
		NewLine();
		AddPointToLine({ 64000, 50000, 50100 });
		AddPointToLine({ 64800, 49600, 50400 });
		AddPointToLine({ 65600, 49000, 50800 });
		AddPointToLine({ 66400, 48500, 51100 });
		AddPointToLine({ 67200, 48400, 50800 });
		AddPointToLine({ 68000, 48700, 50400 });
		AddPointToLine({ 68800, 49200, 50000 });
		AddPointToLine({ 69600, 49600, 50300 });

		// КОРОТКАЯ
		NewLine();
		AddPointToLine({ 66500, 50500, 49800 });
		AddPointToLine({ 67100, 50700, 50100 });
		AddPointToLine({ 67700, 50800, 50500 });
		AddPointToLine({ 68300, 50600, 50800 });
		AddPointToLine({ 68900, 50300, 50500 });


		// ============================================================
		// УЧАСТОК 08
		// ============================================================

		// ДЛИННАЯ С ИЗГИБОМ ВНИЗ
		NewLine();
		AddPointToLine({ 68000, 50000, 50200 });
		AddPointToLine({ 68800, 50200, 50500 });
		AddPointToLine({ 69600, 50600, 50900 });
		AddPointToLine({ 70400, 51200, 51200 });
		AddPointToLine({ 71200, 51600, 50900 });
		AddPointToLine({ 72000, 51500, 50500 });
		AddPointToLine({ 72800, 51100, 50100 });
		AddPointToLine({ 73600, 50600, 50400 });

		// КОРОТКАЯ С ИЗГИБОМ ВВЕРХ
		NewLine();
		AddPointToLine({ 70500, 49400, 49700 });
		AddPointToLine({ 71100, 49100, 50000 });
		AddPointToLine({ 71700, 48700, 50400 });
		AddPointToLine({ 72300, 48600, 50700 });
		AddPointToLine({ 72900, 48800, 50400 });

		// ДЛИННАЯ ВОЛНА
		NewLine();
		AddPointToLine({ 72500, 50000, 50000 });
		AddPointToLine({ 73300, 49800, 50300 });
		AddPointToLine({ 74100, 49600, 50700 });
		AddPointToLine({ 74900, 49800, 51000 });
		AddPointToLine({ 75700, 50200, 50700 });
		AddPointToLine({ 76500, 50500, 50300 });
		AddPointToLine({ 77300, 50300, 49900 });
		AddPointToLine({ 78100, 49800, 50200 });

		// КОРОТКАЯ С ИЗГИБОМ ВНИЗ
		NewLine();
		AddPointToLine({ 75000, 51000, 50500 });
		AddPointToLine({ 75600, 51200, 50800 });
		AddPointToLine({ 76200, 51500, 51100 });
		AddPointToLine({ 76800, 51400, 50800 });
		AddPointToLine({ 77400, 51100, 50400 });


		// ============================================================
		// УЧАСТОК 09
		// ============================================================

		// ДЛИННАЯ С ПЕТЛЁЙ
		NewLine();
		AddPointToLine({ 77000, 49300, 49900 });
		AddPointToLine({ 77800, 48900, 50200 });
		AddPointToLine({ 78600, 48600, 50600 });
		AddPointToLine({ 79400, 48800, 50900 });
		AddPointToLine({ 80000, 49300, 50600 });
		AddPointToLine({ 80200, 49900, 50200 });
		AddPointToLine({ 79900, 50500, 49800 });
		AddPointToLine({ 79300, 50800, 49500 });
		AddPointToLine({ 78700, 50600, 49900 });
		AddPointToLine({ 78600, 50100, 50300 });
		AddPointToLine({ 79000, 49700, 50600 });
		AddPointToLine({ 79600, 49700, 50300 });

		// КОРОТКАЯ
		NewLine();
		AddPointToLine({ 79000, 51500, 50400 });
		AddPointToLine({ 79600, 51300, 50700 });
		AddPointToLine({ 80200, 51200, 51000 });
		AddPointToLine({ 80800, 51400, 50700 });
		AddPointToLine({ 81400, 51600, 50300 });

		// ДЛИННАЯ С ИЗГИБОМ ВВЕРХ
		NewLine();
		AddPointToLine({ 80500, 50200, 50100 });
		AddPointToLine({ 81300, 49800, 50400 });
		AddPointToLine({ 82100, 49200, 50800 });
		AddPointToLine({ 82900, 48700, 51100 });
		AddPointToLine({ 83700, 48600, 50800 });
		AddPointToLine({ 84500, 48900, 50400 });
		AddPointToLine({ 85300, 49400, 50000 });
		AddPointToLine({ 86100, 49800, 50300 });

		// КОРОТКАЯ С ИЗГИБОМ ВНИЗ
		NewLine();
		AddPointToLine({ 83000, 50700, 50500 });
		AddPointToLine({ 83600, 50900, 50800 });
		AddPointToLine({ 84200, 51200, 51100 });
		AddPointToLine({ 84800, 51100, 50800 });
		AddPointToLine({ 85400, 50800, 50400 });


		// ============================================================
		// УЧАСТОК 10
		// ============================================================

		// ДЛИННАЯ ВОЛНА
		NewLine();
		AddPointToLine({ 85000, 50100, 49800 });
		AddPointToLine({ 85800, 49800, 50100 });
		AddPointToLine({ 86600, 49600, 50500 });
		AddPointToLine({ 87400, 49800, 50800 });
		AddPointToLine({ 88200, 50200, 50500 });
		AddPointToLine({ 89000, 50500, 50100 });
		AddPointToLine({ 89800, 50300, 49700 });
		AddPointToLine({ 90600, 49800, 50000 });

		// КОРОТКАЯ С ИЗГИБОМ ВВЕРХ
		NewLine();
		AddPointToLine({ 87500, 48900, 50300 });
		AddPointToLine({ 88100, 48600, 50600 });
		AddPointToLine({ 88700, 48200, 50900 });
		AddPointToLine({ 89300, 48100, 50600 });
		AddPointToLine({ 89900, 48300, 50200 });

		// ДЛИННАЯ С ИЗГИБОМ ВНИЗ
		NewLine();
		AddPointToLine({ 89500, 50500, 50100 });
		AddPointToLine({ 90300, 50700, 50400 });
		AddPointToLine({ 91100, 51100, 50800 });
		AddPointToLine({ 91900, 51600, 51100 });
		AddPointToLine({ 92700, 51900, 50800 });
		AddPointToLine({ 93500, 51700, 50400 });
		AddPointToLine({ 94300, 51200, 50000 });
		AddPointToLine({ 95100, 50700, 50300 });

		// КОРОТКАЯ
		NewLine();
		AddPointToLine({ 92500, 49600, 49800 });
		AddPointToLine({ 93100, 49400, 50100 });
		AddPointToLine({ 93700, 49500, 50500 });
		AddPointToLine({ 94300, 49800, 50800 });
		AddPointToLine({ 94900, 50000, 50500 });


		// ============================================================
		// УЧАСТОК 11
		// ============================================================

		// ДЛИННАЯ С ПЕТЛЁЙ
		NewLine();
		AddPointToLine({ 94000, 50300, 50000 });
		AddPointToLine({ 94800, 49900, 50300 });
		AddPointToLine({ 95600, 49500, 50700 });
		AddPointToLine({ 96400, 49600, 51000 });
		AddPointToLine({ 97100, 50100, 50700 });
		AddPointToLine({ 97400, 50700, 50300 });
		AddPointToLine({ 97200, 51300, 49900 });
		AddPointToLine({ 96600, 51600, 49600 });
		AddPointToLine({ 96000, 51400, 50000 });
		AddPointToLine({ 95900, 50900, 50400 });
		AddPointToLine({ 96300, 50500, 50700 });
		AddPointToLine({ 96900, 50500, 50400 });

		// КОРОТКАЯ С ИЗГИБОМ ВВЕРХ
		NewLine();
		AddPointToLine({ 96000, 49000, 49700 });
		AddPointToLine({ 96600, 48700, 50000 });
		AddPointToLine({ 97200, 48300, 50400 });
		AddPointToLine({ 97800, 48200, 50700 });
		AddPointToLine({ 98400, 48400, 50400 });

		// ДЛИННАЯ ВОЛНА
		NewLine();
		AddPointToLine({ 97500, 50100, 50200 });
		AddPointToLine({ 98300, 49900, 50500 });
		AddPointToLine({ 99100, 49700, 50900 });
		AddPointToLine({ 99900, 49900, 51200 });
		AddPointToLine({ 100700, 50300, 50900 });
		AddPointToLine({ 101500, 50600, 50500 });
		AddPointToLine({ 102300, 50400, 50100 });
		AddPointToLine({ 103100, 50000, 50400 });

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
		//NewStar({
		//	.x = 30000,
		//	.y = 23000,
		//	.z = 23000,
		//	.rad = 6630,
		//	.r = 0,
		//	.g = 4,
		//	.b = 100,
		//	.brightness = 100
		//	});
		//
		//
		//// B — выше A, левее D
		//NewStar({
		//	.x = 36000,
		//	.y = 52000,
		//	.z = 60000,
		//	.rad = 6630,
		//	.r = 0,
		//	.g = 4,
		//	.b = 100,
		//	.brightness = 100
		//	});
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
			vs::maze.params.particlesCount = count/ in.skipper;
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

		vs::fish= {
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

		Drawer::NullDrawer({1,(int)gX*(int)gY});
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

	void Pillars(int count,int skipper, pMode mode)
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

		Drawer::NullDrawer({1,(int)gX*(int)gY});
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
		float4 base_color = float4(r,g,b, 1);

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

		Drawer::NullDrawer({1,(int)gX*(int)gY});
		
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
		InsideNebula({pillars_cnt,1,pMode::point,100,252,400});
		OuterSpace(outerSpace_cnt, 1, pMode::point);
		Galaxy({galaxy_cnt,14,pMode::point,254,220,41});

		//mid
		RenderTarget::Set({texture::pBufMid,0});
		RenderTarget::Clear({ 0,0,0,0 });
		Galaxy({ galaxy_cnt, 4, pMode::glow ,254,220,41 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });
		RenderTarget::Clear({ 0,0,0,0 });

		Tree(pillars_cnt, 1394 / 2, pMode::glow);
		InsideNebula({pillars_cnt,1394,pMode::glow,100,202,400});
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
		Islands({ pillars_cnt/2,1,pMode::point,130,112,10 });
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

		
		ScorpBall({ pillars_cnt/2,1,pMode::point,1390,925,111 });
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

		int pillars_cnt = 3725470/2 / in.quality;
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

		PillarsHand(pillars_cnt, 1394/2, pMode::glow);
		InsideNebula({ pillars_cnt, 1394, pMode::glow ,100,200,600});
	//	OuterSpace(outerSpace_cnt, 64, pMode::glow);

		
	}

	cmd(Saggitarius, int quality)
	{
		reflect;

		int pillars_cnt = 3725470/in.quality;
		int outerSpace_cnt = 6853 / in.quality;
		int neutronStar_cnt = 279620 / in.quality;
		int galaxy_cnt = 182361 / in.quality;

		//hi
		RenderTarget::Set({ texture::pBuf,0 });
		RenderTarget::Clear({ 0,0,0,0 });

			Pillars(pillars_cnt,1,pMode::point);
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
		Culling::Set({cullmode::off});
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
