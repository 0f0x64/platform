void Init ()
{

CreateVS (0, shadersData::lineDrawer);
CreateVS (1, shadersData::objViewer);
CreateVS (2, shadersData::quad);
CreateVS (3, shadersData::simpleCube);
CreatePS (0, shadersData::basic);
CreatePS (1, shadersData::colorFill);
CreatePS (2, shadersData::cubemapCreator);
CreatePS (3, shadersData::cubeMapViewer);
CreatePS (4, shadersData::genNormals);
CreatePS (5, shadersData::obj1);
CreatePS (6, shadersData::simpleFx);

};