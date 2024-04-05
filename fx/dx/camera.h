namespace Camera
{
	XMMATRIX View;
	XMMATRIX Projection;
	XMMATRIX DepthView;
	XMMATRIX DepthProjection;

	void Set()
	{
		View = XMMatrixTranslation(0, 0, 1);
		DepthView = View;
		Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);
		DepthProjection = Projection;
	}
}