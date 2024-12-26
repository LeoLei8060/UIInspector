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

void UIAInspector::inspectWindow(HWND hwnd, QTreeWidget *tree, const POINT &pt)
{
    if (!m_automation || !hwnd || !tree)
        return;

    IUIAutomationElement *element = nullptr;
    HRESULT               hr = m_automation->ElementFromHandle(hwnd, &element);
    if (SUCCEEDED(hr) && element) {
        inspectElement(element, tree, nullptr, pt);
        element->Release();
    }
}

void UIAInspector::inspectElement(IUIAutomationElement *element,
                                  QTreeWidget          *tree,
                                  QTreeWidgetItem      *parentItem,
                                  const POINT          &pt)
{
    if (!element)
        return;

    RECT rect;
    element->get_CurrentBoundingRectangle(&rect);

    if (rect.right <= rect.left || rect.bottom <= rect.top) {
        return;
    }

    //        RECT screenRect;
    //        SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);
    //        if (rect.right < screenRect.left || rect.left > screenRect.right || rect.bottom < screenRect.top
    //            || rect.top > screenRect.bottom) {
    //            return;
    //        }
    //    if (pt.x < rect.left || pt.x > rect.right || pt.y < rect.top || pt.y > rect.bottom)
    //        return;

    CONTROLTYPEID controlType;
    element->get_CurrentControlType(&controlType);

    BSTR name;
    element->get_CurrentName(&name);

    QString elementName = QString::fromWCharArray(name ? name : L"");
    SysFreeString(name);

    QTreeWidgetItem *item;
#if 1 // opt
    if (parentItem) {
        item = new QTreeWidgetItem(parentItem);
    } else {
        item = new QTreeWidgetItem(tree);
    }

    item->setText(0, getControlTypeName(controlType));
    item->setText(1, elementName);
    item->setText(2, getElementRect(element));
    item->setText(3, getElementStates(element));
#endif
    IUIAutomationTreeWalker *walker;
    m_automation->get_ControlViewWalker(&walker);
    if (walker) {
        IUIAutomationElement *child;
        walker->GetFirstChildElement(element, &child);
        while (child) {
            inspectElement(child, tree, item, pt);
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
    case UIA_CalendarControlTypeId:
        return "Calendar";
    case UIA_CheckBoxControlTypeId:
        return "CheckBox";
    case UIA_ComboBoxControlTypeId:
        return "ComboBox";
    case UIA_EditControlTypeId:
        return "Edit";
    case UIA_HyperlinkControlTypeId:
        return "Hyperlink";
    case UIA_ImageControlTypeId:
        return "Image";
    case UIA_ListItemControlTypeId:
        return "ListItem";
    case UIA_ListControlTypeId:
        return "List";
    case UIA_MenuControlTypeId:
        return "Menu";
    case UIA_MenuBarControlTypeId:
        return "MenuBar";
    case UIA_MenuItemControlTypeId:
        return "MenuItem";
    case UIA_ProgressBarControlTypeId:
        return "ProgressBar";
    case UIA_RadioButtonControlTypeId:
        return "RadioButton";
    case UIA_ScrollBarControlTypeId:
        return "ScrollBar";
    case UIA_SliderControlTypeId:
        return "Slider";
    case UIA_SpinnerControlTypeId:
        return "Spinner";
    case UIA_StatusBarControlTypeId:
        return "StatusBar";
    case UIA_TabControlTypeId:
        return "Tab";
    case UIA_TabItemControlTypeId:
        return "TabItem";
    case UIA_TextControlTypeId:
        return "Text";
    case UIA_ToolBarControlTypeId:
        return "ToolBar";
    case UIA_ToolTipControlTypeId:
        return "ToolTip";
    case UIA_TreeControlTypeId:
        return "Tree";
    case UIA_TreeItemControlTypeId:
        return "TreeItem";
    case UIA_WindowControlTypeId:
        return "Window";
    case UIA_DocumentControlTypeId:
        return "Document";
    case UIA_GroupControlTypeId:
        return "Group";
    default:
        return QString("Unknown%1").arg((int) controlType);
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
