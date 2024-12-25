#pragma once

#include <QTreeWidget>
#include <windows.h>
#include <UIAutomation.h>

class UIAInspector {
public:
    UIAInspector();
    ~UIAInspector();

    void inspectWindow(HWND hwnd, QTreeWidget* tree);

private:
    void inspectElement(IUIAutomationElement* element, QTreeWidget* tree, QTreeWidgetItem* parentItem);
    QString getControlTypeName(CONTROLTYPEID controlType);
    QString getElementRect(IUIAutomationElement* element);
    QString getElementStates(IUIAutomationElement* element);

    IUIAutomation* m_automation;
};
