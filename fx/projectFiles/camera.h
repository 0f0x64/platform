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

	API(setCamKey, timestamp camTime, keyType camType, position eye, position at, position up, int angle, sliderType sType, position slide, camAxis axisType, rotation fly, int jitter)
	{
		if (!camPass) {

			int t = timer::timeCursor / SAMPLES_IN_FRAME;

			camData* sCam;

			if (camTime <= t || camCounter == 0)
			{
				sCam = &Prev;
				prevTime = camTime;
				if (camCounter == 0) prevTime = 0;

				currentCamType = camType;
				slider = sType;
				camAxisType = axisType;
				camJitter = jitter;

				camSlide = XMVectorSet((float)slide.x, (float)slide.y, (float)slide.z,0);
				camFly = XMVectorSet((float)fly.x, (float)fly.y, (float)fly.z,0);

			}
			else
			{
				sCam = &Next;
				camPass = true;
				nextTime = camTime;
			}

			sCam->eye = XMVectorSet((float)eye.x, (float)eye.y, (float)eye.z,0);
			sCam->at = XMVectorSet((float)at.x, (float)at.y, (float)at.z,0);
			sCam->up = XMVectorSet((float)up.x, (float)up.y, (float)up.z,0);
			sCam->angle = (float)angle;

			camCounter++;
		}
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