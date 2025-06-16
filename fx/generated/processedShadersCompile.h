void Init ()
{

CreateVS (0, shadersData::objViewer);
CreateVS (1, shadersData::quad);
CreateVS (2, shadersData::simpleCube);
CreateVS (3, shadersData::box);
CreateVS (4, shadersData::letter);
CreateVS (5, shadersData::lineDrawer);
CreateVS (6, shadersData::lineDrawer3d);
CreatePS (0, shadersData::basic);
CreatePS (1, shadersData::cat);
CreatePS (2, shadersData::cubemapCreator);
CreatePS (3, shadersData::cubeMapViewer);
CreatePS (4, shadersData::genNormals);
CreatePS (5, shadersData::obj1);
CreatePS (6, shadersData::box_ps);
CreatePS (7, shadersData::letter_ps);
CreatePS (8, shadersData::lineDrawerUV_ps);
CreatePS (9, shadersData::lineDrawer_ps);

};