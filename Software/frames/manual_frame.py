import customtkinter as ctk
import tkinter
import os
from PIL import Image
import csv
from datetime import datetime, timedelta
import frames.serial_handler as ui_serial
import re
from frames.serial_handler import MsgType 
from frames.serial_handler import CycleStatus 
from frames.serial_handler import data_lists 
from frames.serial_handler import data_lists_manual
from tkinter import messagebox
import datetime

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
        if data == MsgType.NEW_CYCLE_SENT or data == MsgType.ESP_DISCONNECTED:
            for widget in self.scrollable_frame.winfo_children():
                widget.destroy()
            return    
        
        if data == MsgType.ESP_SYNCRONIZED and (ui_serial.cycle_status == CycleStatus.CYCLE_RUNNING or ui_serial.cycle_status == CycleStatus.CYCLE_FINISHED):
            num_measurements = len(data_lists_manual['id'])
            start_index = max(0, num_measurements - 50)

            for i in range(start_index, num_measurements):
                self.frame_line = ctk.CTkFrame(self.scrollable_frame)
                self.frame_line.pack(fill="x")

                self.in_frame = ctk.CTkFrame(self.frame_line)
                self.in_frame.pack(fill="x")

                date, hour = self.calculate_datetime(i)
                
                self.label_time = ctk.CTkLabel(self.in_frame, text=hour, corner_radius=0, width=150) 
                self.label_time.pack(side='left')

                self.label_date = ctk.CTkLabel(self.in_frame, text=date, corner_radius=0, width=200) 
                self.label_date.pack(side='left')

                self.label_od = ctk.CTkLabel(self.in_frame, text="{0:.2f}".format(data_lists_manual['od'][i]), corner_radius=0, width=150)
                self.label_od.pack(side='left')

                self.label_ph = ctk.CTkLabel(self.in_frame, text="{0:.2f}".format(data_lists_manual['ph'][i]), corner_radius=0, width=150)
                self.label_ph.pack(side='left')

                self.label_light = ctk.CTkLabel(self.in_frame, text=f"{data_lists_manual['light'][i]}", corner_radius=0, width=150)
                self.label_light.pack(side='left')

                self.label_temp = ctk.CTkLabel(self.in_frame, text="{0:.2f}".format(data_lists_manual['temperature'][i]), corner_radius=0, width=200)
                self.label_temp.pack(side='left')

                self.label_cycle = ctk.CTkLabel(self.in_frame, text=ui_serial.cycle_alias, corner_radius=0, width=150) 
                self.label_cycle.pack(side='left')

            self.scrollable_frame._parent_canvas.yview_moveto(1.0)
         
        if data == MsgType.NEW_MEASUREMENT: 
            num_measurements = len(data_lists_manual['id'])
            if num_measurements == 0:
                return

            last_index = num_measurements - 1  

            self.frame_line = ctk.CTkFrame(self.scrollable_frame)
            self.frame_line.pack(fill="x")

            self.in_frame = ctk.CTkFrame(self.frame_line)
            self.in_frame.pack(fill="x")

            self.label_time = ctk.CTkLabel(self.in_frame, text=datetime.datetime.now().strftime("%H:%M:%S"), corner_radius=0, width=150)
            self.label_time.pack(side='left')

            self.label_date = ctk.CTkLabel(self.in_frame, text=datetime.datetime.now().strftime("%d/%m/%Y"), corner_radius=0, width=200)
            self.label_date.pack(side='left')

            self.label_od = ctk.CTkLabel(self.in_frame, text="{0:.2f}".format(data_lists_manual['od'][last_index]), corner_radius=0, width=150)
            self.label_od.pack(side='left')

            self.label_ph = ctk.CTkLabel(self.in_frame, text="{0:.2f}".format(data_lists_manual['ph'][last_index]), corner_radius=0, width=150)
            self.label_ph.pack(side='left')

            self.label_light = ctk.CTkLabel(self.in_frame, text=f"{data_lists_manual['light'][last_index]}", corner_radius=0, width=150)
            self.label_light.pack(side='left')

            self.label_temp = ctk.CTkLabel(self.in_frame, text="{0:.2f}".format(data_lists_manual['temperature'][last_index]), corner_radius=0, width=200)
            self.label_temp.pack(side='left')

            if ui_serial.cycle_status == CycleStatus.CYCLE_RUNNING:
                self.label_cycle = ctk.CTkLabel(self.in_frame, text=ui_serial.cycle_alias, corner_radius=0, width=150)
            else:
                self.label_cycle = ctk.CTkLabel(self.in_frame, "-", corner_radius=0, width=150)
            self.label_cycle.pack(side='left')

            children = self.scrollable_frame.winfo_children()
            if len(children) > 50:
                children[0].destroy()  

            self.scrollable_frame._parent_canvas.yview_moveto(1.0)
        
        if data == MsgType.ESP_DISCONNECTED or data == MsgType.CYCLE_DELETED:
            for widget in self.scrollable_frame.winfo_children():
                widget.destroy()

    def calculate_datetime(self, intervals):
        initial_time = datetime.strptime(ui_serial.cycle_id, "%Y%m%d_%H%M")
        seconds_elapsed = intervals * ui_serial.cycle_interval
        current_time = initial_time + timedelta(seconds=seconds_elapsed)

        current_date = current_time.strftime("%d/%m/%Y") 
        current_hour = current_time.strftime("%H:%M:%S") 

        return current_date, current_hour

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
        if data == MsgType.ESP_DISCONNECTED:
            self.esp_disconnected() 

        if data == MsgType.NEW_MEASUREMENT and ui_serial.cycle_status == CycleStatus.CYCLE_MANUAL: 
            self.light_button.configure(text = f"{data_lists_manual['light'][-1]}")
            self.ph_button.configure(text = "{0:.2f}".format(data_lists_manual['ph'][-1]))
            self.do_button.configure(text = "{0:.2f}".format(data_lists_manual['od'][-1]))
            self.temp_button.configure(text = "{0:.2f}".format(data_lists_manual['temperature'][-1]))             

    def esp_disconnected(self):
        self.light_button.configure(text = "--")
        self.ph_button.configure(text = "--")
        self.do_button.configure(text = "--")
        self.temp_button.configure(text = "--")

class SetPointsFrame(ctk.CTkFrame):

    def __init__(self, master):
        super().__init__(master) 
        ui_serial.publisher.subscribe(self.process_data_set_points)

        self.validate = self.register(self.only_numbers)

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

        self.label_control = ctk.CTkLabel(self.left_frame, text="Control Válvulas", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=10, pady=(10, 0), sticky="nsew")

        self.label_co2 = ctk.CTkLabel(self.left_frame, text="CO2", font=ctk.CTkFont(weight="bold"))
        self.label_co2.grid(row=1, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.co2_button = ctk.CTkButton(self.left_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, command=self.co2_button_event, width=100, state="disabled")
        self.co2_button.grid(row=2, column=0, padx=10, pady=0, sticky="ns")

        self.label_o2 = ctk.CTkLabel(self.left_frame, text="O2", font=ctk.CTkFont(weight="bold"))
        self.label_o2.grid(row=3, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.o2_button = ctk.CTkButton(self.left_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, command=self.o2_button_event, width=100, state="disabled")
        self.o2_button.grid(row=4, column=0, padx=10, pady=0, sticky="ns")

        self.label_n2 = ctk.CTkLabel(self.left_frame, text="N2", font=ctk.CTkFont(weight="bold"))
        self.label_n2.grid(row=5, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.n2_button = ctk.CTkButton(self.left_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, command=self.n2_button_event, width=100, state="disabled")
        self.n2_button.grid(row=6, column=0, padx=10, pady=(0, 10), sticky="ns")

        self.label_air = ctk.CTkLabel(self.left_frame, text="Aire", font=ctk.CTkFont(weight="bold"))
        self.label_air.grid(row=7, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.air_button = ctk.CTkButton(self.left_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, command=self.air_button_event, width=100, state="disabled")
        self.air_button.grid(row=8, column=0, padx=10, pady=0, sticky="ns")

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
        self.right_frame.grid_columnconfigure(0, weight=1)

        self.label_control = ctk.CTkLabel(self.right_frame, text="Control Variables", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=10, pady=(10, 0), sticky="nsew")

        self.label_cold = ctk.CTkLabel(self.right_frame, text="Agua Fría", font=ctk.CTkFont(weight="bold"))
        self.label_cold.grid(row=1, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.cold_button = ctk.CTkButton(self.right_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, command=self.cold_button_event, width=100, state="disabled")
        self.cold_button.grid(row=2, column=0, padx=10, pady=0, sticky="ns")

        self.label_hot = ctk.CTkLabel(self.right_frame, text="Agua Caliente", font=ctk.CTkFont(weight="bold"))
        self.label_hot.grid(row=3, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.hot_button = ctk.CTkButton(self.right_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, command=self.hot_button_event, width=100, state="disabled")
        self.hot_button.grid(row=4, column=0, padx=10, pady=0, sticky="ns")

        self.light_text_var = tkinter.StringVar()
        self.label_light = ctk.CTkLabel(self.right_frame, text="Luz [%]", font=ctk.CTkFont(weight="bold"))
        self.label_light.grid(row=5, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.entry_light = ctk.CTkEntry(self.right_frame, textvariable=self.light_text_var, justify="center", width=100, state="disabled", validate="key", validatecommand=(self.validate, "%P"))
        self.entry_light.grid(row=6, column=0, padx=10, pady=0, sticky="ns")

        self.send_button = ctk.CTkButton(self.right_frame, text="Enviar", command=self.send_button_event, width=100, state="disabled")
        self.send_button.grid(row=7, column=0, padx=10, pady=(20, 10), sticky="ns")

    def process_data_set_points(self, data):
        if data == MsgType.ESP_CONNECTED:
            self.esp_connected()        
        
        if data == MsgType.ESP_DISCONNECTED:
            self.esp_disconnected() 

        if data == MsgType.NEW_MEASUREMENT and ui_serial.cycle_status == CycleStatus.CYCLE_MANUAL:
            self.update_buttons()

    def esp_connected(self):
        self.entry_light.configure(state = "normal")
        self.cold_button.configure(state = "normal")
        self.hot_button.configure(state = "normal")
        self.send_button.configure(state = "normal")

        self.co2_button.configure(state = "normal")
        self.o2_button.configure(state = "normal")
        self.air_button.configure(state = "normal")
        self.n2_button.configure(state = "normal")

        self.hot_button.configure(text="Apagado")
        self.hot_button.configure(fg_color="red")
        self.cold_button.configure(text="Apagado")
        self.cold_button.configure(fg_color="red")

        self.co2_button.configure(text="Apagado")
        self.co2_button.configure(fg_color="red")
        self.o2_button.configure(text="Apagado")
        self.o2_button.configure(fg_color="red")
        self.air_button.configure(text="Apagado")
        self.air_button.configure(fg_color="red")
        self.n2_button.configure(text="Apagado")
        self.n2_button.configure(fg_color="red")
    
    def esp_disconnected(self):
        self.entry_light.configure(state = "disabled")
        self.hot_button.configure(state = "disabled")
        self.cold_button.configure(state = "disabled")
        self.send_button.configure(state = "disabled") 

        self.co2_button.configure(state = "disabled")
        self.o2_button.configure(state = "disabled")
        self.air_button.configure(state = "disabled")
        self.n2_button.configure(state = "disabled")  

        self.hot_button.configure(text="Desconectado")
        self.hot_button.configure(fg_color="orange") 
        self.cold_button.configure(text="Desconectado")
        self.cold_button.configure(fg_color="orange")  

        self.co2_button.configure(text="Desconectado")
        self.co2_button.configure(fg_color="orange") 
        self.o2_button.configure(text="Desconectado")
        self.o2_button.configure(fg_color="orange") 
        self.air_button.configure(text="Desconectado")
        self.air_button.configure(fg_color="orange") 
        self.n2_button.configure(text="Desconectado")
        self.n2_button.configure(fg_color="orange")    

    def update_buttons(self):
        if data_lists_manual['co2'][-1] == 0:
            self.co2_button.configure(text="Apagado")
            self.co2_button.configure(fg_color="red")
        else:
            self.co2_button.configure(text="Encendido")
            self.co2_button.configure(fg_color="green")
        if data_lists_manual['o2'][-1] == 0:
            self.o2_button.configure(text="Apagado")
            self.o2_button.configure(fg_color="red")
        else:
            self.o2_button.configure(text="Encendido")
            self.o2_button.configure(fg_color="green")
        if data_lists_manual['air'][-1] == 0:
            self.air_button.configure(text="Apagado")
            self.air_button.configure(fg_color="red")
        else:
            self.air_button.configure(text="Encendido")
            self.air_button.configure(fg_color="green")
        if data_lists_manual['n2'][-1] == 0:
            self.n2_button.configure(text="Apagado")
            self.n2_button.configure(fg_color="red")
        else:
            self.n2_button.configure(text="Encendido")
            self.n2_button.configure(fg_color="green")

    def co2_button_event(self):
        if ui_serial.cycle_status != CycleStatus.CYCLE_RUNNING:
            ui_serial.cycle_status = CycleStatus.CYCLE_MANUAL
            if self.co2_button.cget("text") == "Encendido":
                ui_serial.publisher.send_data(b"#C0$")
                self.co2_button.configure(text="Apagado")
                self.co2_button.configure(fg_color="red")
            elif self.co2_button.cget("text") == "Apagado":
                ui_serial.publisher.send_data(b"#C1$")
                self.co2_button.configure(text="Encendido")
                self.co2_button.configure(fg_color="green")
        else: 
            messagebox.showwarning("Advertencia", "Para controlar manualmente, el ciclo debe estar pausado")

    def o2_button_event(self):
        if ui_serial.cycle_status != CycleStatus.CYCLE_RUNNING:
            ui_serial.cycle_status = CycleStatus.CYCLE_MANUAL
            if self.o2_button.cget("text") == "Encendido":
                ui_serial.publisher.send_data(b"#O0$")
                self.o2_button.configure(text="Apagado")
                self.o2_button.configure(fg_color="red")
            elif self.o2_button.cget("text") == "Apagado":
                ui_serial.publisher.send_data(b"#O1$")
                self.o2_button.configure(text="Encendido")
                self.o2_button.configure(fg_color="green")
        else: 
            messagebox.showwarning("Advertencia", "Para controlar manualmente, el ciclo debe estar pausado")
    
    def air_button_event(self):
        if ui_serial.cycle_status != CycleStatus.CYCLE_RUNNING:
            ui_serial.cycle_status = CycleStatus.CYCLE_MANUAL
            if self.air_button.cget("text") == "Encendido":
                ui_serial.publisher.send_data(b"#A0$")
                self.air_button.configure(text="Apagado")
                self.air_button.configure(fg_color="red")
            elif self.air_button.cget("text") == "Apagado":
                ui_serial.publisher.send_data(b"#A1$")
                self.air_button.configure(text="Encendido")
                self.air_button.configure(fg_color="green")
        else: 
            messagebox.showwarning("Advertencia", "Para controlar manualmente, el ciclo debe estar pausado")

    def n2_button_event(self):
        if ui_serial.cycle_status != CycleStatus.CYCLE_RUNNING:
            ui_serial.cycle_status = CycleStatus.CYCLE_MANUAL
            if self.n2_button.cget("text") == "Encendido":
                ui_serial.publisher.send_data(b"#N0$")
                self.n2_button.configure(text="Apagado")
                self.n2_button.configure(fg_color="red")
            elif self.n2_button.cget("text") == "Apagado":
                ui_serial.publisher.send_data(b"#N1$")
                self.n2_button.configure(text="Encendido")
                self.n2_button.configure(fg_color="green")
        else: 
            messagebox.showwarning("Advertencia", "Para controlar manualmente, el ciclo debe estar pausado")
    
    def cold_button_event(self):
        if ui_serial.cycle_status != CycleStatus.CYCLE_RUNNING:
            ui_serial.cycle_status = CycleStatus.CYCLE_MANUAL
            if self.cold_button.cget("text") == "Encendido":
                ui_serial.publisher.send_data(b"#COLD0$")
                self.cold_button.configure(text="Apagado")
                self.cold_button.configure(fg_color="red")
            elif self.cold_button.cget("text") == "Apagado":
                ui_serial.publisher.send_data(b"#HOT0$")
                self.hot_button.configure(text="Apagado")
                self.hot_button.configure(fg_color="red")
                ui_serial.publisher.send_data(b"#COLD1$")
                self.cold_button.configure(text="Encendido")
                self.cold_button.configure(fg_color="green")
        else: 
            messagebox.showwarning("Advertencia", "Para controlar manualmente, el ciclo debe estar pausado")
    
    def hot_button_event(self):
        if ui_serial.cycle_status != CycleStatus.CYCLE_RUNNING:
            ui_serial.cycle_status = CycleStatus.CYCLE_MANUAL
            if self.hot_button.cget("text") == "Encendido":
                ui_serial.publisher.send_data(b"#HOT0$")
                self.hot_button.configure(text="Apagado")
                self.hot_button.configure(fg_color="red")
            elif self.hot_button.cget("text") == "Apagado":
                ui_serial.publisher.send_data(b"#COLD0$")
                self.cold_button.configure(text="Apagado")
                self.cold_button.configure(fg_color="red")
                ui_serial.publisher.send_data(b"#HOT1$")
                self.hot_button.configure(text="Encendido")
                self.hot_button.configure(fg_color="green")
        else: 
            messagebox.showwarning("Advertencia", "Para controlar manualmente, el ciclo debe estar pausado")

    def send_button_event(self):
        if ui_serial.cycle_status != CycleStatus.CYCLE_RUNNING:
            val = int(self.entry_light.get())
            if val < 0 or val > 100:
                messagebox.showwarning("Advertencia", "El valor de luz debe ser porcentual")
            else:
                ui_serial.cycle_status = CycleStatus.CYCLE_MANUAL
                ui_serial.publisher.send_data(str.encode("#L{:03d}$".format(val)))
        else: 
            messagebox.showwarning("Advertencia", "Para controlar manualmente, el ciclo debe estar pausado")

    def only_numbers(self, text):
        return text.isdigit() or text == ""

class ManualFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master, corner_radius=0, fg_color="transparent")

        datalog_path = os.path.join(os.getcwd(), "test_data")
        
        self.grid_columnconfigure((0, 1), weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=3)      
        
        self.setpoints_frame = SetPointsFrame(self)
        self.setpoints_frame.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")

        self.instant_values_frame = InstantValuesFrame(self)
        self.instant_values_frame.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")

        self.log_frame = LogFrame(self)
        self.log_frame.grid(row=1, column=0, padx=10, pady=10, sticky="nsew", columnspan=3)
