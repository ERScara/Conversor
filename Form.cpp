#include "Form.h"
#include "Conversor.h"
#using <mscorlib.dll>

using namespace System;
using namespace Conversor;
using namespace System::Data;
using namespace System::Data::SqlClient;
using namespace System::Globalization;
using namespace System::Threading;
using namespace System::Drawing;
using namespace System::Windows::Forms;

ConvForm::ConvForm()
{
	InitForm();
    Setup_Menu();
    Setup_Combo();
    Setup_Radio();
    Setup_ToolBar();
    Setup_StatusBar();
    InitToolTips();
    except = gcnew TextExcept();
    dataManager = gcnew DataManager();
    except->ErrorMess += gcnew TextExcept::ErrorMessage(this, &ConvForm::HandleCalculationError);
    defaultFont = this->Font;
    boldFont = gcnew System::Drawing::Font(defaultFont, System::Drawing::FontStyle::Bold);
    LoadResults();
}

void ConvForm::InitForm()
{
	this->Text = L"Temperature Unit Converter";
	this->Size = Drawing::Size(410, 638);
    this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
    this->Icon = gcnew System::Drawing::Icon("Thermo.ico");
    this->BackColor = System::Drawing::Color::WhiteSmoke;
    this->FormClosing += gcnew FormClosingEventHandler(this, &ConvForm::ConvForm_FormClosing);
	this->MaximizeBox = false;
	this->MinimizeBox = true;
	this->StartPosition = FormStartPosition::CenterScreen;

	this->grpBox = gcnew GroupBox;
	this->grpBox->Location = Point(20, 30);
	this->grpBox->Size = Drawing::Size(210, 100);
	this->grpBox->Text = L"Converter";

    this->lblGF = gcnew Label;
    this->lblGF->Location = Point(16, 36);
    this->lblGF->AutoSize = true;
    this->lblGF->Text = L"&Fahrenheit: ";
    this->grpBox->Controls->Add(lblGF);

    this->txtGF = gcnew TextBox;
    this->txtGF->Location = Point(80, 32);
    this->txtGF->Font = gcnew System::Drawing::Font("Consolas", 9);
    this->txtGF->Text = L"";
    this->grpBox->Controls->Add(txtGF);

    this->lblGC = gcnew Label;
    this->lblGC->Location = Point(33, 68);
    this->lblGC->AutoSize = true;
    this->lblGC->Text = L"&Celsius: ";
    this->grpBox->Controls->Add(lblGC);

    this->txtGC = gcnew TextBox;
    this->txtGC->Location = Point(80, 64);
    this->txtGC->Font = gcnew System::Drawing::Font("Consolas", 9);
    this->txtGC->Text = L"";
    this->grpBox->Controls->Add(txtGC);

    this->lblUnitF = gcnew Label;
    this->lblUnitF->Location = Point(180, 36);
    this->lblUnitF->AutoSize = true;
    this->lblUnitF->Text = L" &F";
    this->grpBox->Controls->Add(lblUnitF);

    this->lblUnitC = gcnew Label;
    this->lblUnitC->Location = Point(180, 68);
    this->lblUnitC->AutoSize = true;
    this->lblUnitC->Text = L"&ºC";
    this->grpBox->Controls->Add(lblUnitC);

	this->Controls->Add(grpBox);

    this->btnCalculate = gcnew Button;
    this->btnCalculate->Location = Point(290, 50);
    this->btnCalculate->Text = L"&Convert";
    this->btnCalculate->BackColor = System::Drawing::Color::White;
    this->btnCalculate->Click += gcnew EventHandler(this, &ConvForm::ConvertirClick);
    this->btnCalculate->Cursor = Cursors::Hand;
    this->Controls->Add(btnCalculate);

    this->btnClose = gcnew Button;
    this->btnClose->Location = Point(290, 520);
    this->btnClose->Text = L"&Close";
    this->btnClose->BackColor = System::Drawing::Color::White;
    this->btnClose->Click += gcnew EventHandler(this, &ConvForm::CloseClick);
    this->btnClose->Cursor = Cursors::Hand;
    this->Controls->Add(btnClose);

    this->DeleteData = gcnew Button;
    this->DeleteData->Location = Point(290, 200);
    this->DeleteData->Size = Drawing::Size(90, 30);
    this->DeleteData->Text = L"Delete Data";
    this->DeleteData->BackColor = System::Drawing::Color::White;
    this->DeleteData->Click += gcnew EventHandler(this, &ConvForm::DeleteResults_Click);
    this->DeleteData->Cursor = Cursors::Hand;
    this->Controls->Add(DeleteData);

    this->tablel = gcnew Label;
    this->tablel->Font = gcnew System::Drawing::Font(L"Verdana", 9);
    this->tablel->Location = Point(140, 140);
	this->tablel->BorderStyle = BorderStyle::None;
	this->tablel->BackColor = System::Drawing::Color::White;
    this->tablel->Text = L"Data Table";
    this->tablel->AutoSize = true;
    this->Controls->Add(tablel);

    this->Icon = gcnew System::Drawing::Icon("Thermo.ico");

    this->Exprtload = gcnew Button;
    this->Exprtload->Location = Point(290, 150);
    this->Exprtload->Size = Drawing::Size(90, 30);
    this->Exprtload->Text = L"Export Data";
    this->Exprtload->BackColor = System::Drawing::Color::White;
    this->Exprtload->Click += gcnew EventHandler(this, &ConvForm::Export_Click);
    this->Exprtload->Cursor = Cursors::Hand;
    this->Controls->Add(Exprtload);

    this->status = gcnew StatusBar;
    this->status->ShowPanels = true;
    this->status->Dock = DockStyle::Bottom;
    this->Controls->Add(status);

    this->Tools = gcnew ToolBar;
	this->Tools->Appearance = ToolBarAppearance::Normal;
	this->Tools->AllowDrop = true;
    this->Tools->Dock = DockStyle::Top;
    this->Controls->Add(Tools);
}

void ConvForm::DeleteResults_Click(Object^pSender, EventArgs^Args)
{
    String^ selectedAction = this->combo1->SelectedItem->ToString();
    bool success = false;
    try {
    if (selectedAction == "Delete Results" || selectedAction == "Eliminar Resultados") {
        success = true;
        if (success == true) {
            try {
                dataManager->DeleteAllResults();
            }
            catch (Exception^ dbEx) {
                String^ errorMessage = dbEx->Message;
                if (dbEx->InnerException != nullptr) {
                    errorMessage = dbEx->InnerException->Message;
                }
                MessageBox::Show(L"Database Error." + errorMessage);
            }
        }
        LoadHistory();
    }
    else {
        System::Windows::Forms::DialogResult result = MessageBox::Show("Select 'Delete Results' to delete the whole table.", "Remember", MessageBoxButtons::OK, MessageBoxIcon::Error);
    }
    } catch (Exception^ ex) {
        String^ finalErrorMessage = (ex->InnerException != nullptr)
            ? L"SQL: " + ex->InnerException->Message
            : ex->Message;
        MessageBox::Show(finalErrorMessage, L"Fatal Error.");
    }
}

void ConvForm::ConvertirClick(Object^ pSender, EventArgs^ Args)
{
    if (txtGC->Text && String::IsNullOrEmpty(txtGF->Text)) {
        except->OnString(this->txtGC->Text);
    }
    else if (txtGF->Text && String::IsNullOrEmpty(txtGC->Text)) {
        except->OnString(this->txtGF->Text);
    }
    else if (txtGC->Text && txtGF) {
        except->OnString(this->txtGC->Text);
        except->OnString(this->txtGF->Text);
    }
    bool success = false;
    try{
    if (!String::IsNullOrEmpty(txtGC->Text) && String::IsNullOrEmpty(txtGF->Text)) {
	   double dblCelsius = Convert::ToDouble(txtGC->Text, CultureInfo::InvariantCulture);
	
	   TempConvert^ conv = gcnew TempConvert();
       double dblFahrenheit = conv->ConvertC2F(dblCelsius);

       String^ selectedAction = this->combo1->SelectedItem->ToString();

       try {
           if (selectedAction == "Store Results" || selectedAction == "Almacenar Resultados") {
               this->txtGF->Text = dblFahrenheit.ToString("F2", CultureInfo::InvariantCulture);
               success = true;
               if (success == true) {
                   try {
                       dataManager->InsertResult(Math::Round(dblFahrenheit, 2), Math::Round(dblCelsius, 2));
                       LoadHistory();
                   }
                   catch (Exception^ dbEx) {
                       String^ errorMessage = dbEx->Message;
                       if (dbEx->InnerException != nullptr) {
                           errorMessage = dbEx->InnerException->Message;
                       }
                       MessageBox::Show(L"Database Error." + errorMessage);
                   }
               }
           }
           else {
               System::Windows::Forms::DialogResult result = MessageBox::Show("Data should be stored selecting 'Store Results'.", "Remember", MessageBoxButtons::OK, MessageBoxIcon::Error);
           }
       } 
       catch (Exception^ ex) {
           String^ finalErrorMessage = (ex->InnerException != nullptr)
               ? L"SQL: " + ex->InnerException->Message
               : ex->Message;
           MessageBox::Show(finalErrorMessage, L"Fatal Error.");
       }
    } else if (!String::IsNullOrEmpty(txtGF->Text) && String::IsNullOrEmpty(txtGC->Text)) {
        double dblFahrenheit = Convert::ToDouble(txtGF->Text, CultureInfo::InvariantCulture);

        TempConvert^ conv = gcnew TempConvert();
        double dblCelsius = conv->ConvertF2C(dblFahrenheit);

        String^ selectedAction = this->combo1->SelectedItem->ToString();

        try {
            if (selectedAction == "Store Results" || selectedAction == "Almacenar Resultados") {
                this->txtGC->Text = dblCelsius.ToString("F2", CultureInfo::InvariantCulture);
                success = true;
                if (success == true) {
                    try {
                        dataManager->InsertResult(Math::Round(dblFahrenheit, 2), Math::Round(dblCelsius, 2));
                        LoadHistory();
                    }
                    catch (Exception^ dbEx) {
                        String^ errorMessage = dbEx->Message;
                        if (dbEx->InnerException != nullptr) {
                            errorMessage = dbEx->InnerException->Message;
                        }
                        MessageBox::Show(L"Database Error." + errorMessage);
                    }
                }
            }
            else {
                MessageBox::Show(L"Error! Data should be stored selecting 'Store Results'.");
            }
        }
        catch (Exception^ ex) {
            String^ finalErrorMessage = (ex->InnerException != nullptr)
                ? L"SQL: " + ex->InnerException->Message
                : ex->Message;
            MessageBox::Show(finalErrorMessage, L"Fatal Error.");
        }
    } 
   } catch (FormatException^ex){}
}

void ConvForm::CloseClick(Object^ pSender, EventArgs^ Args)
{
    this->Close();
}

void ConvForm::Setup_StatusBar() {
    
    this->status1 = gcnew StatusBarPanel();
    this->status1->BorderStyle = StatusBarPanelBorderStyle::Sunken;
    this->status1->AutoSize = StatusBarPanelAutoSize::Contents;
    this->status2 = gcnew StatusBarPanel();
    this->status2->BorderStyle = StatusBarPanelBorderStyle::Sunken;
    this->status2->AutoSize = StatusBarPanelAutoSize::Spring;


    status->Panels->Add(status1);
    status->Panels->Add(status2);
}

void ConvForm::Setup_Combo(void){
    combo1 = gcnew ComboBox();
    combo1->DropDownStyle = ComboBoxStyle::DropDownList;
    combo1->Location = Point(255, 92);
    Label^ l1 = gcnew Label();
    l1->Text = L"Store Results";
    combo1->Width = l1->PreferredWidth + 60;
    combo1->Items->Add(L"Store Results");
    combo1->Items->Add(L"Delete Results");
    combo1->SelectedIndex = 0;
    combo1->Cursor = Cursors::Hand;

    Controls->Add(combo1);
}

void ConvForm::InitToolTips() {
    this->dialog = gcnew ToolTip();
    this->dialog->IsBalloon = true;
    this->dialog->ToolTipIcon = ToolTipIcon::Info;
    this->dialog->ToolTipTitle = L"Temperature";
    this->dialog->Active = true;
    this->txtGC->Leave += gcnew EventHandler(this, &ConvForm::textGC_Leave);
    this->txtGF->Leave += gcnew EventHandler(this, &ConvForm::textGF_Leave);
}

void ConvForm::textGC_Leave(Object^ pSender, EventArgs^ Args) {
    if (!String::IsNullOrWhiteSpace(txtGC->Text) && !String::IsNullOrEmpty(txtGF->Text)) {
        this->dialog->Show("Clear both fields to convert new values.", txtGC, 110, -20, 2000);
    }
}
void ConvForm::textGF_Leave(Object^ pSender, EventArgs^ Args) {
    if (!String::IsNullOrWhiteSpace(txtGF->Text) && !String::IsNullOrEmpty(txtGC->Text)) {
        this->dialog->Show("Clear both fields to convert new values.", txtGF, 110, -20, 2000);
    }
}

void ConvForm::Setup_Radio()
{
    this->highBox = gcnew GroupBox;
    this->highBox->Location = Point(30, 470);
    this->highBox->Size = Drawing::Size(220, 80);
    this->highBox->Text = L"Highlight Temperatures";

    radioC = gcnew RadioButton();
    radioC->Text = L"Celsius";
    radioC->Location = Point(40, 489);
    radioC->Size = System::Drawing::Size(60, 20);
    radioC->BringToFront();
    radioC->Click += gcnew System::EventHandler(this, &ConvForm::Setup_RadioChecked);
    radioC->Cursor = Cursors::Hand;
    this->highBox->Controls->Add(radioC);
    radioF = gcnew RadioButton();
    radioF->Text = L"Fahrenheit";
    radioF->Location = Point(140, 489);
    radioF->Size = System::Drawing::Size(80, 20);
    radioF->Click += gcnew System::EventHandler(this, &ConvForm::Setup_RadioChecked);
    radioF->Cursor = Cursors::Hand;
    RNone = gcnew RadioButton();
    RNone->Text = L"No Highlight";
    RNone->Location = Point(40, 520);
    RNone->Click += gcnew System::EventHandler(this, &ConvForm::Setup_RadioChecked);
    RNone->Size = System::Drawing::Size(90, 20);
    RNone->BringToFront();
    RNone->Cursor = Cursors::Hand;
    HighBoth = gcnew CheckBox();
    HighBoth->Text = L"Highlight Both";
    HighBoth->Location = Point(140, 520);
    HighBoth->Checked = false;
    HighBoth->CheckedChanged += gcnew System::EventHandler(this, &ConvForm::Setup_Checked);
    HighBoth->Size = System::Drawing::Size(100, 20);
    HighBoth->BringToFront();
    HighBoth->Cursor = Cursors::Hand;
    this->highBox->Controls->Add(radioF);
    radioF->BringToFront();
    Controls->Add(radioC);
    Controls->Add(radioF);
    Controls->Add(RNone);
    Controls->Add(HighBoth);
    this->Controls->Add(highBox);
    if (txtGC != nullptr) {
        txtGC->Font = boldFont;
        txtGC->Refresh();
    }
    if (txtGF != nullptr) {
        txtGF->Font = boldFont;
        txtGF->Refresh();
    }
}

void ConvForm::LoadResults()
{
    DataManager^ dm = gcnew DataManager();
    SqlConnection^ conn = gcnew SqlConnection(dm->GetConnectionString());

    this->status1->Text = L"Connecting to the database...";
    this->status2->Text = L"Wait...";

    try {
        conn->Open();
       
        String^ sql =
            L"SELECT Fahrenheit, Celsius FROM Conversions ORDER BY ID ASC;";
        SqlDataAdapter^ da = gcnew SqlDataAdapter(sql, conn);
        DataTable^ dt = gcnew DataTable();
        da->Fill(dt);
        this->status1->Text = L"Database Connection Success.";
        this->status2->Text = L"Ready.";
        
        dgvHistory = gcnew DataGridView();
        dgvHistory->Location = Point(70, 160);
        dgvHistory->Size = Drawing::Size(207, 300);
        dgvHistory->ReadOnly = true;
        dgvHistory->AllowUserToAddRows = false;
        dgvHistory->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::AllCells;
        dgvHistory->BringToFront();
        this->dgvHistory->DataSource = dt;
        this->Controls->Add(dgvHistory);
    }
    catch (Exception^ ex) {
        this->status1->Text = L"Connection Failed.";
        this->status2->Text = L"Error!";
        MessageBox::Show("Table could not be loaded: " + ex->Message + " Database Error");
    }
    __finally {
        if (conn != nullptr && conn->State == ConnectionState::Open) conn->Close();
    }
}

void ConvForm::Setup_Menu(void)
{
    menuBar = gcnew MainMenu();
    fileMenu = gcnew MenuItem("&File");
    this->menuBar->MenuItems->Add(fileMenu);
    Item1 = gcnew MenuItem("&About...");
    Item1->Click += gcnew EventHandler(this, &ConvForm::MenuItem_About_Click);
    fileMenu->MenuItems->Add(Item1);
    Item2 = gcnew MenuItem("&Exit");
    Item2->Click += gcnew EventHandler(this, &ConvForm::MenuItem_Exit_Click);
    fileMenu->MenuItems->Add(Item2);
    settingsMenu = gcnew MenuItem("&Settings");
    this->menuBar->MenuItems->Add(settingsMenu);
    submenu1 = gcnew MenuItem("&Language");
    settingsMenu->MenuItems->Add(submenu1);
    Item1_1 = gcnew MenuItem("&English");
    Item1_1->Click += gcnew EventHandler(this, &ConvForm::MenuItem_English_Click);
    Item1_1->Checked = true;
    Item1_2 = gcnew MenuItem("&Spanish");
    Item1_2->Checked = false;
    Item1_2->Click += gcnew EventHandler(this, &ConvForm::MenuItem_Spanish_Click);

    submenu1->MenuItems->Add(Item1_1);
    submenu1->MenuItems->Add(Item1_2);

    this->Menu = menuBar;
}


void ConvForm::MenuItem_About_Click(Object^ pSender, EventArgs^ Args)
{
    Form^ aboutForm = gcnew Form();
    aboutForm->Text =  L"About the Temperature Unit Converter";
    aboutForm->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
    aboutForm->Size = System::Drawing::Size(370, 160);
    aboutForm->BackColor = System::Drawing::Color::LightGray;
    aboutForm->MaximizeBox = false;
    aboutForm->MinimizeBox = false;
    aboutForm->StartPosition = FormStartPosition::CenterScreen;

    Label^ label1 = gcnew Label();
    label1->Text = L"Made by Esteban Rodolfo Scaramuzza, 2025.";
    label1->Size = System::Drawing::Size(label1->PreferredWidth, label1->PreferredHeight);
    label1->Location = Point(90, 30);

    Label^ label2 = gcnew Label();
    Bitmap^ iconBitmap = gcnew Bitmap("Thermo.bmp");
    label2->Image = iconBitmap;
    label2->Size = System::Drawing::Size(48, 48);
    label2->Location = Point(15, 20);
    label2->AutoSize = false;
    label2->ImageAlign = ContentAlignment::MiddleLeft;

    aboutForm->Controls->Add(label1);
    aboutForm->Controls->Add(label2);

    OKButton = gcnew Button();
    OKButton->Text = L"OK";
    OKButton->Size = System::Drawing::Size(40, 25);
    OKButton->BackColor = System::Drawing::Color::White;
    OKButton->Cursor = Cursors::Hand;
    OKButton->Location = Point(290, 85);

    OKButton->Click += gcnew EventHandler(this, &ConvForm::OKButton_Cliked);
    aboutForm->Controls->Add(OKButton);
    aboutForm->ShowDialog();
}

void ConvForm::MenuItem_Exit_Click(Object^ pSender, EventArgs^ Args)
{
   this->Close();
}

void ConvForm::Export_Click(Object^ pSender, EventArgs^ Args)
{
    try {
        ExporttoCSV();
    }
    catch (Exception^ ex) {
        MessageBox::Show(ex->Message, "Export Error encountered!");
    }
}

void ConvForm::Setup_ToolBar() {
    
	ImageList^ toolImages = gcnew ImageList();
    toolImages->ImageSize = Drawing::Size(20, 20);
	toolImages->TransparentColor = Drawing::Color::Transparent;
    toolImages->Images->Add(Drawing::Image::FromFile("DISCO.BMP"));
    toolImages->Images->Add(Drawing::Image::FromFile("DELETE.BMP"));
	
    ToolBarButton^ butnExport = gcnew ToolBarButton();
    butnExport->ImageIndex = 0;
	butnExport->ToolTipText = L"Export Data to CSV File";
    ToolBarButton^ butnDelete = gcnew ToolBarButton();
    butnDelete->ImageIndex = 1;
    butnDelete->ToolTipText = L"Delete the Whole Table";
    this->Tools->Buttons->Add(butnExport);
    this->Tools->Buttons->Add(butnDelete);
    this->Tools->ImageList = toolImages;
    this->Tools->ButtonClick += gcnew ToolBarButtonClickEventHandler(this, &ConvForm::Event_ButtonClicked);
    this->Tools->Cursor = Cursors::Hand;
}

void ConvForm::MenuItem_English_Click(Object^ pSender, EventArgs^ Args) {
    Item1_1->Checked = true;
    Item1_2->Checked = false;

    CultureInfo^ englishCulture = gcnew CultureInfo("en-GB");

    Thread::CurrentThread->CurrentCulture = englishCulture;
    Thread::CurrentThread->CurrentUICulture = englishCulture;
    AppSettings^ settings = gcnew AppSettings();
    settings->InitializeMenu();
    if (resManager == nullptr) {
        resManager = gcnew ResourceManager("ConversorWebService.English", Assembly::GetExecutingAssembly());
    }
    this->Text = resManager->GetString("ConvForm.Text", englishCulture);

    fileMenu->Text = resManager->GetString("MainMenu.Text", englishCulture);
    Item1->Text = resManager->GetString("Item1.Text", englishCulture);
    Item2->Text = resManager->GetString("Item2.Text", englishCulture);
    tablel->Text = resManager->GetString("Tablel.Text", englishCulture);
    settingsMenu->Text = resManager->GetString("Settings.Text", englishCulture);
    submenu1->Text = resManager->GetString("SMenu.Text", englishCulture);
    Item1_1->Text = resManager->GetString("I1.Text", englishCulture);
    Item1_2->Text = resManager->GetString("I2.Text", englishCulture);
    btnClose->Text = resManager->GetString("BtnClose.Text", englishCulture);
    btnCalculate->Text = resManager->GetString("BtnCalculate.Text", englishCulture);
    DeleteData->Text = resManager->GetString("DeleteData.Text", englishCulture);
    Exprtload->Text = resManager->GetString("Exprtload.Text", englishCulture);
    highBox->Text = resManager->GetString("HighBox.Text", englishCulture);
    grpBox->Text = resManager->GetString("GrpBox.Text", englishCulture);
    RNone->Text = resManager->GetString("RNone.Text", englishCulture);
    HighBoth->Text = resManager->GetString("HighBoth.Text", englishCulture);
    status1->Text = resManager->GetString("Status1.Text", englishCulture);
    status2->Text = resManager->GetString("Status2.Text", englishCulture);
    combo1->Items->Clear();
    combo1->Items->Add(resManager->GetString("Combo.Text", englishCulture));
    combo1->Items->Add(resManager->GetString("Combo1.Text", englishCulture));
    combo1->SelectedIndex = 0;
   
    AppSettings::SaveLanguage("en-GB");
}

void ConvForm::MenuItem_Spanish_Click(Object^ pSender, EventArgs^ Args) {
    Item1_1->Checked = false;
    Item1_2->Checked = true;

    CultureInfo^ spanishCulture = gcnew CultureInfo("es-ES");

    Thread::CurrentThread->CurrentCulture = spanishCulture;
    Thread::CurrentThread->CurrentUICulture = spanishCulture;
    AppSettings^ settings = gcnew AppSettings();
    settings->InitializeMenu();
    if (resManager == nullptr) {
        resManager = gcnew ResourceManager("ConversorWebService.Español", Assembly::GetExecutingAssembly());
    }
    this->Text = resManager->GetString("ConvForm.Text", spanishCulture);

    fileMenu->Text = resManager->GetString("MainMenu.Text", spanishCulture);
    Item1->Text = resManager->GetString("Item1.Text", spanishCulture);
    Item2->Text = resManager->GetString("Item2.Text", spanishCulture);
    tablel->Text = resManager->GetString("Tablel.Text", spanishCulture);
    settingsMenu->Text = resManager->GetString("Settings.Text", spanishCulture);
    submenu1->Text = resManager->GetString("SMenu.Text", spanishCulture);
    Item1_1->Text = resManager->GetString("I1.Text", spanishCulture);
    Item1_2->Text = resManager->GetString("I2.Text", spanishCulture);
    btnClose->Text = resManager->GetString("BtnClose.Text", spanishCulture);
    btnCalculate->Text = resManager->GetString("BtnCalculate.Text", spanishCulture);
    DeleteData->Text = resManager->GetString("DeleteData.Text", spanishCulture);
    Exprtload->Text = resManager->GetString("Exprtload.Text", spanishCulture);
    highBox->Text = resManager->GetString("HighBox.Text", spanishCulture);
    grpBox->Text = resManager->GetString("GrpBox.Text", spanishCulture);
    RNone->Text = resManager->GetString("RNone.Text", spanishCulture);
    HighBoth->Text = resManager->GetString("HighBoth.Text", spanishCulture);
	status1->Text = resManager->GetString("Status1.Text", spanishCulture);
	status2->Text = resManager->GetString("Status2.Text", spanishCulture);
    combo1->Items->Clear();
    combo1->Items->Add(resManager->GetString("Combo.Text", spanishCulture));
    combo1->Items->Add(resManager->GetString("Combo1.Text", spanishCulture));
    combo1->SelectedIndex = 0;

    AppSettings::SaveLanguage("es-ES");
}


void ConvForm::OKButton_Cliked(Object^ pSender, EventArgs^ Args)
{
    Button^ clickedButton = dynamic_cast<Button^>(pSender);
    Form ^ parentForm = clickedButton->FindForm();

    parentForm->Close();
}