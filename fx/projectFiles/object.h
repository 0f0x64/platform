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
// A -> B
// Рваные / плавающие нити
// ============================================================


// ------------------------------------------------------------
// НИТЬ 1
// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 31500, 25000, 25000 });
		AddPointToLine({ 32500, 25800, 26300 });
		AddPointToLine({ 33200, 27500, 27800 });
		AddPointToLine({ 32500, 29200, 29500 });
		AddPointToLine({ 30700, 30500, 31300 });
		AddPointToLine({ 29000, 31800, 32900 });
		AddPointToLine({ 28200, 33700, 34500 });
		AddPointToLine({ 29100, 35400, 36100 });
		AddPointToLine({ 31000, 36700, 37500 });
		AddPointToLine({ 33000, 38200, 38900 });
		AddPointToLine({ 34200, 40100, 40200 });
		AddPointToLine({ 33300, 41800, 41600 });


		// ------------------------------------------------------------
		// НИТЬ 2
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 30500, 25500, 24500 });
		AddPointToLine({ 31300, 27000, 25800 });
		AddPointToLine({ 32000, 28600, 27600 });
		AddPointToLine({ 31100, 30200, 29300 });
		AddPointToLine({ 29400, 31700, 31000 });
		AddPointToLine({ 27500, 32900, 32600 });
		AddPointToLine({ 26500, 34700, 34100 });
		AddPointToLine({ 27200, 36500, 35700 });
		AddPointToLine({ 29000, 37800, 37400 });
		AddPointToLine({ 31200, 39400, 39100 });
		AddPointToLine({ 32500, 41200, 40700 });


		// ------------------------------------------------------------
		// НИТЬ 3
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 29500, 26500, 25800 });
		AddPointToLine({ 30000, 28000, 27100 });
		AddPointToLine({ 29700, 29600, 28600 });
		AddPointToLine({ 28300, 31000, 30200 });
		AddPointToLine({ 26500, 32500, 31900 });
		AddPointToLine({ 24800, 34000, 33400 });
		AddPointToLine({ 24000, 35800, 35000 });
		AddPointToLine({ 24800, 37400, 36800 });
		AddPointToLine({ 26500, 38900, 38200 });
		AddPointToLine({ 28500, 40500, 39800 });
		AddPointToLine({ 30000, 42200, 41400 });


		// ------------------------------------------------------------
		// НИТЬ 4
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 32000, 27000, 26500 });
		AddPointToLine({ 33500, 28500, 27800 });
		AddPointToLine({ 34200, 30200, 29400 });
		AddPointToLine({ 33500, 31800, 31100 });
		AddPointToLine({ 31500, 33100, 32700 });
		AddPointToLine({ 29200, 34600, 34200 });
		AddPointToLine({ 27800, 36500, 35800 });
		AddPointToLine({ 28600, 38300, 37400 });
		AddPointToLine({ 30500, 39700, 39100 });
		AddPointToLine({ 32700, 41400, 40700 });
		AddPointToLine({ 34000, 43000, 42300 });


		// ------------------------------------------------------------
		// НИТЬ 5
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 33500, 29000, 27500 });
		AddPointToLine({ 34800, 30500, 28700 });
		AddPointToLine({ 35200, 32200, 30100 });
		AddPointToLine({ 34100, 33700, 31800 });
		AddPointToLine({ 32000, 35100, 33500 });
		AddPointToLine({ 29800, 36700, 35000 });
		AddPointToLine({ 28500, 38600, 36700 });
		AddPointToLine({ 29500, 40200, 38200 });
		AddPointToLine({ 31700, 41800, 39800 });
		AddPointToLine({ 33800, 43500, 41400 });


		// ------------------------------------------------------------
		// НИТЬ 6
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 34500, 31500, 28500 });
		AddPointToLine({ 36000, 33000, 29900 });
		AddPointToLine({ 36500, 34700, 31400 });
		AddPointToLine({ 35500, 36300, 33000 });
		AddPointToLine({ 33300, 37800, 34600 });
		AddPointToLine({ 31000, 39400, 36100 });
		AddPointToLine({ 30000, 41200, 37800 });
		AddPointToLine({ 31200, 42900, 39400 });
		AddPointToLine({ 33500, 44500, 41000 });
		AddPointToLine({ 35800, 45900, 42600 });


		// ------------------------------------------------------------
		// НИТЬ 7
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 35000, 34000, 30000 });
		AddPointToLine({ 36500, 35500, 31500 });
		AddPointToLine({ 37200, 37200, 33100 });
		AddPointToLine({ 36500, 38900, 34800 });
		AddPointToLine({ 34500, 40500, 36500 });
		AddPointToLine({ 32300, 42100, 38200 });
		AddPointToLine({ 31500, 43900, 39900 });
		AddPointToLine({ 33000, 45500, 41400 });
		AddPointToLine({ 35200, 47000, 42900 });


		// ------------------------------------------------------------
		// НИТЬ 8
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 36000, 36500, 31500 });
		AddPointToLine({ 37800, 38000, 32900 });
		AddPointToLine({ 38600, 39700, 34500 });
		AddPointToLine({ 37800, 41400, 36200 });
		AddPointToLine({ 35500, 43000, 38000 });
		AddPointToLine({ 33300, 44600, 39700 });
		AddPointToLine({ 32700, 46300, 41400 });
		AddPointToLine({ 34200, 47800, 43000 });
		AddPointToLine({ 36958, 49071, 44941 });


		// ============================================================
		// РАЗРЫВ
		// Здесь специально нет соединения.
		// ============================================================


		// ------------------------------------------------------------
		// НИТЬ 9
		// После разрыва нити снова начинают сходиться.
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 37500, 50000, 45500 });
		AddPointToLine({ 39000, 51500, 46800 });
		AddPointToLine({ 39800, 53000, 48200 });
		AddPointToLine({ 39000, 54500, 49700 });
		AddPointToLine({ 37200, 55800, 51200 });
		AddPointToLine({ 35000, 57000, 52700 });
		AddPointToLine({ 34200, 58500, 54200 });


		// ------------------------------------------------------------
		// НИТЬ 10
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 39000, 50500, 46200 });
		AddPointToLine({ 40700, 52000, 47600 });
		AddPointToLine({ 41500, 53600, 49100 });
		AddPointToLine({ 40700, 55200, 50700 });
		AddPointToLine({ 38800, 56600, 52300 });
		AddPointToLine({ 36500, 58000, 53800 });
		AddPointToLine({ 35500, 59500, 55300 });


		// ------------------------------------------------------------
		// НИТЬ 11
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 40500, 51500, 47000 });
		AddPointToLine({ 42200, 53100, 48400 });
		AddPointToLine({ 43000, 54800, 50000 });
		AddPointToLine({ 42100, 56400, 51600 });
		AddPointToLine({ 40200, 57900, 53200 });
		AddPointToLine({ 38000, 59300, 54700 });
		AddPointToLine({ 37000, 60800, 56200 });


		// ------------------------------------------------------------
		// НИТЬ 12
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 42000, 52500, 47800 });
		AddPointToLine({ 43800, 54100, 49200 });
		AddPointToLine({ 44700, 55800, 50700 });
		AddPointToLine({ 43800, 57500, 52300 });
		AddPointToLine({ 41800, 59000, 53900 });
		AddPointToLine({ 39500, 60500, 55400 });
		AddPointToLine({ 38500, 62000, 56900 });


		// ============================================================
		// ПОДХОД К B
		// Нити постепенно собираются возле B,
		// но не входят в саму звезду.
		// ============================================================


		// ------------------------------------------------------------
		// НИТЬ 13
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 44000, 59500, 55000 });
		AddPointToLine({ 45500, 61000, 56500 });
		AddPointToLine({ 47200, 62500, 58000 });
		AddPointToLine({ 49000, 64000, 59500 });
		AddPointToLine({ 51000, 65500, 61000 });
		AddPointToLine({ 53500, 67000, 62500 });
		AddPointToLine({ 56000, 68500, 64000 });
		AddPointToLine({ 59000, 70000, 65500 });


		// ------------------------------------------------------------
		// НИТЬ 14
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 45500, 60500, 55800 });
		AddPointToLine({ 47000, 62000, 57400 });
		AddPointToLine({ 48800, 63600, 58900 });
		AddPointToLine({ 50800, 65100, 60400 });
		AddPointToLine({ 53200, 66600, 62000 });
		AddPointToLine({ 55800, 68100, 63500 });
		AddPointToLine({ 58500, 69500, 65000 });
		AddPointToLine({ 61000, 70800, 66400 });


		// ------------------------------------------------------------
		// НИТЬ 15
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 47000, 61500, 56500 });
		AddPointToLine({ 48600, 63100, 58000 });
		AddPointToLine({ 50500, 64700, 59600 });
		AddPointToLine({ 52700, 66200, 61200 });
		AddPointToLine({ 55000, 67700, 62800 });
		AddPointToLine({ 57500, 69100, 64400 });
		AddPointToLine({ 60000, 70400, 65800 });
		AddPointToLine({ 62500, 71600, 67000 });


		// ------------------------------------------------------------
		// НИТЬ 16
		// Последний короткий кусок перед B.
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 63500, 70000, 66500 });
		AddPointToLine({ 65000, 70700, 67500 });
		AddPointToLine({ 66500, 71400, 68400 });
		AddPointToLine({ 68000, 72000, 69400 });
		AddPointToLine({ 69500, 72400, 70500 });


		// ------------------------------------------------------------
		// НИТЬ 17
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 64500, 69000, 67000 });
		AddPointToLine({ 66000, 69800, 68000 });
		AddPointToLine({ 67500, 70600, 69200 });
		AddPointToLine({ 69000, 71300, 70400 });


		// ------------------------------------------------------------
		// НИТЬ 18
		// Обход B сбоку.
		// Не заходим в радиус звезды.
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 65000, 68000, 69000 });
		AddPointToLine({ 66800, 69200, 70500 });
		AddPointToLine({ 68500, 70400, 71800 });
		AddPointToLine({ 70000, 71500, 73000 });
		AddPointToLine({ 71500, 72200, 74400 });

		// ============================================================
// ROUTE A -> C
// Рваные / плавающие нити
// ============================================================


// ------------------------------------------------------------
// НИТЬ 1
// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 31442, 15000, 24500 });
		AddPointToLine({ 35131, 19000, 26000 });
		AddPointToLine({ 35500, 24500, 27800 });
		AddPointToLine({ 36000, 27200, 29500 });
		AddPointToLine({ 37000, 31000, 31000 });
		AddPointToLine({ 36500, 32800, 32500 });
		AddPointToLine({ 35000, 34500, 33800 });
		AddPointToLine({ 33500, 36000, 35000 });


		// ------------------------------------------------------------
		// НИТЬ 2
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 32536, 29700, 25000 });
		AddPointToLine({ 34200, 28800, 26800 });
		AddPointToLine({ 35800, 28200, 28600 });
		AddPointToLine({ 37200, 30200, 30400 });
		AddPointToLine({ 37800, 32000, 32100 });
		AddPointToLine({ 37000, 33800, 33500 });
		AddPointToLine({ 35200, 35500, 34900 });
		AddPointToLine({ 33800, 37200, 36300 });


		// ------------------------------------------------------------
		// НИТЬ 3
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 30502, 30504, 24800 });
		AddPointToLine({ 31800, 29664, 26500 });
		AddPointToLine({ 33200, 29400, 28200 });
		AddPointToLine({ 34500, 31200, 30000 });
		AddPointToLine({ 34800, 33000, 31700 });
		AddPointToLine({ 33800, 34800, 33400 });
		AddPointToLine({ 32200, 36600, 35000 });
		AddPointToLine({ 31500, 38400, 36600 });


		// ------------------------------------------------------------
		// НИТЬ 4
		// Более глубокий изгиб
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 21976, 26500, 25500 });
		AddPointToLine({ 25700, 28500, 27200 });
		AddPointToLine({ 32000, 30500, 29000 });
		AddPointToLine({ 32500, 32500, 30800 });
		AddPointToLine({ 31800, 34500, 32500 });
		AddPointToLine({ 30000, 36500, 34200 });
		AddPointToLine({ 28800, 38500, 35800 });
		AddPointToLine({ 29500, 40500, 37500 });


		// ------------------------------------------------------------
		// НИТЬ 5
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 34000, 27000, 25800 });
		AddPointToLine({ 35500, 29000, 27600 });
		AddPointToLine({ 37000, 31000, 29500 });
		AddPointToLine({ 38200, 33000, 31300 });
		AddPointToLine({ 37800, 35000, 33000 });
		AddPointToLine({ 36200, 37000, 34600 });
		AddPointToLine({ 34500, 39000, 36300 });
		AddPointToLine({ 33800, 41000, 38000 });


		// ------------------------------------------------------------
		// НИТЬ 6
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 35000, 29000, 26500 });
		AddPointToLine({ 36500, 31000, 28400 });
		AddPointToLine({ 38000, 33000, 30300 });
		AddPointToLine({ 39200, 35000, 32100 });
		AddPointToLine({ 38800, 37000, 33900 });
		AddPointToLine({ 37200, 39000, 35700 });
		AddPointToLine({ 35500, 41000, 37500 });
		AddPointToLine({ 34800, 43000, 39300 });


		// ------------------------------------------------------------
		// НИТЬ 7
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 36000, 30500, 27200 });
		AddPointToLine({ 37500, 32500, 29000 });
		AddPointToLine({ 39000, 34500, 30900 });
		AddPointToLine({ 40200, 36500, 32800 });
		AddPointToLine({ 39800, 38500, 34600 });
		AddPointToLine({ 38200, 40500, 36500 });
		AddPointToLine({ 36500, 42500, 38400 });
		AddPointToLine({ 35800, 44500, 40200 });


		// ------------------------------------------------------------
		// НИТЬ 8
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 37000, 32000, 28000 });
		AddPointToLine({ 38800, 34000, 29900 });
		AddPointToLine({ 40200, 36000, 31900 });
		AddPointToLine({ 41500, 38000, 33900 });
		AddPointToLine({ 41000, 40000, 35900 });
		AddPointToLine({ 39400, 42000, 37900 });
		AddPointToLine({ 37800, 44000, 39900 });
		AddPointToLine({ 37200, 46000, 41800 });


		// ------------------------------------------------------------
		// РАЗРЫВ
		// ------------------------------------------------------------


		// ------------------------------------------------------------
		// НИТЬ 9
		// Выход из нижней части маршрута
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 38500, 47500, 43000 });
		AddPointToLine({ 40200, 48800, 44200 });
		AddPointToLine({ 42200, 50000, 45400 });
		AddPointToLine({ 44200, 50800, 46600 });
		AddPointToLine({ 46200, 51000, 47800 });
		AddPointToLine({ 48000, 50500, 49000 });


		// ------------------------------------------------------------
		// НИТЬ 10
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 37500, 48500, 43500 });
		AddPointToLine({ 39200, 49800, 44800 });
		AddPointToLine({ 41200, 51000, 46100 });
		AddPointToLine({ 43300, 51800, 47400 });
		AddPointToLine({ 45400, 52000, 48800 });
		AddPointToLine({ 47500, 51500, 50100 });


		// ------------------------------------------------------------
		// НИТЬ 11
		// Волна вверх
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 39500, 49000, 44000 });
		AddPointToLine({ 41500, 50500, 45300 });
		AddPointToLine({ 43800, 51700, 46800 });
		AddPointToLine({ 46200, 52200, 48300 });
		AddPointToLine({ 48500, 51800, 49800 });
		AddPointToLine({ 50700, 50800, 51300 });


		// ------------------------------------------------------------
		// НИТЬ 12
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 41000, 50000, 44500 });
		AddPointToLine({ 43000, 51500, 45900 });
		AddPointToLine({ 45200, 52500, 47500 });
		AddPointToLine({ 47500, 52800, 49000 });
		AddPointToLine({ 49800, 52200, 50500 });
		AddPointToLine({ 52000, 51000, 52000 });


		// ------------------------------------------------------------
		// НИТЬ 13
		// Начинаем сильно уходить вправо
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 44000, 52500, 45500 });
		AddPointToLine({ 46000, 53200, 47000 });
		AddPointToLine({ 48200, 53500, 48500 });
		AddPointToLine({ 50500, 53000, 50000 });
		AddPointToLine({ 52800, 51800, 51500 });
		AddPointToLine({ 55000, 50000, 53000 });
		AddPointToLine({ 57000, 48000, 54500 });


		// ------------------------------------------------------------
		// НИТЬ 14
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 45500, 53500, 46200 });
		AddPointToLine({ 47800, 54200, 47800 });
		AddPointToLine({ 50200, 54000, 49500 });
		AddPointToLine({ 52500, 53000, 51000 });
		AddPointToLine({ 54800, 51200, 52800 });
		AddPointToLine({ 57000, 49200, 54600 });
		AddPointToLine({ 59200, 47500, 56300 });


		// ------------------------------------------------------------
		// НИТЬ 15
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 47000, 54500, 47000 });
		AddPointToLine({ 49200, 55000, 48600 });
		AddPointToLine({ 51500, 54500, 50200 });
		AddPointToLine({ 53800, 53200, 51800 });
		AddPointToLine({ 56000, 51200, 53600 });
		AddPointToLine({ 58200, 49000, 55500 });
		AddPointToLine({ 60400, 47200, 57400 });


		// ------------------------------------------------------------
		// НИТЬ 16
		// Волна перед C
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 49000, 55500, 47800 });
		AddPointToLine({ 51200, 55800, 49200 });
		AddPointToLine({ 53500, 55000, 50700 });
		AddPointToLine({ 55700, 53500, 52500 });
		AddPointToLine({ 58000, 51500, 54300 });
		AddPointToLine({ 60200, 49500, 56000 });
		AddPointToLine({ 62500, 47800, 57800 });


		// ------------------------------------------------------------
		// НИТЬ 17
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 51000, 56000, 48500 });
		AddPointToLine({ 53200, 56200, 50000 });
		AddPointToLine({ 55500, 55500, 51600 });
		AddPointToLine({ 57800, 54000, 53200 });
		AddPointToLine({ 60000, 52000, 54800 });
		AddPointToLine({ 62200, 50000, 56400 });
		AddPointToLine({ 64500, 48200, 58000 });


		// ------------------------------------------------------------
		// НИТЬ 18
		// Последняя длинная нить
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 53500, 56500, 49200 });
		AddPointToLine({ 55700, 56500, 50700 });
		AddPointToLine({ 58000, 55800, 52200 });
		AddPointToLine({ 60200, 54500, 53700 });
		AddPointToLine({ 62500, 52800, 55200 });
		AddPointToLine({ 64700, 50800, 56800 });
		AddPointToLine({ 67000, 49000, 58200 });


		// ------------------------------------------------------------
		// НИТЬ 19
		// Подход к C
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 62500, 51500, 57500 });
		AddPointToLine({ 64500, 50000, 58200 });
		AddPointToLine({ 66500, 48500, 59000 });
		AddPointToLine({ 68500, 47200, 49500 });
		AddPointToLine({ 70500, 46500, 50000 });


		// ------------------------------------------------------------
		// НИТЬ 20
		// Подход к C с нижней стороны
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 64000, 52500, 57000 });
		AddPointToLine({ 66000, 51000, 58000 });
		AddPointToLine({ 68000, 49500, 59000 });
		AddPointToLine({ 70000, 48200, 50000 });
		AddPointToLine({ 72000, 47500, 51000 });


		// ------------------------------------------------------------
		// НИТЬ 21
		// Обход звезды C
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 69000, 45500, 50500 });
		AddPointToLine({ 71000, 45000, 50000 });
		AddPointToLine({ 73000, 45200, 49500 });
		AddPointToLine({ 75000, 46000, 49000 });
		AddPointToLine({ 77000, 47200, 48500 });


		// ------------------------------------------------------------
		// НИТЬ 22
		// Ещё один короткий обход C
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 70500, 44500, 51500 });
		AddPointToLine({ 72500, 44200, 51000 });
		AddPointToLine({ 74500, 44800, 50500 });
		AddPointToLine({ 76500, 46000, 50000 });

		// ============================================================
// РАЗВИЛКА A -> C
// Ветка от середины маршрута A-C -> D
// ============================================================


// ============================================================
// ЦЕНТР РАЗВИЛКИ
// Ответвление начинается примерно здесь:
// {50000, 52000, 49000}
// ============================================================


// ------------------------------------------------------------
// НИТЬ 1
// Основная ветка вверх к D
// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 48500, 50500, 47500 });
		AddPointToLine({ 49200, 51800, 48800 });
		AddPointToLine({ 50000, 53200, 50200 });
		AddPointToLine({ 51200, 54500, 51800 });
		AddPointToLine({ 52500, 55700, 53500 });
		AddPointToLine({ 53500, 57000, 55300 });
		AddPointToLine({ 53200, 58500, 57000 });
		AddPointToLine({ 52000, 60000, 59000 });
		AddPointToLine({ 50800, 61500, 61000 });
		AddPointToLine({ 51200, 62800, 63200 });
		AddPointToLine({ 52500, 63800, 65200 });


		// ------------------------------------------------------------
		// НИТЬ 2
		// Чуть левее основной
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 47800, 51000, 46800 });
		AddPointToLine({ 48200, 52400, 48200 });
		AddPointToLine({ 48800, 53900, 49600 });
		AddPointToLine({ 49800, 55300, 51200 });
		AddPointToLine({ 50800, 56700, 53000 });
		AddPointToLine({ 51200, 58200, 55000 });
		AddPointToLine({ 50500, 59600, 57200 });
		AddPointToLine({ 49500, 61000, 59500 });
		AddPointToLine({ 49700, 62300, 61800 });
		AddPointToLine({ 51000, 63500, 64000 });
		AddPointToLine({ 52500, 64500, 66000 });


		// ------------------------------------------------------------
		// НИТЬ 3
		// Более сильный изгиб влево
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 47200, 51500, 47000 });
		AddPointToLine({ 46800, 53000, 48200 });
		AddPointToLine({ 47200, 54500, 49700 });
		AddPointToLine({ 48200, 56000, 51200 });
		AddPointToLine({ 49000, 57500, 53000 });
		AddPointToLine({ 48500, 59000, 55000 });
		AddPointToLine({ 47500, 60500, 57000 });
		AddPointToLine({ 47800, 62000, 59200 });
		AddPointToLine({ 49000, 63300, 61500 });
		AddPointToLine({ 50500, 64300, 63800 });


		// ------------------------------------------------------------
		// НИТЬ 4
		// Правый изгиб
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 49500, 51000, 48000 });
		AddPointToLine({ 50500, 52400, 49300 });
		AddPointToLine({ 52000, 53800, 50800 });
		AddPointToLine({ 53500, 55000, 52500 });
		AddPointToLine({ 54800, 56300, 54500 });
		AddPointToLine({ 55000, 57800, 56500 });
		AddPointToLine({ 54200, 59300, 58500 });
		AddPointToLine({ 53000, 60800, 60500 });
		AddPointToLine({ 52500, 62200, 62500 });
		AddPointToLine({ 53500, 63500, 64500 });


		// ------------------------------------------------------------
		// НИТЬ 5
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 50200, 51500, 48500 });
		AddPointToLine({ 51500, 52900, 50000 });
		AddPointToLine({ 53000, 54300, 51500 });
		AddPointToLine({ 54200, 55700, 53200 });
		AddPointToLine({ 55000, 57200, 55000 });
		AddPointToLine({ 54500, 58700, 56800 });
		AddPointToLine({ 53500, 60200, 58700 });
		AddPointToLine({ 52800, 61700, 60700 });
		AddPointToLine({ 53500, 63100, 63000 });
		AddPointToLine({ 54800, 64200, 65000 });


		// ------------------------------------------------------------
		// НИТЬ 6
		// Волнистая центральная
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 49000, 52500, 48500 });
		AddPointToLine({ 50000, 54000, 50000 });
		AddPointToLine({ 51500, 55400, 51700 });
		AddPointToLine({ 52500, 56800, 53500 });
		AddPointToLine({ 52000, 58300, 55200 });
		AddPointToLine({ 50500, 59800, 57000 });
		AddPointToLine({ 49800, 61200, 59000 });
		AddPointToLine({ 50500, 62600, 61300 });
		AddPointToLine({ 52000, 63800, 63800 });
		AddPointToLine({ 53500, 64600, 65800 });


		// ------------------------------------------------------------
		// НИТЬ 7
		// Самая левая нить
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 46500, 52000, 47500 });
		AddPointToLine({ 45500, 53500, 48800 });
		AddPointToLine({ 45800, 55000, 50200 });
		AddPointToLine({ 46800, 56500, 51800 });
		AddPointToLine({ 47500, 58000, 53500 });
		AddPointToLine({ 46800, 59500, 55300 });
		AddPointToLine({ 46200, 61000, 57200 });
		AddPointToLine({ 47000, 62400, 59400 });
		AddPointToLine({ 48500, 63700, 61500 });


		// ------------------------------------------------------------
		// НИТЬ 8
		// Правая крайняя нить
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 50500, 52500, 49000 });
		AddPointToLine({ 52000, 53800, 50500 });
		AddPointToLine({ 53800, 55100, 52300 });
		AddPointToLine({ 55500, 56400, 54200 });
		AddPointToLine({ 56500, 57800, 56200 });
		AddPointToLine({ 56000, 59300, 58200 });
		AddPointToLine({ 54800, 60700, 60200 });
		AddPointToLine({ 54500, 62100, 62200 });
		AddPointToLine({ 55500, 63400, 64200 });


		// ============================================================
		// РАЗРЫВ
		// Нити не соединяются в одну сплошную трубу.
		// ============================================================


		// ------------------------------------------------------------
		// НИТЬ 9
		// Продолжение после разрыва
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 48500, 54500, 51000 });
		AddPointToLine({ 49200, 55800, 52500 });
		AddPointToLine({ 50000, 57100, 54200 });
		AddPointToLine({ 50800, 58400, 56000 });
		AddPointToLine({ 51500, 59700, 57800 });
		AddPointToLine({ 52000, 61000, 59800 });
		AddPointToLine({ 51800, 62200, 61800 });


		// ------------------------------------------------------------
		// НИТЬ 10
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 49500, 54800, 51500 });
		AddPointToLine({ 50500, 56200, 53000 });
		AddPointToLine({ 51500, 57600, 54600 });
		AddPointToLine({ 52200, 59000, 56200 });
		AddPointToLine({ 52500, 60400, 58000 });
		AddPointToLine({ 52200, 61800, 60200 });
		AddPointToLine({ 53000, 63200, 62500 });


		// ------------------------------------------------------------
		// НИТЬ 11
		// Изгиб вправо, затем обратно
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 50500, 55000, 52000 });
		AddPointToLine({ 52000, 56400, 53800 });
		AddPointToLine({ 53500, 57800, 55500 });
		AddPointToLine({ 54500, 59200, 57000 });
		AddPointToLine({ 54200, 60600, 58800 });
		AddPointToLine({ 53200, 62000, 60700 });
		AddPointToLine({ 53000, 63300, 62800 });


		// ------------------------------------------------------------
		// НИТЬ 12
		// Более глубокая волна
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 47500, 55200, 51500 });
		AddPointToLine({ 48000, 56700, 53200 });
		AddPointToLine({ 47800, 58200, 55000 });
		AddPointToLine({ 47000, 59700, 56800 });
		AddPointToLine({ 47500, 61200, 58700 });
		AddPointToLine({ 48800, 62600, 60800 });
		AddPointToLine({ 50500, 63800, 63000 });


		// ============================================================
		// ПОДХОД К D
		// ============================================================


		// ------------------------------------------------------------
		// НИТЬ 13
		// Обход D слева
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 49500, 61500, 60000 });
		AddPointToLine({ 49000, 62500, 62000 });
		AddPointToLine({ 49500, 63600, 64200 });
		AddPointToLine({ 50500, 64500, 66200 });
		AddPointToLine({ 51800, 65100, 68000 });
		AddPointToLine({ 53200, 65400, 69200 });


		// ------------------------------------------------------------
		// НИТЬ 14
		// Обход D справа
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 52500, 61500, 60500 });
		AddPointToLine({ 53500, 62600, 62500 });
		AddPointToLine({ 54800, 63600, 64500 });
		AddPointToLine({ 56000, 64400, 66200 });
		AddPointToLine({ 57000, 65000, 67800 });
		AddPointToLine({ 56500, 65500, 69300 });


		// ------------------------------------------------------------
		// НИТЬ 15
		// Последняя короткая нить перед D
		// Не попадает в центр звезды.
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 51500, 63500, 65000 });
		AddPointToLine({ 52200, 64200, 66500 });
		AddPointToLine({ 53000, 64800, 68000 });
		AddPointToLine({ 54000, 65100, 69400 });
		AddPointToLine({ 55000, 65000, 70500 });


		// ------------------------------------------------------------
		// НИТЬ 16
		// Ещё один короткий обход
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 53500, 63000, 65500 });
		AddPointToLine({ 54500, 63800, 67000 });
		AddPointToLine({ 55700, 64500, 68500 });
		AddPointToLine({ 57000, 65000, 69800 });

		// ============================================================
// ROUTE D -> E
// Рваные / плавающие нити
// ============================================================


// ------------------------------------------------------------
// НИТЬ 1
// Выход из D
// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 53500, 65000, 70000 });
		AddPointToLine({ 54200, 66200, 71200 });
		AddPointToLine({ 53800, 67600, 72800 });
		AddPointToLine({ 52500, 69000, 74500 });
		AddPointToLine({ 51000, 70300, 76000 });
		AddPointToLine({ 49500, 71600, 77500 });
		AddPointToLine({ 48000, 73000, 79000 });
		AddPointToLine({ 46500, 74400, 80800 });
		AddPointToLine({ 45000, 75800, 82500 });


		// ------------------------------------------------------------
		// НИТЬ 2
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 52500, 65500, 70800 });
		AddPointToLine({ 53000, 67000, 72000 });
		AddPointToLine({ 52300, 68400, 73700 });
		AddPointToLine({ 50800, 69800, 75200 });
		AddPointToLine({ 49200, 71100, 76800 });
		AddPointToLine({ 47500, 72500, 78500 });
		AddPointToLine({ 45800, 73900, 80200 });
		AddPointToLine({ 44200, 75400, 81800 });
		AddPointToLine({ 42500, 76800, 83500 });


		// ------------------------------------------------------------
		// НИТЬ 3
		// Сильнее уходит влево
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 51500, 65800, 71500 });
		AddPointToLine({ 51000, 67200, 72800 });
		AddPointToLine({ 49800, 68600, 74200 });
		AddPointToLine({ 48000, 70000, 75800 });
		AddPointToLine({ 46000, 71300, 77500 });
		AddPointToLine({ 44200, 72700, 79200 });
		AddPointToLine({ 42500, 74100, 81000 });
		AddPointToLine({ 40800, 75600, 82800 });
		AddPointToLine({ 39200, 77100, 84500 });


		// ------------------------------------------------------------
		// НИТЬ 4
		// Верхняя волна
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 54000, 66000, 70500 });
		AddPointToLine({ 54800, 67500, 72000 });
		AddPointToLine({ 54500, 69000, 73800 });
		AddPointToLine({ 53500, 70400, 75400 });
		AddPointToLine({ 51800, 71800, 77000 });
		AddPointToLine({ 50000, 73100, 78700 });
		AddPointToLine({ 48000, 74500, 80500 });
		AddPointToLine({ 46200, 75900, 82300 });
		AddPointToLine({ 44500, 77300, 84000 });


		// ------------------------------------------------------------
		// НИТЬ 5
		// Нижняя волна
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 51000, 66500, 70000 });
		AddPointToLine({ 49800, 67900, 71600 });
		AddPointToLine({ 48200, 69400, 73300 });
		AddPointToLine({ 46300, 70800, 75000 });
		AddPointToLine({ 44500, 72200, 76700 });
		AddPointToLine({ 43000, 73700, 78400 });
		AddPointToLine({ 41400, 75200, 80200 });
		AddPointToLine({ 39800, 76800, 82000 });
		AddPointToLine({ 38000, 78200, 83800 });


		// ------------------------------------------------------------
		// НИТЬ 6
		// Волна вправо-влево
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 52000, 67000, 71000 });
		AddPointToLine({ 52600, 68400, 72500 });
		AddPointToLine({ 52000, 69800, 74100 });
		AddPointToLine({ 50500, 71200, 75800 });
		AddPointToLine({ 48800, 72600, 77500 });
		AddPointToLine({ 47000, 74000, 79200 });
		AddPointToLine({ 45200, 75500, 81000 });
		AddPointToLine({ 43500, 77000, 82700 });
		AddPointToLine({ 41800, 78500, 84400 });


		// ------------------------------------------------------------
		// НИТЬ 7
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 50000, 67500, 70500 });
		AddPointToLine({ 48800, 69000, 72000 });
		AddPointToLine({ 47200, 70400, 73800 });
		AddPointToLine({ 45500, 71900, 75500 });
		AddPointToLine({ 43800, 73400, 77300 });
		AddPointToLine({ 42000, 74900, 79100 });
		AddPointToLine({ 40300, 76400, 80900 });
		AddPointToLine({ 38600, 77900, 82700 });
		AddPointToLine({ 37000, 79400, 84400 });


		// ------------------------------------------------------------
		// НИТЬ 8
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 54500, 68000, 71200 });
		AddPointToLine({ 55000, 69500, 72800 });
		AddPointToLine({ 54200, 71000, 74600 });
		AddPointToLine({ 52800, 72400, 76300 });
		AddPointToLine({ 51000, 73800, 78000 });
		AddPointToLine({ 49200, 75200, 79800 });
		AddPointToLine({ 47500, 76700, 81600 });
		AddPointToLine({ 45800, 78200, 83300 });
		AddPointToLine({ 44000, 79700, 85000 });


		// ============================================================
		// РАЗРЫВ
		// ============================================================


		// ------------------------------------------------------------
		// НИТЬ 9
		// Продолжение после разрыва
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 43000, 79000, 84000 });
		AddPointToLine({ 41200, 80000, 85500 });
		AddPointToLine({ 39500, 81000, 86800 });
		AddPointToLine({ 37800, 81900, 88000 });
		AddPointToLine({ 36000, 82800, 89100 });
		AddPointToLine({ 34200, 83700, 90200 });
		AddPointToLine({ 32500, 84600, 91200 });


		// ------------------------------------------------------------
		// НИТЬ 10
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 44500, 79500, 84500 });
		AddPointToLine({ 42800, 80600, 86000 });
		AddPointToLine({ 41000, 81700, 87300 });
		AddPointToLine({ 39200, 82700, 88600 });
		AddPointToLine({ 37400, 83700, 89800 });
		AddPointToLine({ 35500, 84700, 91000 });
		AddPointToLine({ 33800, 85700, 92000 });


		// ------------------------------------------------------------
		// НИТЬ 11
		// Сильная волна
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 42000, 80200, 85000 });
		AddPointToLine({ 40500, 81500, 86400 });
		AddPointToLine({ 39000, 82800, 87800 });
		AddPointToLine({ 37200, 84000, 89200 });
		AddPointToLine({ 35000, 85100, 90500 });
		AddPointToLine({ 33000, 86100, 91600 });
		AddPointToLine({ 31200, 87000, 92500 });


		// ------------------------------------------------------------
		// НИТЬ 12
		// Более верхняя траектория
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 44000, 80500, 85000 });
		AddPointToLine({ 42500, 81800, 86600 });
		AddPointToLine({ 40800, 83100, 88200 });
		AddPointToLine({ 39000, 84300, 89800 });
		AddPointToLine({ 37000, 85500, 91200 });
		AddPointToLine({ 35000, 86600, 92400 });
		AddPointToLine({ 33000, 87700, 93400 });


		// ------------------------------------------------------------
		// НИТЬ 13
		// Начинаем обход E
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 36000, 85000, 90000 });
		AddPointToLine({ 34200, 86200, 91200 });
		AddPointToLine({ 32500, 87300, 92200 });
		AddPointToLine({ 30800, 88300, 93000 });
		AddPointToLine({ 29200, 89100, 93600 });
		AddPointToLine({ 27500, 89700, 94000 });


		// ------------------------------------------------------------
		// НИТЬ 14
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 34500, 84500, 89500 });
		AddPointToLine({ 32800, 85800, 90800 });
		AddPointToLine({ 31000, 87000, 91900 });
		AddPointToLine({ 29200, 88100, 92900 });
		AddPointToLine({ 27500, 89000, 93700 });
		AddPointToLine({ 25800, 89700, 94200 });


		// ------------------------------------------------------------
		// НИТЬ 15
		// Верхний обход звезды E
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 33000, 85500, 90500 });
		AddPointToLine({ 31200, 86800, 91800 });
		AddPointToLine({ 29400, 87900, 93000 });
		AddPointToLine({ 27600, 88800, 94000 });
		AddPointToLine({ 25800, 89400, 94700 });
		AddPointToLine({ 24000, 89600, 95200 });


		// ------------------------------------------------------------
		// НИТЬ 16
		// Нижний обход E
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 32000, 86500, 90000 });
		AddPointToLine({ 30200, 87700, 91400 });
		AddPointToLine({ 28400, 88800, 92600 });
		AddPointToLine({ 26600, 89700, 93600 });
		AddPointToLine({ 24800, 90300, 94400 });
		AddPointToLine({ 23000, 90600, 95000 });


		// ------------------------------------------------------------
		// НИТЬ 17
		// Короткая нить перед E
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 28000, 88000, 92500 });
		AddPointToLine({ 26200, 88900, 93600 });
		AddPointToLine({ 24500, 89500, 94500 });
		AddPointToLine({ 22800, 89900, 95200 });


		// ------------------------------------------------------------
		// НИТЬ 18
		// Последний подход к E
		// Не входит в центр звезды.
		// ------------------------------------------------------------

		NewLine();
		AddPointToLine({ 30000, 89500, 92500 });
		AddPointToLine({ 28200, 90100, 93500 });
		AddPointToLine({ 26400, 90500, 94300 });
		AddPointToLine({ 24600, 90800, 95000 });
		AddPointToLine({ 22800, 90900, 95500 });

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
			.x = 30000,
			.y = 23000,
			.z = 23000,
			.rad = 6630,
			.r = 0,
			.g = 4,
			.b = 100,
			.brightness = 100
			});


		// B — выше A, левее D
		NewStar({
			.x = 36000,
			.y = 52000,
			.z = 60000,
			.rad = 6630,
			.r = 0,
			.g = 4,
			.b = 100,
			.brightness = 100
			});


		// D — выше и правее B
		NewStar({
			.x = 52000,
			.y = 65000,
			.z = 72000,
			.rad = 6630,
			.r = 0,
			.g = 4,
			.b = 100,
			.brightness = 100
			});


		// E — самая высокая и левая
		NewStar({
			.x = 15000,
			.y = 90000,
			.z = 95000,
			.rad = 6630,
			.r = 0,
			.g = 4,
			.b = 100,
			.brightness = 100
			});


		// C — справа и значительно ниже D
		NewStar({
			.x = 85000,
			.y = 48000,
			.z = 48000,
			.rad = 6630,
			.r = 0,
			.g = 4,
			.b = 100,
			.brightness = 100
			});

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
