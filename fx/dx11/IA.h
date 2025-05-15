namespace InputAssembler
{

	void IA(topology topoType)
	{
		D3D11_PRIMITIVE_TOPOLOGY ttype[] = { 
			D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			D3D10_PRIMITIVE_TOPOLOGY_LINELIST,
			D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP 
		};

		context->IASetPrimitiveTopology(ttype[(int)topoType]);
		context->IASetInputLayout(NULL);
		context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	}

}