namespace BasicCam
{

	camData Prev;
	camData Next;

	bool camPass = false;
	int prevTime = 0;
	int nextTime = 0;
	keyType currentCamType = keyType::set;

	COMMAND (setCamKey, timestamp camTime, keyType camType, position eye, position at, position up, int angle)
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

		gapi.cam(eye, at, up, angle);
	}


}