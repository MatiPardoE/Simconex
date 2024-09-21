import customtkinter as ctk
import os
from PIL import Image

class LogFrame(ctk.CTkScrollableFrame):

    def __init__(self, master):
        super().__init__(master) 

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1)

        self.label_control = ctk.CTkLabel(self, text="Registro de datos", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=(20, 10), pady=(10, 0), sticky="w")

        datalog = [
            ["Hora ", "  Fecha   ", "OD [%]", " pH  ", "Luz [%]", "Temperatura [°C]", " Ciclo  "],
            ["12:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["12:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["13:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["13:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["14:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["14:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["15:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["12:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["12:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["13:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["13:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["14:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["14:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["15:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["12:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["12:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["13:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["13:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["14:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["14:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["15:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"]
        ]

        self.frame_lines = ctk.CTkFrame(self)
        self.frame_lines.grid(row=1, column=0, padx=(20, 10), pady=(10, 10), sticky="ew")
        self.frame_lines.grid_columnconfigure(0, weight=1)
        self.frame_lines.grid_rowconfigure(0, weight=1)

        for i in range(len(datalog)):
            self.frame_line = ctk.CTkFrame(self.frame_lines)
            self.frame_line.grid(row=i, column=0, padx=5, pady=0, sticky="ew")

            self.frame_line.grid_rowconfigure(0, weight=1)
            self.frame_line.grid_columnconfigure(0, weight=1)
            self.frame_line.grid_columnconfigure(1, weight=1)
            self.frame_line.grid_columnconfigure(2, weight=1)
            self.frame_line.grid_columnconfigure(3, weight=1)
            self.frame_line.grid_columnconfigure(4, weight=1)
            self.frame_line.grid_columnconfigure(5, weight=1)
            self.frame_line.grid_columnconfigure(6, weight=1)

            self.label_time = ctk.CTkLabel(self.frame_line, text=datalog[i][0], corner_radius=0)
            self.label_time.grid(row=0, column=0, padx=5, pady=0, sticky="ew")

            self.label_date = ctk.CTkLabel(self.frame_line, text=datalog[i][1], corner_radius=0)
            self.label_date.grid(row=0, column=1, padx=5, pady=0, sticky="ew")

            self.label_od = ctk.CTkLabel(self.frame_line, text=datalog[i][2], corner_radius=0)
            self.label_od.grid(row=0, column=2, padx=5, pady=0, sticky="ew")

            self.label_ph = ctk.CTkLabel(self.frame_line, text=datalog[i][3], corner_radius=0)
            self.label_ph.grid(row=0, column=3, padx=5, pady=0, sticky="ew")

            self.label_light = ctk.CTkLabel(self.frame_line, text=datalog[i][4], corner_radius=0)
            self.label_light.grid(row=0, column=4, padx=5, pady=0, sticky="ew")

            self.label_temp = ctk.CTkLabel(self.frame_line, text=datalog[i][5], corner_radius=0)
            self.label_temp.grid(row=0, column=5, padx=5, pady=0, sticky="ew")

            self.label_cycle = ctk.CTkLabel(self.frame_line, text=datalog[i][6], corner_radius=0)
            self.label_cycle.grid(row=0, column=6, padx=5, pady=0, sticky="ew")

            if i==0:
                self.label_time.configure(font=ctk.CTkFont(size=15, weight="bold"))
                self.label_date.configure(font=ctk.CTkFont(size=15, weight="bold"))
                self.label_od.configure(font=ctk.CTkFont(size=15, weight="bold"))
                self.label_ph.configure(font=ctk.CTkFont(size=15, weight="bold"))
                self.label_light.configure(font=ctk.CTkFont(size=15, weight="bold"))
                self.label_temp.configure(font=ctk.CTkFont(size=15, weight="bold"))
                self.label_cycle.configure(font=ctk.CTkFont(size=15, weight="bold"))

class InstantValuesFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 

        image_path = os.path.join(os.getcwd(), "images")

        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)

        self.left_frame = ctk.CTkFrame(self)
        self.left_frame.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")

        self.reactor_img = ctk.CTkImage(Image.open(os.path.join(image_path, "reactor_edited.png")), size=(150, 300))
        self.reactor_img_label = ctk.CTkLabel(self.left_frame, text="", image=self.reactor_img)
        self.reactor_img_label.grid(row=0, column=0, padx=0, pady=0, sticky="nsew")

        self.right_frame = ctk.CTkFrame(self)
        self.right_frame.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")

        self.label_light = ctk.CTkLabel(self.right_frame, text="Luz [%]", font=ctk.CTkFont(weight="bold"))
        self.label_light.grid(row=0, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.light_button = ctk.CTkButton(self.right_frame, text="42", fg_color="white", hover=False, state="disabled", text_color_disabled="black")
        self.light_button.grid(row=1, column=0, padx=10, pady=0, sticky="nsew")

        self.label_ph = ctk.CTkLabel(self.right_frame, text="pH", font=ctk.CTkFont(weight="bold"))
        self.label_ph.grid(row=2, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.ph_button = ctk.CTkButton(self.right_frame, text="6.536", fg_color="white", hover=False, state="disabled", text_color_disabled="black")
        self.ph_button.grid(row=3, column=0, padx=10, pady=0, sticky="nsew")

        self.label_do = ctk.CTkLabel(self.right_frame, text="OD [%]", font=ctk.CTkFont(weight="bold"))
        self.label_do.grid(row=4, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.do_button = ctk.CTkButton(self.right_frame, text="342.4", fg_color="white", hover=False, state="disabled", text_color_disabled="black")
        self.do_button.grid(row=5, column=0, padx=10, pady=0, sticky="nsew")

        self.label_temp = ctk.CTkLabel(self.right_frame, text="Temperatura [°C]", font=ctk.CTkFont(weight="bold"))
        self.label_temp.grid(row=6  , column=0, padx=10, pady=(10,0), sticky="nsew")
        self.temp_button = ctk.CTkButton(self.right_frame, text="24.3", fg_color="white", hover=False, state="disabled", text_color_disabled="black")
        self.temp_button.grid(row=7, column=0, padx=10, pady=0, sticky="nsew")

class SetPointsFrame(ctk.CTkFrame):

    def __init__(self, master):
        super().__init__(master) 

        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)

        self.left_frame = ctk.CTkFrame(self)
        self.left_frame.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")

        self.label_co2 = ctk.CTkLabel(self.left_frame, text="CO2", font=ctk.CTkFont(weight="bold"))
        self.label_co2.grid(row=0, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.co2_button = ctk.CTkButton(self.left_frame, text="Encendido", fg_color="green", text_color_disabled="white", hover=False, command=self.co2_button_event)
        self.co2_button.grid(row=1, column=0, padx=10, pady=0, sticky="nsew")

        self.label_o2 = ctk.CTkLabel(self.left_frame, text="O2", font=ctk.CTkFont(weight="bold"))
        self.label_o2.grid(row=2, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.o2_button = ctk.CTkButton(self.left_frame, text="Encendido", fg_color="green", text_color_disabled="white", hover=False, command=self.o2_button_event)
        self.o2_button.grid(row=3, column=0, padx=10, pady=0, sticky="nsew")

        self.label_air = ctk.CTkLabel(self.left_frame, text="Aire", font=ctk.CTkFont(weight="bold"))
        self.label_air.grid(row=4, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.air_button = ctk.CTkButton(self.left_frame, text="Encendido", fg_color="green", text_color_disabled="white", hover=False, command=self.air_button_event)
        self.air_button.grid(row=5, column=0, padx=10, pady=0, sticky="nsew")

        self.label_pump = ctk.CTkLabel(self.left_frame, text="Bomba", font=ctk.CTkFont(weight="bold"))
        self.label_pump.grid(row=6  , column=0, padx=10, pady=(10,0), sticky="nsew")
        self.pump_button = ctk.CTkButton(self.left_frame, text="Encendido", fg_color="green", text_color_disabled="white", hover=False, command=self.pump_button_event)
        self.pump_button.grid(row=7, column=0, padx=10, pady=0, sticky="nsew")

        self.right_frame = ctk.CTkFrame(self)
        self.right_frame.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")

        self.label_light = ctk.CTkLabel(self.right_frame, text="Luz [%]", font=ctk.CTkFont(weight="bold"))
        self.label_light.grid(row=0, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.entry_light = ctk.CTkEntry(self.right_frame, justify="center")
        self.entry_light.insert(0, "42")
        self.entry_light.grid(row=1, column=0, padx=10, pady=0, sticky="nsew")

        self.label_ph = ctk.CTkLabel(self.right_frame, text="pH", font=ctk.CTkFont(weight="bold"))
        self.label_ph.grid(row=2, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.entry_ph = ctk.CTkEntry(self.right_frame, justify="center")
        self.entry_ph.insert(0, "6.536")
        self.entry_ph.grid(row=3, column=0, padx=10, pady=0, sticky="nsew")

        self.label_do = ctk.CTkLabel(self.right_frame, text="OD [%]", font=ctk.CTkFont(weight="bold"))
        self.label_do.grid(row=4, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.entry_do = ctk.CTkEntry(self.right_frame, justify="center")
        self.entry_do.insert(0, "342.4")
        self.entry_do.grid(row=5, column=0, padx=10, pady=0, sticky="nsew")

        self.label_temp = ctk.CTkLabel(self.right_frame, text="Temperatura [°C]", font=ctk.CTkFont(weight="bold"))
        self.label_temp.grid(row=6  , column=0, padx=10, pady=(10,0), sticky="nsew")
        self.entry_temp = ctk.CTkEntry(self.right_frame, justify="center")
        self.entry_temp.insert(0, "24.3")
        self.entry_temp.grid(row=7, column=0, padx=10, pady=0, sticky="nsew")

        self.send_button = ctk.CTkButton(self.right_frame, text="Enviar", command=self.send_button_event)
        self.send_button.grid(row=8, column=0, padx=10, pady=10, sticky="nsew")

    def co2_button_event(self):
        if self.co2_button.cget("text") == "Encendido":
            self.co2_button.configure(text="Apagado")
            self.co2_button.configure(fg_color="red")
        elif self.co2_button.cget("text") == "Apagado":
            self.co2_button.configure(text="Encendido")
            self.co2_button.configure(fg_color="green")

    def o2_button_event(self):
        if self.o2_button.cget("text") == "Encendido":
            self.o2_button.configure(text="Apagado")
            self.o2_button.configure(fg_color="red")
        elif self.o2_button.cget("text") == "Apagado":
            self.o2_button.configure(text="Encendido")
            self.o2_button.configure(fg_color="green")
    
    def air_button_event(self):
        if self.air_button.cget("text") == "Encendido":
            self.air_button.configure(text="Apagado")
            self.air_button.configure(fg_color="red")
        elif self.air_button.cget("text") == "Apagado":
            self.air_button.configure(text="Encendido")
            self.air_button.configure(fg_color="green")

    def pump_button_event(self):
        if self.pump_button.cget("text") == "Encendido":
            self.pump_button.configure(text="Apagado")
            self.pump_button.configure(fg_color="red")
        elif self.pump_button.cget("text") == "Apagado":
            self.pump_button.configure(text="Encendido")
            self.pump_button.configure(fg_color="green")

    def send_button_event(self):
        print(self.entry_light.cget("textvariable"))
        print(self.entry_ph.cget("textvariable"))
        print(self.entry_do.cget("textvariable"))
        print(self.entry_temp.cget("textvariable"))

class ManualRecordFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 

        image_path = os.path.join(os.getcwd(), "images")

        self.grid_columnconfigure(0, weight=1)

        self.label_control = ctk.CTkLabel(self, text="Control manual", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=20, pady=(10, 0), sticky="w")

        self.frame_buttons = ctk.CTkFrame(self)
        self.frame_buttons.grid(row=1, column=0, padx=20, pady=(10, 0), sticky="ew")

        self.frame_buttons.grid_rowconfigure(0, weight=1)
        self.frame_buttons.grid_columnconfigure(0, weight=1)
        self.frame_buttons.grid_columnconfigure(1, weight=1)
        self.frame_buttons.grid_columnconfigure(2, weight=1)

        self.play_pause_image = ctk.CTkImage(Image.open(os.path.join(image_path, "play.png")), size=(40, 40))
        self.play_pause_image_label = ctk.CTkLabel(self.frame_buttons, text="", image=self.play_pause_image)
        self.play_pause_image_label.grid(row=0, column=0, padx=5, pady=5, sticky="ew")

        self.stop_image = ctk.CTkImage(Image.open(os.path.join(image_path, "stop.png")), size=(40, 40))
        self.stop_image_label = ctk.CTkLabel(self.frame_buttons, text="", image=self.stop_image)
        self.stop_image_label.grid(row=0, column=1, padx=5, pady=5, sticky="ew")

        self.bin_image = ctk.CTkImage(Image.open(os.path.join(image_path, "bin.png")), size=(40, 40))
        self.bin_image_label = ctk.CTkLabel(self.frame_buttons, text="", image=self.stop_image)
        self.bin_image_label.grid(row=0, column=2, padx=5, pady=5, sticky="ew")

        self.frame_commands = ctk.CTkFrame(self)
        self.frame_commands.grid(row=2, column=0, padx=20, pady=(10, 0), sticky="ew")

        self.frame_commands.grid_rowconfigure(0, weight=1)
        self.frame_commands.grid_columnconfigure(0, weight=1)
        self.frame_commands.grid_columnconfigure(1, weight=1)
        self.frame_commands.grid_columnconfigure(2, weight=1)

        self.entry_interval = ctk.CTkLabel(self.frame_commands, text="Intervalo:")
        self.entry_interval.grid(row=0, column=0, padx=5, pady=5, sticky="ew")

        self.entry_interval = ctk.CTkEntry(self.frame_commands, placeholder_text="15", width=60)
        self.entry_interval.grid(row=0, column=1, padx=5, pady=5, sticky="ew")

        self.combobox_unit = ctk.CTkComboBox(self.frame_commands, state="readonly", values=["min", "seg"], width=80)
        self.combobox_unit.grid(row=0, column=2, padx=5, pady=5, sticky="ew")

        self.main_button_interval = ctk.CTkButton(master=self.frame_commands, text="Enviar", command=self.send_button_event, width=80)
        self.main_button_interval.grid(row=0, column=3, padx=5, pady=5, sticky="ew")

    def send_button_event(self):
        print("Enviar")

class ManualFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master, corner_radius=0, fg_color="transparent")
        
        self.grid_columnconfigure((0, 1, 2), weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=3)      
        
        self.setpoints_frame = SetPointsFrame(self)
        self.setpoints_frame.grid(row=0, column=0, padx=10, pady=(10, 0), sticky="nsew")

        self.instant_values_frame = InstantValuesFrame(self)
        self.instant_values_frame.grid(row=0, column=1, padx=10, pady=0, sticky="nsew")

        self.manual_record_frame = ManualRecordFrame(self)
        self.manual_record_frame.grid(row=0, column=2, padx=10, pady=(10, 0), sticky="nsew")

        self.log_frame = LogFrame(self)
        self.log_frame.grid(row=1, column=0, padx=10, pady=(10, 0), sticky="nsew", columnspan=3)

