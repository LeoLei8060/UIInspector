#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QTreeWidget>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void startCapture();
    void processCapture();
    
    QPushButton* m_captureButton;
    QTreeWidget* m_componentList;
    bool m_isCapturing = false;
};
