#include <largevolumeraycasterapplication.h>

int main(int argc, char ** argv)
{
	return ysl::app::LargeVolumeRayCaster(argc, argv,
		800, 600, "C:\\data\\subregion_9000_10700_2_128.lvd").Exec();
}
