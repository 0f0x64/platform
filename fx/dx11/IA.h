namespace InputAssembler
{

	void IA(topology topoType)
	{
		D3D11_PRIMITIVE_TOPOLOGY ttype = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		switch (topoType)
		{
			case topology::triList:
				ttype = D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
				break;
			case topology::lineList:
				ttype = D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
				break;
			case topology::lineStrip:
				ttype = D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;
		}

		context->IASetPrimitiveTopology(ttype);
		context->IASetInputLayout(NULL);
		context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	}

}