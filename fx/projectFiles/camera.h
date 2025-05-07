namespace BasicCam
{
	int camCounter = 0;
	camData Prev;
	camData Next;

	bool camPass = false;
	int prevTime = 0;
	int nextTime = 0;
	keyType currentCamType = keyType::set;
	camAxis camAxisType = camAxis::local;
	sliderType slider = sliderType::slide;
	int camJitter = 0;
	XMVECTOR camSlide;
	XMVECTOR camFly;

	cmd(setCamKey, timestamp camTime; keyType camType;
					int eye_x; int eye_y; int eye_z;
					int at_x; int at_y; int at_z;
					int up_x; int up_y; int up_z;
					int angle;
					sliderType sType;
					int slide_x; int slide_y; int slide_z;
					camAxis axisType;
					int fly_x; int fly_y; int fly_z;
					int jitter;)
	{
		reflect;

		if (!camPass) {

			int t = timer::timeCursor / SAMPLES_IN_FRAME;

			camData* sCam;

			if (in.camTime <= t || camCounter == 0)
			{
				sCam = &Prev;
				prevTime = in.camTime;
				if (camCounter == 0) prevTime = 0;

				currentCamType = in.camType;
				slider = in.sType;
				camAxisType = in.axisType;
				camJitter = in.jitter;

				camSlide = XMVectorSet((float)in.slide_x, (float)in.slide_y, (float)in.slide_z,0);
				camFly = XMVectorSet((float)in.fly_x, (float)in.fly_y, (float)in.fly_z,0);

			}
			else
			{
				sCam = &Next;
				camPass = true;
				nextTime = in.camTime;
			}

			sCam->eye = XMVectorSet((float)in.eye_x, (float)in.eye_y, (float)in.eye_z,0);
			sCam->at = XMVectorSet((float)in.at_x, (float)in.at_y, (float)in.at_z,0);
			sCam->up = XMVectorSet((float)in.up_x, (float)in.up_y, (float)in.up_z,0);
			sCam->angle = (float)in.angle;

			camCounter++;
		}

		reflect_close;
	}

	void processCam()
	{
		float q = intToFloatDenom;
		float a = 0;
		if (currentCamType == keyType::slide) a = (timer::timeCursor -prevTime*SAMPLES_IN_FRAME)/(float)((nextTime-prevTime)* SAMPLES_IN_FRAME);
		
		XMVECTOR eye, at, up;
		float angle = lerp((float)Prev.angle, (float)Next.angle, a);
		eye = XMVectorLerp(Prev.eye, Next.eye, a) / q;
		at = XMVectorLerp(Prev.at, Next.at, a) / q;
		up = XMVectorLerp(Prev.up, Next.up, a) / q;

		float sTime = (timer::timeCursor - prevTime * SAMPLES_IN_FRAME)*.0001f;

		auto m = XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up));
		auto slide = XMVector3Transform(camSlide * sTime / q, m);

		if (slider == sliderType::follow || slider == sliderType::slide)
		{
			eye += slide;
		}

		if (slider == sliderType::pan || slider == sliderType::slide)
		{
			at += slide;
		}

		auto axisZ = eye - at;
		auto axisY = up;
		auto axisX = XMVector3Cross(axisZ, axisY);

		if (camAxisType == camAxis::global)
		{
			axisZ = XMVectorSet(0,0,1,0);
			axisY = XMVectorSet(0,1,0,0);
			axisX = XMVectorSet(0,1,0,0);
		}
		
		XMMATRIX fly;
		sTime = (timer::timeCursor - prevTime * SAMPLES_IN_FRAME) * .00001f;
		auto flyX = XMMatrixRotationAxis(axisX, DegreesToRadians(XMVectorGetX(camFly) * sTime));
		auto flyY = XMMatrixRotationAxis(axisY, DegreesToRadians(XMVectorGetY(camFly) * sTime));
		auto flyZ = XMMatrixRotationAxis(axisZ, DegreesToRadians(XMVectorGetZ(camFly) * sTime));
		fly = XMMatrixMultiply(XMMatrixMultiply( flyX,flyY),flyZ);

		eye = XMVector4Transform(eye-at, fly)+at;
		up = XMVector4Transform(up, fly);

		at += .0001f * camJitter * XMVectorSin((XMVectorSet(100, 111, 161,0) * (float)camJitter * sTime / 1000.f));
		
		positionF eye_c = { XMVectorGetX(eye),XMVectorGetY(eye), XMVectorGetZ(eye) };
		positionF at_c =  { XMVectorGetX(at), XMVectorGetY(at),  XMVectorGetZ(at) };
		positionF up_c =  { XMVectorGetX(up), XMVectorGetY(up),  XMVectorGetZ(up) };

		Camera::Set(eye_c, at_c, up_c, angle);
	}


}