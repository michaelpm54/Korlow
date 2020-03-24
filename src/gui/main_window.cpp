#include "gui/main_window.h"

#include <QMenuBar>
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	setAttribute(Qt::WA_QuitOnClose);
	resize(860, 660);
	setWindowTitle("Korlow");
	createMenuBar();
	show();
}

void MainWindow::createMenuBar()
{
	QMenuBar *mb { menuBar() };

	QMenu *fileMenu(new QMenu("&File"));

	QAction *openAction(new QAction("&Open"));
	openAction->setShortcut(QKeySequence(Qt::Modifier::CTRL + Qt::Key::Key_O));
	connect(openAction, &QAction::triggered, this, [this]()
		{
			emit openFile(QFileDialog::getOpenFileName(
				this,
				"Open File",
				QDir::currentPath(),
				"ROM (*.gb *.rom *.bin)"
			).toStdString());
		}
	);
	fileMenu->addAction(openAction);

	fileMenu->addSeparator();

	QAction *quitAction(new QAction("&Quit"));
	quitAction->setShortcut(QKeySequence(Qt::Modifier::CTRL + Qt::Key::Key_Q));
	connect(quitAction, &QAction::triggered, this, [this](){ close(); });
	fileMenu->addAction(quitAction);

	mb->addMenu(fileMenu);
}
