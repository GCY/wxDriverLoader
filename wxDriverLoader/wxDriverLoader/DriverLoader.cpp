#include "DriverLoader.h"

DECLARE_APP(App)
IMPLEMENT_APP(App)

bool App::OnInit()
{
	Frame *frame = new Frame(wxT("wxDriverLoader"),wxSize(600,400));

	frame->Show(true);

	return true;
}

Frame::Frame(const wxString &title,const wxSize &size):wxFrame(NULL,wxID_ANY,title,wxDefaultPosition,size,wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION | wxSYSTEM_MENU)
{
	CreateUI();
}

void Frame::CreateUI()
{
	wxMenu *file = new wxMenu;
	file->Append(wxID_OPEN,wxT("O&pen\tAlt-o"),wxT("Open Driver"));
	file->AppendSeparator();
	file->Append(ID_START,wxT("S&tart\tAlt-r"),wxT("Start Driver"));
	file->Append(ID_STOP,wxT("S&top\tAlt-s"),wxT("Stop Driver"));
	file->AppendSeparator();
	file->Append(wxID_EXIT,wxT("E&xit\tAlt-q"),wxT("Exit"));

	wxMenuBar *bar = new wxMenuBar;
	bar->Append(file,wxT("file"));

	wxToolBar *tool = new wxToolBar(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTB_HORIZONTAL | wxNO_BORDER);
	
	wxBitmap open_bmp(fileopen_xpm);
	wxBitmap start_bmp(start_xpm);
	wxBitmap stop_bmp(stop_xpm);

	tool->AddTool(wxID_OPEN,open_bmp,wxT("Open Driver"));
	tool->AddTool(ID_START,start_bmp,wxT("Start Driver"));
	tool->AddTool(ID_STOP,stop_bmp,wxT("Stop Driver"));
	tool->Realize();
	SetToolBar(tool);

	msg = new MSGList(this);

	SetMenuBar(bar);
	CreateStatusBar(2);
}

void Frame::OnOpenDriver(wxCommandEvent &event)
{
	msg->AddMessage(wxT("********************Driver Select¡I********************"));

	wxFileDialog dlg(this,wxT("Select File"),wxT("."),wxT("*.sys"),wxT("SYS files (*.sys)|*.sys"),wxOPEN);
	
	if(dlg.ShowModal() == wxID_OK){
		name = dlg.GetFilename();
		path = dlg.GetPath();
		wxString str;
		str.Printf(wxT("Open File %s Success¡I"),name);
		msg->AddMessage(str);
	}
	else{
		msg->AddMessage(wxT("Open File Fail¡I"));
	}
}

void Frame::OnStartDriver(wxCommandEvent &event)
{
	msg->AddMessage(wxT("********************Load Driver¡I********************"));

	DWORD error = 0;

	SC_HANDLE mgr = NULL;

	mgr = OpenSCManagerW(NULL,NULL,SC_MANAGER_ALL_ACCESS);

	if(mgr == NULL){
		wxString str;
		str.Printf(wxT("OpenSCManager Error¡G%d"),error);
		msg->AddMessage(str);
		CloseServiceHandle(mgr);
		return;
	}
	else{
		msg->AddMessage(wxT("OpenSCManager Success¡I"));
	}

	SC_HANDLE ddk = NULL;

	ddk = CreateServiceW(mgr,name.wc_str(),name.wc_str(),SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,SERVICE_DEMAND_START,SERVICE_ERROR_NORMAL,
		path.wc_str(),NULL,NULL,NULL,NULL,NULL);

	if(ddk ==NULL){
		error = GetLastError();

		if((error != ERROR_IO_PENDING) && (error != ERROR_SERVICE_EXISTS)){
			wxString str;
			str.Printf(wxT("CreateService Error¡G%d"),error);
			msg->AddMessage(str);
			CloseServiceHandle(mgr);
			return;
		}

		ddk = OpenService(mgr,name.wc_str(),SERVICE_ALL_ACCESS);
		if(ddk == NULL){
			error = GetLastError();
			wxString str;
			str.Printf(wxT("OpenService Error Code¡G%d"),error);
			msg->AddMessage(str);
			CloseServiceHandle(mgr);
			return;
		}
		else{
			msg->AddMessage(wxT("OpenService Success¡I"));
		}
	}
	else{
		msg->AddMessage(wxT("CreateService Success¡I"));
	}

	error = StartServiceW(ddk,NULL,NULL);

	if(!error){
		error = GetLastError();

		if((error != ERROR_IO_PENDING) && (error != ERROR_SERVICE_EXISTS)){
			wxString str;
			str.Printf(wxT("StartService Error¡G%d"),error);
			msg->AddMessage(str);
			CloseServiceHandle(mgr);
			CloseServiceHandle(ddk);
			return;
		}
		else{
			if(error == ERROR_IO_PENDING){
				msg->AddMessage(wxT("StartService ERROR_IO_PENDING¡I"));
				CloseServiceHandle(mgr);
				CloseServiceHandle(ddk);
				return;
			}
			else{
				msg->AddMessage(wxT("StartService Fail ERROR_SERVICE_ALREADY_RUNNING¡I"));
				CloseServiceHandle(mgr);
				CloseServiceHandle(ddk);
				return;
			}
		}
	}
	else{
		msg->AddMessage(wxT("StartService Success¡I"));
	}

	if(mgr){
		CloseServiceHandle(mgr);
	}
	if(ddk){
		CloseServiceHandle(ddk);
	}
}

void Frame::OnStopDriver(wxCommandEvent &event)
{
	msg->AddMessage(wxT("********************Unload Driver¡I********************"));

	DWORD error = 0;

	SC_HANDLE mgr = NULL;

	mgr = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);

	if(mgr == NULL){
		wxString str;
		str.Printf(wxT("OpenSCManager Error¡G%d"),error);
		msg->AddMessage(str);
		return;
	}
	else{
		msg->AddMessage(wxT("OpenSCManager Success¡I"));
	}

	SC_HANDLE ddk = NULL;

	ddk = OpenService(mgr,name.wc_str(),SERVICE_ALL_ACCESS);
	
	if(ddk == NULL){
		error = GetLastError();
		wxString str;
		str.Printf(wxT("OpenService Error Code¡G%d"),error);
		msg->AddMessage(str);
		CloseServiceHandle(mgr);
		return;
	}
	else{
		msg->AddMessage(wxT("OpenService Success¡I"));
	}

	SERVICE_STATUS status;

	if(!ControlService(ddk,SERVICE_CONTROL_STOP,&status)){
		error = GetLastError();
		wxString str;
		str.Printf(wxT("ControlService Error Code¡G%d"),error);
		msg->AddMessage(str);
	}
	else{
		msg->AddMessage(wxT("ControlService Success¡I"));
	}

	if(!DeleteService(ddk)){
		error = GetLastError();
		wxString str;
		str.Printf(wxT("DeleteService Error Code¡G%d"),error);
		msg->AddMessage(str);
	}
	else{
		msg->AddMessage(wxT("DeleteService Success¡I"));
	}

	if(mgr){
		CloseServiceHandle(mgr);
	}
	if(ddk){
		CloseServiceHandle(ddk);
	}
}

void Frame::OnExit(wxCommandEvent &event)
{
	Close();
}

MSGList::MSGList(wxWindow *parent):wxListCtrl(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxLC_REPORT | wxLC_SINGLE_SEL)
{
	item_num = 0;

	wxListItem item;
	item.SetText(wxT("Message"));
	InsertColumn(COLUMN_0,item);
	SetColumnWidth(COLUMN_0,600);
}

void MSGList::AddMessage(const wxString &msg)
{
	InsertItem(item_num,msg,COLUMN_0);
	++item_num;
}