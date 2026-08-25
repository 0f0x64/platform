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
		float4 basePoint[10000];
		float4 point[smoothPointMAX];
		float4 upVector[smoothPointMAX];
		int basePointCount = 0;
		int pointCount = 0;
	};

	struct {
		starline line[200];
		int lineCount = 0;
	} starLineList;

	int currentLine = -1;
	int currentPoint = 0;
	
	cmd(SetLineCount, int lineCount)
	{
		reflect;
		starLineList.lineCount = in.lineCount;
	}

	const int denom = 1;

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
		if (stepsPerSegment < 2) stepsPerSegment = 2;

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
		AddPointToLine({ 0,-8,-16 });
		AddPointToLine({ 100,-111,0 });
		AddPointToLine({ 200,0,0 });
		AddPointToLine({ 200,79,0 });
		AddPointToLine({ 100,110,0 });
		AddPointToLine({ 0,6,0 });

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

		NewStar({ 27,-63,21,263 });

		NewStar({ 82,-99,3,56 });

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

/*for (int j = 0; j < 60; j++)
{
	NewLine();

	// Прямая линия из 5 точек. Для идеального лерпа этого достаточно,
	// сплайн Катмулла-Рома прорисует её как ровную световую струну
	int g = 5;

	// Случайный масштаб (расстояние от центра звезды до начала луча)
	float randScale = (float)rand() / RAND_MAX;
	float r_start = (3.0f + (randScale * 10.0f) / 7.0f) * 0.35f;

	// Длина самого лазерного штриха (в твоих пропорциях, например, небольшая фиксированная длина)
	float rayLength = 3.2f;
	float r_end = r_start + rayLength;

	// Рандомный разворот всей линии целиком по двум осям (Yaw и Pitch)
	float yaw = ((float)rand() / RAND_MAX) * XM_2PI; // Поворот вокруг Y
	float pitch = ((float)rand() / RAND_MAX) * XM_PI;  // Наклон вокруг X

	XMMATRIX finalRotation = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);

	for (int i = 0; i < g; i++)
	{
		// t строго от 0.0 (начало штриха) до 1.0 (конец штриха)
		float t = (float)i / (float)(g - 1);

		// ЧЕСТНЫЙ ЛЕРП: линия абсолютно прямая и направлена строго вдоль оси Y наружу
		float x_raw = 0.0f;
		float y_raw = r_start + (r_end - r_start) * t;
		float z_raw = 0.0f;

		XMVECTOR basePoint = XMVectorSet(x_raw, y_raw, z_raw, 1.0f);

		// Поворачиваем всю прямую линию одинаково
		XMVECTOR rotatedPoint = XMVector3Transform(basePoint, finalRotation);

		// ТВОЙ ИСХОДНЫЙ МАСШТАБ (* 40)
		int _x = (int)(XMVectorGetX(rotatedPoint) * 40.0f);
		int _y = (int)(XMVectorGetY(rotatedPoint) * 40.0f);
		int _z = (int)(XMVectorGetZ(rotatedPoint) * 40.0f);

		AddPointToLine({ _x, _y, _z });
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

			vs::maze.params.particlesCount = in.count;
			vs::maze.params.basePointsCount = starLineList.line[i].pointCount;

			for (int j = 0; j < starLineList.line[i].pointCount; j++)
			{
				vs::maze.params.basePoint[j] = starLineList.line[i].point[j];
			}

			vs::maze.set();

			Drawer::NullDrawer({ 1,in.count / in.skipper });
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
