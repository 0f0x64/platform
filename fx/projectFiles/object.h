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
	XMMATRIX heroWorld;

	void ShowMesh(dx11::ConstBuf::sbObject* obj, int count, int skipper, pMode mode, int r, int g, int b, triMode tMode, int xPos, int yPos, int zPos, int brightness, int tickness,int zoom)
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
				.modelPos = float4(xPos/100.,yPos / 100.,zPos / 100.,0),
				.triCount = float4(obj->triangleCount,0,0,0),
				.brightness = float4(brightness,0,0,0),
				.tickness = float4(tickness,0,0,0),
				.zoom = float4(zm,zm,zm,1)
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

	cmd(Mesh, int quality, int xPos, int yPos, int zPos, int brightness, int tickness, switcher stencil,int zoom)
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
			ShowMesh(MeshPtr, (int)MeshPtr->triangleCount,1,pMode::point,0,0,0, triMode::on, in.xPos, in.yPos, in.zPos,in.brightness,in.tickness,in.zoom);
		}

		Culling::Set({ cullmode::off });
		DepthBuf::Mode({ depthmode::readonly });
		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});

		ShowMesh(MeshPtr, count, 1, pMode::point, 100, 252, 1400, triMode::off, in.xPos, in.yPos, in.zPos, in.brightness, in.tickness,in.zoom);
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

	const int smoothPointMAX = 1000;

	struct starline {
		float4 basePoint[100];
		float4 point[smoothPointMAX];
		int basePointCount = 0;
		int pointCount = 0;
	};

	struct {
		starline line[100];
		int lineCount = 0;
	} starLineList;

	cmd(SetLineCount, int lineCount)
	{
		reflect;
		starLineList.lineCount = in.lineCount;
	}

	cmd(SetPointCountInLine, int line, int pointCount)
	{
		reflect;
		starLineList.line[in.line].basePointCount = in.pointCount;
	}

	cmd(SetPointPosInLine, int line, int point, int x,int y, int z, int a)
	{
		reflect;
		starLineList.line[in.line].basePoint[in.point] = float4(in.x,in.y,in.z,in.a);
	}

	// Вспомогательная функция сплайна Кэтмулла-Рома
	float4 catmullRom(const float4& p0, const float4& p1, const float4& p2, const float4& p3, float t) {
		float t2 = t * t;
		float t3 = t2 * t;

		float4 result;
		result.x = 0.5f * ((2.0f * p1.x) + (-p0.x + p2.x) * t + (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 + (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);
		result.y = 0.5f * ((2.0f * p1.y) + (-p0.y + p2.y) * t + (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 + (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);
		result.z = 0.5f * ((2.0f * p1.z) + (-p0.z + p2.z) * t + (2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t2 + (-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t3);
		result.w = 0.5f * ((2.0f * p1.w) + (-p0.w + p2.w) * t + (2.0f * p0.w - 5.0f * p1.w + 4.0f * p2.w - p3.w) * t2 + (-p0.w + 3.0f * p1.w - 3.0f * p2.w + p3.w) * t3);

		return result;
	}

	void smoothStarline(starline& line, int stepsPerSegment) {
		line.pointCount = 0; // Сбрасываем старый результат сглаживания

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

	void initPatches()
	{
		// init maze
		int ln = 0;
		int pt = 0;

		SetLineCount({ 1 });
		SetPointCountInLine({ ln,12 });

		pt = 0;
		SetPointPosInLine({ ln,pt++, 0,0,0 });
		SetPointPosInLine({ ln,pt++, 10,6,0 });
		SetPointPosInLine({ ln,pt++, 10,0,10 });
		SetPointPosInLine({ ln,pt++, 0,4,10 });

		SetPointPosInLine({ ln,pt++, -10,0,20 });
		SetPointPosInLine({ ln,pt++, 20,0,20 });
		SetPointPosInLine({ ln,pt++, 20,0,-10 });
		SetPointPosInLine({ ln,pt++, 0,0,-10 });

		SetPointPosInLine({ ln,pt++, 0,20,-10 });
		SetPointPosInLine({ ln,pt++, 10,25,10 });
		SetPointPosInLine({ ln,pt++, 10,10,10 });
		SetPointPosInLine({ ln,pt++, 10,-5,20 });

		smoothStarline(starLineList.line[ln], 12);
		//
	}

	cmd(Maze, int count, int skipper, pMode mode, int r, int g, int b)
	{
		reflect;

		int gX = sqrt(in.count / in.skipper);
		int gY = sqrt(in.count / in.skipper);

		psModeSet(in.mode);

		for (int i = 0; i < starLineList.lineCount; i++)
		{

			vs::maze = {
				.params = {
					.model = XMMatrixTranspose(XMMatrixTranslation(0,0,0)),
					.gX = gX,
					.gY = gY,
					.mode = (int)in.mode,
					.skipper = in.skipper,
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

		vrg({ pillars_cnt/2,1,pMode::point,1390,925,111 });
		Maze({ 200000,1,pMode::point,1390,925,111 });

		OuterSpace(outerSpace_cnt, 1, pMode::point);
		//NeutronStar(neutronStar_cnt, 1, pMode::point);

		//Galaxy({ galaxy_cnt, 14, pMode::point ,100,200,300 });

		//RenderTarget::Set({ texture::pBuf,0 });
		//RenderTarget::Clear({ 0,0,0,0 });

		//call show obj

		Culling::Set({ cullmode::off });
		DepthBuf::Mode({ depthmode::off });

		//mid
		RenderTarget::Set({ texture::pBufMid,0 });
		vrg({ pillars_cnt,94,pMode::glow,20,30,75 });
		Maze({ 200000,94,pMode::glow,20,30,75 });

		//Galaxy({ galaxy_cnt, 4, pMode::glow ,100,200,300 });

		//low
		RenderTarget::Set({ texture::pBufLow,0 });


		//Pillars(pillars_cnt, 10394, pMode::glow);
		OuterSpace(outerSpace_cnt, 64, pMode::glow);

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
