#include "uiainspector.h"
#include <QDebug>

UIAInspector::UIAInspector()
    : m_automation(nullptr)
{
    HRESULT hr = CoInitialize(nullptr);
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(__uuidof(CUIAutomation),
                              nullptr,
                              CLSCTX_INPROC_SERVER,
                              __uuidof(IUIAutomation),
                              reinterpret_cast<void **>(&m_automation));
        if (FAILED(hr)) {
            qDebug() << "Failed to create UIAutomation instance";
        }
    }
}

UIAInspector::~UIAInspector()
{
    if (m_automation) {
        m_automation->Release();
    }
    CoUninitialize();
}

void UIAInspector::inspectWindow(HWND hwnd, QTreeWidget *tree)
{
    if (!m_automation || !hwnd || !tree)
        return;

    IUIAutomationElement *element = nullptr;
    HRESULT               hr = m_automation->ElementFromHandle(hwnd, &element);
    if (SUCCEEDED(hr) && element) {
        inspectElement(element, tree, nullptr);
        element->Release();
    }
}

void UIAInspector::inspectElement(IUIAutomationElement *element, QTreeWidget *tree, QTreeWidgetItem *parentItem)
{
    if (!element)
        return;

    CONTROLTYPEID controlType;
    element->get_CurrentControlType(&controlType);

    BSTR name;
    element->get_CurrentName(&name);

    QString elementName = QString::fromWCharArray(name ? name : L"");
    SysFreeString(name);

    QTreeWidgetItem *item;
    if (parentItem) {
        item = new QTreeWidgetItem(parentItem);
    } else {
        item = new QTreeWidgetItem(tree);
    }

    item->setText(0, getControlTypeName(controlType));
    item->setText(1, elementName);
    item->setText(2, getElementRect(element));
    item->setText(3, getElementStates(element));

    IUIAutomationTreeWalker *walker;
    m_automation->get_ControlViewWalker(&walker);
    if (walker) {
        IUIAutomationElement *child;
        walker->GetFirstChildElement(element, &child);
        while (child) {
            inspectElement(child, tree, item);
            IUIAutomationElement *next;
            walker->GetNextSiblingElement(child, &next);
            child->Release();
            child = next;
        }
        walker->Release();
    }
}

QString UIAInspector::getControlTypeName(CONTROLTYPEID controlType)
{
    switch (controlType) {
    case UIA_ButtonControlTypeId:
        return "Button";
    case UIA_CheckBoxControlTypeId:
        return "CheckBox";
    case UIA_ComboBoxControlTypeId:
        return "ComboBox";
    case UIA_EditControlTypeId:
        return "Edit";
    case UIA_ListControlTypeId:
        return "List";
    case UIA_MenuControlTypeId:
        return "Menu";
    case UIA_RadioButtonControlTypeId:
        return "RadioButton";
    case UIA_TextControlTypeId:
        return "Text";
    case UIA_WindowControlTypeId:
        return "Window";
    default:
        return "Unknown";
    }
}

QString UIAInspector::getElementRect(IUIAutomationElement *element)
{
    RECT rect;
    element->get_CurrentBoundingRectangle(&rect);
    return QString("(%1,%2,%3,%4)").arg(rect.left).arg(rect.top).arg(rect.right).arg(rect.bottom);
}

QString UIAInspector::getElementStates(IUIAutomationElement *element)
{
    QStringList states;

    BOOL value;
    element->get_CurrentIsEnabled(&value);
    if (!value)
        states << "Disabled";

    element->get_CurrentIsOffscreen(&value);
    if (value)
        states << "Offscreen";

    element->get_CurrentHasKeyboardFocus(&value);
    if (value)
        states << "Focused";

    return states.join(", ");
}
