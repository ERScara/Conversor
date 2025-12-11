#pragma once
#include "Conversor.h"
#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Data.dll>
#using <System.Windows.Forms.dll>

using namespace System;
using namespace System::Drawing;
using namespace System::Data;
using namespace System::Data::SqlClient;
using namespace System::Resources;
using namespace System::Reflection;
using namespace System::Threading;
using namespace System::IO;
using namespace System::Text;
using namespace System::Windows::Forms;
using namespace System::Globalization;
using namespace System::Runtime::InteropServices;

const unsigned int MB_OKCANCEL = 0x00000001;
const unsigned int MB_ICONINFORMATION = 0x00000040;
const unsigned int MB_ICONWARNING = 0x00000030;
const int IDOK = 1;

ref class DataManager {
private:
	String^ connectionString = L"Server=(LocalDB)\\MSSQLLocalDB;" L"Database=Conversor;" L"AttachDbFilename=C:\\Users\\Esteban\\source\\repos\\NewProjects\\WebServices\\ConversorWebService\\Conversor.mdf;"  L"Integrated Security = True;";
	void CreateTable(SqlConnection^ conn) {
		String^ sql =
			L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='Conversions')" +
			L"CREATE TABLE Conversions (" +
			L"     ID INT IDENTITY(1, 1)  PRIMARY KEY," +
			L"     Fahrenheit DECIMAL(18, 2) NOT NULL," +
			L"     Celsius    DECIMAL(18, 2) NOT NULL" +
			L");";
		SqlCommand^ cmd = gcnew SqlCommand(sql, conn);
		cmd->ExecuteNonQuery();
	}
public:
	DataManager() {
		try {
			if (!System::IO::File::Exists("C:\\Users\\Esteban\\source\\repos\\NewProjects\\WebServices\\ConversorWebService\\Conversor.mdf")) {
				throw gcnew Exception("Database file not found at specified path.");
			}
			else if (connectionString == nullptr || connectionString == "") {
				throw gcnew Exception("Connection string is null.");
			}
		}
		catch (Exception^ ex) {
			System::Windows::Forms::DialogResult result = MessageBox::Show("Error initializing DataManager: " + ex->Message, "Data Manager Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
		}
	}
	void InsertResult(double fahrenheit, double celsius) {
		SqlConnection^ conn = gcnew SqlConnection(connectionString);
		try {
			conn->Open();
			String^ data =
				L"INSERT INTO Conversions (Fahrenheit, Celsius) " +
				L"VALUES (@F, @C)";
			SqlCommand^ cmd = gcnew SqlCommand(data, conn);

			cmd->Parameters->AddWithValue(L"@F", fahrenheit);
			cmd->Parameters->AddWithValue(L"@C", celsius);
			cmd->ExecuteNonQuery();
		}
		catch (Exception^ ex) {
			throw gcnew Exception(L"Error inserting into table: " + ex->Message);
		}
		__finally {
			if (conn != nullptr && conn->State == ConnectionState::Open) conn->Close();
		}
	}
	void DeleteAllResults() {
		SqlConnection^ conn = gcnew SqlConnection(connectionString);
		try {
			conn->Open();
			String^ sql = L"DELETE FROM Conversions;";
			SqlCommand^ cmd = gcnew SqlCommand(sql, conn);
			cmd->ExecuteNonQuery();
		}
		catch (Exception^ ex) {
			throw gcnew Exception(L"Error deleting all results: " + ex->Message);
		}
		__finally {
			if (conn != nullptr && conn->State == ConnectionState::Open) conn->Close();
		}
	}
	DataTable^ GetHistory() {
		SqlConnection^ conn = gcnew SqlConnection(connectionString);
		DataTable^ dataTable = gcnew DataTable();
		try {
			conn->Open();
			String^ selectSql = L"SELECT Fahrenheit, Celsius FROM Conversions ORDER BY ID ASC;";
			SqlDataAdapter^ adapter = gcnew SqlDataAdapter(selectSql, conn);
			adapter->Fill(dataTable);
			return dataTable;
		} catch (Exception^ ex) {
			throw gcnew Exception(L"Error obtaining data history from Conversions:" + ex->Message);
		}
		__finally {
			if (conn != nullptr && conn->State == ConnectionState::Open) conn->Close();
		}
		return dataTable;
	}
	
	String^ GetConnectionString() {
		return connectionString;
	}
	void ImportCSVtoSQL(String^ csvPath) {
		SqlConnection^ conn = gcnew SqlConnection(connectionString);
		try {
			conn->Open();

			StreamReader^ sr = gcnew StreamReader(csvPath);

			String^ headerLine = sr->ReadLine();
			while (!sr->EndOfStream) {
				String^ line = sr->ReadLine();
				array<String^>^ values = line->Split(',');

				String^ query = L"INSERT INTO Conversions (Fahrenheit, Celsius) " +
					L"VALUES (@F, @C)";
				SqlCommand^ cmd = gcnew SqlCommand(query, conn);

				cmd->Parameters->AddWithValue(L"@F", values[0]);
				cmd->Parameters->AddWithValue(L"@C", values[1]);
				cmd->ExecuteNonQuery();
			}
			sr->Close();
			conn->Close();

			MessageBox::Show("Data imported successfully");
		}
		catch (Exception^ ex) {
			MessageBox::Show("Error importing: " + ex->Message);
		}
	}
};

ref class TextExcept {
public:
	delegate void ErrorMessage(System::String^ errorMessage);
	event ErrorMessage^ ErrorMess;
	double OnString(System::String^ inputString)
	{
		try {
			return System::Convert::ToDouble(inputString, CultureInfo::InvariantCulture);
		}
		catch (System::FormatException^ ex)
		{
			ex;
			ErrorMess(L"Error! Cannot convert text to number. Please enter a number.");
		}
		return 0.0;
	}
};

public ref class CSV {
public: 
	static void WriteDataTableToCSV(DataTable^ dataTable, String^ filePath) {
		StreamWriter^ sw = gcnew StreamWriter(filePath, false, Encoding::UTF8);
		try {
			for each (DataRow ^ row in dataTable->Rows) {
				for (int i = 0; i < dataTable->Columns->Count; i++) {
					String^ value = row[i]->ToString();
					if (value->Contains(",") || value->Contains("\"")) {
						value = "\"" + value->Replace("\"", "\"\"") + "\"";
					}
					sw->Write(value);
					if (i < dataTable->Columns->Count - 1) sw->Write(",");
				}
				sw->WriteLine();
			}
			sw->Close();
		}
		catch (Exception^ ex) {
			throw gcnew Exception("Error writing file to CSV: " + ex->Message);
		}
		__finally {
			if (sw != nullptr) {
				sw->Close();
			}
		}
	}
};


public ref class AppSettings {
public: 
	static String^ LanguageCode = "en-ES";
	static void SaveLanguage(String^ code) {
		LanguageCode = code;
	}
	static String^ LoadLanguage() {
		return LanguageCode;
	}
	ResourceManager^ resManager;
	Void InitializeMenu() { 
		if (resManager == nullptr) {
			resManager = gcnew ResourceManager("ConversorWebService.Español", Assembly::GetExecutingAssembly());
		}
	}
};

public ref class NativeMethods abstract sealed
{
public:
	[DllImport("User32.dll", CharSet = CharSet::Auto)]
		static int MessageBox(IntPtr hw, String^ text, String^ caption, unsigned int type);
};


ref class ConvForm :public Form
{
public:
	ConvForm();
	void InitForm();
	void InitToolTips();
	void Setup_StatusBar();
	void Setup_Menu();
	void Setup_ToolBar();
	void Setup_Combo();
	void Setup_Radio();
	void LoadResults();
private:
	TextExcept^ except;
	GroupBox^ grpBox;
	GroupBox^ highBox;
	Label^ lblGF;
	Label^ lblGC;
	TextBox^ txtGF;
	TextBox^ txtGC;
	System::Drawing::Font^ defaultFont;
	System::Drawing::Font^ boldFont;
	Label^ lblUnitF;
	Label^ lblUnitC;
	Button^ btnCalculate;
	void ConvertirClick(Object^ pSender, EventArgs^ Args);
	MainMenu^ menuBar;
	MenuStrip^ menuStrip;
	MenuItem^ fileMenu;
	MenuItem^ settingsMenu;
	MenuItem^ submenu1;
	MenuItem^ Item1_1;
	MenuItem^ Item1_2;
	MenuItem^ Item1;
	MenuItem^ Item2;
	ToolTip^ dialog;
	Button^ Exprtload;
	Button^ OKButton;
	ResourceManager^ resManager;
	Label^ label1;
	Label^ label2;
	System::Windows::Forms::Timer^ refreshTimer;
	void OnRefreshTimerTick(Object^ pSender, EventArgs^ Args) {
		DataManager^ dm = gcnew DataManager();
		DataTable^ dataTable = dm->GetHistory();
		dgvHistory->DataSource = dataTable;
	}
	void OKButton_Cliked(Object^ pSender, EventArgs^ Args);
	void MenuItem_About_Click(Object^ pSender, EventArgs^ Args);
	void MenuItem_Exit_Click(Object^ pSender, EventArgs^ Args);
	void MenuItem_English_Click(Object^ pSender, EventArgs^ Args);
	void MenuItem_Spanish_Click(Object^ pSender, EventArgs^ Args);
	void Export_Click(Object^ pSender, EventArgs^ Args);
	void ConvForm_FormClosing(Object^ pSender, FormClosingEventArgs^ Args) {
		if (!ConfirmExit()) {
			Args->Cancel = true;
		}
	}
	void textGC_Leave(Object^ pSender, EventArgs^ Args);
	void textGF_Leave(Object^ pSender, EventArgs^ Args);
	bool ConfirmExit() {
		String^ theText = L"Are you sure you want to exit?";
		String^ theCaption = L"Exiting...";

		int result = NativeMethods::MessageBox(IntPtr::Zero, theText, theCaption, MB_OKCANCEL | MB_ICONINFORMATION);

		return (result == IDOK);
	}
	bool ConfirmDeleteTable() {
		String^ theText = L"Are you sure you want to delete \nall the results from the table?";
		String^ theCaption = L"Confirm Deletion";

		int result = NativeMethods::MessageBox(IntPtr::Zero, theText, theCaption, MB_OKCANCEL | MB_ICONWARNING);

		return (result == IDOK);
	}
	Button^ btnClose;
	void CloseClick(Object^ pSender, EventArgs^ Args);

	void OpenButtonClick(Object^ pSender, ToolBarButtonClickEventArgs^ Args) {
		OpenFileDialog^ openFile = gcnew OpenFileDialog();
		openFile->Filter = "CSV files (*.csv)|*.csv";
		openFile->Title = "Select a File to Open";
		if (openFile->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			String^ csvPath = openFile->FileName;
			DataManager^ dm = gcnew DataManager();
			dm->ImportCSVtoSQL(csvPath);
		}
	}

	void ExporttoCSV() 
	{
		SaveFileDialog^ saveFileDialog = gcnew SaveFileDialog();
		saveFileDialog->Filter = "CSV files (*.csv)|*.csv";
		saveFileDialog->Title = "Save table Conversions";
		saveFileDialog->DefaultExt = "csv";

		if (saveFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			DataManager^ dm = gcnew DataManager;
			DataTable^ dataTable = dm->GetHistory();
			String^ filePath = saveFileDialog->FileName;
			CSV::WriteDataTableToCSV(dataTable, filePath);
			MessageBox::Show("Exported Successfully!");
		}
	}

	void HandleCalculationError(System::String^ errorMessage)
	{
		MessageBox::Show(errorMessage, L"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
	ComboBox^ combo1;
	Label^ l1;
	Label^ labs;
	Label^ tablel;
	RadioButton^ radioC;
	RadioButton^ radioF;
	RadioButton^ RNone;
	ToolBar^ Tools;
	StatusBar^ status;
	StatusBarPanel^ status1;
	StatusBarPanel^ status2;
	CheckBox^ HighBoth;
	DataGridView^ dgvResults;
	Button^ DeleteData;
	DataManager^ dataManager;
	DataGridView^ dgvHistory;
	void DeleteResults_Click(Object^pSender, EventArgs^Args);
	void LoadHistory() {
		
		if (dataManager == nullptr) {
			dataManager = gcnew DataManager();
		}
		try {
			DataTable^ history = dataManager->GetHistory();
			if (dgvHistory != nullptr){
			   dgvHistory->DataSource = history;
			   dgvHistory->AutoResizeColumns(DataGridViewAutoSizeColumnsMode::AllCells);
			}
		}
		catch (Exception^ ex) {
			MessageBox::Show(L"Error loading history: ", ex->Message);
		}
	}
	void Event_ButtonClicked(Object^ pSender, ToolBarButtonClickEventArgs^ Args) {
		if (Args->Button->ToolTipText == L"Open a File") {
			OpenButtonClick(pSender, Args);
		}
        else if (Args->Button->ToolTipText == L"Export Data to CSV File") {
			Export_Click(pSender, Args);
		}
		else if (Args->Button->ToolTipText == L"Delete the Whole Table") {
			DeleteResults_Click(pSender, Args);
		}
	}
	void Setup_RadioChecked(Object^ pSender, EventArgs^ Args)
	{
		RadioButton^ rb = safe_cast<RadioButton^>(pSender);
		if (rb == radioC)
		{
			if(txtGC != nullptr) txtGC->Font = boldFont;
			if (txtGF != nullptr) txtGF->Font = defaultFont;
		}
		else if (rb == radioF) {
			if (txtGF != nullptr) txtGF->Font = boldFont;
			if (txtGC != nullptr) txtGC->Font = defaultFont;
		}
		else if (rb == RNone) {
			if (txtGF != nullptr) txtGF->Font = defaultFont;
			if (txtGC != nullptr) txtGC->Font = defaultFont;
		}
	}
	void Setup_Checked(Object^ pSender, EventArgs^ Args)
	{
		CheckBox^ cb = safe_cast<CheckBox^>(pSender);
		if (cb->Checked) {
			if (txtGC != nullptr) txtGC->Font = boldFont;
			if (txtGF != nullptr) txtGF->Font = boldFont;
		}
		else if (!cb->Checked) 
		{
			if (txtGF != nullptr) txtGF->Font = defaultFont;
			if (txtGC != nullptr) txtGC->Font = defaultFont;
		}
	}
};
