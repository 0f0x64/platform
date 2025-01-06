namespace ViewCam
{


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
		float angle = (float)atan2(XMVectorGetX(v2), -XMVectorGetY(v2));
		return RadiansToDegrees(angle) + 180.f;
	}

	void ToggleViewMode()
	{
		Camera::viewCam.overRide = !Camera::viewCam.overRide;
	}

	void AxisCamPitch(float zy)
	{
		if (flyToCam < 1.f) return;
		storedCamera = currentCamera;

		float vl = XMVectorGetX(XMVector3Length(currentCamera.ViewVec));
		float a = (float)((int)(floor(getFlatAngle())) % 360);

		auto m2 = XMMatrixRotationAxis(XMVECTOR{ 0,1,0 }, DegreesToRadians(-a - 90));

		auto view = XMVECTOR{ 0,0,-vl };
		auto up = XMVECTOR{ 0,1,0 };
		auto m = XMMatrixRotationX(DegreesToRadians(-zy));
		view = XMVector3Transform(view, m);
		up = XMVector3Transform(up, m);

		view = XMVector3Transform(view, m2);
		up = XMVector3Transform(up, m2);

		targetCamera = currentCamera;
		targetCamera.ViewVec = view;
		targetCamera.upVec = up;

		flyToCam = 0.f;

	}

	void AxisCamYaw(float xz)
	{
		if (flyToCam < 1.f) return;
		storedCamera = currentCamera;

		float vl = XMVectorGetX(XMVector3Length(currentCamera.ViewVec));
		float a = (float)((int)(floor(getFlatAngle())) % 360);

		auto view = XMVECTOR{ vl,0,0 };
		auto up = XMVECTOR{ 0,1,0 };

		float ta = ((xz > 0.f) ? floorf(a / 90.f) : ceilf(a / 90.f)) * 90.f + xz;
		auto m = XMMatrixRotationY(DegreesToRadians(-ta));
		view = XMVector3Transform(view, m);
		up = XMVector3Transform(up, m);

		targetCamera = currentCamera;
		targetCamera.ViewVec = view;
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
			if (isKeyDown(VK_LBUTTON))
			{//adjust cam angle
				Camera::viewCam.angle += delta / 50.f;
			}
			else
			{//adjust distance from target
				currentCamera.ViewVec = XMVectorScale(currentCamera.ViewVec, 1.f - delta / 2000.f);
				storedCamera = currentCamera;
			}
		}
	}


	void lbDown()
	{
		storedCamera = currentCamera;
	}

	void mbDown()
	{
		storedCamera = currentCamera;
		editor::ui::mouseLastAngle = editor::ui::mouseAngle;
	}

	void rbDown()
	{
		storedCamera = currentCamera;
		
	}

	float axisAlpha = 0;


	void ProcessInput()
	{
		if (!drag.check(drag.context::cameraView)) return;

		if (!isKeyDown(CAM_KEY))
		{
			return;
		}

		float speed = 4.f;
		editor::ui::point3df r = { 0,0,0 };
		editor::ui::point3df t = { 0,0,0 };

		if (ui::rbDown)
		{
			t.x = ui::mouseDelta.x * speed * 4.f;
			t.y = -ui::mouseDelta.y * speed * 4.f;
			t.z = 0.f;
		}
		else
		{
			if (ui::mbDown)
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
			bool rollMode = true;

			if (rollMode)
			{
				auto c = XMMatrixTranspose(XMMatrixLookAtLH(storedCamera.ViewVec, XMVECTOR{ 0,0,0 }, storedCamera.upVec));
				XMVECTOR translation = XMVector3Transform(XMVECTOR{ t.x, t.y, t.z }, c);
				auto rm = XMMatrixRotationY(-r.y);
				rm = XMMatrixMultiply(XMMatrixRotationAxis(XMVector3Cross(storedCamera.ViewVec , storedCamera.upVec), -r.x),rm);
				rm = XMMatrixMultiply(XMMatrixRotationAxis(storedCamera.ViewVec, r.z), rm);
				currentCamera.ViewVec = XMVector3Transform(storedCamera.ViewVec, rm);
				currentCamera.upVec = XMVector3Transform(storedCamera.upVec, rm);
				currentCamera.Target = storedCamera.Target - translation;
			}
			else
			{
				auto c = XMMatrixTranspose(XMMatrixLookAtLH(storedCamera.ViewVec, XMVECTOR{ 0,0,0 }, storedCamera.upVec));
				XMVECTOR translation = XMVector3Transform(XMVECTOR{ t.x, t.y, t.z }, c);
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
	}

	int counter;

	void StorePoint3d(float x, float y, float z, float w=1)
	{
		ui::Line::buffer[counter].x = x;
		ui::Line::buffer[counter].y = y;
		ui::Line::buffer[counter].z = z;
		ui::Line::buffer[counter].w = w;
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
		StorePoint3d(0, 0, 0, 0);
		StorePoint3d(-axisSize, 0, 0, 1);

		StorePoint3d(-axisSize, 0, 0, 0);
		StorePoint3d(-axisSize + arrowSize, arrowSize / 2, 0, 1);

		StorePoint3d(-axisSize, 0, 0, 0);
		StorePoint3d(-axisSize + arrowSize, -arrowSize / 2, 0, 1);

		int yA = counter;
		int zA = yA * 2;
		for (int x = 0; x < 6; x++)
		{
			ui::Line::buffer[yA + x].x = ui::Line::buffer[x].y;
			ui::Line::buffer[yA + x].y = -ui::Line::buffer[x].x;
			ui::Line::buffer[yA + x].z = -ui::Line::buffer[x].z;
			ui::Line::buffer[yA + x].w = ui::Line::buffer[x].w;

			ui::Line::buffer[zA + x].x = ui::Line::buffer[x].z;
			ui::Line::buffer[zA + x].y = ui::Line::buffer[x].y;
			ui::Line::buffer[zA + x].z = ui::Line::buffer[x].x;
			ui::Line::buffer[zA + x].w = ui::Line::buffer[x].w;
		}

		ui::Line::Setup();

		XMVECTOR Eye = currentCamera.ViewVec;
		//ConstBuf::drawerMat.model = XMMatrixTranspose(XMMatrixLookAtLH(Eye, XMVECTOR{ 0,0,0 }, currentCamera.upVec));
		auto m = ConstBuf::camera.view[0];
		ConstBuf::drawerMat.model = ConstBuf::camera.view[0];
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

	void setCamMat()
	{
		XMVECTOR Eye = currentCamera.ViewVec + currentCamera.Target;
		XMVECTOR At = currentCamera.Target;
		Camera::viewCam.view = XMMatrixLookAtLH(Eye, At, currentCamera.upVec);
		Camera::viewCam.proj = XMMatrixPerspectiveFovLH(DegreesToRadians(Camera::viewCam.angle), dx11::iaspect, 0.01f, 100.0f);
	}

	float switcherTimer = 0;
	float captureTimer = 0;

	void Draw()
	{
		if (uiContext != uiContext_::camera) return;

		bool fade = isKeyDown(CAM_KEY) || flyToCam < 1.f;
		axisAlpha = clamp(axisAlpha + (fade ? .4f : -.05f), 0.f, 1.f);
		axisAlpha = 1;
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

		setCamMat();

		DrawAxis();

		bool boxSelected = false;
		char cbutton[] = "capture  viewCam";
		ui::Box::Setup();
		float w = ui::Text::getTextLen(cbutton, ui::style::text::width) + ui::style::text::width * 2;
		float x = .5f - w / 2.f; float y = .9f + ui::style::text::height;

			ui::style::Base();
			if (!strcmp(cmdParamDesc[currentCmd].funcName, "setCamKey"))
			{
				ui::style::box::g += captureTimer;
			}
			else
			{
				ui::style::box::r += captureTimer;
			}

			if (isMouseOver(x, y, w, ui::style::box::height) && drag.isFree())
			{
				ui::style::box::outlineBrightness = 1.f;
				if (ui::lbDown)
				{
					if (!strcmp(cmdParamDesc[currentCmd].funcName, "setCamKey"))
					{
						float q = intToFloatDenom;
						auto eye = currentCamera.ViewVec*q + currentCamera.Target*q;
						cmdParamDesc[currentCmd].param[2].value[0] = (int)XMVectorGetX(eye);
						cmdParamDesc[currentCmd].param[2].value[1] = (int)XMVectorGetY(eye);
						cmdParamDesc[currentCmd].param[2].value[2] = (int)XMVectorGetZ(eye);
						auto at = currentCamera.Target*q;			 
						cmdParamDesc[currentCmd].param[3].value[0] = (int)XMVectorGetX(at);
						cmdParamDesc[currentCmd].param[3].value[1] = (int)XMVectorGetY(at);
						cmdParamDesc[currentCmd].param[3].value[2] = (int)XMVectorGetZ(at);
						auto up = currentCamera.upVec*q;			 
						cmdParamDesc[currentCmd].param[4].value[0] = (int)XMVectorGetX(up);
						cmdParamDesc[currentCmd].param[4].value[1] = (int)XMVectorGetY(up);
						cmdParamDesc[currentCmd].param[4].value[2] = (int)XMVectorGetZ(up);

						cmdParamDesc[currentCmd].param[5].value[0] = (int)Camera::viewCam.angle;

					}

					boxSelected = true;
					captureTimer = 1;
				}
			}
			ui::Box::Draw(x, y, w);

			char cbutton2[100];
			strcpy (cbutton2, !Camera::viewCam.overRide ? "switch  to  free  camera  [esc]" : "switch  to  keyed  camera  [esc]");
			float w2 = ui::Text::getTextLen(cbutton2, ui::style::text::width) + ui::style::text::width * 2;
			float x2 = .5f - w2 / 2.f; float y2 = .025f;

			ui::style::Base();
			ui::style::box::r += switcherTimer;
			ui::style::box::g += switcherTimer;
			ui::style::box::b += switcherTimer;

			if (isMouseOver(x2, y2, w2, ui::style::box::height) && drag.isFree())
			{
				ui::style::box::outlineBrightness = 1.f;
				if (ui::lbDown && switcherTimer == 0.)
				{
					editor::ViewCam::ToggleViewMode();
					switcherTimer = 1;
					boxSelected = true;
				}
			}
			ui::Box::Draw(x2, y2, w2);

			switcherTimer = clamp(switcherTimer - .1f,0.f,1.f);
			captureTimer = clamp(captureTimer - .1f, 0.f, 1.f);


		ui::Text::Setup();
		ui::Text::Draw(cbutton, x+ ui::style::text::width, y+ ui::style::text::height/8.f);
		ui::Text::Draw(cbutton2, x2 + ui::style::text::width, y2 + ui::style::text::height / 8.f);


		char str[32];
		char str2[123] = "angle:";
		_itoa((int)Camera::viewCam.angle, str,10);
		strcat(str2, str);
		w = ui::Text::getTextLen(str2, ui::style::text::width);
		ui::Text::Draw(str2, .5f-w/2.f, .9f);


		if (!boxSelected&& ui::lbDown)
		{
			drag.set(drag.context::cameraView);
		}

	}

}

