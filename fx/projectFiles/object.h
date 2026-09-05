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
	float length = ::std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

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

	struct mesh {
		bool loaded = false;
		float4 modelCenterScale = float4(0, 0, 0, 0);

		ConstBuf::vertex* vArray = nullptr;
		ConstBuf::index* iArray = nullptr;

		uint32_t vertexCount = 0;
		uint32_t triangleCount = 0;

		ID3D11Buffer* pSBuffer[2] = { nullptr, nullptr };
		ID3D11ShaderResourceView* pSB_SRV[2] = { nullptr, nullptr };

		void CreateSB(int slot, int size, int count, auto& data)
		{

			if (pSBuffer[slot]) { pSBuffer[slot]->Release(); pSBuffer[slot] = nullptr; }
			if (pSB_SRV[slot]) { pSB_SRV[slot]->Release(); pSB_SRV[slot] = nullptr; }


			D3D11_SUBRESOURCE_DATA initData = {};
			initData.pSysMem = data;

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = size * count;
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Для чтения в шейдере
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			bufferDesc.StructureByteStride = size; // Обязательно для Structured Buffer

			HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &pSBuffer[slot]);

			// 2. Создаем Shader Resource View (SRV)
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_UNKNOWN; // Всегда UNKNOWN для Structured Buffer
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.NumElements = count;

			HRESULT hr2 = device->CreateShaderResourceView(pSBuffer[slot], &srvDesc, &pSB_SRV[slot]);
		}

		void BindSB(int slot)
		{
			context->VSSetShaderResources(slot, 1, &pSB_SRV[slot]);
		}

		void LoadObj(const char* name)
		{
			if (loaded) return;

			if (ConstBuf::LoadObjToPointersGLTF(name, &vArray, &iArray, vertexCount, triangleCount))
			{
				ConstBuf::gltfAnim::scene.modelPath = name ? name : "";
				ConstBuf::gltfAnim::scene.animationPath.clear();
				ConstBuf::gltfAnim::scene.status = "Model loaded";
				if (vertexCount > 0)
				{
					float xMax = vArray[0].position.x;
					float xMin = vArray[0].position.x;
					float yMax = vArray[0].position.y;
					float yMin = vArray[0].position.y;
					float zMax = vArray[0].position.z;
					float zMin = vArray[0].position.z;

					for (int i = 1; i < vertexCount; i++)
					{
						xMax = max(xMax, vArray[i].position.x);
						xMin = min(xMin, vArray[i].position.x);
						yMax = max(yMax, vArray[i].position.y);
						yMin = min(yMin, vArray[i].position.y);
						zMax = max(zMax, vArray[i].position.z);
						zMin = min(zMin, vArray[i].position.z);
					}

					float xCenter = (xMax + xMin) / 2.0f;
					float yCenter = (yMax + yMin) / 2.0f;
					float zCenter = (zMax + zMin) / 2.0f;
					float xSize = xMax - xMin;
					float ySize = yMax - yMin;
					float zSize = zMax - zMin;
					float maxSize = max(max(xSize, ySize), zSize);
					float scale = maxSize > 0.00001f ? (4.0f / maxSize) : 1.0f;

					modelCenterScale = float4(xCenter, yCenter, zCenter, scale);
				}

				CreateSB(0, sizeof(ConstBuf::vertex), vertexCount, vArray);
				CreateSB(1, sizeof(ConstBuf::index), triangleCount, iArray);

				loaded = true;
				Log("GLTF model loaded successfully\n");

				ConstBuf::gltfAnim::LoadAnimationFile("..//fx//projectFiles//Idle.glb", true); // 1 Бездействие
				ConstBuf::gltfAnim::LoadAnimationFile("..//fx//projectFiles//Landing_Misha.glb", true); // 2 Присяд
				ConstBuf::gltfAnim::LoadAnimationFile("..//fx//projectFiles//Walk.glb", true); // 3 Ходьба
				ConstBuf::gltfAnim::LoadAnimationFile("..//fx//projectFiles//Run.glb", true); // 4 Бег
				ConstBuf::gltfAnim::LoadAnimationFile("..//fx//projectFiles//Falling.glb", true); // 5 Падение
				ConstBuf::gltfAnim::LoadAnimationFile("..//fx//projectFiles//Braking.glb", true); // 6 Торможение
				ConstBuf::gltfAnim::LoadAnimationFile("..//fx//projectFiles//TurnAroundRight.glb", true); // 7 Разворот через правое плечо
				ConstBuf::gltfAnim::LoadAnimationFile("..//fx//projectFiles//Sliding.glb", true); // 8 Скольжение

				ConstBuf::gltfAnim::scene.animations[0].isPlaying = false;

				ConstBuf::gltfAnim::scene.animations[1].looped = true;

				ConstBuf::gltfAnim::scene.animations[2].speed = 0.0f;
				ConstBuf::gltfAnim::scene.animations[2].weight = 100000.0f;

				ConstBuf::gltfAnim::scene.animations[3].looped = true;

				ConstBuf::gltfAnim::scene.animations[4].looped = true;

				ConstBuf::gltfAnim::scene.animations[5].looped = true;
				ConstBuf::gltfAnim::scene.animations[5].speed = 0.1f;

				ConstBuf::gltfAnim::scene.animations[6].speed = 0.0f;
				ConstBuf::gltfAnim::scene.animations[6].weight = 10000.0f;

				ConstBuf::gltfAnim::scene.animations[7].speed = 0.0f;
				ConstBuf::gltfAnim::scene.animations[7].weight = 10000000.0f;

				ConstBuf::gltfAnim::scene.animations[8].speed = 0.0f;
				ConstBuf::gltfAnim::scene.animations[8].weight = 10000.0f;
			}
			else
			{
				ConstBuf::gltfAnim::scene.status = "Model load failed";
				Log("GLTF model load failed\n");
			}
		}

		void LoadToShaders() {
			BindSB(0);
			BindSB(1);
		}
	};

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

	XMMATRIX heroOnRails;
	XMMATRIX heroWorld;

	void ShowMesh(mesh* obj, int count, int skipper, pMode mode, int r, int g, int b, triMode tMode, int xPos, int yPos, int zPos, int brightness, int tickness,int zoom, int onLineOfs, int jumpCharge)
	{

		int gX = sqrt(count / skipper);
		int gY = sqrt(count / skipper);

		psModeSet(mode);
		float zm = zoom / 100. + 1;

		float4 centerScale = ConstBuf::gltfAnim::scene.modelCenterScale;
		uint32_t triCnt = (obj && obj->loaded) ? obj->triangleCount : ConstBuf::triangleCount;

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
				.triCount = float4(triCnt,0,0,0),
				.brightness = float4(brightness,0,0,0),
				.tickness = float4(tickness,0,0,0),
				.modelCenterScale = centerScale,
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

		dx11::ConstBuf::gltfAnim::Update(1.0f / FRAMES_PER_SECOND);
		dx11::ConstBuf::gltfAnim::BindBones(dx11::context);

		if (obj && obj->loaded) {
			obj->LoadToShaders();
		}
		else {
			ConstBuf::BindSB(0);
			ConstBuf::BindSB(1);
		}


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

	cmd(Mesh, mesh* obj, int quality, int xPos, int yPos, int zPos, int brightness, int tickness, switcher stencil,int zoom, int onLineOfs, int jumpCharge)
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
			uint32_t triCnt = (in.obj && in.obj->loaded) ? in.obj->triangleCount : ConstBuf::triangleCount;
			ShowMesh(in.obj, (int)triCnt,1,pMode::point,0,0,0, triMode::on, in.xPos, in.yPos, in.zPos,in.brightness,in.tickness,in.zoom,in.onLineOfs, in.jumpCharge);
		}

		Culling::Set({ cullmode::off });
		DepthBuf::Mode({ depthmode::readonly });
		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});

		ShowMesh(in.obj, count, 1, pMode::point, 100, 252, 1400, triMode::off, in.xPos, in.yPos, in.zPos, in.brightness, in.tickness,in.zoom, in.onLineOfs, in.jumpCharge);
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
		float4 upVector[smoothPointMAX];
		int basePointCount = 0;
		int pointCount = 0;
	};

	struct {
		starline line[100];
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

		int stepsPerSegment = totalLength/25.;
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

		int X = static_cast<int>(::std::floor(x)) & 255;
		int Y = static_cast<int>(::std::floor(y)) & 255;
		int Z = static_cast<int>(::std::floor(z)) & 255;

		x -= ::std::floor(x);
		y -= ::std::floor(y);
		z -= ::std::floor(z);

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
		

//#define Actual
#ifdef Actual
		NewLine();
		AddPointToLine({ 0,0,0 });
		AddPointToLine({ 100,0,0 });
		AddPointToLine({ 200,0,0 });
		AddPointToLine({ 200,110,0 });
		AddPointToLine({ 100,110,0 });
		AddPointToLine({ 0,10,0 });

		NewLine();
		AddPointToLine({ 0,0,3 });
		AddPointToLine({ 100,0,10 });
		AddPointToLine({ 200,0,13 });
		AddPointToLine({ 200,110,13 });
		AddPointToLine({ 100,110,3 });
		AddPointToLine({ 0,10,3 });
#endif

#define Tisha
#ifdef Tisha
		// ============================================================
// УЧАСТОК 1 — ГЕОМЕТРИЧЕСКИ РАЗВЁРНУТЫЙ
// Направление движения: от бывшего конца к бывшему началу
// Y = 1208 -> Y = 0
// ============================================================


// ============================================================
// Л1 — бывшая последняя линия
// ============================================================

		NewLine();
		AddPointToLine({ -2,    0,  19 });
		AddPointToLine({ 3,   28,  22 });
		AddPointToLine({ 8,   53,  26 });
		AddPointToLine({ 10,   78,  30 });
		AddPointToLine({ 8,  103, 34 });
		AddPointToLine({ 3,  128, 35 });
		AddPointToLine({ -2, 153,  32 });
		AddPointToLine({ -6, 178,  27 });
		AddPointToLine({ -8, 203,  21 });
		AddPointToLine({ -4, 263,  16 });

		NewLine();
		AddPointToLine({ 0, 252,  15 });
		AddPointToLine({ 5, 275,  19 });
		AddPointToLine({ 9, 298,  24 });
		AddPointToLine({ 10, 323,  29 });
		AddPointToLine({ 6, 348,  31 });
		AddPointToLine({ 1, 373,  28 });
		AddPointToLine({ -3, 398,  23 });
		AddPointToLine({ -5, 423,  17 });
		AddPointToLine({ -1, 448,  13 });
		AddPointToLine({ 1, 520,  15 });

		NewLine();
		AddPointToLine({ 2, 486,  13 });
		AddPointToLine({ 9, 517,  18 });
		AddPointToLine({ 9, 543,  24 });
		AddPointToLine({ 5, 568,  29 });
		AddPointToLine({ 0, 593,  31 });
		AddPointToLine({ -4, 618,  29 });
		AddPointToLine({ -6, 643,  24 });
		AddPointToLine({ -5, 668,  18 });
		AddPointToLine({ 2, 716,  21 });
		AddPointToLine({ -1, 719,   -22 });

		NewLine();
		AddPointToLine({ 38, 748,   7 });
		AddPointToLine({ -3, 713,   9 });
		AddPointToLine({ -7, 749,  14 });
		AddPointToLine({ -8, 772,  20 });
		AddPointToLine({ -3, 790,  25 });
		AddPointToLine({ 3, 821,  27 });
		AddPointToLine({ 8, 838,  25 });
		AddPointToLine({ 11, 863,  21 });
		AddPointToLine({ 10, 886,  16 });
		AddPointToLine({ 7, 913,  11 });
		AddPointToLine({ 2, 938,   7 });
		AddPointToLine({ -1, 964,   5 });

		NewLine();
		AddPointToLine({ -6, 983,   7 });
		AddPointToLine({ -8,1008,  11 });
		AddPointToLine({ -5,1033,  15 });
		AddPointToLine({ 0,1058,  17 });
		AddPointToLine({ 5,1083,  16 });
		AddPointToLine({ 8,1108,  13 });
		AddPointToLine({ 9,1133,   9 });
		AddPointToLine({ 8,1158,   5 });
		AddPointToLine({ 6,1183,   2 });
		AddPointToLine({ 5,1202,   0 });


		// ============================================================
		// Л2 — бывшие 3 линии
		// ============================================================

		NewLine();
		AddPointToLine({ -6, 469, 16 });
		AddPointToLine({ -8, 493, 21 });
		AddPointToLine({ -6, 518, 27 });
		AddPointToLine({ -2, 543, 32 });
		AddPointToLine({ 3, 568, 35 });
		AddPointToLine({ 7, 593, 34 });
		AddPointToLine({ 7, 618, 30 });
		AddPointToLine({ 4, 643, 24 });
		AddPointToLine({ -1,668, 18 });
		AddPointToLine({ -4,696, 14 });

		NewLine();
		AddPointToLine({ -27,734,15 });
		AddPointToLine({ -8,732,19 });
		AddPointToLine({ -4,753,25 });
		AddPointToLine({ 1,785,30 });
		AddPointToLine({ 5,813,32 });
		AddPointToLine({ 7,838,30 });
		AddPointToLine({ 4,863,26 });
		AddPointToLine({ 0,888,21 });
		AddPointToLine({ -4,913,16 });
		AddPointToLine({ -5,939,12 });

		NewLine();
		AddPointToLine({ -1,959,10 });
		AddPointToLine({ 4,983,12 });
		AddPointToLine({ 8,1008,16 });
		AddPointToLine({ 9,1033,20 });
		AddPointToLine({ 7,1058,22 });
		AddPointToLine({ 3,1083,21 });
		AddPointToLine({ -1,1108,18 });
		AddPointToLine({ -4,1133,14 });
		AddPointToLine({ -5,1158,10 });
		AddPointToLine({ -4,1183, 7 });
		AddPointToLine({ 23,1250, 22 });


		// ============================================================
		// Л3
		// ============================================================

		NewLine();
		AddPointToLine({ -8,485,19 });
		AddPointToLine({ -9,513,24 });
		AddPointToLine({ -8,538,29 });
		AddPointToLine({ -4,563,34 });
		AddPointToLine({ 1,588,36 });
		AddPointToLine({ 5,613,34 });
		AddPointToLine({ 7,638,29 });
		AddPointToLine({ 6,663,23 });
		AddPointToLine({ 3,688,18 });
		AddPointToLine({ -1,713,15 });

		NewLine();
		AddPointToLine({ -8,732,18 });
		AddPointToLine({ -9,758,23 });
		AddPointToLine({ -5,783,28 });
		AddPointToLine({ 0,808,31 });
		AddPointToLine({ 4,833,29 });
		AddPointToLine({ 6,858,25 });
		AddPointToLine({ 5,883,20 });
		AddPointToLine({ 1,908,15 });
		AddPointToLine({ -3,933,10 });
		AddPointToLine({ -6,960, 7 });

		NewLine();
		AddPointToLine({ -7,983, 8 });
		AddPointToLine({ -6,1008,12 });
		AddPointToLine({ -3,1033,16 });
		AddPointToLine({ 1,1058,18 });
		AddPointToLine({ 4,1083,17 });
		AddPointToLine({ 5,1108,14 });
		AddPointToLine({ 3,1133,10 });
		AddPointToLine({ -1,1158, 6 });
		AddPointToLine({ -4,1183, 3 });
		AddPointToLine({ -5,1208, 0 });


		// ============================================================
		// Л4
		// ============================================================

		NewLine();
		AddPointToLine({ -7,738,14 });
		AddPointToLine({ -9,763,19 });
		AddPointToLine({ -5,788,24 });
		AddPointToLine({ 0,813,26 });
		AddPointToLine({ 5,838,23 });
		AddPointToLine({ 8,863,19 });
		AddPointToLine({ 9,888,14 });
		AddPointToLine({ 7,913, 9 });
		AddPointToLine({ 4,938, 5 });
		AddPointToLine({ 1,963, 2 });

		NewLine();
		AddPointToLine({ -4,983, 4 });
		AddPointToLine({ -8,1008,8 });
		AddPointToLine({ -9,1033,12 });
		AddPointToLine({ -7,1058,14 });
		AddPointToLine({ -3,1083,13 });
		AddPointToLine({ 0,1108,10 });
		AddPointToLine({ 3,1133, 6 });
		AddPointToLine({ 5,1158, 2 });
		AddPointToLine({ 5,1183,-1 });
		AddPointToLine({ 3,1208,-4 });


		// ============================================================
		// Л5
		// ============================================================

		NewLine();
		AddPointToLine({ -5,738, 0 });
		AddPointToLine({ -4,763, 3 });
		AddPointToLine({ -2,788, 5 });
		AddPointToLine({ 1,813, 5 });
		AddPointToLine({ 4,838, 3 });
		AddPointToLine({ 5,863,-1 });
		AddPointToLine({ 3,888,-4 });
		AddPointToLine({ -1,913,-5 });
		AddPointToLine({ -4,938,-3 });
		AddPointToLine({ -5,963, 1 });

		NewLine();
		AddPointToLine({ -3,983, 4 });
		AddPointToLine({ 0,1008,5 });
		AddPointToLine({ 3,1033,4 });
		AddPointToLine({ 5,1058,2 });
		AddPointToLine({ 5,1083,-1 });
		AddPointToLine({ 3,1108,-4 });
		AddPointToLine({ -1,1133,-5 });
		AddPointToLine({ -4,1158,-3 });
		AddPointToLine({ -5,1183, 1 });
		AddPointToLine({ -3,1208, 4 });


		// ============================================================
		// УЧАСТОК 2
		// Перенесён к концу участка 1
		// Основное направление теперь продолжается от Y ≈ 1208
		// ============================================================


		// ============================================================
		// Л1 — 0°
		// ============================================================

		NewLine();
		AddPointToLine({ 10,  1243, 19 });
		AddPointToLine({ 17, 1242, 19 });
		AddPointToLine({ 25, 1240, 19 });
		AddPointToLine({ 33, 1237, 19 });
		AddPointToLine({ 41, 1233, 19 });
		AddPointToLine({ 48, 1228, 19 });
		AddPointToLine({ 54, 1222, 19 });
		AddPointToLine({ 58, 1215, 19 });
		AddPointToLine({ 60, 1208, 19 });

		NewLine();
		AddPointToLine({ 30, 1208, 19 });
		AddPointToLine({ 84, 1210, 23 });
		AddPointToLine({ 100, 1209, 25 });
		AddPointToLine({ 115, 1207, 22 });
		AddPointToLine({ 130, 1209, 18 });
		AddPointToLine({ 142, 1208, 17 });

		NewLine();
		AddPointToLine({ 148, 1208, 19 });
		AddPointToLine({ 162, 1206, 15 });
		AddPointToLine({ 178, 1207, 13 });
		AddPointToLine({ 194, 1210, 16 });
		AddPointToLine({ 209, 1209, 21 });
		AddPointToLine({ 223, 1207, 23 });

		NewLine();
		AddPointToLine({ 230, 1208, 22 });
		AddPointToLine({ 245, 1210, 25 });
		AddPointToLine({ 261, 1211, 26 });
		AddPointToLine({ 277, 1209, 23 });
		AddPointToLine({ 293, 1206, 18 });
		AddPointToLine({ 307, 1208, 15 });

		NewLine();
		AddPointToLine({ 314, 1209, 17 });
		AddPointToLine({ 330, 1211, 14 });
		AddPointToLine({ 346, 1210, 12 });
		AddPointToLine({ 362, 1208, 15 });
		AddPointToLine({ 379, 1206, 20 });
		AddPointToLine({ 396, 1209, 23 });
		AddPointToLine({ 413, 1208, 21 });


		// ============================================================
		// Л2 — 90°
		// ============================================================

		NewLine();
		AddPointToLine({ 2, 1243, 35 });
		AddPointToLine({ 8, 1242, 35 });
		AddPointToLine({ 15,1240, 35 });
		AddPointToLine({ 22,1237, 35 });
		AddPointToLine({ 29,1233, 35 });
		AddPointToLine({ 36,1228, 35 });
		AddPointToLine({ 42,1222, 35 });
		AddPointToLine({ 46,1215, 35 });
		AddPointToLine({ 48,1208, 35 });

		NewLine();
		AddPointToLine({ 49,1209,35 });
		AddPointToLine({ 72,1211,31 });
		AddPointToLine({ 88,1210,29 });
		AddPointToLine({ 104,1208,32 });
		AddPointToLine({ 120,1206,37 });
		AddPointToLine({ 134,1209,39 });

		NewLine();
		AddPointToLine({ 140,1209,37 });
		AddPointToLine({ 155,1207,33 });
		AddPointToLine({ 171,1208,30 });
		AddPointToLine({ 187,1210,33 });
		AddPointToLine({ 203,1209,38 });
		AddPointToLine({ 218,1207,40 });

		NewLine();
		AddPointToLine({ 225,1208,37 });
		AddPointToLine({ 240,1210,32 });
		AddPointToLine({ 256,1211,29 });
		AddPointToLine({ 272,1209,32 });
		AddPointToLine({ 288,1206,37 });
		AddPointToLine({ 304,1208,40 });

		NewLine();
		AddPointToLine({ 311,1209,37 });
		AddPointToLine({ 327,1211,33 });
		AddPointToLine({ 343,1210,30 });
		AddPointToLine({ 359,1208,33 });
		AddPointToLine({ 375,1206,38 });
		AddPointToLine({ 392,1209,40 });
		AddPointToLine({ 409,1208,37 });


		// ============================================================
		// Л3 — 180°
		// ============================================================

		NewLine();
		AddPointToLine({ -13,1243,19 });
		AddPointToLine({ -7,1242,19 });
		AddPointToLine({ 0,1240,19 });
		AddPointToLine({ 7,1237,19 });
		AddPointToLine({ 14,1233,19 });
		AddPointToLine({ 21,1228,19 });
		AddPointToLine({ 27,1222,19 });
		AddPointToLine({ 31,1215,19 });
		AddPointToLine({ 33,1208,19 });

		NewLine();
		AddPointToLine({ 43,1208,19 });
		AddPointToLine({ 57,1210,15 });
		AddPointToLine({ 73,1209,12 });
		AddPointToLine({ 89,1207,15 });
		AddPointToLine({ 105,1209,20 });
		AddPointToLine({ 119,1208,23 });

		NewLine();
		AddPointToLine({ 126,1208,21 });
		AddPointToLine({ 141,1206,25 });
		AddPointToLine({ 157,1207,27 });
		AddPointToLine({ 173,1209,24 });
		AddPointToLine({ 189,1210,18 });
		AddPointToLine({ 204,1208,15 });

		NewLine();
		AddPointToLine({ 211,1209,17 });
		AddPointToLine({ 226,1211,22 });
		AddPointToLine({ 242,1210,26 });
		AddPointToLine({ 258,1208,24 });
		AddPointToLine({ 274,1206,18 });
		AddPointToLine({ 290,1208,14 });

		NewLine();
		AddPointToLine({ 297,1209,16 });
		AddPointToLine({ 313,1211,12 });
		AddPointToLine({ 329,1210,10 });
		AddPointToLine({ 345,1208,14 });
		AddPointToLine({ 361,1206,20 });
		AddPointToLine({ 377,1208,24 });
		AddPointToLine({ 394,1209,21 });


		// ============================================================
		// Л4 — 270°
		// ============================================================

		NewLine();
		AddPointToLine({ 2,1243,3 });
		AddPointToLine({ 8,1242,3 });
		AddPointToLine({ 15,1240,3 });
		AddPointToLine({ 22,1237,3 });
		AddPointToLine({ 29,1233,3 });
		AddPointToLine({ 36,1228,3 });
		AddPointToLine({ 42,1222,3 });
		AddPointToLine({ 46,1215,3 });
		AddPointToLine({ 48,1208,3 });

		NewLine();
		AddPointToLine({ 46,1209,3 });
		AddPointToLine({ 72,1211,7 });
		AddPointToLine({ 88,1210,9 });
		AddPointToLine({ 104,1208,6 });
		AddPointToLine({ 120,1206,1 });
		AddPointToLine({ 134,1209,-1 });

		NewLine();
		AddPointToLine({ 140,1209,1 });
		AddPointToLine({ 155,1207,5 });
		AddPointToLine({ 171,1208,8 });
		AddPointToLine({ 187,1210,5 });
		AddPointToLine({ 203,1209,0 });
		AddPointToLine({ 218,1207,-3 });

		NewLine();
		AddPointToLine({ 225,1208,1 });
		AddPointToLine({ 240,1210,6 });
		AddPointToLine({ 256,1211,9 });
		AddPointToLine({ 272,1209,5 });
		AddPointToLine({ 288,1206,0 });
		AddPointToLine({ 304,1208,-3 });

		NewLine();
		AddPointToLine({ 311,1209,1 });
		AddPointToLine({ 327,1211,5 });
		AddPointToLine({ 343,1210,8 });
		AddPointToLine({ 359,1208,5 });
		AddPointToLine({ 375,1206,0 });
		AddPointToLine({ 392,1209,-3 });
		AddPointToLine({ 409,1208,0 });


		// ============================================================
		// УЧАСТОК 3
		// Начало после участка 2
		// ============================================================


		// ============================================================
		// ЛИНИЯ 1
		// ============================================================

		NewLine();
		AddPointToLine({ 413, 1208,  19 });
		AddPointToLine({ 420, 1208,   2 });
		AddPointToLine({ 439, 1208, -10 });
		AddPointToLine({ 506, 1208, -34 });
		AddPointToLine({ 567, 1208,  18 });

		NewLine();
		AddPointToLine({ 413, 1208,  19 });
		AddPointToLine({ 431, 1206,  31 });
		AddPointToLine({ 445, 1203,  44 });
		AddPointToLine({ 443, 1204,  57 });
		AddPointToLine({ 438, 1207,  69 });
		AddPointToLine({ 423, 1210,  82 });
		AddPointToLine({ 398, 1212,  94 });
		AddPointToLine({ 389, 1211, 109 });
		AddPointToLine({ 393, 1205, 124 });

		NewLine();
		AddPointToLine({ 388, 1211, 119 });
		AddPointToLine({ 393, 1207, 134 });
		AddPointToLine({ 413, 1202, 149 });
		AddPointToLine({ 433, 1204, 164 });
		AddPointToLine({ 443, 1211, 179 });
		AddPointToLine({ 428, 1214, 194 });
		AddPointToLine({ 401, 1213, 209 });
		AddPointToLine({ 391, 1208, 222 });
		AddPointToLine({ 395, 1202, 236 });

		NewLine();
		AddPointToLine({ 385, 1207, 226 });
		AddPointToLine({ 395, 1204, 241 });
		AddPointToLine({ 423, 1201, 256 });
		AddPointToLine({ 440, 1205, 271 });
		AddPointToLine({ 438, 1211, 286 });
		AddPointToLine({ 418, 1214, 301 });
		AddPointToLine({ 388, 1212, 316 });
		AddPointToLine({ 385, 1208, 329 });
		AddPointToLine({ 403, 1202, 341 });

		NewLine();
		AddPointToLine({ 383, 1206, 331 });
		AddPointToLine({ 395, 1203, 346 });
		AddPointToLine({ 431, 1202, 361 });
		AddPointToLine({ 443, 1208, 376 });
		AddPointToLine({ 428, 1214, 391 });
		AddPointToLine({ 398, 1214, 406 });
		AddPointToLine({ 383, 1209, 421 });
		AddPointToLine({ 388, 1205, 434 });
		AddPointToLine({ 423, 1201, 446 });

		NewLine();
		AddPointToLine({ 401, 1203, 437 });
		AddPointToLine({ 429, 1201, 452 });
		AddPointToLine({ 449, 1204, 467 });
		AddPointToLine({ 446, 1210, 482 });
		AddPointToLine({ 421, 1215, 497 });
		AddPointToLine({ 396, 1212, 512 });
		AddPointToLine({ 391, 1206, 527 });
		AddPointToLine({ 401, 1203, 540 });
		AddPointToLine({ 411, 1202, 552 });

		NewLine();
		AddPointToLine({ 411, 1202, 542 });
		AddPointToLine({ 429, 1203, 557 });
		AddPointToLine({ 451, 1208, 572 });
		AddPointToLine({ 441, 1213, 587 });
		AddPointToLine({ 406, 1213, 602 });
		AddPointToLine({ 409, 1209, 615 });
		AddPointToLine({ 421, 1203, 627 });


		// ============================================================
		// ЛИНИЯ 2
		// ============================================================

		NewLine();
		AddPointToLine({ 413,1208,19 });
		AddPointToLine({ 395,1210,34 });
		AddPointToLine({ 383,1213,49 });
		AddPointToLine({ 391,1214,64 });
		AddPointToLine({ 418,1211,79 });
		AddPointToLine({ 435,1207,94 });
		AddPointToLine({ 435,1203,109 });
		AddPointToLine({ 423,1202,121 });
		AddPointToLine({ 413,1203,129 });

		NewLine();
		AddPointToLine({ 435,1203,119 });
		AddPointToLine({ 418,1203,134 });
		AddPointToLine({ 391,1206,149 });
		AddPointToLine({ 383,1211,164 });
		AddPointToLine({ 398,1214,179 });
		AddPointToLine({ 425,1213,194 });
		AddPointToLine({ 443,1208,209 });
		AddPointToLine({ 438,1204,222 });
		AddPointToLine({ 423,1203,234 });

		NewLine();
		AddPointToLine({ 398,1202,224 });
		AddPointToLine({ 388,1203,239 });
		AddPointToLine({ 383,1206,254 });
		AddPointToLine({ 393,1211,269 });
		AddPointToLine({ 423,1215,284 });
		AddPointToLine({ 441,1211,299 });
		AddPointToLine({ 435,1203,314 });
		AddPointToLine({ 418,1201,327 });
		AddPointToLine({ 408,1201,339 });

		NewLine();
		AddPointToLine({ 385,1205,329 });
		AddPointToLine({ 383,1208,344 });
		AddPointToLine({ 388,1211,359 });
		AddPointToLine({ 413,1215,374 });
		AddPointToLine({ 438,1212,389 });
		AddPointToLine({ 443,1206,404 });
		AddPointToLine({ 423,1202,419 });
		AddPointToLine({ 403,1202,432 });
		AddPointToLine({ 393,1203,444 });

		NewLine();
		AddPointToLine({ 383,1209,434 });
		AddPointToLine({ 388,1213,449 });
		AddPointToLine({ 398,1214,464 });
		AddPointToLine({ 418,1212,479 });
		AddPointToLine({ 443,1208,494 });
		AddPointToLine({ 431,1203,509 });
		AddPointToLine({ 401,1202,524 });
		AddPointToLine({ 388,1204,537 });
		AddPointToLine({ 383,1207,549 });

		NewLine();
		AddPointToLine({ 383,1207,539 });
		AddPointToLine({ 398,1211,554 });
		AddPointToLine({ 423,1215,569 });
		AddPointToLine({ 443,1212,584 });
		AddPointToLine({ 448,1207,599 });
		AddPointToLine({ 433,1203,609 });
		AddPointToLine({ 413,1203,619 });


		// ============================================================
		// ЛИНИЯ 3
		// ============================================================

		NewLine();
		AddPointToLine({ 413,1208,19 });
		AddPointToLine({ 425,1213,35 });
		AddPointToLine({ 438,1213,51 });
		AddPointToLine({ 443,1209,67 });
		AddPointToLine({ 427,1205,84 });
		AddPointToLine({ 401,1203,101 });
		AddPointToLine({ 378,1206,114 });
		AddPointToLine({ 385,1209,126 });
		AddPointToLine({ 397,1212,135 });

		NewLine();
		AddPointToLine({ 384,1206,125 });
		AddPointToLine({ 397,1210,134 });
		AddPointToLine({ 422,1214,151 });
		AddPointToLine({ 441,1209,164 });
		AddPointToLine({ 444,1202,181 });
		AddPointToLine({ 419,1203,194 });
		AddPointToLine({ 391,1208,211 });
		AddPointToLine({ 382,1210,224 });
		AddPointToLine({ 406,1215,241 });

		NewLine();
		AddPointToLine({ 383,1212,231 });
		AddPointToLine({ 406,1214,239 });
		AddPointToLine({ 436,1211,256 });
		AddPointToLine({ 447,1203,269 });
		AddPointToLine({ 430,1201,286 });
		AddPointToLine({ 401,1206,299 });
		AddPointToLine({ 382,1213,316 });
		AddPointToLine({ 389,1216,327 });
		AddPointToLine({ 406,1212,341 });

		NewLine();
		AddPointToLine({ 383,1209,331 });
		AddPointToLine({ 410,1213,346 });
		AddPointToLine({ 443,1210,359 });
		AddPointToLine({ 430,1201,376 });
		AddPointToLine({ 401,1200,389 });
		AddPointToLine({ 383,1207,406 });
		AddPointToLine({ 399,1216,419 });
		AddPointToLine({ 427,1215,434 });
		AddPointToLine({ 433,1209,446 });

		NewLine();
		AddPointToLine({ 443,1207,437 });
		AddPointToLine({ 431,1201,452 });
		AddPointToLine({ 415,1200,465 });
		AddPointToLine({ 394,1207,482 });
		AddPointToLine({ 383,1214,495 });
		AddPointToLine({ 407,1217,512 });
		AddPointToLine({ 437,1212,525 });
		AddPointToLine({ 446,1205,540 });
		AddPointToLine({ 440,1203,552 });

		NewLine();
		AddPointToLine({ 423,1200,542 });
		AddPointToLine({ 409,1204,550 });
		AddPointToLine({ 390,1210,567 });
		AddPointToLine({ 382,1216,580 });
		AddPointToLine({ 403,1213,597 });
		AddPointToLine({ 426,1206,608 });
		AddPointToLine({ 413,1203,621 });

		// ============================================================
// ЛИНИЯ 2 — РАНДОМИЗИРОВАННАЯ
// ВТОРАЯ ШИРОКАЯ ЛИНИЯ ТРЕТЬЕГО УЧАСТКА
// Перенесено: X +417, Y +1208, Z +18
// Начало: { 567, 1208, 18 }
// ============================================================

// 2A
		NewLine();
		AddPointToLine({ 567, 1208,  18 });
		AddPointToLine({ 546, 1213,  32 });
		AddPointToLine({ 533, 1210,  47 });
		AddPointToLine({ 539, 1216,  62 });
		AddPointToLine({ 566, 1212,  79 });
		AddPointToLine({ 593, 1205,  92 });
		AddPointToLine({ 597, 1200, 107 });
		AddPointToLine({ 581, 1206, 121 });
		AddPointToLine({ 568, 1201, 129 });

		// 2B
		NewLine();
		AddPointToLine({ 588, 1204, 119 });
		AddPointToLine({ 573, 1200, 134 });
		AddPointToLine({ 549, 1207, 147 });
		AddPointToLine({ 533, 1214, 164 });
		AddPointToLine({ 546, 1217, 177 });
		AddPointToLine({ 575, 1211, 194 });
		AddPointToLine({ 600, 1206, 207 });
		AddPointToLine({ 596, 1201, 222 });
		AddPointToLine({ 523, 1206, 229 });

		// 2C
		NewLine();
		AddPointToLine({ 549, 1203, 224 });
		AddPointToLine({ 536, 1201, 239 });
		AddPointToLine({ 531, 1209, 252 });
		AddPointToLine({ 544, 1216, 269 });
		AddPointToLine({ 573, 1214, 282 });
		AddPointToLine({ 596, 1207, 299 });
		AddPointToLine({ 588, 1200, 312 });
		AddPointToLine({ 566, 1204, 327 });
		AddPointToLine({ 554, 1202, 339 });


		// ============================================================
		// ЦЕНТРАЛЬНАЯ СПИРАЛЬНАЯ ВОРОНКА
		// ПОДКЛЮЧЕНИЕ К 2C
		// ============================================================

		// Вход 1
		NewLine();
		AddPointToLine({ 544, 1216, 269 });
		AddPointToLine({ 537, 1215, 276 });
		AddPointToLine({ 535, 1214, 284 });
		AddPointToLine({ 537, 1213, 292 });
		AddPointToLine({ 545, 1213, 298 });
		AddPointToLine({ 555, 1213, 302 });

		// Вход 2
		NewLine();
		AddPointToLine({ 573, 1214, 282 });
		AddPointToLine({ 567, 1215, 288 });
		AddPointToLine({ 562, 1214, 294 });
		AddPointToLine({ 559, 1213, 298 });
		AddPointToLine({ 555, 1213, 302 });

		// Вход 3
		NewLine();
		AddPointToLine({ 596, 1207, 299 });
		AddPointToLine({ 587, 1208, 301 });
		AddPointToLine({ 577, 1210, 302 });
		AddPointToLine({ 567, 1212, 302 });
		AddPointToLine({ 555, 1213, 302 });


		// ============================================================
		// СПИРАЛЬНАЯ ВОРОНКА — НИТЬ 1
		// ============================================================

		NewLine();
		AddPointToLine({ 555, 1213, 302 });
		AddPointToLine({ 572, 1211, 306 });
		AddPointToLine({ 587, 1208, 316 });
		AddPointToLine({ 598, 1203, 331 });
		AddPointToLine({ 601, 1197, 348 });
		AddPointToLine({ 596, 1191, 365 });
		AddPointToLine({ 583, 1185, 378 });
		AddPointToLine({ 567, 1179, 384 });
		AddPointToLine({ 551, 1173, 380 });
		AddPointToLine({ 538, 1167, 368 });
		AddPointToLine({ 532, 1161, 352 });
		AddPointToLine({ 534, 1155, 335 });
		AddPointToLine({ 544, 1149, 322 });
		AddPointToLine({ 558, 1143, 315 });
		AddPointToLine({ 573, 1137, 317 });
		AddPointToLine({ 584, 1131, 328 });
		AddPointToLine({ 589, 1125, 343 });
		AddPointToLine({ 585, 1119, 358 });
		AddPointToLine({ 574, 1113, 368 });
		AddPointToLine({ 560, 1107, 370 });
		AddPointToLine({ 549, 1101, 362 });
		AddPointToLine({ 543, 1095, 349 });
		AddPointToLine({ 545, 1089, 336 });
		AddPointToLine({ 554, 1083, 328 });
		AddPointToLine({ 565, 1077, 329 });
		AddPointToLine({ 574, 1071, 337 });
		AddPointToLine({ 577, 1065, 348 });
		AddPointToLine({ 572, 1059, 357 });
		AddPointToLine({ 563, 1053, 360 });
		AddPointToLine({ 556, 1047, 355 });
		AddPointToLine({ 554, 1041, 346 });
		AddPointToLine({ 560, 1035, 339 });
		AddPointToLine({ 567, 1028, 338 });


		// ============================================================
		// СПИРАЛЬНАЯ ВОРОНКА — НИТЬ 2
		// ============================================================

		NewLine();
		AddPointToLine({ 555, 1213, 302 });
		AddPointToLine({ 545, 1212, 316 });
		AddPointToLine({ 540, 1209, 333 });
		AddPointToLine({ 543, 1204, 351 });
		AddPointToLine({ 554, 1198, 365 });
		AddPointToLine({ 569, 1192, 374 });
		AddPointToLine({ 586, 1186, 372 });
		AddPointToLine({ 598, 1180, 360 });
		AddPointToLine({ 601, 1174, 343 });
		AddPointToLine({ 596, 1168, 327 });
		AddPointToLine({ 583, 1162, 316 });
		AddPointToLine({ 568, 1156, 314 });
		AddPointToLine({ 554, 1150, 322 });
		AddPointToLine({ 547, 1144, 335 });
		AddPointToLine({ 549, 1138, 349 });
		AddPointToLine({ 559, 1132, 358 });
		AddPointToLine({ 572, 1126, 359 });
		AddPointToLine({ 582, 1120, 351 });
		AddPointToLine({ 586, 1114, 339 });
		AddPointToLine({ 581, 1108, 329 });
		AddPointToLine({ 571, 1102, 325 });
		AddPointToLine({ 560, 1096, 330 });
		AddPointToLine({ 554, 1090, 340 });
		AddPointToLine({ 557, 1084, 350 });
		AddPointToLine({ 566, 1078, 355 });
		AddPointToLine({ 574, 1072, 350 });
		AddPointToLine({ 577, 1066, 341 });
		AddPointToLine({ 572, 1060, 334 });
		AddPointToLine({ 565, 1054, 333 });
		AddPointToLine({ 559, 1048, 338 });
		AddPointToLine({ 558, 1042, 345 });
		AddPointToLine({ 563, 1036, 349 });
		AddPointToLine({ 567, 1030, 346 });


		// ============================================================
		// СПИРАЛЬНАЯ ВОРОНКА — НИТЬ 3
		// ============================================================

		NewLine();
		AddPointToLine({ 555, 1213, 302 });
		AddPointToLine({ 549, 1211, 318 });
		AddPointToLine({ 553, 1208, 335 });
		AddPointToLine({ 564, 1203, 349 });
		AddPointToLine({ 579, 1197, 355 });
		AddPointToLine({ 592, 1191, 350 });
		AddPointToLine({ 598, 1185, 337 });
		AddPointToLine({ 595, 1179, 322 });
		AddPointToLine({ 583, 1173, 312 });
		AddPointToLine({ 568, 1167, 310 });
		AddPointToLine({ 554, 1161, 317 });
		AddPointToLine({ 546, 1155, 330 });
		AddPointToLine({ 547, 1149, 344 });
		AddPointToLine({ 557, 1143, 353 });
		AddPointToLine({ 570, 1137, 354 });
		AddPointToLine({ 581, 1131, 346 });
		AddPointToLine({ 585, 1125, 335 });
		AddPointToLine({ 580, 1119, 325 });
		AddPointToLine({ 570, 1113, 320 });
		AddPointToLine({ 560, 1107, 324 });
		AddPointToLine({ 554, 1101, 334 });
		AddPointToLine({ 556, 1095, 344 });
		AddPointToLine({ 565, 1089, 350 });
		AddPointToLine({ 574, 1083, 347 });
		AddPointToLine({ 578, 1077, 338 });
		AddPointToLine({ 574, 1071, 331 });
		AddPointToLine({ 566, 1065, 329 });
		AddPointToLine({ 560, 1059, 334 });
		AddPointToLine({ 559, 1053, 341 });
		AddPointToLine({ 564, 1047, 346 });
		AddPointToLine({ 570, 1041, 345 });
		AddPointToLine({ 573, 1035, 340 });
		AddPointToLine({ 567, 1030, 338 });


		// ============================================================
		// ПРОДОЛЖЕНИЕ ВТОРОЙ ШИРОКОЙ ЛИНИИ
		// ============================================================

		// 2D
		NewLine();
		AddPointToLine({ 539, 1206, 327 });
		AddPointToLine({ 534, 1212, 342 });
		AddPointToLine({ 548, 1216, 359 });
		AddPointToLine({ 574, 1213, 372 });
		AddPointToLine({ 598, 1207, 389 });
		AddPointToLine({ 594, 1201, 402 });
		AddPointToLine({ 574, 1200, 419 });
		AddPointToLine({ 555, 1206, 430 });
		AddPointToLine({ 546, 1213, 444 });

		// 2E
		NewLine();
		AddPointToLine({ 548, 1210, 434 });
		AddPointToLine({ 548, 1216, 447 });
		AddPointToLine({ 561, 1213, 464 });
		AddPointToLine({ 583, 1209, 477 });
		AddPointToLine({ 600, 1204, 494 });
		AddPointToLine({ 588, 1200, 507 });
		AddPointToLine({ 561, 1205, 524 });
		AddPointToLine({ 538, 1202, 537 });
		AddPointToLine({ 547, 1204, 549 });

		// 2F
		NewLine();
		AddPointToLine({ 541, 1207, 539 });
		AddPointToLine({ 556, 1214, 552 });
		AddPointToLine({ 580, 1216, 569 });
		AddPointToLine({ 598, 1209, 582 });
		AddPointToLine({ 593, 1202, 599 });
		AddPointToLine({ 576, 1201, 609 });
		AddPointToLine({ 567, 1203, 618 });
#endif


		/*for (int i = 0; i < starsCount; i++)
		{
			// Начинаем с i + 1, чтобы не проверять i==j и не дублировать пары (j,i)
			for (int j = i + 1; j < starsCount; j++)
			{
				float4 start = gemini[i];
				float4 end = gemini[j];

				// Проверка дистанции в исходном диапазоне -1...1
				if (distance(start, end) < .75)
				{
					NewLine();
					int seg = 10;
					for (int k = 0; k <= seg; k++)
					{
						float4 p = lerp3(start, end, k / (float)seg);

						float rs = .02*sin((k / (float)seg)*PI);
						p.x += getRandFloat()*rs;
						p.y += getRandFloat()*rs;
						p.z += getRandFloat()*rs;

						// Масштабируем координаты точек для игрового мира
						float scale = 600;
						p.x *= scale;
						p.y *= scale;
						p.z *= scale;

						AddPoint(p);
					}
				}
			}
		}*/

			
		//------------end user space---------------
		//-----------------------------------------

		for (int j = 0; j < starLineList.lineCount; j++)
		{
			smoothStarline(starLineList.line[j]);
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
