#include <largevolumeraycasterapplication.h>

int main(int argc, char ** argv)
{
	return ysl::app::LargeVolumeRayCaster(argc, argv,
		800, 600, "C:\\data\\subregion_9000_10700_2_128.lvd").Exec();

	//return ysl::app::LargeVolumeRayCaster(argc, argv,
	//	800, 600, "C:\\data\\subregion_2_128.lvd").Exec();
	//return ysl::app::LargeVolumeRayCaster(argc, argv,
	//	800, 600, "D:\\Desktop\\data\\test.lvd").Exec();
	//return ysl::app::LargeVolumeRayCaster(argc, argv,
	//	800, 600, "D:\\scidata\\abc\\s1_512_512_512_2_128.lvd").Exec();
	//return ysl::app::LargeVolumeRayCaster(argc, argv,
	//	800, 600, "D:\\Desktop\\s1.lvd").Exec();
	//return ysl::app::LargeVolumeRayCaster(argc, argv,
	//	800, 600, "D:\\Desktop\\head.lvd").Exec();
	//return ysl::app::OITMeshRenderer(argc, argv, 800, 600).Exec();

}
