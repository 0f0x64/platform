#define MAX_CAM 64
#define MAX_CAM_KEY 64
Camera::camData cam[MAX_CAM];
Camera::camData camTmp;

void regCam()
{
	int i = 0;

	cam[i].angle = DegreesToRadians(120.);
	cam[i].eye = { 0,0,3 };
	cam[i].at = { 0,0,0 };
	cam[i].up = { 0,1,0 };
	i++;

	cam[i].angle = DegreesToRadians(120.);
	cam[i].eye = { 3,0,0 };
	cam[i].at = { 0,0,0 };
	cam[i].up = { 0,1,0 };
	i++;
}

void setCam(int i)
{
	api.cam(cam[i]);
}

void slideCam(int i, int j, float a)
{
	camTmp.angle = lerp(cam[i].angle, cam[j].angle, a);

	camTmp.eye.x = lerp(cam[i].eye.x, cam[j].eye.x, a);
	camTmp.eye.y = lerp(cam[i].eye.y, cam[j].eye.y, a);
	camTmp.eye.z = lerp(cam[i].eye.z, cam[j].eye.z, a);

	camTmp.at.x = lerp(cam[i].at.x, cam[j].at.x, a);
	camTmp.at.y = lerp(cam[i].at.y, cam[j].at.y, a);
	camTmp.at.z = lerp(cam[i].at.z, cam[j].at.z, a);

	camTmp.up.x = lerp(cam[i].up.x, cam[j].up.x, a);
	camTmp.up.y = lerp(cam[i].up.y, cam[j].up.y, a);
	camTmp.up.z = lerp(cam[i].up.z, cam[j].up.z, a);

	api.cam(camTmp);
}

typedef struct {
	int pos;
	int type;
} camKey_;

camKey_ camKey[MAX_CAM_KEY];

void regCamKeys()
{
	int i = 0;

}