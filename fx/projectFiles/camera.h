namespace BasicCam
{

	camData Prev;
	camData Next;

	bool camPass = false;
	int prevTime = 0;
	int nextTime = 0;
	keyType currentCamType = keyType::set;
	camAxis camAxisType = camAxis::local;
	sliderType slider = sliderType::slide;
	int camJitter = 0;

	COMMAND (setCamKey, timestamp camTime, keyType camType, position eye, position at, position up, int angle, sliderType sType, position slide, camAxis axisType, rotation fly, int jitter)
	{
		#include REFLECT(setCamKey)

		if (!camPass) {

			int t = timer::timeCursor / SAMPLES_IN_FRAME;

			camData* sCam;

			if (camTime <= t)
			{
				sCam = &Prev;
				prevTime = camTime;
				currentCamType = camType;
				slider = sType;
				camAxisType = axisType;
				camJitter = jitter;

				sCam->slide = (float)slide.x;
				sCam->slide = (float)slide.y;
				sCam->slide = (float)slide.z;
				sCam->fly.x = (float)fly.x;
				sCam->fly.y = (float)fly.y;
				sCam->fly.z = (float)fly.z;


			}
			else
			{
				sCam = &Next;
				camPass = true;
				nextTime = camTime;
			}

			sCam->eye.x = (float)eye.x;
			sCam->eye.y = (float)eye.y;
			sCam->eye.z = (float)eye.z;
			sCam->at.x = (float)at.x;
			sCam->at.y = (float)at.y;
			sCam->at.z = (float)at.z;
			sCam->up.x = (float)up.x;
			sCam->up.y = (float)up.y;
			sCam->up.z = (float)up.z;
			sCam->angle = (float)angle;

		}

		REFLECT_CLOSE;
	}

	void processCam()
	{
		float q = intToFloatDenom;
		float a = 0;
		if (currentCamType == keyType::slide) a = (timer::timeCursor -prevTime*SAMPLES_IN_FRAME)/(float)((nextTime-prevTime)* SAMPLES_IN_FRAME);
		
		positionF eye, at, up;
		float angle = lerp((float)Prev.angle, (float)Next.angle, a);
		eye.x = lerp(Prev.eye.x, Next.eye.x, a) / q;
		eye.y = lerp(Prev.eye.y, Next.eye.y, a) / q;
		eye.z = lerp(Prev.eye.z, Next.eye.z, a) / q;
		at.x = lerp(Prev.at.x, Next.at.x, a) / q;
		at.y = lerp(Prev.at.y, Next.at.y, a) / q;
		at.z = lerp(Prev.at.z, Next.at.z, a) / q;
		up.x = lerp(Prev.up.x, Next.up.x, a) / q;
		up.y = lerp(Prev.up.y, Next.up.y, a) / q;
		up.z = lerp(Prev.up.z, Next.up.z, a) / q;

		float sTime = (timer::timeCursor - prevTime * SAMPLES_IN_FRAME)*.0001;

		auto m2 = XMMatrixLookAtLH(XMVectorSet(eye.x, eye.y, eye.z, 0.0f), XMVectorSet(at.x, at.y, at.z, 0.0f), XMVectorSet(up.x, up.y, up.z, 0.0f));
		auto m = XMMatrixTranspose(m2);
		auto slide = XMVector3Transform(XMVectorSet(Prev.slide.x, Prev.slide.y, Prev.slide.z,0) * sTime / q, m);

		if (slider == sliderType::follow || slider == sliderType::slide)
		{
			eye.x += XMVectorGetX(slide); eye.y += XMVectorGetY(slide); eye.z += XMVectorGetZ(slide);
		}

		if (slider == sliderType::pan || slider == sliderType::slide)
		{
			at.x += XMVectorGetX(slide); at.y += XMVectorGetY(slide); at.z += XMVectorGetZ(slide);
		}

		auto axisZ = XMVectorSet(eye.x - at.x, eye.y - at.y, eye.z - at.z,0);
		auto axisY = XMVectorSet(up.x, up.y, up.z,0);
		auto axisX = XMVector3Cross(axisZ, axisY);

		if (camAxisType == camAxis::global)
		{
			axisZ = XMVectorSet(0,0,1,0);
			axisY = XMVectorSet(0,1,0,0);
			axisX = XMVectorSet(0,1,0,0);
		}

		XMMATRIX fly;
		sTime = (timer::timeCursor - prevTime * SAMPLES_IN_FRAME) * .00001;
		auto flyX = XMMatrixRotationAxis(axisX,DegreesToRadians(Prev.fly.x * sTime));
		auto flyY = XMMatrixRotationAxis(axisY, DegreesToRadians(Prev.fly.y * sTime));
		auto flyZ = XMMatrixRotationAxis(axisZ, DegreesToRadians(Prev.fly.z * sTime));
		fly = XMMatrixMultiply(XMMatrixMultiply( flyX,flyY),flyZ);

		XMVECTOR eye_ = XMVector4Transform(XMVECTOR{ eye.x-at.x,eye.y-at.y,eye.z-at.z,0 }, fly);
		XMVECTOR up_ = XMVector4Transform(XMVECTOR{ up.x,up.y,up.z,0 }, fly);
		eye.x = XMVectorGetX(eye_)+at.x; eye.y = XMVectorGetY(eye_)+at.y; eye.z = XMVectorGetZ(eye_)+at.z;
		up.x = XMVectorGetX(up_); up.y = XMVectorGetY(up_); up.z = XMVectorGetZ(up_);

		float jAmp = .0001;
		at.x += jAmp * camJitter * sinf(		camJitter * sTime / 1000.f);
		at.y += jAmp * camJitter * sinf(111.3*	camJitter *	sTime / 1000.f);
		at.z += jAmp * camJitter * sinf(161.45*	camJitter *	sTime / 1000.f);

		gapi.cam(eye, at, up, angle);
	}


}