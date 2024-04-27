namespace ViewCam
{
	#define CAM_KEY VK_CONTROL

	XMMATRIX storedCam;

	void Init()
	{
		Camera::viewCam.Init();
	}

	void ToggleViewMode()
	{
		Camera::viewCam.overRide = !Camera::viewCam.overRide;
	}

	void Wheel(float delta)
	{
		if (!isKeyDown(CAM_KEY)) return;

		XMMATRIX transM = XMMatrixTranslation(0, 0, delta / 2000.);
		Camera::viewCam.view = XMMatrixMultiply(Camera::viewCam.view, transM);
	}


	void lbDown()
	{
		storedCam = Camera::viewCam.view;
	}

	void mbDown()
	{
		storedCam = Camera::viewCam.view;
	}

	void rbDown()
	{
		storedCam = Camera::viewCam.view;
		editor::ui::mouseLastAngle = editor::ui::mouseAngle;
	}


	void ProcessInput()
	{
		editor::ui::point3df moveDelta = { 0,0,0 };

		if (isKeyDown(VK_RIGHT)) {
			moveDelta.x = .01;
		}
		if (isKeyDown(VK_LEFT)) {
			moveDelta.x = .01;
		}

		if (isKeyDown(VK_UP)) {
			moveDelta.z = .01;
		}
		if (isKeyDown(VK_DOWN)) {
			moveDelta.z = -.01;
		}


		if (!isKeyDown(VK_CONTROL)) return;

		float speed = 4.f;
		editor::ui::point3df r = { 0,0,0 };
		editor::ui::point3df t = { 0,0,0 };

		if (ui::mbDown)
		{
			t.x = ui::mouseDelta.x * speed*4.;
			t.y = -ui::mouseDelta.y * speed*4.;
			t.z = 0.f;
		}

		if (ui::rbDown)
		{
			r.x = 0.f;
			r.y = 0.f;
			r.z = ui::mouseAngleDelta;
		}

		if (ui::lbDown)
		{
			r.x = -ui::mouseDelta.y * speed;
			r.y = -ui::mouseDelta.x * speed;
			r.z = 0.f;
		}

		if (ui::lbDown||ui::rbDown||ui::mbDown)
		{
			XMVECTOR scale, rot, trans;
			XMMatrixDecompose(&scale, &rot, &trans, Camera::viewCam.view);
			XMMATRIX transMInv = XMMatrixTranslationFromVector(-trans);
			XMMATRIX transM = XMMatrixTranslationFromVector(trans);
			
			XMMATRIX moveM = XMMatrixTranslation(t.x,t.y,t.z);
			Camera::viewCam.view = XMMatrixMultiply(storedCam, transMInv);
			Camera::viewCam.view = XMMatrixMultiply(Camera::viewCam.view, XMMatrixRotationX(r.x));
			Camera::viewCam.view = XMMatrixMultiply(Camera::viewCam.view, XMMatrixRotationY(r.y));
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

	void Draw()
	{
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
		ui::Line::Draw3d(counter*3);
	
	}

}
