#include "settings.h"

#include <QDateTime>
#include <QFile>
#include <QApplication>
#include <QGuiApplication>
#include <QHeaderView>
#include <QScrollBar>
#include <QTabWidget>

#include <QtDebug>

Settings* Settings::settings = NULL;

/** Private constructor. */
Settings::Settings(const QString &organization, const QString &application)
	: QSettings(organization, application)
{

}

/** Singleton pattern to be able to easily use settings everywhere in the app. */
Settings* Settings::instance()
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
		return "oxygen";
	} else {
		return theme;
	}
}

/** Returns volume from the slider. */
int Settings::volume() const
{
   if (value("volume").isNull()) {
	   return 90;
   } else {
	   return value("volume").toInt();
   }
}

void Settings::setThemeName(const QString &theme)
{
	setValue("theme", theme.toLower());
	emit themeHasChanged();
}


void Settings::setVolume(int v)
{
	setValue("volume", v);
}
