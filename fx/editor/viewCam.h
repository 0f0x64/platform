namespace ViewCam
{
#define CAM_KEY VK_CONTROL
#define CAM_KEY2 VK_SHIFT

	typedef struct  {
		XMVECTOR Target;
		XMVECTOR ViewVec;
		XMVECTOR upVec;
	} naviCam;

	naviCam currentCamera;
	naviCam storedCamera;
	naviCam targetCamera;

	float flyToCam = 1.f;

	float camDistance = 13;

	void ExtractCameraVectors(const XMMATRIX& viewMatrix, XMVECTOR& eye, XMVECTOR& at, XMVECTOR& up)
	{
		XMMATRIX cameraMatrix = XMMatrixInverse(nullptr, viewMatrix);
		eye = cameraMatrix.r[3];
		at = XMVectorSet(-cameraMatrix.r[2].m128_f32[0], -cameraMatrix.r[2].m128_f32[1], -cameraMatrix.r[2].m128_f32[2], 0.0f);
		up = cameraMatrix.r[1];
	}

	float getCamDistance()
	{
		XMVECTOR eye, at, up;
		ExtractCameraVectors(Camera::viewCam.view, eye, at, up);
		XMVectorSetW(eye, 0);
		XMVectorSetW(at, 0);
		auto l = XMVector4Length(eye - at);
		return XMVectorGetX(l);
	}

	void Init()
	{
		Camera::viewCam.Init();
	}

	float getFlatAngle()
	{
		auto right = XMVector3Cross(currentCamera.ViewVec, currentCamera.upVec);
		XMVECTOR v2 = { XMVectorGetX(right), XMVectorGetZ(right) };
		v2 = XMVector2Normalize(v2);
		float angle = atan2(XMVectorGetX(v2), -XMVectorGetY(v2));
		return RadiansToDegrees(angle) + 180.;
	}

	float getHAngle()
	{
		auto right = XMVector3Cross(currentCamera.ViewVec, currentCamera.upVec);
		XMVECTOR v2 = { XMVectorGetZ(currentCamera.upVec), XMVectorGetY(currentCamera.upVec) };
		//v2 = XMVector2Normalize(v2);
		float angle = atan2(XMVectorGetX(v2), XMVectorGetY(v2));
		return RadiansToDegrees(angle);
	}

	void ToggleViewMode()
	{
		Camera::viewCam.overRide = !Camera::viewCam.overRide;
	}

	void AxisCam(float xz, float zy)
	{
		if (flyToCam < 1.f) return;
		storedCamera = currentCamera;

		float vl = XMVectorGetX(XMVector3Length(currentCamera.ViewVec));
		float a = floor(getFlatAngle());
		if (a >= 360.f) a = 0;

		float b = floor(getHAngle());
		if (b >= 360.f) b = 0;

		auto view = XMVECTOR{ vl,0,0 };
		auto up = XMVECTOR{ 0,1,0 };

		if (xz != 0)
		{
			float ta;
			if (xz > 0) ta = floor(a / 90.f) * 90.f + xz;
			if (xz < 0) ta = ceil(a / 90.f) * 90.f + xz;
			auto m = XMMatrixRotationY(DegreesToRadians(-ta));
			view = XMVector3Transform(view, m);
			up = XMVector3Transform(up, m);
		}

		if (zy != 0)
		{
			float ta;
			if (zy > 0) ta = floor(b / 90.f) * 90.f + zy;
			if (zy < 0) ta = ceil(b / 90.f) * 90.f + zy;

			XMVECTOR right = XMVector3Cross(currentCamera.ViewVec, currentCamera.upVec);

			//right = XMVector3Transform(right, ((XMMatrixLookToLH( XMVECTOR{ 0,0,0 }, currentCamera.ViewVec, currentCamera.upVec))));
			//auto m = XMMatrixRotationAxis(right,DegreesToRadians(-zy));
			auto m = XMMatrixRotationAxis(right, DegreesToRadians(-ta));

			view = XMVector3Transform(currentCamera.ViewVec, m);
			up = XMVector3Transform(currentCamera.upVec, m);

			//view = XMVector3Transform(view, m);
			//up = XMVector3Transform(up, m);
		}


		targetCamera = currentCamera;
		targetCamera.ViewVec = view;
		//targetCamera.upVec = XMVECTOR{ up.x,up.y,up.z };
		targetCamera.upVec = up;

		flyToCam = 0.f;
	}

	void TransCam(float x, float y, float z)
	{
		targetCamera = currentCamera;
		targetCamera.Target = XMVECTOR{ x,y,z };

		flyToCam = 0.f;
	}

	void Wheel(float delta)
	{
		if (isKeyDown(CAM_KEY)) 
		{
			currentCamera.ViewVec = XMVectorScale(currentCamera.ViewVec, 1. - delta / 2000.f);
			storedCamera = currentCamera;
		}

		if (isKeyDown(CAM_KEY2)) {
			Camera::viewCam.angle += delta / 50.f;
			Camera::viewCam.proj = XMMatrixPerspectiveFovLH(DegreesToRadians(Camera::viewCam.angle), width / (FLOAT)height, 0.01f, 100.0f);
		}

	}


	void lbDown()
	{
		storedCamera = currentCamera;
	}

	void mbDown()
	{
		storedCamera = currentCamera;
	}

	void rbDown()
	{
		storedCamera = currentCamera;
		editor::ui::mouseLastAngle = editor::ui::mouseAngle;
	}

	float axisAlpha = 0;


	void ProcessInput()
	{
		if (!(isKeyDown(CAM_KEY) || isKeyDown(CAM_KEY2))) return;

		float speed = 4.f;
		editor::ui::point3df r = { 0,0,0 };
		editor::ui::point3df t = { 0,0,0 };

		if (ui::mbDown || (ui::lbDown && isKeyDown(CAM_KEY2)))
		{
			t.x = ui::mouseDelta.x * speed * 4.f;
			t.y = -ui::mouseDelta.y * speed * 4.f;
			t.z = 0.f;
		}
		else
		{
			if (ui::rbDown)
			{
				r.x = 0.f;
				r.y = 0.f;
				r.z = ui::mouseAngleDelta;
			}
			else if (ui::lbDown)
			{
				r.x = -ui::mouseDelta.y * speed;
				r.y = -ui::mouseDelta.x * speed;
				r.z = 0.f;
			}
		}

		if (ui::lbDown || ui::rbDown || ui::mbDown)
		{
			auto c = XMMatrixTranspose(XMMatrixLookAtLH(storedCamera.ViewVec, XMVECTOR{ 0,0,0 }, storedCamera.upVec));
			XMVECTOR translation = XMVector3Transform(XMVECTOR{ t.x, t.y, t.z },c);
			auto rm = XMMatrixRotationRollPitchYaw(-r.x, -r.y, -r.z);
			
			currentCamera.ViewVec = XMVector3Transform(storedCamera.ViewVec, XMMatrixInverse(NULL, c));
			currentCamera.ViewVec = XMVector3Transform(currentCamera.ViewVec, rm);
			currentCamera.ViewVec = XMVector3Transform(currentCamera.ViewVec, c);
			
			currentCamera.upVec = XMVector3Transform(storedCamera.upVec, XMMatrixInverse(NULL, c));
			currentCamera.upVec = XMVector3Transform(currentCamera.upVec, rm);
			currentCamera.upVec = XMVector3Transform(currentCamera.upVec, c);
			
			currentCamera.Target = storedCamera.Target - translation;
		}
	}

	int counter;

	void StorePoint3d(float x, float y, float z)
	{
		ui::Line::buffer[counter].x = x;
		ui::Line::buffer[counter].y = y;
		ui::Line::buffer[counter].z = z;
		counter++;
	}

	void ExtractPitchYawRollFromXMMatrix(float* flt_p_PitchOut, float* flt_p_YawOut, float* flt_p_RollOut, const XMMATRIX* XMMatrix_p_Rotation)
	{
		XMFLOAT4X4 XMFLOAT4X4_Values;
		XMStoreFloat4x4(&XMFLOAT4X4_Values, DirectX::XMMatrixTranspose(*XMMatrix_p_Rotation));
		*flt_p_PitchOut = (float)asin(-XMFLOAT4X4_Values._23);
		*flt_p_YawOut = (float)atan2(XMFLOAT4X4_Values._13, XMFLOAT4X4_Values._33);
		*flt_p_RollOut = (float)atan2(XMFLOAT4X4_Values._21, XMFLOAT4X4_Values._22);
	}

	void DrawAxis()
	{
		float axisSize = 2.;
		float arrowSize = .1;
		counter = 0;
		StorePoint3d(0, 0, 0);
		StorePoint3d(-axisSize, 0, 0);
		StorePoint3d(-axisSize, 0, 0);
		StorePoint3d(-axisSize + arrowSize, arrowSize / 2, 0);
		StorePoint3d(-axisSize, 0, 0);
		StorePoint3d(-axisSize + arrowSize, -arrowSize / 2, 0);

		int yA = counter;
		int zA = yA * 2;
		for (int x = 0; x < 6; x++)
		{
			ui::Line::buffer[yA + x].x = ui::Line::buffer[x].y;
			ui::Line::buffer[yA + x].y = -ui::Line::buffer[x].x;
			ui::Line::buffer[yA + x].z = -ui::Line::buffer[x].z;

			ui::Line::buffer[zA + x].x = ui::Line::buffer[x].z;
			ui::Line::buffer[zA + x].y = ui::Line::buffer[x].y;
			ui::Line::buffer[zA + x].z = ui::Line::buffer[x].x;
		}

		ui::Line::Setup();

		XMVECTOR Eye = currentCamera.ViewVec;
		ConstBuf::drawerMat.model = XMMatrixTranspose(XMMatrixLookAtLH(Eye, XMVECTOR{ 0,0,0 }, currentCamera.upVec));
		ConstBuf::UpdateDrawerMat();

		ui::Line::Draw3d(counter * 3 / 2, 1, 1, 1, axisAlpha);
	}

	bool isSetup = false;

	void setup()
	{
		if (isSetup) return;

		isSetup = true;

		targetCamera.Target = XMVECTOR{ 0,0,0,1 };
		targetCamera.ViewVec = XMVECTOR{ 0,0,13,1 };
		targetCamera.upVec = XMVECTOR{ 0,1,0,1 };

		currentCamera = targetCamera;
		storedCamera = targetCamera;
	}

	void Draw()
	{
		setup();

		bool fade = isKeyDown(CAM_KEY) || isKeyDown(CAM_KEY2) || flyToCam < 1.f;
		axisAlpha = clamp(axisAlpha + (fade ? .4f : -.05f), 0.f, 1.f);

		if (flyToCam < 1.f)
		{
			flyToCam = clamp(flyToCam + 1.f/9.f, 0.f, 1.f);//divde by even number prevent lerp vectors to zero for 180 degrees transformatrions
			
			auto vLen = XMVector3Length(targetCamera.ViewVec);

			currentCamera.Target = XMVectorLerp (storedCamera.Target, targetCamera.Target, flyToCam);
			currentCamera.ViewVec = XMVectorLerp(storedCamera.ViewVec, targetCamera.ViewVec, flyToCam);
			currentCamera.upVec = XMVectorLerp  (storedCamera.upVec, targetCamera.upVec, flyToCam);

			currentCamera.ViewVec = vLen * XMVector3Normalize(currentCamera.ViewVec);
		}

		ProcessInput();

		XMVECTOR Eye = currentCamera.ViewVec + currentCamera.Target;
		XMVECTOR At = currentCamera.Target;
		Camera::viewCam.view = XMMatrixLookAtLH(Eye, At, currentCamera.upVec);

		DrawAxis();


		
		ui::Text::Setup();
		char str[132];
		char str2[132];

		auto aa = getHAngle();
		_gcvt((aa), 10, str);
		ui::Text::Draw(str, .5, .5);

	}

}
