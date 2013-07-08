#include "playlist.h"

#include <QApplication>
#include <QDropEvent>
#include <QHeaderView>
#include <QScrollBar>
#include <QTime>

#include <fileref.h>
#include <tag.h>

#include "columnutils.h"
#include "settings.h"
#include "nofocusitemdelegate.h"
#include "library/librarytreeview.h"
#include "tabplaylist.h"
#include "stardelegate.h"

#include <QtDebug>

Playlist::Playlist(QWidget *parent) :
	QTableView(parent), _dropDownIndex(NULL)
{
	qMediaPlaylist = new QMediaPlaylist(this);
	_playlistModel = new PlaylistModel(this);

	this->setModel(_playlistModel);

	Settings *settings = Settings::getInstance();
	// Init direct members
	this->setAcceptDrops(true);
	this->setAlternatingRowColors(settings->colorsAlternateBG());

	this->setColumnHidden(5, true);
	this->setColumnHidden(6, true);
	this->setDragDropMode(QAbstractItemView::DragDrop);
	this->setDragEnabled(true);
	this->setDropIndicatorShown(true);
	this->setItemDelegate(new NoFocusItemDelegate(this));
	this->setItemDelegateForColumn(5, new StarDelegate);
	this->setHorizontalHeader(new QHeaderView(Qt::Horizontal, this));
	// Select only by rows, not cell by cell
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->setShowGrid(false);
	this->setStyleSheet(settings->styleSheet(this));

	// Init child members
	verticalScrollBar()->setStyleSheet(settings->styleSheet(this->verticalScrollBar()));
	verticalHeader()->setVisible(false);
	horizontalHeader()->setStyleSheet(settings->styleSheet(horizontalHeader()));
	horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	horizontalHeader()->setHighlightSections(false);
	horizontalHeader()->setSectionsMovable(true);

	// Context menu on tracks
	trackProperties = new QMenu(this);
	QAction *removeFromCurrentPlaylist = trackProperties->addAction(tr("Remove from playlist"));
    connect(removeFromCurrentPlaylist, &QAction::triggered, this, &Playlist::removeSelectedTracks);

	// Context menu on header of columns
	columns = new QMenu(this);
	connect(columns, SIGNAL(triggered(QAction*)), this, SLOT(toggleSelectedColumn(QAction*)));

	// Hide the selected column in context menu
    connect(horizontalHeader(), &QWidget::customContextMenuRequested, this, &Playlist::showColumnsMenu);
    connect(horizontalHeader(), &QHeaderView::sectionMoved, this, &Playlist::saveColumnsState);

	// Set row height
	verticalHeader()->setDefaultSectionSize(QFontMetrics(settings->font(Settings::PLAYLIST)).height());
}

void Playlist::init()
{
	QStringList labels = (QStringList() << "#" << tr("Title") << tr("Album") << tr("Length") << tr("Artist") << tr("Rating") << tr("Year"));

	_playlistModel->setColumnCount(labels.count());

	// Initialize values for the Header (label and horizontal resize mode)
	Settings *settings = Settings::getInstance();
	for (int i = 0; i < labels.size(); i++) {
		_playlistModel->setHeaderData(i, Qt::Horizontal, labels.at(i), Qt::DisplayRole);
		_playlistModel->setHeaderData(i, Qt::Horizontal, settings->font(Settings::PLAYLIST), Qt::FontRole);
	}

	for (int i = 0; i < _playlistModel->columnCount(); i++) {
		QString label = labels.at(i);

		// Match actions with columns using index of labels
		QAction *actionColumn = new QAction(label, this);
		actionColumn->setData(i);
		actionColumn->setEnabled(actionColumn->text() != tr("Title"));
		actionColumn->setCheckable(true);
		actionColumn->setChecked(!isColumnHidden(i));

		// Then populate the context menu
		columns->addAction(actionColumn);
	}

	// Load columns state
	horizontalHeader()->restoreState(Settings::getInstance()->value("playlistColumnsState").toByteArray());
	for (int i = 0; i < horizontalHeader()->count(); i++) {
		bool hidden = horizontalHeader()->isSectionHidden(i);
		setColumnHidden(i, hidden);
		columns->actions().at(i)->setChecked(!hidden);
	}
}

void Playlist::insertMedias(int rowIndex, const QList<QMediaContent> &medias)
{
	qMediaPlaylist->insertMedia(rowIndex, medias);
	_playlistModel->insertMedias(rowIndex, medias);
}

/** Display a context menu with the state of all columns. */
void Playlist::showColumnsMenu(const QPoint &pos)
{
	columns->exec(mapToGlobal(pos));
}

/** Retranslate header columns. */
void Playlist::retranslateUi()
{
	const QStringList labels = (QStringList() << "#" << tr("Title") << tr("Album") << tr("Length") << tr("Artist") << tr("Rating") << tr("Year"));

	// Initialize values for the Header (label and horizontal resize mode)
	for (int i = 0; i < labels.size(); i++) {
		_playlistModel->setHeaderData(i, Qt::Horizontal, labels.at(i), Qt::DisplayRole);
		columns->actions().at(i)->setText(labels.at(i));
	}
}

/** Redefined to display a small context menu in the view. */
void Playlist::contextMenuEvent(QContextMenuEvent *event)
{
	QModelIndex index = this->indexAt(event->pos());
	QStandardItem *item = _playlistModel->itemFromIndex(index);
	if (item != NULL) {
		trackProperties->exec(event->globalPos());
	}
}

void Playlist::dragEnterEvent(QDragEnterEvent *event)
{
	// If the source of the drag and drop is another application, do nothing?
	if (event->source() == NULL) {
		event->ignore();
	} else {
		event->acceptProposedAction();
	}
}

void Playlist::dragMoveEvent(QDragMoveEvent *event)
{
	event->acceptProposedAction();
	_dropDownIndex = new QModelIndex();
	// Kind of hack to keep track of position?
	*_dropDownIndex = indexAt(event->pos());
	repaint();
	delete _dropDownIndex;
	_dropDownIndex = NULL;
}

/** Redefined to be able to move tracks between playlists or internally. */
void Playlist::dropEvent(QDropEvent *event)
{
	QObject *source = event->source();
	int row = this->indexAt(event->pos()).row();
	if (TreeView *view = qobject_cast<TreeView*>(source)) {
		view->insertToPlaylist(row);
	} else if (Playlist *target = qobject_cast<Playlist*>(source)) {
		if (target && target == this) {
			QList<QStandardItem*> rowsToHighlight = _playlistModel->internalMove(indexAt(event->pos()), selectionModel()->selectedRows());
			// Highlight rows that were just moved
			foreach (QStandardItem *item, rowsToHighlight) {
				for (int c = 0; c < _playlistModel->columnCount(); c++) {
					QModelIndex index = _playlistModel->index(item->row(), c);
					selectionModel()->select(index, QItemSelectionModel::Select);
				}
			}
		} else if (target && target != this) {
			// If the drop occurs at the end of the playlist, indexAt is invalid
			if (row == -1) {
				row = qMediaPlaylist->mediaCount();
			}
			QList<QMediaContent> medias;
			foreach (QModelIndex index, target->selectionModel()->selectedRows()) {
				medias.append(target->mediaPlaylist()->media(index.row()));
			}
			this->insertMedias(row, medias);

			// Highlight rows that were just moved
			this->clearSelection();
			for (int r = 0; r < medias.count(); r++) {
				for (int c = 0; c < _playlistModel->columnCount(); c++) {
					QModelIndex index = _playlistModel->index(row + r, c);
					selectionModel()->select(index, QItemSelectionModel::Select);
				}
			}
			if (!Settings::getInstance()->copyTracksFromPlaylist()) {
				target->removeSelectedTracks();
			}
		}
	}
}

void Playlist::mouseMoveEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton))
		return;
	if ((event->pos() - _dragStartPosition).manhattanLength() < QApplication::startDragDistance())
		return;
	QTableView::mouseMoveEvent(event);
}

void Playlist::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		_dragStartPosition = event->pos();
	}
	QTableView::mousePressEvent(event);
}

/** Redefined to display a thin line to help user for dropping tracks. */
void Playlist::paintEvent(QPaintEvent *event)
{
	QTableView::paintEvent(event);
	if (_dropDownIndex) {
		// Where to draw the indicator line
		int rowDest = _dropDownIndex->row() >= 0 ? _dropDownIndex->row() : _playlistModel->rowCount();
		int height = this->rowHeight(0);
		QPainter painter(viewport());
		/// TODO computes color from user define settings
		painter.setPen(Qt::black);
		painter.drawLine(viewport()->rect().left(), rowDest * height,
						 viewport()->rect().right(), rowDest * height);
	}
}

void Playlist::resizeEvent(QResizeEvent *)
{
	QList<int> ratios(QList<int>() << 0 << 5 << 4 << 1 << 3 << 2 << 0);
	ColumnUtils::resizeColumns(this, ratios);
}

/** Toggle the selected column from the context menu. */
void Playlist::toggleSelectedColumn(QAction *action)
{
	int columnIndex = action->data().toInt();
	this->setColumnHidden(columnIndex, !isColumnHidden(columnIndex));
	QList<int> ratios(QList<int>() << 0 << 5 << 4 << 1 << 3 << 0 << 0);
	ColumnUtils::resizeColumns(this, ratios);
	this->saveColumnsState();
}

/** Save state when one checks or moves a column. */
void Playlist::saveColumnsState(int /*logicalIndex*/, int /*oldVisualIndex*/, int /*newVisualIndex*/)
{
	// The pair "playlistColumnsState" is only used in this class, so there's no need to create specific getter and setter
	Settings::getInstance()->setValue("playlistColumnsState", horizontalHeader()->saveState());
}

/** Move selected tracks downward. */
void Playlist::moveTracksDown()
{
	/*if (currentItem()) {
		int currentRow = currentItem()->row();
		if (currentRow < rowCount()-1) {
			for (int c=0; c < columnCount(); c++) {
				QTableWidgetItem *currentItem = takeItem(currentRow, c);
				QTableWidgetItem *nextItem = takeItem(currentRow+1, c);
				setItem(currentRow, c, nextItem);
				setItem(currentRow+1, c, currentItem);
			}
			sources.swap(currentRow, currentRow+1);
			this->setCurrentIndex(_playlistModel()->index(currentRow+1, 0));
			if (currentRow == track) {
				track++;
			}
		}
	}*/
}

/** Move selected tracks upward. */
void Playlist::moveTracksUp(int i)
{
	/*QList<QTableWidgetItem *> selection = selectedItems();
	int prev = -1;
	for (int k = selection.length() - 1; k >= 0; k--) {
		int current = selection.at(k)->row();
		if (current != prev) {
			QTableWidgetItem *item = takeItem(current, 0);
			QTableWidgetItem *itemBelow = takeItem(current+1, 0);
			setItem(current, 0, itemBelow);
			setItem(current+1, 0, item);
			prev = current;
		}
	}*/

	/*if (this->selectionModel()->hasSelection()) {
		int currentRow = currentItem()->row();
		if (currentRow > 0) {
			for (int c=0; c < columnCount(); c++) {
				QTableWidgetItem *currentItem = takeItem(currentRow, c);
				QTableWidgetItem *previousItem = takeItem(currentRow-1, c);
				setItem(currentRow, c, previousItem);
				setItem(currentRow-1, c, currentItem);
			}
			sources.swap(currentRow, currentRow-1);
			setCurrentIndex(_playlistModel()->index(currentRow-1, 0));
			if (currentRow == track) {
				track--;
			}
		}
	}*/
}

/** Remove selected tracks from the playlist. */
void Playlist::removeSelectedTracks()
{
	QModelIndexList indexes = this->selectionModel()->selectedRows();
	for (int i = indexes.size() - 1; i >= 0; i--) {
		int row = indexes.at(i).row();
		_playlistModel->removeRow(row);
		qMediaPlaylist->removeMedia(row);
	}
}

/** Change the style of the current track. Moreover, this function is reused when the user is changing fonts in the settings. */
void Playlist::highlightCurrentTrack()
{
	QStandardItem *it = NULL;
	const QFont font = Settings::getInstance()->font(Settings::PLAYLIST);
	if (_playlistModel->rowCount() > 0) {
		for (int i=0; i < _playlistModel->rowCount(); i++) {
			for (int j = 0; j < _playlistModel->columnCount(); j++) {
				it = _playlistModel->item(i, j);
				QFont itemFont = font;
				itemFont.setBold(false);
				itemFont.setItalic(false);
				it->setFont(itemFont);
			}
		}
		for (int j=0; j < _playlistModel->columnCount(); j++) {
			it = _playlistModel->item(qMediaPlaylist->currentIndex(), j);
			// If there is actually one selected track in the playlist
			if (it != NULL) {
				QFont itemFont = font;
				itemFont.setBold(true);
				itemFont.setItalic(true);
				it->setFont(itemFont);
			}
		}
	}
}