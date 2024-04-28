#pragma once

#include "ResourceIds.hpp"
#include "DGModule.hpp"
#include "UCModule.hpp"
#include "DGPanel.hpp"
#include "ACAPinc.h"
#include "json.hpp"

#include "IElementDataExporter.h"

#include <vector>
#include <memory>
#include <string>
#include <set>

class ExportDialog: public DG::ModalDialog,
					public DG::PanelObserver,
					public DG::ButtonItemObserver,
					public DG::ListBoxObserver,
					public DG::UserControlObserver,
					public DG::CompoundItemObserver
{
private:

	enum Control : short
	{
		DialogResourceId = ID_EXPORT_DLG,
		ExportButtonId = 1,
		CancelButtonId = 2,
		PopupId = 3,
		ListBoxId = 4
	};

	enum TabId
	{		
		ElementTypeTab = 1,
		ElementCountTab = 2,
		StatusTab = 3,
		PopupTab = 4
	};

	struct ExportListItem
	{
		API_ElemTypeID elementType;
		int elementCount;
		bool exportStatus;
	};

	std::vector<ExportListItem> exportListItems;

	static const short tabCount = PopupTab;
	static inline const unsigned int statusPopupWidth = 100;
	static inline const unsigned int statusPopupColumnNumber = 1;

	DG::Button exportButton;
	DG::Button cancelButton;
	DG::MultiSelListBox exportList;
	UC::TextPopup exportStatusPopup;

	IElementDataExporter* elementDataExporter;

public:

	ExportDialog(const std::map<API_ElemTypeID, int>& elementTypes, IElementDataExporter* exporter);
	~ExportDialog();
	
protected:

	void PanelIdle(const DG::PanelIdleEvent& ev) override;
	void PanelResized(const DG::PanelResizeEvent& ev) override;
	void ButtonClicked(const DG::ButtonClickEvent& ev) override;
	void UserControlChanged(const DG::UserControlChangeEvent& ev) override;
	void ListBoxSelectionChanged(const DG::ListBoxSelectionEvent& ev) override;

	void UpdateListHeader();
	void InitPopupControl();
	void SetPopUpAfterSelection();
	void RebuildListContent();

private:

	std::set<API_ElemTypeID> GetInclusionFilter();
	void Export();
};
