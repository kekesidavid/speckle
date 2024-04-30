#include "ExportDialog.h"
#include "ElementTypeToStringConverter.h"
#include "ElementDataManager.h"

#include <string>


ExportDialog::ExportDialog(const std::map<API_ElemTypeID, int>& elementTypes, IElementDataExporter* exporter):
	DG::ModalDialog(ACAPI_GetOwnResModule(), DialogResourceId, ACAPI_GetOwnResModule()),
	exportButton(GetReference(), ExportButtonId),
	cancelButton(GetReference(), CancelButtonId),
	exportList(GetReference(), ListBoxId),
	exportStatusPopup(GetReference(), PopupId),
	elementDataExporter(exporter)
{
	exportButton.Attach(*this);
	cancelButton.Attach(*this);
	exportList.Attach(*this);
	Attach(*this);
	AttachToAllItems(*this);

	for (const auto& pair : elementTypes)
	{
		exportListItems.push_back({ pair.first, pair.second, true });
	}

	UpdateListHeader();
	InitPopupControl();
	RebuildListContent();
}


ExportDialog::~ExportDialog()
{
	Detach(*this);
}

static std::string ConvertBoolToStatus(bool b)
{
	if (b)
	{
		return "yes";
	}
	else
	{
		return "no";
	}
}


void ExportDialog::PanelIdle(const DG::PanelIdleEvent& /*ev*/)
{

}


void ExportDialog::PanelResized(const DG::PanelResizeEvent& ev)
{
	short hGrow = ev.GetHorizontalChange();
	short vGrow = ev.GetVerticalChange();

	if (hGrow != 0 || vGrow != 0)
	{
		BeginMoveResizeItems();

		exportList.Resize(hGrow, vGrow);
		exportButton.Move(hGrow, vGrow);
		cancelButton.Move(hGrow, vGrow);

		EndMoveResizeItems();
	}

	UpdateListHeader();
}


void ExportDialog::ButtonClicked(const DG::ButtonClickEvent& ev)
{
	if (ev.GetSource() == &exportButton)
	{
		Export();
	}
	else if (ev.GetSource() == &cancelButton)
	{
		PostCloseRequest(Cancel);
	}
}

void ExportDialog::UserControlChanged(const DG::UserControlChangeEvent& ev)
{
	if (ev.GetSource() == &exportStatusPopup)
	{
		short selectedRowsCount = exportList.GetSelectedCount();
		short selectedItemIndex = 0;
		bool newValue = (exportStatusPopup.GetValue() - 1);
		exportStatusPopup.SetValue(0);

		for (short i = 0; i < selectedRowsCount; i++)
		{
			selectedItemIndex = exportList.GetSelectedItem(selectedItemIndex + 1);
			auto& selectedItem = exportListItems[selectedItemIndex - 1];	
			selectedItem.exportStatus = newValue;
		}

		RebuildListContent();
	}
}

void ExportDialog::UpdateListHeader()
{
	const short width = exportList.GetItemWidth() - statusPopupWidth;
	const short tabWidth = width / (tabCount-1);

	exportList.SetTabFieldCount(tabCount);
	exportList.SetHeaderSynchronState(false);
	exportList.SetHeaderItemSize(ElementTypeTab, tabWidth);
	exportList.SetHeaderItemSize(ElementCountTab, tabWidth);
	exportList.SetHeaderItemSize(StatusTab, tabWidth);
	exportList.SetHeaderItemSize(PopupTab, statusPopupWidth);

	short pos = 0;
	exportList.SetTabFieldProperties(ElementTypeTab, pos, pos + tabWidth, DG::ListBox::Left, DG::ListBox::NoTruncate, false);
	pos += tabWidth;
	exportList.SetTabFieldProperties(ElementCountTab, pos, pos + tabWidth, DG::ListBox::Left, DG::ListBox::NoTruncate, false);
	pos += tabWidth;
	exportList.SetTabFieldProperties(StatusTab, pos, pos + tabWidth, DG::ListBox::Left, DG::ListBox::NoTruncate, false);
	pos += tabWidth;
	exportList.SetTabFieldProperties(PopupTab, pos, pos + statusPopupWidth, DG::ListBox::Left, DG::ListBox::NoTruncate, false);

	exportList.SetHeaderItemText(ElementTypeTab, "Element Type");
	exportList.SetHeaderItemText(ElementCountTab, "Element Count");
	exportList.SetHeaderItemText(StatusTab, "Export");
	exportList.SetHeaderItemText(PopupTab, "");
}

void ExportDialog::InitPopupControl()
{
	const short width = exportList.GetItemWidth();
	const short tabWidth = width / tabCount;

	exportStatusPopup.Initialize("no", statusPopupWidth, statusPopupColumnNumber);
	exportStatusPopup.AppendItem("no");
	exportStatusPopup.AppendItem("yes");
	exportStatusPopup.Hide();
}

void ExportDialog::SetPopUpAfterSelection()
{
	exportStatusPopup.SetValue(0);
	exportList.SetOnTabItem(PopupTab, exportStatusPopup);
}

void ExportDialog::RebuildListContent()
{
	exportList.DisableDraw();
	if (exportList.GetItemCount() != 0)
	{
		exportList.DeleteItem(DG::ListBox::AllItems);
	}
	
	short i = 1;
	for (const auto& listItem : exportListItems)
	{
		ElementTypeToStringConverter converter;
		std::string elemTypeString = converter.ElementTypeToString(listItem.elementType);
		std::string elemTypeCountString = std::to_string(listItem.elementCount);

		exportList.AppendItem();
		exportList.SetTabItemText(i, ElementTypeTab, elemTypeString.c_str());
		exportList.SetTabItemText(i, ElementCountTab, elemTypeCountString.c_str());
		exportList.SetTabItemText(i, StatusTab, ConvertBoolToStatus(listItem.exportStatus).c_str());
		i++;
	}

	exportList.EnableDraw();
	exportList.Redraw();
}

void ExportDialog::ListBoxSelectionChanged(const DG::ListBoxSelectionEvent& ev)
{
	if (ev.GetSource() == &exportList)
	{
		if (exportList.GetSelectedItem() > 0)
		{
			SetPopUpAfterSelection();
		}
	}
}

std::set<API_ElemTypeID> ExportDialog::GetInclusionFilter()
{
	std::set<API_ElemTypeID> inclusionFilter;

	for (auto item : exportListItems)
	{
		if (item.exportStatus)
		{
			inclusionFilter.insert(item.elementType);
		}
	}

	return inclusionFilter;
}

void ExportDialog::Export()
{
	ElementDataManager elementDataManager;
	const auto& inclusionFilter = GetInclusionFilter();
	const auto& selectedElementIds = elementDataManager.GetSelectedElemetIds(inclusionFilter);
	const auto& elementJsonData = elementDataManager.GetElementDataListAsJson(selectedElementIds);

	if (elementDataExporter != nullptr)
	{
		elementDataExporter->Export(elementJsonData);
	}
	else
	{
		// TODO notify user and log this
	}
}