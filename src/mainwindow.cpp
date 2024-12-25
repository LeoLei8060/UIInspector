#include "mainwindow.h"
#include "uiainspector.h"
#include <Windows.h>
#include <QApplication>
#include <QMouseEvent>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto centralWidget = new QWidget(this);
    auto layout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    m_captureButton = new QPushButton("Drag to capture", this);
    layout->addWidget(m_captureButton);

    m_componentList = new QTreeWidget(this);
    m_componentList->setHeaderLabels({"type", "name", "position", "state"});
    layout->addWidget(m_componentList);

    m_captureButton->installEventFilter(this);

    resize(800, 600);
    setWindowTitle("UI Inspector");
}

void MainWindow::startCapture()
{
    m_isCapturing = true;
    m_captureButton->setText("Capturing...");
    setCursor(Qt::CrossCursor);
}

void MainWindow::processCapture()
{
    m_isCapturing = false;
    m_captureButton->setText("Drag to capture");
    setCursor(Qt::ArrowCursor);

    POINT pt;
    GetCursorPos(&pt);

    HWND hwnd = WindowFromPoint(pt);
    if (!hwnd) {
        return;
    }

    m_componentList->clear();

    UIAInspector inspector;
    inspector.inspectWindow(hwnd, m_componentList);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_captureButton) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                startCapture();
                return true;
            }
        }
    }

    if (m_isCapturing && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            processCapture();
            return true;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}
