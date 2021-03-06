#ifndef CUSTOMIZEOPTIONSDIALOG_H
#define CUSTOMIZEOPTIONSDIALOG_H

#include <QDir>
#include <QDialog>
#include <QToolButton>
#include <QTranslator>

#include "ui_customizeoptionsdialog.h"

/**
 * \brief		The CustomizeOptionsDialog class is a very important class. It is designed to help one to customize options of Miam-Player.
 * \details
 * \author      Matthieu Bachelier
 * \copyright   GNU General Public License v3
 */
class CustomizeOptionsDialog : public QDialog, public Ui::CustomizeOptionsDialog
{
	Q_OBJECT
private:
	QTranslator customTranslator, defaultQtTranslator;
	QMap<QString, QString> languages;

public:
	CustomizeOptionsDialog(QWidget *parent = 0);

	/** Third panel in this dialog: shorcuts has to be initialized in the end. */
	void initShortcuts();

	/** Is it necessary to redefined this from the UI class just for this init label? */
	void retranslateUi(CustomizeOptionsDialog *dialog);

protected:
	/** Redefined to add custom behaviour. */
	virtual void closeEvent(QCloseEvent *);

	/** Redefined to inspect shortcuts. */
	virtual bool eventFilter(QObject *obj, QEvent *e);

public slots:
	/** Adds a new music location in the library. */
	void addMusicLocation(const QString &musicLocation);

	/** Adds a external music locations in the library (Drag & Drop). */
	void addMusicLocations(const QList<QDir> &dirs);

	/** Change language at runtime. */
	void changeLanguage(const QString &language);

	/** Redefined to initialize theme from settings. */
	void open();

private slots:
	void checkShortcutsIntegrity();

	/** Delete a music location previously chosen by the user. */
	void deleteSelectedLocation();

	void initCloseActionForPlaylists();
	void initDragDropAction();

	/** Open a dialog for letting the user to choose a music directory. */
	void openLibraryDialog();

	void updateMusicLocations();

signals:
	void aboutToBindShortcut(const QString &objectName, const QKeySequence &keySequence);

	void defaultLocationFileExplorerHasChanged(const QDir &location);

	/** Signal sent whether the music locations have changed or not. */
	void musicLocationsHaveChanged(const QStringList &oldLocations, const QStringList &newLocations);
};

#endif // CUSTOMIZEOPTIONSDIALOG_H
