#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
	MainWindow(QWidget *parent = nullptr);

signals:
	void openFile(const std::string &path);

private:
	void createMenuBar();
};

#endif // MAIN_WINDOW
