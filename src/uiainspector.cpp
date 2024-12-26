#include "uiainspector.h"
#include <QDebug>
#include <QElapsedTimer>

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
    QElapsedTimer totalTimer;
    totalTimer.start();

    if (!element || !m_automation)
        return;

    QElapsedTimer createTimer;
    createTimer.start();

    IUIAutomationCondition *visibleCondition = nullptr;
    VARIANT                 varProp;
    varProp.vt = VT_BOOL;
    varProp.boolVal = VARIANT_FALSE; // NOT IsOffscreen
    m_automation->CreatePropertyCondition(UIA_IsOffscreenPropertyId, varProp, &visibleCondition);

    qint64 createTime = createTimer.elapsed();
    qDebug() << "create time:" << createTime << "ms";

    QElapsedTimer findAllTimer;
    findAllTimer.start();

    IUIAutomationElementArray *elements = nullptr;
    HRESULT hr = element->FindAll(TreeScope_Subtree, visibleCondition, &elements);
    visibleCondition->Release();

    qint64 findAllTime = findAllTimer.elapsed();
    qDebug() << "FindAll time:" << findAllTime << "ms";

    if (FAILED(hr) || !elements)
        return;

    int count = 0;
    elements->get_Length(&count);
    qDebug() << "count:" << count;

    QElapsedTimer loopTimer;
    loopTimer.start();

    for (int i = 0; i < count; i++) {
        IUIAutomationElement *childElement = nullptr;
        if (SUCCEEDED(elements->GetElement(i, &childElement)) && childElement) {
            RECT rect;
            childElement->get_CurrentBoundingRectangle(&rect);

            CONTROLTYPEID controlType;
            childElement->get_CurrentControlType(&controlType);

            BSTR name;
            childElement->get_CurrentName(&name);
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
            item->setText(2, getElementRect(childElement));
            item->setText(3, getElementStates(childElement));
            if (rect.right > rect.left && rect.bottom > rect.top && pt.x >= rect.left
                && pt.x <= rect.right && pt.y >= rect.top && pt.y <= rect.bottom) {
                qDebug() << elementName << rect.left << rect.top << rect.right << rect.bottom;
            }
            childElement->Release();
        }
    }

    qint64 loopTime = loopTimer.elapsed();
    qDebug() << "Loop time:" << loopTime << "ms";

    elements->Release();

    qint64 totalTime = totalTimer.elapsed();
    qDebug() << "Total time:" << totalTime << "ms";
}

void UIAInspector::quickInspect(const POINT &pt, QTreeWidget *tree)
{
    if (!m_automation || !tree)
        return;

    QElapsedTimer timer;
    timer.start();

    IUIAutomationElement *element = nullptr;
    HRESULT hr = m_automation->ElementFromPoint(pt, &element);

    if (SUCCEEDED(hr) && element) {
        tree->clear();
        QTreeWidgetItem *item = new QTreeWidgetItem(tree);
        addElementToTree(element, tree, item);
        element->Release();
    }

    qint64 elapsed = timer.elapsed();
    qDebug() << "Quick inspect time:" << elapsed << "ms";
}

void UIAInspector::addElementToTree(IUIAutomationElement *element,
                                    QTreeWidget          *tree,
                                    QTreeWidgetItem      *item)
{
    if (!element || !item)
        return;

    CONTROLTYPEID controlType;
    element->get_CurrentControlType(&controlType);

    BSTR name;
    element->get_CurrentName(&name);
    QString elementName = QString::fromWCharArray(name ? name : L"");
    SysFreeString(name);

    RECT rect;
    element->get_CurrentBoundingRectangle(&rect);

    item->setText(0, getControlTypeName(controlType));
    item->setText(1, elementName);
    item->setText(2, getElementRect(element));
    item->setText(3, getElementStates(element));
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
