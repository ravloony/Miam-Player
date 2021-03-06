#include "trackdao.h"

TrackDAO::TrackDAO(QObject *parent) :
	GenericDAO(parent, GenericDAO::Track), _rating(0)
{}

TrackDAO::TrackDAO(const TrackDAO &other) :
	GenericDAO(other),
	_album(other.album()),
	_artist(other.artist()),
	_artistAlbum(other.artistAlbum()),
	_disc(other.disc()),
	_length(other.length()),
	_source(other.source()),
	_trackNumber(other.trackNumber()),
	_uri(other.uri()),
	_year(other.year()),
	_rating(other.rating())
{}

TrackDAO& TrackDAO::operator=(const TrackDAO& other)
{
	GenericDAO::operator=(other);
	_album = other.album();
	_artist = other.artist();
	_artistAlbum = other.artistAlbum();
	_disc = other.disc();
	_length = other.length();
	_source = other.source();
	_rating = other.rating();
	_trackNumber = other.trackNumber();
	_uri = other.uri();
	_year = other.year();
	return *this;
}

TrackDAO::~TrackDAO() {}

QString TrackDAO::album() const { return _album; }
void TrackDAO::setAlbum(const QString &album) { _album = album; }

QString TrackDAO::artist() const { return _artist; }
void TrackDAO::setArtist(const QString &artist) { _artist = artist; }

QString TrackDAO::artistAlbum() const { return _artistAlbum; }
void TrackDAO::setArtistAlbum(const QString &artistAlbum) { _artistAlbum = artistAlbum; }

QString TrackDAO::disc() const { return _disc; }
void TrackDAO::setDisc(const QString &disc) { _disc = disc; }

QString TrackDAO::length() const { return _length; }
void TrackDAO::setLength(const QString &length) { _length = length; }

int TrackDAO::rating() const { return _rating; }
void TrackDAO::setRating(int rating) { _rating = rating; }

QString TrackDAO::source() const { return _source; }
void TrackDAO::setSource(const QString &source) { _source = source; }

QString TrackDAO::trackNumber() const { return _trackNumber; }
void TrackDAO::setTrackNumber(const QString &trackNumber) { _trackNumber = trackNumber; }

QString TrackDAO::uri() const { return _uri; }
void TrackDAO::setUri(const QString &uri) { _uri = uri; }

QString TrackDAO::year() const { return _year; }
void TrackDAO::setYear(const QString &year) { _year = year; }
