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

	cmd(NewStar, int x, int y, int z, int rad)
	{
		reflect;

		currentLine++;
		currentPoint = 0;
		starLineList.lineCount = currentLine + 1;

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
		// УЧАСТОК 1 — ГЕОМЕТРИЧЕСКИ РАЗВЁРНУТЫЙ
		// Масштаб всех координат: ×100
		// ============================================================


		// ============================================================
		// Л1 — бывшая последняя линия
		// ============================================================

		NewLine();
		AddPointToLine({ -200,     0,  1900 });
		AddPointToLine({ 300,  2800, 2200 });
		AddPointToLine({ 800,  5300, 2600 });
		AddPointToLine({ 1000,  7800, 3000 });
		AddPointToLine({ 800, 10300, 3400 });
		AddPointToLine({ 300, 12800, 3500 });
		AddPointToLine({ -200, 15300, 3200 });
		AddPointToLine({ -600, 17800, 2700 });
		AddPointToLine({ -800, 20300, 2100 });
		AddPointToLine({ -400, 26300, 1600 });

		NewLine();
		AddPointToLine({ 0, 25200, 1500 });
		AddPointToLine({ 500, 27500, 1900 });
		AddPointToLine({ 900, 29800, 2400 });
		AddPointToLine({ 1000, 32300, 2900 });
		AddPointToLine({ 600, 34800, 3100 });
		AddPointToLine({ 100, 37300, 2800 });
		AddPointToLine({ -300, 39800, 2300 });
		AddPointToLine({ -500, 42300, 1700 });
		AddPointToLine({ -100, 44800, 1300 });
		AddPointToLine({ 100, 52000, 1500 });

		NewLine();
		AddPointToLine({ 200, 48600, 1300 });
		AddPointToLine({ 900, 51700, 1800 });
		AddPointToLine({ 900, 54300, 2400 });
		AddPointToLine({ 500, 56800, 2900 });
		AddPointToLine({ 0, 59300, 3100 });
		AddPointToLine({ -400, 61800, 2900 });
		AddPointToLine({ -600, 64300, 2400 });
		AddPointToLine({ -500, 66800, 1800 });
		AddPointToLine({ 200, 71600, 2100 });
		AddPointToLine({ -100, 71900,-2200 });

		NewLine();
		AddPointToLine({ 3800, 74800,  700 });
		AddPointToLine({ -300, 71300,  900 });
		AddPointToLine({ -700, 74900, 1400 });
		AddPointToLine({ -800, 77200, 2000 });
		AddPointToLine({ -300, 79000, 2500 });
		AddPointToLine({ 300, 82100, 2700 });
		AddPointToLine({ 800, 83800, 2500 });
		AddPointToLine({ 1100, 86300, 2100 });
		AddPointToLine({ 1000, 88600, 1600 });
		AddPointToLine({ 700, 91300, 1100 });
		AddPointToLine({ 200, 93800,  700 });
		AddPointToLine({ -100, 96400,  500 });

		NewLine();
		AddPointToLine({ -600,  98300,  700 });
		AddPointToLine({ -800, 100800, 1100 });
		AddPointToLine({ -500, 103300, 1500 });
		AddPointToLine({ 0, 105800, 1700 });
		AddPointToLine({ 500, 108300, 1600 });
		AddPointToLine({ 800, 110800, 1300 });
		AddPointToLine({ 900, 113300,  900 });
		AddPointToLine({ 800, 115800,  500 });
		AddPointToLine({ 600, 118300,  200 });
		AddPointToLine({ 500, 120200,    0 });


		// ============================================================
		// Л2 — бывшие 3 линии
		// ============================================================

		NewLine();
		AddPointToLine({ -600, 46900, 1600 });
		AddPointToLine({ -800, 49300, 2100 });
		AddPointToLine({ -600, 51800, 2700 });
		AddPointToLine({ -200, 54300, 3200 });
		AddPointToLine({ 300, 56800, 3500 });
		AddPointToLine({ 700, 59300, 3400 });
		AddPointToLine({ 700, 61800, 3000 });
		AddPointToLine({ 400, 64300, 2400 });
		AddPointToLine({ -100, 66800, 1800 });
		AddPointToLine({ -400, 69600, 1400 });

		NewLine();
		AddPointToLine({ -2700, 73400, 1500 });
		AddPointToLine({ -800, 73200, 1900 });
		AddPointToLine({ -400, 75300, 2500 });
		AddPointToLine({ 100, 78500, 3000 });
		AddPointToLine({ 500, 81300, 3200 });
		AddPointToLine({ 700, 83800, 3000 });
		AddPointToLine({ 400, 86300, 2600 });
		AddPointToLine({ 0, 88800, 2100 });
		AddPointToLine({ -400, 91300, 1600 });
		AddPointToLine({ -500, 93900, 1200 });

		NewLine();
		AddPointToLine({ -100, 95900, 1000 });
		AddPointToLine({ 400, 98300, 1200 });
		AddPointToLine({ 800,100800, 1600 });
		AddPointToLine({ 900,103300, 2000 });
		AddPointToLine({ 700,105800, 2200 });
		AddPointToLine({ 300,108300, 2100 });
		AddPointToLine({ -100,110800, 1800 });
		AddPointToLine({ -400,113300, 1400 });
		AddPointToLine({ -500,115800, 1000 });
		AddPointToLine({ -400,118300,  700 });
		AddPointToLine({ 2300,125000, 2200 });


		// ============================================================
		// Л3
		// ============================================================

		NewLine();
		AddPointToLine({ -800, 48500, 1900 });
		AddPointToLine({ -900, 51300, 2400 });
		AddPointToLine({ -800, 53800, 2900 });
		AddPointToLine({ -400, 56300, 3400 });
		AddPointToLine({ 100, 58800, 3600 });
		AddPointToLine({ 500, 61300, 3400 });
		AddPointToLine({ 700, 63800, 2900 });
		AddPointToLine({ 600, 66300, 2300 });
		AddPointToLine({ 300, 68800, 1800 });
		AddPointToLine({ -100, 71300, 1500 });

		NewLine();
		AddPointToLine({ -800, 73200, 1800 });
		AddPointToLine({ -900, 75800, 2300 });
		AddPointToLine({ -500, 78300, 2800 });
		AddPointToLine({ 0, 80800, 3100 });
		AddPointToLine({ 400, 83300, 2900 });
		AddPointToLine({ 600, 85800, 2500 });
		AddPointToLine({ 500, 88300, 2000 });
		AddPointToLine({ 100, 90800, 1500 });
		AddPointToLine({ -300, 93300, 1000 });
		AddPointToLine({ -600, 96000,  700 });

		NewLine();
		AddPointToLine({ -700, 98300,  800 });
		AddPointToLine({ -600,100800, 1200 });
		AddPointToLine({ -300,103300, 1600 });
		AddPointToLine({ 100,105800, 1800 });
		AddPointToLine({ 400,108300, 1700 });
		AddPointToLine({ 500,110800, 1400 });
		AddPointToLine({ 300,113300, 1000 });
		AddPointToLine({ -100,115800,  600 });
		AddPointToLine({ -400,118300,  300 });
		AddPointToLine({ -500,120800,    0 });


		// ============================================================
		// Л4
		// ============================================================

		NewLine();
		AddPointToLine({ -700, 73800, 1400 });
		AddPointToLine({ -900, 76300, 1900 });
		AddPointToLine({ -500, 78800, 2400 });
		AddPointToLine({ 0, 81300, 2600 });
		AddPointToLine({ 500, 83800, 2300 });
		AddPointToLine({ 800, 86300, 1900 });
		AddPointToLine({ 900, 88800, 1400 });
		AddPointToLine({ 700, 91300,  900 });
		AddPointToLine({ 400, 93800,  500 });
		AddPointToLine({ 100, 96300,  200 });

		NewLine();
		AddPointToLine({ -400, 98300,  400 });
		AddPointToLine({ -800,100800,  800 });
		AddPointToLine({ -900,103300, 1200 });
		AddPointToLine({ -700,105800, 1400 });
		AddPointToLine({ -300,108300, 1300 });
		AddPointToLine({ 0,110800, 1000 });
		AddPointToLine({ 300,113300,  600 });
		AddPointToLine({ 500,115800,  200 });
		AddPointToLine({ 500,118300, -100 });
		AddPointToLine({ 300,120800, -400 });


		// ============================================================
		// Л5
		// ============================================================

		NewLine();
		AddPointToLine({ -500, 73800,    0 });
		AddPointToLine({ -400, 76300,  300 });
		AddPointToLine({ -200, 78800,  500 });
		AddPointToLine({ 100, 81300,  500 });
		AddPointToLine({ 400, 83800,  300 });
		AddPointToLine({ 500, 86300, -100 });
		AddPointToLine({ 300, 88800, -400 });
		AddPointToLine({ -100, 91300, -500 });
		AddPointToLine({ -400, 93800, -300 });
		AddPointToLine({ -500, 96300,  100 });

		NewLine();
		AddPointToLine({ -300, 98300,  400 });
		AddPointToLine({ 0,100800,  500 });
		AddPointToLine({ 300,103300,  400 });
		AddPointToLine({ 500,105800,  200 });
		AddPointToLine({ 500,108300, -100 });
		AddPointToLine({ 300,110800, -400 });
		AddPointToLine({ -100,113300, -500 });
		AddPointToLine({ -400,115800, -300 });
		AddPointToLine({ -500,118300,  100 });
		AddPointToLine({ -300,120800,  400 });


		// ============================================================
		// УЧАСТОК 2
		// ============================================================


		// ============================================================
		// Л1 — 0°
		// ============================================================

		NewLine();
		AddPointToLine({ 1000,124300,1900 });
		AddPointToLine({ 1700,124200,1900 });
		AddPointToLine({ 2500,124000,1900 });
		AddPointToLine({ 3300,123700,1900 });
		AddPointToLine({ 4100,123300,1900 });
		AddPointToLine({ 4800,122800,1900 });
		AddPointToLine({ 5400,122200,1900 });
		AddPointToLine({ 5800,121500,1900 });
		AddPointToLine({ 6000,120800,1900 });

		NewLine();
		AddPointToLine({ 3000,120800,1900 });
		AddPointToLine({ 8400,121000,2300 });
		AddPointToLine({ 10000,120900,2500 });
		AddPointToLine({ 11500,120700,2200 });
		AddPointToLine({ 13000,120900,1800 });
		AddPointToLine({ 14200,120800,1700 });

		NewLine();
		AddPointToLine({ 14800,120800,1900 });
		AddPointToLine({ 16200,120600,1500 });
		AddPointToLine({ 17800,120700,1300 });
		AddPointToLine({ 19400,121000,1600 });
		AddPointToLine({ 20900,120900,2100 });
		AddPointToLine({ 22300,120700,2300 });

		NewLine();
		AddPointToLine({ 23000,120800,2200 });
		AddPointToLine({ 24500,121000,2500 });
		AddPointToLine({ 26100,121100,2600 });
		AddPointToLine({ 27700,120900,2300 });
		AddPointToLine({ 29300,120600,1800 });
		AddPointToLine({ 30700,120800,1500 });

		NewLine();
		AddPointToLine({ 31400,120900,1700 });
		AddPointToLine({ 33000,121100,1400 });
		AddPointToLine({ 34600,121000,1200 });
		AddPointToLine({ 36200,120800,1500 });
		AddPointToLine({ 37900,120600,2000 });
		AddPointToLine({ 39600,120900,2300 });
		AddPointToLine({ 41300,120800,2100 });


		// ============================================================
		// Л2 — 90°
		// ============================================================

		NewLine();
		AddPointToLine({ 200,124300,3500 });
		AddPointToLine({ 800,124200,3500 });
		AddPointToLine({ 1500,124000,3500 });
		AddPointToLine({ 2200,123700,3500 });
		AddPointToLine({ 2900,123300,3500 });
		AddPointToLine({ 3600,122800,3500 });
		AddPointToLine({ 4200,122200,3500 });
		AddPointToLine({ 4600,121500,3500 });
		AddPointToLine({ 4800,120800,3500 });

		NewLine();
		AddPointToLine({ 4900,120900,3500 });
		AddPointToLine({ 7200,121100,3100 });
		AddPointToLine({ 8800,121000,2900 });
		AddPointToLine({ 10400,120800,3200 });
		AddPointToLine({ 12000,120600,3700 });
		AddPointToLine({ 13400,120900,3900 });

		NewLine();
		AddPointToLine({ 14000,120900,3700 });
		AddPointToLine({ 15500,120700,3300 });
		AddPointToLine({ 17100,120800,3000 });
		AddPointToLine({ 18700,121000,3300 });
		AddPointToLine({ 20300,120900,3800 });
		AddPointToLine({ 21800,120700,4000 });

		NewLine();
		AddPointToLine({ 22500,120800,3700 });
		AddPointToLine({ 24000,121000,3200 });
		AddPointToLine({ 25600,121100,2900 });
		AddPointToLine({ 27200,120900,3200 });
		AddPointToLine({ 28800,120600,3700 });
		AddPointToLine({ 30400,120800,4000 });

		NewLine();
		AddPointToLine({ 31100,120900,3700 });
		AddPointToLine({ 32700,121100,3300 });
		AddPointToLine({ 34300,121000,3000 });
		AddPointToLine({ 35900,120800,3300 });
		AddPointToLine({ 37500,120600,3800 });
		AddPointToLine({ 39200,120900,4000 });
		AddPointToLine({ 40900,120800,3700 });


		// ============================================================
		// Л3 — 180°
		// ============================================================

		NewLine();
		AddPointToLine({ -1300,124300,1900 });
		AddPointToLine({ -700,124200,1900 });
		AddPointToLine({ 0,124000,1900 });
		AddPointToLine({ 700,123700,1900 });
		AddPointToLine({ 1400,123300,1900 });
		AddPointToLine({ 2100,122800,1900 });
		AddPointToLine({ 2700,122200,1900 });
		AddPointToLine({ 3100,121500,1900 });
		AddPointToLine({ 3300,120800,1900 });

		NewLine();
		AddPointToLine({ 4300,120800,1900 });
		AddPointToLine({ 5700,121000,1500 });
		AddPointToLine({ 7300,120900,1200 });
		AddPointToLine({ 8900,120700,1500 });
		AddPointToLine({ 10500,120900,2000 });
		AddPointToLine({ 11900,120800,2300 });

		NewLine();
		AddPointToLine({ 12600,120800,2100 });
		AddPointToLine({ 14100,120600,2500 });
		AddPointToLine({ 15700,120700,2700 });
		AddPointToLine({ 17300,120900,2400 });
		AddPointToLine({ 18900,121000,1800 });
		AddPointToLine({ 20400,120800,1500 });

		NewLine();
		AddPointToLine({ 21100,120900,1700 });
		AddPointToLine({ 22600,121100,2200 });
		AddPointToLine({ 24200,121000,2600 });
		AddPointToLine({ 25800,120800,2400 });
		AddPointToLine({ 27400,120600,1800 });
		AddPointToLine({ 29000,120800,1400 });

		NewLine();
		AddPointToLine({ 29700,120900,1600 });
		AddPointToLine({ 31300,121100,1200 });
		AddPointToLine({ 32900,121000,1000 });
		AddPointToLine({ 34500,120800,1400 });
		AddPointToLine({ 36100,120600,2000 });
		AddPointToLine({ 37700,120800,2400 });
		AddPointToLine({ 39400,120900,2100 });


		// ============================================================
		// Л4 — 270°
		// ============================================================

		NewLine();
		AddPointToLine({ 200,124300, 300 });
		AddPointToLine({ 800,124200, 300 });
		AddPointToLine({ 1500,124000, 300 });
		AddPointToLine({ 2200,123700, 300 });
		AddPointToLine({ 2900,123300, 300 });
		AddPointToLine({ 3600,122800, 300 });
		AddPointToLine({ 4200,122200, 300 });
		AddPointToLine({ 4600,121500, 300 });
		AddPointToLine({ 4800,120800, 300 });

		NewLine();
		AddPointToLine({ 4600,120900, 300 });
		AddPointToLine({ 7200,121100, 700 });
		AddPointToLine({ 8800,121000, 900 });
		AddPointToLine({ 10400,120800, 600 });
		AddPointToLine({ 12000,120600, 100 });
		AddPointToLine({ 13400,120900,-100 });

		NewLine();
		AddPointToLine({ 14000,120900, 100 });
		AddPointToLine({ 15500,120700, 500 });
		AddPointToLine({ 17100,120800, 800 });
		AddPointToLine({ 18700,121000, 500 });
		AddPointToLine({ 20300,120900,   0 });
		AddPointToLine({ 21800,120700,-300 });

		NewLine();
		AddPointToLine({ 22500,120800, 100 });
		AddPointToLine({ 24000,121000, 600 });
		AddPointToLine({ 25600,121100, 900 });
		AddPointToLine({ 27200,120900, 500 });
		AddPointToLine({ 28800,120600,   0 });
		AddPointToLine({ 30400,120800,-300 });

		NewLine();
		AddPointToLine({ 31100,120900, 100 });
		AddPointToLine({ 32700,121100, 500 });
		AddPointToLine({ 34300,121000, 800 });
		AddPointToLine({ 35900,120800, 500 });
		AddPointToLine({ 37500,120600,   0 });
		AddPointToLine({ 39200,120900,-300 });
		AddPointToLine({ 40900,120800,   0 });


		// ============================================================
		// УЧАСТОК 3
		// ============================================================


		// ============================================================
		// ЛИНИЯ 1
		// ============================================================

		NewLine();
		AddPointToLine({ 41300,120800, 1900 });
		AddPointToLine({ 42000,120800,  200 });
		AddPointToLine({ 43900,120800,-1000 });
		AddPointToLine({ 50600,120800,-3400 });
		AddPointToLine({ 56700,120800, 1800 });

		NewLine();
		AddPointToLine({ 41300,120800, 1900 });
		AddPointToLine({ 43100,120600, 3100 });
		AddPointToLine({ 44500,120300, 4400 });
		AddPointToLine({ 44300,120400, 5700 });
		AddPointToLine({ 43800,120700, 6900 });
		AddPointToLine({ 42300,121000, 8200 });
		AddPointToLine({ 39800,121200, 9400 });
		AddPointToLine({ 38900,121100,10900 });
		AddPointToLine({ 39300,120500,12400 });

		NewLine();
		AddPointToLine({ 38800,121100,11900 });
		AddPointToLine({ 39300,120700,13400 });
		AddPointToLine({ 41300,120200,14900 });
		AddPointToLine({ 43300,120400,16400 });
		AddPointToLine({ 44300,121100,17900 });
		AddPointToLine({ 42800,121400,19400 });
		AddPointToLine({ 40100,121300,20900 });
		AddPointToLine({ 39100,120800,22200 });
		AddPointToLine({ 39500,120200,23600 });

		NewLine();
		AddPointToLine({ 38500,120700,22600 });
		AddPointToLine({ 39500,120400,24100 });
		AddPointToLine({ 42300,120100,25600 });
		AddPointToLine({ 44000,120500,27100 });
		AddPointToLine({ 43800,121100,28600 });
		AddPointToLine({ 41800,121400,30100 });
		AddPointToLine({ 38800,121200,31600 });
		AddPointToLine({ 38500,120800,32900 });
		AddPointToLine({ 40300,120200,34100 });

		NewLine();
		AddPointToLine({ 38300,120600,33100 });
		AddPointToLine({ 39500,120300,34600 });
		AddPointToLine({ 43100,120200,36100 });
		AddPointToLine({ 44300,120800,37600 });
		AddPointToLine({ 42800,121400,39100 });
		AddPointToLine({ 39800,121400,40600 });
		AddPointToLine({ 38300,120900,42100 });
		AddPointToLine({ 38800,120500,43400 });
		AddPointToLine({ 42300,120100,44600 });

		NewLine();
		AddPointToLine({ 40100,120300,43700 });
		AddPointToLine({ 42900,120100,45200 });
		AddPointToLine({ 44900,120400,46700 });
		AddPointToLine({ 44600,121000,48200 });
		AddPointToLine({ 42100,121500,49700 });
		AddPointToLine({ 39600,121200,51200 });
		AddPointToLine({ 39100,120600,52700 });
		AddPointToLine({ 40100,120300,54000 });
		AddPointToLine({ 41100,120200,55200 });

		NewLine();
		AddPointToLine({ 41100,120200,54200 });
		AddPointToLine({ 42900,120300,55700 });
		AddPointToLine({ 45100,120800,57200 });
		AddPointToLine({ 44100,121300,58700 });
		AddPointToLine({ 40600,121300,60200 });
		AddPointToLine({ 40900,120900,61500 });
		AddPointToLine({ 42100,120300,62700 });


		// ============================================================
		// ЛИНИЯ 2
		// ============================================================

		NewLine();
		AddPointToLine({ 41300,120800,1900 });
		AddPointToLine({ 39500,121000,3400 });
		AddPointToLine({ 38300,121300,4900 });
		AddPointToLine({ 39100,121400,6400 });
		AddPointToLine({ 41800,121100,7900 });
		AddPointToLine({ 43500,120700,9400 });
		AddPointToLine({ 43500,120300,10900 });
		AddPointToLine({ 42300,120200,12100 });
		AddPointToLine({ 41300,120300,12900 });

		NewLine();
		AddPointToLine({ 43500,120300,11900 });
		AddPointToLine({ 41800,120300,13400 });
		AddPointToLine({ 39100,120600,14900 });
		AddPointToLine({ 38300,121100,16400 });
		AddPointToLine({ 39800,121400,17900 });
		AddPointToLine({ 42500,121300,19400 });
		AddPointToLine({ 44300,120800,20900 });
		AddPointToLine({ 43800,120400,22200 });
		AddPointToLine({ 42300,120300,23400 });

		NewLine();
		AddPointToLine({ 39800,120200,22400 });
		AddPointToLine({ 38800,120300,23900 });
		AddPointToLine({ 38300,120600,25400 });
		AddPointToLine({ 39300,121100,26900 });
		AddPointToLine({ 42300,121500,28400 });
		AddPointToLine({ 44100,121100,29900 });
		AddPointToLine({ 43500,120300,31400 });
		AddPointToLine({ 41800,120100,32700 });
		AddPointToLine({ 40800,120100,33900 });

		NewLine();
		AddPointToLine({ 38500,120500,32900 });
		AddPointToLine({ 38300,120800,34400 });
		AddPointToLine({ 38800,121100,35900 });
		AddPointToLine({ 41300,121500,37400 });
		AddPointToLine({ 43800,121200,38900 });
		AddPointToLine({ 44300,120600,40400 });
		AddPointToLine({ 42300,120200,41900 });
		AddPointToLine({ 40300,120200,43200 });
		AddPointToLine({ 39300,120300,44400 });

		NewLine();
		AddPointToLine({ 38300,120900,43400 });
		AddPointToLine({ 38800,121300,44900 });
		AddPointToLine({ 39800,121400,46400 });
		AddPointToLine({ 41800,121200,47900 });
		AddPointToLine({ 44300,120800,49400 });
		AddPointToLine({ 43100,120300,50900 });
		AddPointToLine({ 40100,120200,52400 });
		AddPointToLine({ 38800,120400,53700 });
		AddPointToLine({ 38300,120700,54900 });

		NewLine();
		AddPointToLine({ 38300,120700,53900 });
		AddPointToLine({ 39800,121100,55400 });
		AddPointToLine({ 42300,121500,56900 });
		AddPointToLine({ 44300,121200,58400 });
		AddPointToLine({ 44800,120700,59900 });
		AddPointToLine({ 43300,120300,60900 });
		AddPointToLine({ 41300,120300,61900 });


		// ============================================================
		// ЛИНИЯ 3
		// ============================================================

		NewLine();
		AddPointToLine({ 41300,120800,1900 });
		AddPointToLine({ 42500,121300,3500 });
		AddPointToLine({ 43800,121300,5100 });
		AddPointToLine({ 44300,120900,6700 });
		AddPointToLine({ 42700,120500,8400 });
		AddPointToLine({ 40100,120300,10100 });
		AddPointToLine({ 37800,120600,11400 });
		AddPointToLine({ 38500,120900,12600 });
		AddPointToLine({ 39700,121200,13500 });

		NewLine();
		AddPointToLine({ 38400,120600,12500 });
		AddPointToLine({ 39700,121000,13400 });
		AddPointToLine({ 42200,121400,15100 });
		AddPointToLine({ 44100,120900,16400 });
		AddPointToLine({ 44400,120200,18100 });
		AddPointToLine({ 41900,120300,19400 });
		AddPointToLine({ 39100,120800,21100 });
		AddPointToLine({ 38200,121000,22400 });
		AddPointToLine({ 40600,121500,24100 });

		NewLine();
		AddPointToLine({ 38300,121200,23100 });
		AddPointToLine({ 40600,121400,23900 });
		AddPointToLine({ 43600,121100,25600 });
		AddPointToLine({ 44700,120300,26900 });
		AddPointToLine({ 43000,120100,28600 });
		AddPointToLine({ 40100,120600,29900 });
		AddPointToLine({ 38200,121300,31600 });
		AddPointToLine({ 38900,121600,32700 });
		AddPointToLine({ 40600,121200,34100 });

		NewLine();
		AddPointToLine({ 38300,120900,33100 });
		AddPointToLine({ 41000,121300,34600 });
		AddPointToLine({ 44300,121000,35900 });
		AddPointToLine({ 43000,120100,37600 });
		AddPointToLine({ 40100,120000,38900 });
		AddPointToLine({ 38300,120700,40600 });
		AddPointToLine({ 39900,121600,41900 });
		AddPointToLine({ 42700,121500,43400 });
		AddPointToLine({ 43300,120900,44600 });

		NewLine();
		AddPointToLine({ 44300,120700,43700 });
		AddPointToLine({ 43100,120100,45200 });
		AddPointToLine({ 41500,120000,46500 });
		AddPointToLine({ 39400,120700,48200 });
		AddPointToLine({ 38300,121400,49500 });
		AddPointToLine({ 40700,121700,51200 });
		AddPointToLine({ 43700,121200,52500 });
		AddPointToLine({ 44600,120500,54000 });
		AddPointToLine({ 44000,120300,55200 });

		NewLine();
		AddPointToLine({ 42300,120000,54200 });
		AddPointToLine({ 40900,120400,55000 });
		AddPointToLine({ 39000,121000,56700 });
		AddPointToLine({ 38200,121600,58000 });
		AddPointToLine({ 40300,121300,59700 });
		AddPointToLine({ 42600,120600,60800 });
		AddPointToLine({ 41300,120300,62100 });


		// ============================================================
		// ЛИНИЯ 2 — РАНДОМИЗИРОВАННАЯ
		// ВТОРАЯ ШИРОКАЯ ЛИНИЯ ТРЕТЬЕГО УЧАСТКА
		// ============================================================

		// 2A
		NewLine();
		AddPointToLine({ 56700,120800,1800 });
		AddPointToLine({ 54600,121300,3200 });
		AddPointToLine({ 53300,121000,4700 });
		AddPointToLine({ 53900,121600,6200 });
		AddPointToLine({ 56600,121200,7900 });
		AddPointToLine({ 59300,120500,9200 });
		AddPointToLine({ 59700,120000,10700 });
		AddPointToLine({ 58100,120600,12100 });
		AddPointToLine({ 56800,120100,12900 });

		// 2B
		NewLine();
		AddPointToLine({ 58800,120400,11900 });
		AddPointToLine({ 57300,120000,13400 });
		AddPointToLine({ 54900,120700,14700 });
		AddPointToLine({ 53300,121400,16400 });
		AddPointToLine({ 54600,121700,17700 });
		AddPointToLine({ 57500,121100,19400 });
		AddPointToLine({ 60000,120600,20700 });
		AddPointToLine({ 59600,120100,22200 });
		AddPointToLine({ 52300,120600,22900 });

		// 2C
		NewLine();
		AddPointToLine({ 54900,120300,22400 });
		AddPointToLine({ 53600,120100,23900 });
		AddPointToLine({ 53100,120900,25200 });
		AddPointToLine({ 54400,121600,26900 });
		AddPointToLine({ 57300,121400,28200 });
		AddPointToLine({ 59600,120700,29900 });
		AddPointToLine({ 58800,120000,31200 });
		AddPointToLine({ 56600,120400,32700 });
		AddPointToLine({ 55400,120200,33900 });


		// ============================================================
		// ЦЕНТРАЛЬНАЯ СПИРАЛЬНАЯ ВОРОНКА
		// ============================================================

		// Вход 1
		NewLine();
		AddPointToLine({ 54400,121600,26900 });
		AddPointToLine({ 53700,121500,27600 });
		AddPointToLine({ 53500,121400,28400 });
		AddPointToLine({ 53700,121300,29200 });
		AddPointToLine({ 54500,121300,29800 });
		AddPointToLine({ 55500,121300,30200 });

		// Вход 2
		NewLine();
		AddPointToLine({ 57300,121400,28200 });
		AddPointToLine({ 56700,121500,28800 });
		AddPointToLine({ 56200,121400,29400 });
		AddPointToLine({ 55900,121300,29800 });
		AddPointToLine({ 55500,121300,30200 });

		// Вход 3
		NewLine();
		AddPointToLine({ 59600,120700,29900 });
		AddPointToLine({ 58700,120800,30100 });
		AddPointToLine({ 57700,121000,30200 });
		AddPointToLine({ 56700,121200,30200 });
		AddPointToLine({ 55500,121300,30200 });


		// ============================================================
		// СПИРАЛЬНАЯ ВОРОНКА — НИТЬ 1
		// ============================================================

		NewLine();
		AddPointToLine({ 55500,121300,30200 });
		AddPointToLine({ 57200,121100,30600 });
		AddPointToLine({ 58700,120800,31600 });
		AddPointToLine({ 59800,120300,33100 });
		AddPointToLine({ 60100,119700,34800 });
		AddPointToLine({ 59600,119100,36500 });
		AddPointToLine({ 58300,118500,37800 });
		AddPointToLine({ 56700,117900,38400 });
		AddPointToLine({ 55100,117300,38000 });
		AddPointToLine({ 53800,116700,36800 });
		AddPointToLine({ 53200,116100,35200 });
		AddPointToLine({ 53400,115500,33500 });
		AddPointToLine({ 54400,114900,32200 });
		AddPointToLine({ 55800,114300,31500 });
		AddPointToLine({ 57300,113700,31700 });
		AddPointToLine({ 58400,113100,32800 });
		AddPointToLine({ 58900,112500,34300 });
		AddPointToLine({ 58500,111900,35800 });
		AddPointToLine({ 57400,111300,36800 });
		AddPointToLine({ 56000,110700,37000 });
		AddPointToLine({ 54900,110100,36200 });
		AddPointToLine({ 54300,109500,34900 });
		AddPointToLine({ 54500,108900,33600 });
		AddPointToLine({ 55400,108300,32800 });
		AddPointToLine({ 56500,107700,32900 });
		AddPointToLine({ 57400,107100,33700 });
		AddPointToLine({ 57700,106500,34800 });
		AddPointToLine({ 57200,105900,35700 });
		AddPointToLine({ 56300,105300,36000 });
		AddPointToLine({ 55600,104700,35500 });
		AddPointToLine({ 55400,104100,34600 });
		AddPointToLine({ 56000,103500,33900 });
		AddPointToLine({ 56700,102800,33800 });


		// ============================================================
		// СПИРАЛЬНАЯ ВОРОНКА — НИТЬ 2
		// ============================================================

		NewLine();
		AddPointToLine({ 55500,121300,30200 });
		AddPointToLine({ 54500,121200,31600 });
		AddPointToLine({ 54000,120900,33300 });
		AddPointToLine({ 54300,120400,35100 });
		AddPointToLine({ 55400,119800,36500 });
		AddPointToLine({ 56900,119200,37400 });
		AddPointToLine({ 58600,118600,37200 });
		AddPointToLine({ 59800,118000,36000 });
		AddPointToLine({ 60100,117400,34300 });
		AddPointToLine({ 59600,116800,32700 });
		AddPointToLine({ 58300,116200,31600 });
		AddPointToLine({ 56800,115600,31400 });
		AddPointToLine({ 55400,115000,32200 });
		AddPointToLine({ 54700,114400,33500 });
		AddPointToLine({ 54900,113800,34900 });
		AddPointToLine({ 55900,113200,35800 });
		AddPointToLine({ 57200,112600,35900 });
		AddPointToLine({ 58200,112000,35100 });
		AddPointToLine({ 58600,111400,33900 });
		AddPointToLine({ 58100,110800,32900 });
		AddPointToLine({ 57100,110200,32500 });
		AddPointToLine({ 56000,109600,33000 });
		AddPointToLine({ 55400,109000,34000 });
		AddPointToLine({ 55700,108400,35000 });
		AddPointToLine({ 56600,107800,35500 });
		AddPointToLine({ 57400,107200,35000 });
		AddPointToLine({ 57700,106600,34100 });
		AddPointToLine({ 57200,106000,33400 });
		AddPointToLine({ 56500,105400,33300 });
		AddPointToLine({ 55900,104800,33800 });
		AddPointToLine({ 55800,104200,34500 });
		AddPointToLine({ 56300,103600,34900 });
		AddPointToLine({ 56700,103000,34600 });


		// ============================================================
		// СПИРАЛЬНАЯ ВОРОНКА — НИТЬ 3
		// ============================================================

		NewLine();
		AddPointToLine({ 55500,121300,30200 });
		AddPointToLine({ 54900,121100,31800 });
		AddPointToLine({ 55300,120800,33500 });
		AddPointToLine({ 56400,120300,34900 });
		AddPointToLine({ 57900,119700,35500 });
		AddPointToLine({ 59200,119100,35000 });
		AddPointToLine({ 59800,118500,33700 });
		AddPointToLine({ 59500,117900,32200 });
		AddPointToLine({ 58300,117300,31200 });
		AddPointToLine({ 56800,116700,31000 });
		AddPointToLine({ 55400,116100,31700 });
		AddPointToLine({ 54600,115500,33000 });
		AddPointToLine({ 54700,114900,34400 });
		AddPointToLine({ 55700,114300,35300 });
		AddPointToLine({ 57000,113700,35400 });
		AddPointToLine({ 58100,113100,34600 });
		AddPointToLine({ 58500,112500,33500 });
		AddPointToLine({ 58000,111900,32500 });
		AddPointToLine({ 57000,111300,32000 });
		AddPointToLine({ 56000,110700,32400 });
		AddPointToLine({ 55400,110100,33400 });
		AddPointToLine({ 55600,109500,34400 });
		AddPointToLine({ 56500,108900,35000 });
		AddPointToLine({ 57400,108300,34700 });
		AddPointToLine({ 57800,107700,33800 });
		AddPointToLine({ 57400,107100,33100 });
		AddPointToLine({ 56600,106500,32900 });
		AddPointToLine({ 56000,105900,33400 });
		AddPointToLine({ 55900,105300,34100 });
		AddPointToLine({ 56400,104700,34600 });
		AddPointToLine({ 57000,104100,34500 });
		AddPointToLine({ 57300,103500,34000 });
		AddPointToLine({ 56700,103000,33800 });


		// ============================================================
		// ПРОДОЛЖЕНИЕ ВТОРОЙ ШИРОКОЙ ЛИНИИ
		// ============================================================

		// 2D
		NewLine();
		AddPointToLine({ 53900,120600,32700 });
		AddPointToLine({ 53400,121200,34200 });
		AddPointToLine({ 54800,121600,35900 });
		AddPointToLine({ 57400,121300,37200 });
		AddPointToLine({ 59800,120700,38900 });
		AddPointToLine({ 59400,120100,40200 });
		AddPointToLine({ 57400,120000,41900 });
		AddPointToLine({ 55500,120600,43000 });
		AddPointToLine({ 54600,121300,44400 });

		// 2E
		NewLine();
		AddPointToLine({ 54800,121000,43400 });
		AddPointToLine({ 54800,121600,44700 });
		AddPointToLine({ 56100,121300,46400 });
		AddPointToLine({ 58300,120900,47700 });
		AddPointToLine({ 60000,120400,49400 });
		AddPointToLine({ 58800,120000,50700 });
		AddPointToLine({ 56100,120500,52400 });
		AddPointToLine({ 53800,120200,53700 });
		AddPointToLine({ 54700,120400,54900 });

		// 2F
		NewLine();
		AddPointToLine({ 54100,120700,53900 });
		AddPointToLine({ 55600,121400,55200 });
		AddPointToLine({ 58000,121600,56900 });
		AddPointToLine({ 59800,120900,58200 });
		AddPointToLine({ 59300,120200,59900 });
		AddPointToLine({ 57600,120100,60900 });
		AddPointToLine({ 56700,120300,61800 });
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

		NewStar({ 2700,-6300,2100,2630 });

		NewStar({ 8200,-9900,300,560 });

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
					.base_color = float4(in.r / 100.,in.g / 100.,in.b / 100.,1),
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
		
		vrg({ pillars_cnt/2,1,pMode::point,1390,925,111 });
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
		
		vrg({ pillars_cnt,94,pMode::glow,20,30,75 });
		Maze({ 200000,94,pMode::glow,20,30,75 });

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
