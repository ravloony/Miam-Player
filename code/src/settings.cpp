#include "settings.h"

Settings* Settings::settings = NULL;

/** Singleton pattern to be able to easily use settings everywhere in the app. */
Settings* Settings::getInstance()
{
	if (settings == NULL) {
		settings = new Settings;
	}
	return settings;
}

/** Returns the actual theme name. */
QString Settings::theme() const
{
	QString theme = value("theme").toString();
	if (theme.isEmpty()) {
		return "fairytale";
	} else {
		return theme;
	}
}

/** Return the actual size of media buttons. */
int Settings::buttonSize() const
{
	int s = value("buttonSize").toInt();
	if (s == 0) {
		return 20;
	} else {
		return s;
	}
}

/** Returns true if the button in parameter is visible or not. */
bool Settings::isVisible(MediaButton *b) const
{
   QVariant ok = value(b->objectName());
   if (ok.isValid()) {
	   return ok.toBool();
   } else {
	   // For the first run, show buttons anyway
	   return true;
   }
}

/** Returns the language of the application. */
QString Settings::language()
{
	QString l = value("language").toString();
	if (l.isEmpty()) {
		setValue("language", "en");
		return "en";
	} else {
		return l;
	}
}

/** Returns the font of the application. */
QFont Settings::font(FontFamily fontFamily)
{
	QFont font;
	switch(fontFamily) {
	case PLAYLIST:
	case LIBRARY:
	case MENUS:
		font = QFont(value(QString(fontFamily)).toString());
		font.setPointSize(this->fontSize(fontFamily));
	}
	return font;
}

/** Sets the font of the application. */
const int Settings::fontSize(FontFamily fontFamily)
{
	fontPointSizeMap = this->value("fontPointSizeMap").toMap();
	int pointSize = fontPointSizeMap.value(QString(fontFamily)).toInt();
	if (pointSize == 0) {
		pointSize = 8;
	}
	return pointSize;
}

/** Adds a new path in the application. */
void Settings::addMusicLocation(QString location) {
	locations = value("musicLocations").toList();
	if (!locations.contains(location)) {
		locations.append(QVariant(location));
		setValue("musicLocations", locations);
	}
}

/** Removes a path in the application. */
void Settings::removeMusicLocation(QString location) {
	locations = value("musicLocations").toList();
	locations.removeOne(location);
	setValue("musicLocations", locations);
	if (locations.isEmpty()) {
		remove("musicLocations");
	}
}

/** Returns the size of a cover. */
int Settings::coverSize() const
{
	int size = value("coverSize").toInt();
	if (size == 0) {
		size = 48;
	}
	return size;
}

/** Returns the size of the buffer for a cover. */
int Settings::bufferedCoverSize() const
{
	int buffer = value("bufferedCoverSize").toInt();
	if (buffer == 0) {
		buffer = 128;
	}
	return buffer;
}

void Settings::setShortcut(const QString &objectName, const QKeySequence &keySequence)
{
	setValue(QString("shortcut").append(objectName), keySequence.toString());
}

QKeySequence Settings::shortcut(const QString &objectName) const
{
	return QKeySequence(value(QString("shortcut").append(objectName)).toString());
}

/** Sets if the button in parameter is visible or not. */
void Settings::setVisible(MediaButton *b, const bool &value) {
	setValue(b->objectName(), value);
	// The only one button which is checkable is the repeat button
	if (b->isCheckable() && !value) {
		setRepeatPlayBack(value);
	}
}
