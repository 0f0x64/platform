void Init ()
{

CreateVS (0, shadersData::objViewer);
CreateVS (1, shadersData::quad);
CreateVS (2, shadersData::simpleCube);
CreatePS (0, shadersData::basic);
CreatePS (1, shadersData::cubemapCreator);
CreatePS (2, shadersData::cubeMapViewer);
CreatePS (3, shadersData::genNormals);
CreatePS (4, shadersData::obj1);
CreatePS (5, shadersData::simpleFx);

};