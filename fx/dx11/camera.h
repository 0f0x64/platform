namespace Camera 
{
	void Set()
	{
		float t = (float)(timer::frameBeginTime * .0002);
		XMVECTOR Eye = XMVectorSet(sinf(t)*2.f, sinf(t*.5f), cosf(t)*2.f, 0.0f)*(1.f+sinf(t)*.25f);
		XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		ConstBuf::camera.world[0] = XMMatrixIdentity();//world
		ConstBuf::camera.view[0] = XMMatrixTranspose(XMMatrixLookAtLH(Eye, At, Up));
		ConstBuf::camera.proj[0] = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV4*2.4, width / (FLOAT)height, 0.01f, 100.0f));

		ConstBuf::UpdateCamera();
		ConstBuf::SetV(3);
		ConstBuf::SetP(3);
	}
}