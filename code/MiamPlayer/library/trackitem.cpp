#include "trackitem.h"

TrackItem::TrackItem(const TrackDAO *dao) :
	QStandardItem(dao->title())
{
	setData(dao->uri(), Miam::DF_URI);
	setData(dao->titleNormalized(), Miam::DF_NormalizedString);
	setData(dao->trackNumber(), Miam::DF_TrackNumber);
	setData(dao->disc().toInt(), Miam::DF_DiscNumber);
	setData(!dao->uri().startsWith("file://"), Miam::DF_IsRemote);
}

int TrackItem::type() const
{
	return Miam::IT_Track;
}
