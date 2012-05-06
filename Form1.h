#pragma once

#include <stdio.h>
#include <cstdlib>
#include <ctime>

namespace memgame {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	const int ERROR_VALUE = -1;
	const int EMPTY_VALUE = -2;

	enum GAMESTATE {
		GS_STOPPED,
		GS_RUNNING,
		GS_FINISHED
	};

	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	private: 
		System::Collections::Generic::List<System::Windows::Forms::TextBox^> cells;
		int c_dim;
		int c_numcount;
		int c_showtime;
		GAMESTATE game_state;
		int *right_numbers, *inputed_numbers;

	public:
		Form1(void): c_dim(0),c_numcount(0),c_showtime(0),game_state(GS_STOPPED),right_numbers(NULL),inputed_numbers(NULL) {
			InitializeComponent();
		}

	protected:
		~Form1() {
			if (components)
				delete components;
		}

		void save_input(int* outbuffer) {
			for(int i=0; i<c_dim*c_dim; ++i) {
				System::Windows::Forms::TextBox^ tb = cells[i];
				int value;
				if (tb->Text == L"") {
					value = EMPTY_VALUE;
				} else {
					try {
						value = Convert::ToInt32(tb->Text, 10);
					} catch (OverflowException^) {
						value = ERROR_VALUE;
					} catch (FormatException^) {
						value = ERROR_VALUE;
					}
				}
				outbuffer[i] = value;
			}
		}
		void show_input(int* inbuffer) {
			for(int i=0; i<c_dim*c_dim; ++i)
				cells[i]->Text = 
					(inbuffer[i] == EMPTY_VALUE) ? L"" :
					(inbuffer[i] == ERROR_VALUE) ? L"E" :
					inbuffer[i].ToString();
		}
		void clear_text() {
			for(int i=0; i<c_dim*c_dim; ++i)
				cells[i]->Text = L"";
		}
		void highlight_errors() {
			for(int i=0; i<c_dim*c_dim; ++i)
				cells[i]->BackColor = (right_numbers[i] != inputed_numbers[i]) ?
					System::Drawing::Color::Yellow : System::Drawing::SystemColors::Window;
		}
		void clear_highlights() {
			for(int i=0; i<c_dim*c_dim; ++i)
				cells[i]->BackColor = System::Drawing::SystemColors::Window;
		}
		void get_params() {
			c_dim = System::Decimal::ToInt32(num_dim->Value);
			c_numcount = System::Decimal::ToInt32(num_numcount->Value);
			c_showtime = System::Decimal::ToInt32(num_showtime->Value);
		}
		void set_readonly(bool value) {
			for(int i=0; i<c_dim*c_dim; ++i)
				cells[i]->ReadOnly = value;
		}

		void set_state(GAMESTATE state) {
			switch (state) {
				case GS_STOPPED:
				case GS_FINISHED:
					timer->Enabled = false;
					num_dim->Enabled = true;
					num_numcount->Enabled = true;
					num_showtime->Enabled = true;
					bt_create->Enabled = true;
					bt_start->Enabled = true;
					bt_stop->Enabled = false;
					bt_submit->Enabled = false;
					bt_showcorrect->Enabled = (state == GS_FINISHED);
					bt_showmine->Enabled = (state == GS_FINISHED);
					set_readonly(true);
					if (state == GS_FINISHED)
						highlight_errors();
					else {
						clear_highlights();
						clear_text();
					}
					break;
				case GS_RUNNING:
					num_dim->Enabled = false;
					num_numcount->Enabled = false;
					num_showtime->Enabled = false;
					bt_create->Enabled = false;
					bt_start->Enabled = false;
					bt_stop->Enabled = true;
					bt_submit->Enabled = false;
					bt_showcorrect->Enabled = false;
					bt_showmine->Enabled = false;
					clear_highlights();

					timer->Enabled = true;
					break;
			}
			game_state = state;
		}

		void destroy_game() {
			for(int i=0; i<c_dim*c_dim; ++i) {
				System::Windows::Forms::TextBox^ cell = cells[0];
				this->Controls->Remove(cell);
				cells.Remove(cell);
				delete cell;
			}
			c_dim = 0;
			if (right_numbers != NULL) {
				delete right_numbers;
				right_numbers = NULL;
			}
			if (inputed_numbers != NULL) {
				delete inputed_numbers;
				inputed_numbers = NULL;
			}
		}
		void create_game() {
			for(int y=0; y<c_dim; ++y)
				for(int x=0; x<c_dim; ++x) {
					System::Windows::Forms::TextBox^ cell = gcnew System::Windows::Forms::TextBox();
					cell->BackColor = System::Drawing::SystemColors::Window;
					cell->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
					cell->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
						static_cast<System::Byte>(204)));
					cell->Location = System::Drawing::Point(5+40*x, 100+29*y);
					cell->Name = L"text_cell_" + (y*c_dim + x).ToString();
					cell->Size = System::Drawing::Size(35, 24);
					cell->TabIndex = 200;
					cell->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
					this->Controls->Add(cell);
					cells.Add(cell);
				}
			right_numbers = new int[c_dim*c_dim];
			inputed_numbers = new int[c_dim*c_dim];
			num_numcount->Maximum = c_dim*c_dim;
		}

		void start_game() {
			if (c_numcount > c_dim*c_dim)
				return; // too many numbers
			srand((unsigned int)time(NULL));

			// Generate the field
			for(int i=0; i<c_dim*c_dim; ++i)
				right_numbers[i] = EMPTY_VALUE;
			for(int i=0; i<c_numcount; ) {
				int x = rand() % c_dim;
				int y = rand() % c_dim;
				int index = y*c_dim + x;
				if (right_numbers[index] == EMPTY_VALUE) {
					right_numbers[index] = i+1;
					++i;
				}
			}

			timer->Interval = c_showtime;
			set_readonly(true);
			show_input(right_numbers);
			set_state(GS_RUNNING);
		}
		void stop_game(bool reset) {
			set_state(reset ? GS_STOPPED : GS_FINISHED);
			if (!reset) {
				save_input(inputed_numbers);
				highlight_errors();
			}
		}

	protected:

	private: System::Windows::Forms::Timer^  timer;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::NumericUpDown^  num_dim;
	private: System::Windows::Forms::NumericUpDown^  num_numcount;
	private: System::Windows::Forms::NumericUpDown^  num_showtime;
	private: System::Windows::Forms::Button^  bt_create;
	private: System::Windows::Forms::Button^  bt_start;
	private: System::Windows::Forms::Button^  bt_stop;
	private: System::Windows::Forms::Button^  bt_submit;
	private: System::Windows::Forms::Button^  bt_showcorrect;
	private: System::Windows::Forms::Button^  bt_showmine;

	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->bt_create = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->bt_start = (gcnew System::Windows::Forms::Button());
			this->bt_submit = (gcnew System::Windows::Forms::Button());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->num_dim = (gcnew System::Windows::Forms::NumericUpDown());
			this->num_numcount = (gcnew System::Windows::Forms::NumericUpDown());
			this->num_showtime = (gcnew System::Windows::Forms::NumericUpDown());
			this->bt_showcorrect = (gcnew System::Windows::Forms::Button());
			this->bt_showmine = (gcnew System::Windows::Forms::Button());
			this->bt_stop = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->num_dim))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->num_numcount))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->num_showtime))->BeginInit();
			this->SuspendLayout();
			// 
			// timer
			// 
			this->timer->Tick += gcnew System::EventHandler(this, &Form1::timer_Tick);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(88, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Game dimension:";
			// 
			// bt_create
			// 
			this->bt_create->Location = System::Drawing::Point(172, 4);
			this->bt_create->Name = L"bt_create";
			this->bt_create->Size = System::Drawing::Size(75, 23);
			this->bt_create->TabIndex = 2;
			this->bt_create->Text = L"Create field";
			this->bt_create->UseVisualStyleBackColor = true;
			this->bt_create->Click += gcnew System::EventHandler(this, &Form1::bt_create_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(12, 35);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(82, 13);
			this->label2->TabIndex = 3;
			this->label2->Text = L"Numbers count:";
			// 
			// bt_start
			// 
			this->bt_start->Location = System::Drawing::Point(172, 30);
			this->bt_start->Name = L"bt_start";
			this->bt_start->Size = System::Drawing::Size(75, 23);
			this->bt_start->TabIndex = 5;
			this->bt_start->Text = L"Start game";
			this->bt_start->UseVisualStyleBackColor = true;
			this->bt_start->Click += gcnew System::EventHandler(this, &Form1::bt_startstop_Click);
			// 
			// bt_submit
			// 
			this->bt_submit->Location = System::Drawing::Point(253, 5);
			this->bt_submit->Name = L"bt_submit";
			this->bt_submit->Size = System::Drawing::Size(106, 72);
			this->bt_submit->TabIndex = 6;
			this->bt_submit->Text = L"Submit results";
			this->bt_submit->UseVisualStyleBackColor = true;
			this->bt_submit->Click += gcnew System::EventHandler(this, &Form1::bt_submit_Click);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(12, 61);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(81, 13);
			this->label3->TabIndex = 7;
			this->label3->Text = L"Show time (ms):";
			// 
			// num_dim
			// 
			this->num_dim->Location = System::Drawing::Point(100, 7);
			this->num_dim->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {15, 0, 0, 0});
			this->num_dim->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->num_dim->Name = L"num_dim";
			this->num_dim->Size = System::Drawing::Size(66, 20);
			this->num_dim->TabIndex = 9;
			this->num_dim->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->num_dim->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {3, 0, 0, 0});
			// 
			// num_numcount
			// 
			this->num_numcount->Location = System::Drawing::Point(100, 33);
			this->num_numcount->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {225, 0, 0, 0});
			this->num_numcount->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->num_numcount->Name = L"num_numcount";
			this->num_numcount->Size = System::Drawing::Size(66, 20);
			this->num_numcount->TabIndex = 10;
			this->num_numcount->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->num_numcount->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {9, 0, 0, 0});
			// 
			// num_showtime
			// 
			this->num_showtime->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->num_showtime->Location = System::Drawing::Point(99, 59);
			this->num_showtime->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {120000, 0, 0, 0});
			this->num_showtime->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100, 0, 0, 0});
			this->num_showtime->Name = L"num_showtime";
			this->num_showtime->Size = System::Drawing::Size(67, 20);
			this->num_showtime->TabIndex = 11;
			this->num_showtime->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->num_showtime->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {3000, 0, 0, 0});
			// 
			// bt_showcorrect
			// 
			this->bt_showcorrect->Location = System::Drawing::Point(366, 4);
			this->bt_showcorrect->Name = L"bt_showcorrect";
			this->bt_showcorrect->Size = System::Drawing::Size(139, 23);
			this->bt_showcorrect->TabIndex = 12;
			this->bt_showcorrect->Text = L"Show correrct answer";
			this->bt_showcorrect->UseVisualStyleBackColor = true;
			this->bt_showcorrect->Click += gcnew System::EventHandler(this, &Form1::bt_showcorrect_Click);
			// 
			// bt_showmine
			// 
			this->bt_showmine->Location = System::Drawing::Point(366, 30);
			this->bt_showmine->Name = L"bt_showmine";
			this->bt_showmine->Size = System::Drawing::Size(139, 23);
			this->bt_showmine->TabIndex = 13;
			this->bt_showmine->Text = L"Show my answer";
			this->bt_showmine->UseVisualStyleBackColor = true;
			this->bt_showmine->Click += gcnew System::EventHandler(this, &Form1::bt_showmine_Click);
			// 
			// bt_stop
			// 
			this->bt_stop->Location = System::Drawing::Point(172, 56);
			this->bt_stop->Name = L"bt_stop";
			this->bt_stop->Size = System::Drawing::Size(75, 23);
			this->bt_stop->TabIndex = 14;
			this->bt_stop->Text = L"Stop game";
			this->bt_stop->UseVisualStyleBackColor = true;
			this->bt_stop->Click += gcnew System::EventHandler(this, &Form1::bt_stop_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(574, 403);
			this->Controls->Add(this->bt_stop);
			this->Controls->Add(this->bt_showmine);
			this->Controls->Add(this->bt_showcorrect);
			this->Controls->Add(this->num_showtime);
			this->Controls->Add(this->num_numcount);
			this->Controls->Add(this->num_dim);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->bt_submit);
			this->Controls->Add(this->bt_start);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->bt_create);
			this->Controls->Add(this->label1);
			this->Name = L"Form1";
			this->Text = L"Memory game";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->num_dim))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->num_numcount))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->num_showtime))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
private: System::Void bt_create_Click(System::Object^  sender, System::EventArgs^ e) {
			 destroy_game();
			 get_params();
			 create_game();
		 }
private: System::Void bt_startstop_Click(System::Object^  sender, System::EventArgs^ e) {
			 int old_dim = c_dim;
			 get_params();
			 int new_dim = c_dim;
			 if (old_dim != new_dim) {
				 c_dim = old_dim;
				 destroy_game();
				 c_dim = new_dim;
				 create_game();
			 }
			 start_game();
		 }
private: System::Void bt_submit_Click(System::Object^  sender, System::EventArgs^ e) {
			 stop_game(false);
		 }
private: System::Void timer_Tick(System::Object^  sender, System::EventArgs^ e) {
			 if (game_state != GS_RUNNING)
				 return;
			 timer->Enabled = false;
			 clear_text();
			 set_readonly(false);
			 bt_submit->Enabled = true;
		 }
private: System::Void bt_stop_Click(System::Object^  sender, System::EventArgs^  e) {
			 stop_game(true);
		 }
private: System::Void bt_showcorrect_Click(System::Object^  sender, System::EventArgs^  e) {
			 show_input(right_numbers);
		 }
private: System::Void bt_showmine_Click(System::Object^  sender, System::EventArgs^  e) {
			 show_input(inputed_numbers);
		 }
private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
			 set_state(GS_STOPPED);
		 }
};
}

