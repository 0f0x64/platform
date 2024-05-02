namespace ViewCam
{
	#define CAM_KEY VK_CONTROL

	XMMATRIX storedCam;
	XMMATRIX targetCam;
	float flyToCam = 1.f;


	void Init()
	{
		Camera::viewCam.Init();
	}

	void ToggleViewMode()
	{
		Camera::viewCam.overRide = !Camera::viewCam.overRide;
	}

	void AxisCam(float roll, float pitch,float yaw)
	{
		XMVECTOR scale, rot, trans;
		XMMatrixDecompose(&scale, &rot, &trans, Camera::viewCam.view);

		XMMATRIX base = XMMatrixScalingFromVector(scale);
		base = XMMatrixMultiply(base, XMMatrixRotationRollPitchYaw(DegreesToRadians(roll), DegreesToRadians(pitch), DegreesToRadians(yaw)));
		base = XMMatrixMultiply(base, XMMatrixTranslationFromVector(trans));

		targetCam = base;
		flyToCam = 0.f;
	}


	void Wheel(float delta)
	{
		if (isKeyDown(CAM_KEY)) {
			XMMATRIX transM = XMMatrixTranslation(0, 0, -delta / 2000.f);
			Camera::viewCam.view = XMMatrixMultiply(Camera::viewCam.view, transM);
		}

		if (isKeyDown(VK_SHIFT)) {
			Camera::viewCam.angle += delta / 50.f;
			Camera::viewCam.proj = XMMatrixPerspectiveFovLH(DegreesToRadians(Camera::viewCam.angle), width / (FLOAT)height, 0.01f, 100.0f);
		}

	}


	void lbDown()
	{
		storedCam = Camera::viewCam.view;
	}

	void rbDown()
	{
		storedCam = Camera::viewCam.view;
		editor::ui::mouseLastAngle = editor::ui::mouseAngle;
	}

	float axisAlpha = 0;

	void ProcessInput()
	{

		if (!(isKeyDown(VK_CONTROL) || isKeyDown(VK_SHIFT))) return;


		float speed = 4.f;
		editor::ui::point3df r = { 0,0,0 };
		editor::ui::point3df t = { 0,0,0 };

		if (ui::mbDown || (ui::lbDown&&isKeyDown(VK_SHIFT)))
		{
			t.x = ui::mouseDelta.x * speed*4.f;
			t.y = -ui::mouseDelta.y * speed*4.f;
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
		if (ui::lbDown||ui::rbDown||ui::mbDown)
		{
			XMVECTOR scale, rot, trans;
			XMMatrixDecompose(&scale, &rot, &trans, Camera::viewCam.view);
			XMMATRIX transMInv = XMMatrixTranslationFromVector(-trans);
			XMMATRIX transM = XMMatrixTranslationFromVector(trans);
			
			XMMATRIX moveM = XMMatrixTranslation(t.x,t.y,t.z);
			Camera::viewCam.view = XMMatrixMultiply(storedCam, transMInv);
			Camera::viewCam.view = XMMatrixMultiply(Camera::viewCam.view, XMMatrixRotationY(r.y));
			Camera::viewCam.view = XMMatrixMultiply(Camera::viewCam.view, XMMatrixRotationX(r.x));
			Camera::viewCam.view = XMMatrixMultiply(Camera::viewCam.view, XMMatrixRotationZ(r.z));
			Camera::viewCam.view = XMMatrixMultiply(Camera::viewCam.view, moveM);
			Camera::viewCam.view = XMMatrixMultiply(Camera::viewCam.view, transM);
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


	void Draw()
	{
		if (isKeyDown(CAM_KEY) || isKeyDown(VK_SHIFT) || flyToCam < 1.f)
		{
			axisAlpha += .4;
			axisAlpha = clamp(axisAlpha, 0.f, 1.f);
		}
		else
		{
			axisAlpha -= .05;
			axisAlpha = clamp(axisAlpha, 0.f, 1.f);

		}

		if (flyToCam<1.f)
		{
			flyToCam += .025;

			XMVECTOR srcScale, srcRot, srcTrans;
			XMMatrixDecompose(&srcScale, &srcRot, &srcTrans, Camera::viewCam.view);
			XMVECTOR dstScale, dstRot, dstTrans;
			XMMatrixDecompose(&dstScale, &dstRot, &dstTrans, targetCam);

			float srcPitch, srcYaw, srcRoll;
			ExtractPitchYawRollFromXMMatrix(&srcPitch, &srcYaw, &srcRoll, &Camera::viewCam.view);
			float dstPitch, dstYaw, dstRoll;
			ExtractPitchYawRollFromXMMatrix(&dstPitch, &dstYaw, &dstRoll, &targetCam);

			srcPitch = lerp(srcPitch, dstPitch, flyToCam);
			srcYaw = lerp(srcYaw, dstYaw, flyToCam);
			srcRoll = lerp(srcRoll, dstRoll, flyToCam);

			XMMATRIX RM = (XMMatrixRotationRollPitchYaw(srcPitch, srcYaw, srcRoll));

			XMMATRIX base = XMMatrixScalingFromVector(srcScale);
			base = XMMatrixMultiply(base, RM);
			base = XMMatrixMultiply(base, XMMatrixTranslationFromVector(srcTrans));
			
			Camera::viewCam.view = base;
			if (flyToCam > .975f)
			{
				Camera::viewCam.view = targetCam;
				storedCam = targetCam;
			}
		}

		ProcessInput();

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
		ui::Line::Draw3d(counter*3,1,1,1,axisAlpha);
	
	}

}
