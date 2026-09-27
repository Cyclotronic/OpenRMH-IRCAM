#pragma once

// Included libraries

// Included application resources

// Included form headers

// Class namespace
namespace IRCAMThermalViewer {

	// Associated namespaces
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace std;

	// Summary for Form - ColorPaletteCreator
	public ref class ColorPaletteCreator : public System::Windows::Forms::Form {

	public:

		// ------------------------------------ Class Constructor ------------------------------------ //

		ColorPaletteCreator(void) {

			// Init GUI components and objects
			InitializeComponent();
			// Set global objects from this form for global use
			InitializeGlobalFormsObjects();
			
		}

		// ---------------------------- Miscellaneous Class-Specific Methods ----------------------------- //

		void InitializeGlobalFormsObjects() {

			// This routine sets global objects from this form
			// so that these can be accessed from other forms



		}

		// -------------------------------------------------------------------------------------------- //

	protected:

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ColorPaletteCreator() {

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


#pragma region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(ColorPaletteCreator::typeid));
			this->SuspendLayout();
			// 
			// ColorPaletteCreator
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(35)), static_cast<System::Int32>(static_cast<System::Byte>(35)),
				static_cast<System::Int32>(static_cast<System::Byte>(35)));
			this->ClientSize = System::Drawing::Size(685, 520);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"ColorPaletteCreator";
			this->Text = L"ColorPaletteCreator";
			this->ResumeLayout(false);

		}

#pragma endregion

	};
}
