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
		#include REFLINK(setCamKey)

		if (!camPass) {

			int t = timer::timeCursor/ SAMPLES_IN_FRAME;

			if (camTime <= t)
			{
				Prev.eye.x = eye.x;
				Prev.eye.y = eye.y;
				Prev.eye.z = eye.z;

				Prev.at.x = at.x;
				Prev.at.y = at.y;
				Prev.at.z = at.z;

				Prev.up.x = up.x;
				Prev.up.y = up.y;
				Prev.up.z = up.z;

				Prev.angle = angle;

				prevTime = camTime;

				currentCamType = camType;

			}
			else
			{
				Next.eye.x = eye.x;
				Next.eye.y = eye.y;
				Next.eye.z = eye.z;

				Next.at.x = at.x;
				Next.at.y = at.y;
				Next.at.z = at.z;

				Next.up.x = up.x;
				Next.up.y = up.y;
				Next.up.z = up.z;

				Next.angle = angle;

				camPass = true;

				nextTime = camTime;
			}
		}

		refStackBack;
	}

	void processCam()
	{
		float q = intToFloatDenom;
		float a = 0;
		if (currentCamType == keyType::slide) a = (timer::timeCursor -prevTime*SAMPLES_IN_FRAME)/(float)((nextTime-prevTime)* SAMPLES_IN_FRAME);
		
		positionF eye, at, up;
		float angle = lerp((float)Prev.angle, (float)Next.angle, a);

		eye.x = lerp(Prev.eye.x, Next.eye.x, a)/q;
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