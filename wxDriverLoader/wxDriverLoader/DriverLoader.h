#ifndef __DRIVER_LOADER__
#define __DRIVER_LOADER__

#include <wx/wx.h>
#include <wx/listctrl.h>

#include <windows.h>

#include "fileopen.xpm"
#include "start.xpm"
#include "stop.xpm"

enum{
	ID_START = 100,
	ID_STOP
};

enum{
	COLUMN_0 = 0
};

class App:public wxApp
{
public:

	bool OnInit();
};

class MSGList:public wxListCtrl
{
public:

	MSGList(wxWindow*);

	void AddMessage(const wxString&);

private:

	int item_num;
};

class Frame:public wxFrame
{
public:

	Frame(const wxString&,const wxSize&);

	void OnOpenDriver(wxCommandEvent&);
	void OnStartDriver(wxCommandEvent&);
	void OnStopDriver(wxCommandEvent&);
	void OnExit(wxCommandEvent&);
private:

	void CreateUI();
	
	MSGList *msg;
	wxString name;
	wxString path;

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(Frame,wxFrame)
	EVT_MENU(wxID_OPEN,Frame::OnOpenDriver)
	EVT_MENU(wxID_EXIT,Frame::OnExit)
	EVT_MENU(ID_START,Frame::OnStartDriver)
	EVT_MENU(ID_STOP,Frame::OnStopDriver)
END_EVENT_TABLE()

#endif