#pragma once

// Included libraries
#include "RMH_MathConversions_Library.h"

// Included resources
#include "RMH_Application_Information.h"

// Class namespace
namespace IRCAMThermalViewer {

	// Associated class namespace
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	// Summary for Form - WelcomeScreen
	public ref class WelcomeScreen : public System::Windows::Forms::Form {

	public:

		WelcomeScreen(void) {

			// Init GUI components and objects
			InitializeComponent();

			// Enable dark mode for the application title bar
			RMH_Winforms_EnableTitleBarDarkMode(this->Handle);

			// Update the text at the top of the GUI
			RMH_Winforms_ChangeFormTitleBarText(this, "About And General Information");

		}

	protected:

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~WelcomeScreen() {

			if (components) {

				// Delete all form components
				delete components;

			}

		}
	
	protected:

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private: System::ComponentModel::Container ^components;
		private: System::Windows::Forms::Label^ label1;
		private: System::Windows::Forms::PictureBox^ pictureBox1;
		private: System::Windows::Forms::LinkLabel^ linkLabel1;

#pragma region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(WelcomeScreen::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->linkLabel1 = (gcnew System::Windows::Forms::LinkLabel());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Font = (gcnew System::Drawing::Font(L"Arial", 14, System::Drawing::FontStyle::Bold));
			this->label1->ForeColor = System::Drawing::Color::White;
			this->label1->Location = System::Drawing::Point(75, 164);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(441, 66);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Visit My Website: https://rmg-engineering.com/\r\nAny Questions Or Suggestions\?\r\nVi"
				L"sit My Discord Server:";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// linkLabel1
			// 
			this->linkLabel1->AutoSize = true;
			this->linkLabel1->BackColor = System::Drawing::Color::Transparent;
			this->linkLabel1->Font = (gcnew System::Drawing::Font(L"Arial", 20, System::Drawing::FontStyle::Bold));
			this->linkLabel1->LinkColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(88)), static_cast<System::Int32>(static_cast<System::Byte>(101)),
				static_cast<System::Int32>(static_cast<System::Byte>(242)));
			this->linkLabel1->Location = System::Drawing::Point(85, 243);
			this->linkLabel1->Name = L"linkLabel1";
			this->linkLabel1->Size = System::Drawing::Size(420, 32);
			this->linkLabel1->TabIndex = 0;
			this->linkLabel1->TabStop = true;
			this->linkLabel1->Text = L"https://discord.gg/3zq3zXFA8B";
			this->linkLabel1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->linkLabel1->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &WelcomeScreen::linkLabel1_LinkClicked);
			// 
			// pictureBox1
			// 
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->Location = System::Drawing::Point(138, 51);
			this->pictureBox1->Margin = System::Windows::Forms::Padding(0);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(283, 94);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::AutoSize;
			this->pictureBox1->TabIndex = 2;
			this->pictureBox1->TabStop = false;
			// 
			// WelcomeScreen
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->AutoValidate = System::Windows::Forms::AutoValidate::Disable;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(35)), static_cast<System::Int32>(static_cast<System::Byte>(35)),
				static_cast<System::Int32>(static_cast<System::Byte>(35)));
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"$this.BackgroundImage")));
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->CausesValidation = false;
			this->ClientSize = System::Drawing::Size(1121, 655);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->linkLabel1);
			this->DoubleBuffered = true;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"WelcomeScreen";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"WelcomeScreen";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &WelcomeScreen::WelcomeScreen_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &WelcomeScreen::WelcomeScreen_Shown);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}

#pragma endregion

		// ------------ Welcome/About GUI Start-Up And Shutdown Callback Routines ------------- //

		// Welcome/About form start-up callback routine -> 
		private: System::Void WelcomeScreen_Shown(System::Object^ sender, System::EventArgs^ e) {

			// Update the associated form flag
			isWelcomeScreenFormOpen = true;

		}

		// Welcome/About form shutdown callback routine ->
		private: System::Void WelcomeScreen_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) {

			// Update the associated form flag
			isWelcomeScreenFormOpen = false;
			isWelcomeScreenFormDocked = false;
			isWelcomeScreenFormUndocked = false;

			// When the form is closed - hide the form
			this->Hide();
			// Disable "disposing" of the form object
			e->Cancel = true;

		}

		// -------------------------- Welcome/About Screen Callback Routines ------------------------- //

		// Link Label Clicked Callback Routine ->
		private: System::Void linkLabel1_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e) {

			// Navigate to the Discord URL
			RMH_Winforms_OpenLinkURL(RMH_Conversion_StdStringToSystemString(DiscordServerLinkAddress));

		}

		// ------------------------------------------------------------------------------------------ //

};
}
