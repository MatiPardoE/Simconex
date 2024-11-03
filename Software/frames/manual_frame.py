import customtkinter as ctk
import tkinter
import os
from PIL import Image
import csv
from datetime import datetime
import frames.serial_handler as ui_serial
import re

class LogFrame(ctk.CTkFrame):

    def __init__(self, master):
        super().__init__(master) 

        self.datetime_list = []
        self.ph_list = []
        self.od_list = []
        self.temp_list = []
        self.light_list = []

        ui_serial.publisher.subscribe(self.update_log_frame)

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1)

        self.label_control = ctk.CTkLabel(self, text="Registro de datos", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=(20, 10), pady=(10, 0), sticky="w")

        self.frame_lines = ctk.CTkFrame(self, width=1500)
        self.frame_lines.grid(row=1, column=0, padx=10, pady=10)
        self.frame_lines.grid_columnconfigure(0, weight=1)
        self.frame_lines.grid_rowconfigure(0, weight=1)

        self.frame_line = ctk.CTkFrame(self.frame_lines)
        self.frame_line.grid(row=0, column=0, padx=0, pady=0, sticky="ew")

        self.frame_line.grid_rowconfigure(0, weight=1)
        self.frame_line.grid_columnconfigure(0, weight=1)
        self.frame_line.grid_columnconfigure(1, weight=1)
        self.frame_line.grid_columnconfigure(2, weight=1)
        self.frame_line.grid_columnconfigure(3, weight=1)
        self.frame_line.grid_columnconfigure(4, weight=1)
        self.frame_line.grid_columnconfigure(5, weight=1)
        self.frame_line.grid_columnconfigure(6, weight=1)

        self.label_time = ctk.CTkLabel(self.frame_line, text="Hora", corner_radius=0, width=150, font=ctk.CTkFont(size=15, weight="bold"))
        self.label_time.grid(row=0, column=0, padx=5, pady=0, sticky="ns")

        self.label_time = ctk.CTkLabel(self.frame_line, text="Fecha", corner_radius=0, width=200, font=ctk.CTkFont(size=15, weight="bold"))
        self.label_time.grid(row=0, column=1, padx=5, pady=0, sticky="ns")

        self.label_time = ctk.CTkLabel(self.frame_line, text="OD [%]", corner_radius=0, width=150, font=ctk.CTkFont(size=15, weight="bold"))
        self.label_time.grid(row=0, column=2, padx=5, pady=0, sticky="ns")

        self.label_time = ctk.CTkLabel(self.frame_line, text="pH", corner_radius=0, width=150, font=ctk.CTkFont(size=15, weight="bold"))
        self.label_time.grid(row=0, column=3, padx=5, pady=0, sticky="ns")

        self.label_time = ctk.CTkLabel(self.frame_line, text="Luz [%]", corner_radius=0, width=150, font=ctk.CTkFont(size=15, weight="bold"))
        self.label_time.grid(row=0, column=4, padx=5, pady=0, sticky="ns")

        self.label_time = ctk.CTkLabel(self.frame_line, text="Temperatura [°C]", corner_radius=0, width=200, font=ctk.CTkFont(size=15, weight="bold"))
        self.label_time.grid(row=0, column=5, padx=5, pady=0, sticky="ns")

        self.label_time = ctk.CTkLabel(self.frame_line, text="Ciclo", corner_radius=0, width=150, font=ctk.CTkFont(size=15, weight="bold"))
        self.label_time.grid(row=0, column=6, padx=5, pady=0, sticky="ns")

        self.frame_log = ctk.CTkFrame(self.frame_lines)
        self.frame_log.grid(row=1, column=0, padx=5, pady=0, sticky="ew")

        self.scrollable_frame = ctk.CTkScrollableFrame(self.frame_log, width=1500)
        self.scrollable_frame.pack(pady=10, padx=10, fill="both", expand=True)   
  
    def update_log_frame(self, data):
        if ui_serial.state_fbr.get("state") == "running":
            pattern = r"^(\d{8}),(\d{2}\.\d{2}),(\d{3}\.\d{2}),(\d{2}\.\d{2}),(\d{2})$" # linea de log
            match = re.match(pattern, data)
            if match: 
                self.light_list.insert(0, int(match.group(5)))
                self.ph_list.insert(0, float(match.group(2)))
                self.od_list.insert(0, float(match.group(3)))
                self.temp_list.insert(0, float(match.group(4)))
            
                self.frame_line = ctk.CTkFrame(self.scrollable_frame)
                self.frame_line.pack(fill="x")

                self.in_frame = ctk.CTkFrame(self.frame_line)
                self.in_frame.pack(fill="x")
                
                self.label_time = ctk.CTkLabel(self.in_frame, text="12:30", corner_radius=0, width=150)
                self.label_time.pack(side='left')

                self.label_date = ctk.CTkLabel(self.in_frame, text="11/10/2024", corner_radius=0, width=200)
                self.label_date.pack(side='left')

                self.label_od = ctk.CTkLabel(self.in_frame, text=match.group(3), corner_radius=0, width=150)
                self.label_od.pack(side='left')

                self.label_ph = ctk.CTkLabel(self.in_frame, text=match.group(2), corner_radius=0, width=150)
                self.label_ph.pack(side='left')

                self.label_light = ctk.CTkLabel(self.in_frame, text=match.group(5), corner_radius=0, width=150)
                self.label_light.pack(side='left')

                self.label_temp = ctk.CTkLabel(self.in_frame, text=match.group(4), corner_radius=0, width=200)
                self.label_temp.pack(side='left')

                self.label_cycle = ctk.CTkLabel(self.in_frame, text="Ciclo1", corner_radius=0, width=150)
                self.label_cycle.pack(side='left')

                self.scrollable_frame._parent_canvas.yview_moveto(1.0)

class InstantValuesFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 

        ui_serial.publisher.subscribe(self.process_data_instant_values)
        image_path = os.path.join(os.getcwd(), "images")

        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)

        self.left_frame = ctk.CTkFrame(self)
        self.left_frame.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")

        self.left_frame.grid_rowconfigure(0, weight=1)
        self.left_frame.grid_columnconfigure(0, weight=1)

        self.reactor_img = ctk.CTkImage(Image.open(os.path.join(image_path, "reactor_edited.png")), size=(150, 300))
        self.reactor_img_label = ctk.CTkLabel(self.left_frame, text="", image=self.reactor_img)
        self.reactor_img_label.grid(row=0, column=0, padx=0, pady=0, sticky="nsew")

        self.right_frame = ctk.CTkFrame(self)
        self.right_frame.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")     

        self.right_frame.grid_rowconfigure(0, weight=1)
        self.right_frame.grid_rowconfigure(1, weight=1)
        self.right_frame.grid_rowconfigure(2, weight=1)
        self.right_frame.grid_rowconfigure(3, weight=1)
        self.right_frame.grid_rowconfigure(4, weight=1)
        self.right_frame.grid_rowconfigure(5, weight=1)
        self.right_frame.grid_rowconfigure(6, weight=1)
        self.right_frame.grid_rowconfigure(7, weight=1)
        self.right_frame.grid_rowconfigure(8, weight=1)
        self.right_frame.grid_columnconfigure(0, weight=1)   

        self.label_control = ctk.CTkLabel(self.right_frame, text="Estado Variables", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=10, pady=(10, 0), sticky="nsew")

        self.label_light = ctk.CTkLabel(self.right_frame, text="Luz [%]", font=ctk.CTkFont(weight="bold"))
        self.label_light.grid(row=1, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.light_button = ctk.CTkButton(self.right_frame, text="--", fg_color="white", hover=False, state="disabled", text_color_disabled="black", width=100)
        self.light_button.grid(row=2, column=0, padx=10, pady=0, sticky="ns")

        self.label_ph = ctk.CTkLabel(self.right_frame, text="pH", font=ctk.CTkFont(weight="bold"))
        self.label_ph.grid(row=3, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.ph_button = ctk.CTkButton(self.right_frame, text="--", fg_color="white", hover=False, state="disabled", text_color_disabled="black", width=100)
        self.ph_button.grid(row=4, column=0, padx=10, pady=0, sticky="ns")

        self.label_do = ctk.CTkLabel(self.right_frame, text="OD [%]", font=ctk.CTkFont(weight="bold"))
        self.label_do.grid(row=5, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.do_button = ctk.CTkButton(self.right_frame, text="--", fg_color="white", hover=False, state="disabled", text_color_disabled="black", width=100)
        self.do_button.grid(row=6, column=0, padx=10, pady=0, sticky="ns")

        self.label_temp = ctk.CTkLabel(self.right_frame, text="Temperatura [°C]", font=ctk.CTkFont(weight="bold"))
        self.label_temp.grid(row=7, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.temp_button = ctk.CTkButton(self.right_frame, text="--", fg_color="white", hover=False, state="disabled", text_color_disabled="black", width=100)
        self.temp_button.grid(row=8, column=0, padx=10, pady=(0, 10), sticky="ns")

    def process_data_instant_values(self, data):
        if "#Z1!" in data:
            self.esp_disconnected()

        pattern = r"^(\d{8}),(\d{2}\.\d{2}),(\d{3}\.\d{2}),(\d{2}\.\d{2}),(\d{2})$" # linea de log
        match = re.match(pattern, data)
        
        if match:
            self.light_button.configure(text = f"{int(match.group(5))}")
            self.ph_button.configure(text = "{0:.2f}".format(float(match.group(2))))
            self.do_button.configure(text = "{0:.2f}".format(float(match.group(3))))
            self.temp_button.configure(text = "{0:.2f}".format(float(match.group(4))))            
    
    def esp_disconnected(self):
        self.light_button.configure(text = "--")
        self.ph_button.configure(text = "--")
        self.do_button.configure(text = "--")
        self.temp_button.configure(text = "--")

class SetPointsFrame(ctk.CTkFrame):

    def __init__(self, master):
        super().__init__(master) 
        ui_serial.publisher.subscribe(self.process_data_set_points)

        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)

        self.left_frame = ctk.CTkFrame(self)
        self.left_frame.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")

        self.left_frame.grid_rowconfigure(0, weight=1)
        self.left_frame.grid_rowconfigure(1, weight=1)
        self.left_frame.grid_rowconfigure(2, weight=1)
        self.left_frame.grid_rowconfigure(3, weight=1)
        self.left_frame.grid_rowconfigure(4, weight=1)
        self.left_frame.grid_rowconfigure(5, weight=1)
        self.left_frame.grid_rowconfigure(6, weight=1)
        self.left_frame.grid_rowconfigure(7, weight=1)
        self.left_frame.grid_rowconfigure(8, weight=1)
        self.left_frame.grid_columnconfigure(0, weight=1)

        self.label_control = ctk.CTkLabel(self.left_frame, text="Control Salidas", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=10, pady=(10, 0), sticky="nsew")

        self.label_co2 = ctk.CTkLabel(self.left_frame, text="CO2", font=ctk.CTkFont(weight="bold"))
        self.label_co2.grid(row=1, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.co2_button = ctk.CTkButton(self.left_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, command=self.co2_button_event, width=100, state="disabled")
        self.co2_button.grid(row=2, column=0, padx=10, pady=0, sticky="ns")

        self.label_o2 = ctk.CTkLabel(self.left_frame, text="O2", font=ctk.CTkFont(weight="bold"))
        self.label_o2.grid(row=3, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.o2_button = ctk.CTkButton(self.left_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, command=self.o2_button_event, width=100, state="disabled")
        self.o2_button.grid(row=4, column=0, padx=10, pady=0, sticky="ns")

        self.label_air = ctk.CTkLabel(self.left_frame, text="Aire", font=ctk.CTkFont(weight="bold"))
        self.label_air.grid(row=5, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.air_button = ctk.CTkButton(self.left_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, command=self.air_button_event, width=100, state="disabled")
        self.air_button.grid(row=6, column=0, padx=10, pady=0, sticky="ns")

        self.label_pump = ctk.CTkLabel(self.left_frame, text="Bomba", font=ctk.CTkFont(weight="bold"))
        self.label_pump.grid(row=7, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.pump_button = ctk.CTkButton(self.left_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, command=self.pump_button_event, width=100, state="disabled")
        self.pump_button.grid(row=8, column=0, padx=10, pady=(0, 10), sticky="ns")

        self.right_frame = ctk.CTkFrame(self)
        self.right_frame.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")

        self.right_frame.grid_rowconfigure(0, weight=1)
        self.right_frame.grid_rowconfigure(1, weight=1)
        self.right_frame.grid_rowconfigure(2, weight=1)
        self.right_frame.grid_rowconfigure(3, weight=1)
        self.right_frame.grid_rowconfigure(4, weight=1)
        self.right_frame.grid_rowconfigure(5, weight=1)
        self.right_frame.grid_rowconfigure(6, weight=1)
        self.right_frame.grid_rowconfigure(7, weight=1)
        self.right_frame.grid_rowconfigure(8, weight=1)
        self.right_frame.grid_rowconfigure(9, weight=1)
        self.right_frame.grid_columnconfigure(0, weight=1)

        self.label_control = ctk.CTkLabel(self.right_frame, text="Control Variables", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=10, pady=(10, 0), sticky="nsew")

        self.light_text_var = tkinter.StringVar()
        self.label_light = ctk.CTkLabel(self.right_frame, text="Luz [%]", font=ctk.CTkFont(weight="bold"))
        self.label_light.grid(row=1, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.entry_light = ctk.CTkEntry(self.right_frame, textvariable=self.light_text_var, justify="center", width=100, state="disabled")
        self.entry_light.grid(row=2, column=0, padx=10, pady=0, sticky="ns")

        self.ph_text_var = tkinter.StringVar()
        self.label_ph = ctk.CTkLabel(self.right_frame, text="pH", font=ctk.CTkFont(weight="bold"))
        self.label_ph.grid(row=3, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.entry_ph = ctk.CTkEntry(self.right_frame, textvariable=self.ph_text_var, justify="center", width=100, state="disabled")
        self.entry_ph.grid(row=4, column=0, padx=10, pady=0, sticky="ns")

        self.do_text_var = tkinter.StringVar()
        self.label_do = ctk.CTkLabel(self.right_frame, text="OD [%]", font=ctk.CTkFont(weight="bold"))
        self.label_do.grid(row=5, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.entry_do = ctk.CTkEntry(self.right_frame, textvariable=self.do_text_var, justify="center", width=100, state="disabled")
        self.entry_do.grid(row=6, column=0, padx=10, pady=0, sticky="ns")

        self.temp_text_var = tkinter.StringVar()
        self.label_temp = ctk.CTkLabel(self.right_frame, text="Temperatura [°C]", font=ctk.CTkFont(weight="bold"))
        self.label_temp.grid(row=7, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.entry_temp = ctk.CTkEntry(self.right_frame, textvariable=self.temp_text_var, justify="center", width=100, state="disabled")
        self.entry_temp.grid(row=8, column=0, padx=10, pady=0, sticky="ns")

        self.send_button = ctk.CTkButton(self.right_frame, text="Enviar", command=self.send_button_event, width=100, state="disabled")
        self.send_button.grid(row=9, column=0, padx=10, pady=(20, 10), sticky="ns")

    def process_data_set_points(self, data):
        pattern = r"#(STA)([012])\!"
        if re.match(pattern, data):
            self.esp_connected()
        
        if "#Z1!" in data:
            self.esp_disconnected()

        pattern = r"#([LPTDCOWAZ])(\d+)\!"
        matches = re.findall(pattern, data)
        msg_list = [{'id': m[0], 'value': int(m[1])} for m in matches]
        self.update_data(msg_list)

    def esp_connected(self):
        self.entry_light.configure(state = "normal")
        self.entry_ph.configure(state = "normal")
        self.entry_do.configure(state = "normal")
        self.entry_temp.configure(state = "normal")
        self.send_button.configure(state = "normal")
    
    def esp_disconnected(self):
        self.entry_light.configure(state = "disabled")
        self.entry_ph.configure(state = "disabled")
        self.entry_do.configure(state = "disabled")
        self.entry_temp.configure(state = "disabled")
        self.send_button.configure(state = "disabled")        

    def update_data(self, msg_list):
        for msg in msg_list:
            id_msg = msg['id']
            value = msg['value']
            
            if id_msg == 'C':
                if value == 0:
                    self.co2_button.configure(text="Apagado")
                    self.co2_button.configure(fg_color="red")
                    self.co2_button.configure(state="normal")
                elif value == 1:
                    self.co2_button.configure(text="Encendido")
                    self.co2_button.configure(fg_color="green")
                    self.co2_button.configure(state="normal")
            elif id_msg == 'O':
                if value == 0:
                    self.o2_button.configure(text="Apagado")
                    self.o2_button.configure(fg_color="red")
                    self.o2_button.configure(state="normal")
                elif value == 1:
                    self.o2_button.configure(text="Encendido")
                    self.o2_button.configure(fg_color="green")
                    self.o2_button.configure(state="normal")
            elif id_msg == 'A':
                if value == 0:
                    self.air_button.configure(text="Apagado")
                    self.air_button.configure(fg_color="red")
                    self.air_button.configure(state="normal")
                elif value == 1:
                    self.air_button.configure(text="Encendido")
                    self.air_button.configure(fg_color="green")
                    self.air_button.configure(state="normal")
            elif id_msg == 'W':
                if value == 0:
                    self.pump_button.configure(text="Apagado")
                    self.pump_button.configure(fg_color="red")
                    self.pump_button.configure(state="normal")
                elif value == 1:
                    self.pump_button.configure(text="Encendido")
                    self.pump_button.configure(fg_color="green")
                    self.pump_button.configure(state="normal")
            elif id_msg == 'Z':
                self.co2_button.configure(text="Desconectado")
                self.co2_button.configure(fg_color="orange")
                self.co2_button.configure(state="disabled")
                self.o2_button.configure(text="Desconectado")
                self.o2_button.configure(fg_color="orange")
                self.o2_button.configure(state="disabled")
                self.air_button.configure(text="Desconectado")
                self.air_button.configure(fg_color="orange")
                self.air_button.configure(state="disabled")
                self.pump_button.configure(text="Desconectado")
                self.pump_button.configure(fg_color="orange")
                self.pump_button.configure(state="disabled")

    def co2_button_event(self):
        if self.co2_button.cget("text") == "Encendido":
            ui_serial.publisher.send_data(b"#C0!")
            self.co2_button.configure(text="Apagado")
            self.co2_button.configure(fg_color="red")
        elif self.co2_button.cget("text") == "Apagado":
            ui_serial.publisher.send_data(b"#C1!")
            self.co2_button.configure(text="Encendido")
            self.co2_button.configure(fg_color="green")

    def o2_button_event(self):
        if self.o2_button.cget("text") == "Encendido":
            ui_serial.publisher.send_data(b"#O0!")
            self.o2_button.configure(text="Apagado")
            self.o2_button.configure(fg_color="red")
        elif self.o2_button.cget("text") == "Apagado":
            ui_serial.publisher.send_data(b"#O1!")
            self.o2_button.configure(text="Encendido")
            self.o2_button.configure(fg_color="green")
    
    def air_button_event(self):
        if self.air_button.cget("text") == "Encendido":
            ui_serial.publisher.send_data(b"#A0!")
            self.air_button.configure(text="Apagado")
            self.air_button.configure(fg_color="red")
        elif self.air_button.cget("text") == "Apagado":
            ui_serial.publisher.send_data(b"#A1!")
            self.air_button.configure(text="Encendido")
            self.air_button.configure(fg_color="green")

    def pump_button_event(self):
        if self.pump_button.cget("text") == "Encendido":
            ui_serial.publisher.send_data(b"#W0!")
            self.pump_button.configure(text="Apagado")
            self.pump_button.configure(fg_color="red")
        elif self.pump_button.cget("text") == "Apagado":
            ui_serial.publisher.send_data(b"#W1!")
            self.pump_button.configure(text="Encendido")
            self.pump_button.configure(fg_color="green")

    def send_button_event(self):
        ui_serial.publisher.send_data(str.encode(f"#L{int(self.entry_light.get())}!"))
        ui_serial.publisher.send_data(str.encode(f"#P{int(float(self.entry_ph.get())*100)}!"))
        ui_serial.publisher.send_data(str.encode(f"#O{int(float(self.entry_do.get())*100)}!"))
        ui_serial.publisher.send_data(str.encode(f"#T{int(float(self.entry_temp.get())*100)}!"))

class ManualRecordFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 
        ui_serial.publisher.subscribe(self.process_data_manual_record)

        image_path = os.path.join(os.getcwd(), "images")

        self.grid_columnconfigure(0, weight=1)

        self.label_control = ctk.CTkLabel(self, text="Control manual", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=20, pady=(10, 0), sticky="w")

        self.frame_buttons = ctk.CTkFrame(self, width=100)
        self.frame_buttons.grid(row=1, column=0, padx=20, pady=(10, 0))#, sticky="ew")

        self.frame_buttons.grid_rowconfigure(0, weight=1)
        self.frame_buttons.grid_columnconfigure(0, weight=1)
        self.frame_buttons.grid_columnconfigure(1, weight=1)
        self.frame_buttons.grid_columnconfigure(2, weight=1)

        self.play_image = ctk.CTkImage(Image.open(os.path.join(image_path, "play.png")), size=(40, 40))
        self.pause_image = ctk.CTkImage(Image.open(os.path.join(image_path, "pause.png")), size=(40, 40))

        self.play_pause_image_label = ctk.CTkLabel(self.frame_buttons, text="", image=self.play_image)
        self.play_pause_image_label.grid(row=0, column=0, padx=15, pady=5, sticky="ew")

        self.is_playing = True

        self.stop_image = ctk.CTkImage(Image.open(os.path.join(image_path, "stop.png")), size=(40, 40))
        self.stop_image_label = ctk.CTkLabel(self.frame_buttons, text="", image=self.stop_image)
        self.stop_image_label.grid(row=0, column=1, padx=15, pady=5, sticky="ew")


        self.bin_image = ctk.CTkImage(Image.open(os.path.join(image_path, "bin.png")), size=(40, 40))
        self.bin_image_label = ctk.CTkLabel(self.frame_buttons, text="", image=self.bin_image)
        self.bin_image_label.grid(row=0, column=2, padx=15, pady=5, sticky="ew")

        self.frame_commands = ctk.CTkFrame(self)
        self.frame_commands.grid(row=2, column=0, padx=20, pady=(10, 0), sticky="ew")

        self.frame_commands.grid_rowconfigure(0, weight=1)
        self.frame_commands.grid_columnconfigure(0, weight=1)
        self.frame_commands.grid_columnconfigure(1, weight=1)
        self.frame_commands.grid_columnconfigure(2, weight=1)
        self.frame_commands.grid_columnconfigure(3, weight=1)
        self.frame_commands.grid_columnconfigure(4, weight=1)

        self.entry_interval = ctk.CTkLabel(self.frame_commands, text="Intervalo:", justify="right")
        self.entry_interval.grid(row=0, column=0, padx=5, pady=5, sticky="ew")

        self.entry_interval = ctk.CTkEntry(self.frame_commands, placeholder_text="15", width=60, state="disabled")
        self.entry_interval.grid(row=0, column=1, padx=5, pady=5, sticky="ew")

        self.radio_var = tkinter.IntVar(value=0)
        self.radio_button_seg = ctk.CTkRadioButton(master=self.frame_commands, text="seg", variable=self.radio_var, value=0, width=60, state="disabled")
        self.radio_button_seg.grid(row=0, column=2, padx=0, pady=5)
        self.radio_button_min = ctk.CTkRadioButton(master=self.frame_commands, text="min", variable=self.radio_var, value=1, width=60, state="disabled")
        self.radio_button_min.grid(row=0, column=3, padx=0, pady=5)

        self.main_button_interval = ctk.CTkButton(master=self.frame_commands, text="Enviar", command=self.send_button_event, width=80, state="disabled")
        self.main_button_interval.grid(row=0, column=4, padx=5, pady=5, sticky="ew")

    def process_data_manual_record(self, data):
        pattern = r"#(STA)([012])\!"
        if re.match(pattern, data):
            self.esp_connected()
        
        if "#Z1!" in data:
            self.esp_disconnected()

    def esp_connected(self):
        self.play_pause_image_label.bind("<Enter>", self.on_hover)
        self.play_pause_image_label.bind("<Leave>", self.off_hover)
        self.play_pause_image_label.bind("<Button-1>", self.play_pause_event)
        self.is_playing = True

        self.stop_image_label.bind("<Enter>", self.on_hover)
        self.stop_image_label.bind("<Leave>", self.off_hover)

        self.bin_image_label.bind("<Enter>", self.on_hover)
        self.bin_image_label.bind("<Leave>", self.off_hover)

        self.main_button_interval.configure(state = "normal")
        self.entry_interval.configure(state = "normal")
        self.radio_button_seg.configure(state = "normal")
        self.radio_button_min.configure(state = "normal")

    def esp_disconnected(self):
        self.play_pause_image_label.unbind("<Enter>")
        self.play_pause_image_label.unbind("<Leave>")
        self.play_pause_image_label.unbind("<Button-1>")
        self.is_playing = True

        self.stop_image_label.unbind("<Enter>")
        self.stop_image_label.unbind("<Leave>")

        self.bin_image_label.unbind("<Enter>")
        self.bin_image_label.unbind("<Leave>")

        self.main_button_interval.configure(state = "disabled")
        self.entry_interval.configure(state = "disabled")
        self.radio_button_seg.configure(state = "disabled")
        self.radio_button_min.configure(state = "disabled")

    def on_hover(self, event):
        self.play_pause_image_label.configure(cursor="hand2") 
        self.stop_image_label.configure(cursor="hand2") 
        self.bin_image_label.configure(cursor="hand2") 

    def off_hover(self, event):
        self.play_pause_image_label.configure(cursor="arrow") 
        self.stop_image_label.configure(cursor="arrow") 
        self.bin_image_label.configure(cursor="arrow") 
    
    def play_pause_event(self, event):
        if self.is_playing:
            self.play_pause_image_label.configure(image=self.pause_image)
        else:
            self.play_pause_image_label.configure(image=self.play_image)
        self.is_playing = not self.is_playing

    def send_button_event(self):
        print("Enviar")

class ManualFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master, corner_radius=0, fg_color="transparent")

        datalog_path = os.path.join(os.getcwd(), "test_data")
        
        self.grid_columnconfigure((0, 1, 2), weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=3)      
        
        self.setpoints_frame = SetPointsFrame(self)
        self.setpoints_frame.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")

        self.instant_values_frame = InstantValuesFrame(self)
        self.instant_values_frame.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")

        self.manual_record_frame = ManualRecordFrame(self)
        self.manual_record_frame.grid(row=0, column=2, padx=10, pady=10, sticky="nsew")

        self.log_frame = LogFrame(self)#, os.path.join(datalog_path, "datos_generados_logico.csv"))
        self.log_frame.grid(row=1, column=0, padx=10, pady=10, sticky="nsew", columnspan=3)
