#include "genericdao.h"

GenericDAO::GenericDAO(QObject *parent, NodeType nt) :
	QObject(parent), _parent(NULL), _type(nt)
{}

GenericDAO::GenericDAO(const GenericDAO &remoteObject) :
	QObject(remoteObject.parentNode()),
	_checksum(remoteObject.checksum()),
	_host(remoteObject.host()),
	_icon(remoteObject.icon()),
	_id(remoteObject.id()),
	_title(remoteObject.title()),
	_titleNormalized(remoteObject.titleNormalized()),
	_parent(remoteObject.parentNode()),
	_type(remoteObject.type())
{}

GenericDAO& GenericDAO::operator=(const GenericDAO& remoteObject)
{
	//QObject::operator=(other);
	_checksum = remoteObject.checksum();
	_host = remoteObject.host();
	_icon = remoteObject.icon();
	_id = remoteObject.id();
	_title = remoteObject.title();
	_titleNormalized = remoteObject.titleNormalized();
	_parent = remoteObject.parentNode();
	_type = remoteObject.type();
	return *this;
}

GenericDAO::~GenericDAO()
{}

QString GenericDAO::checksum() const { return _checksum; }
void GenericDAO::setChecksum(const QString &checksum) { _checksum = checksum; }

QString GenericDAO::host() const { return _host; }
void GenericDAO::setHost(const QString &host) { _host = host; }

QString GenericDAO::icon() const { return _icon; }
void GenericDAO::setIcon(const QString &icon) { _icon = icon; }

QString GenericDAO::id() const { return _id; }
void GenericDAO::setId(const QString &id) { _id = id; }

void GenericDAO::setParentNode(GenericDAO *parent) { _parent = parent; }
GenericDAO* GenericDAO::parentNode() const { return _parent; }

QString GenericDAO::title() const { return _title; }
void GenericDAO::setTitle(const QString &title) { _title = title; }

QString GenericDAO::titleNormalized() const{ return _titleNormalized; }
void GenericDAO::setTitleNormalized(const QString &titleNormalized) { _titleNormalized = titleNormalized; }

GenericDAO::NodeType GenericDAO::type() const { return _type; }
