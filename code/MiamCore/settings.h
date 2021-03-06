#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#include "miamcore_global.h"

/**
 * \brief		Settings class contains all relevant pairs of (keys, values) used by Miam-Player.
 * \author      Matthieu Bachelier
 * \copyright   GNU General Public License v3
 */
class MIAMCORE_LIBRARY Settings : public QSettings
{
	Q_OBJECT

private:
	/** The unique instance of this class. */
	static Settings *settings;

	/** Private constructor. */
	Settings(const QString &organization = "MmeMiamMiam",
			 const QString &application = "MiamPlayer");

public:
	/** Singleton Pattern to easily use Settings everywhere in the app. */
	static Settings* instance();

	/** Returns the actual theme name. */
	QString theme() const;

	/** Returns volume from the slider. */
	int volume() const;

public slots:
	/** Sets a new theme. */
	void setThemeName(const QString &theme);

	/** Sets volume from the slider. */
	void setVolume(int v);

signals:
	void themeHasChanged();
};

#endif // SETTINGS_H
