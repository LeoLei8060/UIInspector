#pragma once

#include <UIAutomation.h>
#include <windows.h>
#include <QTreeWidget>

class UIAInspector
{
public:
    UIAInspector();
    ~UIAInspector();

    void inspectWindow(HWND hwnd, QTreeWidget *tree, const POINT &pt);

    void quickInspect(const POINT &pt, QTreeWidget *tree);

private:
    void    inspectElement(IUIAutomationElement *element,
                           QTreeWidget          *tree,
                           QTreeWidgetItem      *parentItem,
                           const POINT          &pt);
    QString getControlTypeName(CONTROLTYPEID controlType);
    QString getElementRect(IUIAutomationElement *element);
    QString getElementStates(IUIAutomationElement *element);
    void    addElementToTree(IUIAutomationElement *element,
                             QTreeWidget          *tree,
                             QTreeWidgetItem      *parentItem);

    IUIAutomation *m_automation;
};
