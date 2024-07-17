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

	COMMAND (setCamKey, timestamp camTime, keyType camType, position eye, position at, position up, int angle, sliderType sType, position slide, camAxis axisType, rotation fly, int jitter)
	{
		#include REFLECT(setCamKey)

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

				camSlide = XMVectorSet(slide.x, slide.y, slide.z,0);
				camFly = XMVectorSet(fly.x,fly.y,fly.z,0);

			}
			else
			{
				sCam = &Next;
				camPass = true;
				nextTime = camTime;
			}

			sCam->eye = XMVectorSet(eye.x,eye.y,eye.z,0);
			sCam->at = XMVectorSet(at.x,at.y,at.z,0);
			sCam->up = XMVectorSet(up.x,up.y,up.z,0);
			sCam->angle = (float)angle;

			camCounter++;
		}

		REFLECT_CLOSE;
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

		float sTime = (timer::timeCursor - prevTime * SAMPLES_IN_FRAME)*.0001;

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
		sTime = (timer::timeCursor - prevTime * SAMPLES_IN_FRAME) * .00001;
		auto flyX = XMMatrixRotationAxis(axisX, DegreesToRadians(XMVectorGetX(camFly) * sTime));
		auto flyY = XMMatrixRotationAxis(axisY, DegreesToRadians(XMVectorGetY(camFly) * sTime));
		auto flyZ = XMMatrixRotationAxis(axisZ, DegreesToRadians(XMVectorGetZ(camFly) * sTime));
		fly = XMMatrixMultiply(XMMatrixMultiply( flyX,flyY),flyZ);

		eye = XMVector4Transform(eye-at, fly)+at;
		up = XMVector4Transform(up, fly);

		at += .0001 * camJitter * XMVectorSin((XMVectorSet(100, 111, 161,0) * camJitter * sTime / 1000.f));
		
		positionF eye_c = { XMVectorGetX(eye),XMVectorGetY(eye), XMVectorGetZ(eye) };
		positionF at_c =  { XMVectorGetX(at), XMVectorGetY(at),  XMVectorGetZ(at) };
		positionF up_c =  { XMVectorGetX(up), XMVectorGetY(up),  XMVectorGetZ(up) };

		gapi.cam(eye_c, at_c, up_c, angle);
	}


}