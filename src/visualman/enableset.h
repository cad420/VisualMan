
#ifndef _ENABLESET_H_
#define _ENABLESET_H_
#include <vector>
#include "graphictype.h"

namespace vm
{
class EnableStateSet
{
	friend class RenderContext;

public:
	void Enable( EnableState enable )
	{
		//if (enable == EN_BLEND)
		//{
		//	blendingEnable = true;
		//	return;
		//}
		for ( auto it = enableSet.begin(); it != enableSet.end(); ++it ) {
			if ( *it == enable )
				return;
		}
		enableSet.push_back( enable );
	}
	void Disable( EnableState enable )
	{
		//if (enable == EN_BLEND)
		//{
		//	blendingEnable = false;
		//	return;
		//}
		for ( auto it = enableSet.begin(); it != enableSet.end(); ++it ) {
			if ( *it == enable ) {
				it = enableSet.erase( it );
				return;
			}
		}
	}
	bool IsEnable( EnableState enable ) const
	{
		for ( auto it = enableSet.begin(); it != enableSet.end(); ++it )
			if ( *it == enable )
				return true;
		return false;
	}
	//bool IsBlendingEnable()const { return blendingEnable; }
	//void EnableBlending(bool enable) { blendingEnable = enable; }
private:
	std::vector<EnableState> enableSet;
	//bool blendingEnable = false;
};
}  // namespace vm

#endif
