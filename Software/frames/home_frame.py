import customtkinter as ctk
import tkinter
from PIL import Image
import os
import matplotlib.pyplot as plt
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, NavigationToolbar2Tk)
import matplotlib.dates as mdates
from matplotlib.animation import FuncAnimation
import numpy as np
import csv
from datetime import datetime
import random
import frames.serial_handler as ui_serial
from frames.serial_handler import MsgType 
from frames.serial_handler import data_lists 
from frames.serial_handler import data_lists_expected 
import re
from enum import Enum
import time

class HandshakeStatus(Enum):
    MSG_VALID = 7
    TIMESTAMP = 6
    DATAOUT1 = 5
    DATAOUT0 = 4
    ID1 = 3
    ID0 = 2
    OK = 1
    NOT_YET = 0
    TIMEOUT = -1
    ERROR = -2
    DATA_FAIL = -3

def read_datalog(fname):
    id_list = []
    ph_list = []
    od_list = []
    temp_list = []
    light_list = []
    index = {}
    
    with open(fname, newline='') as csvfile:
        handler = csv.reader(csvfile)
        for i, line in enumerate(handler):
            if i==0:
                for j, column in enumerate(line):
                    if column == "ID":
                        index['id'] = j
                    elif column == "pH":
                        index['ph'] = j
                    elif column == "OD":
                        index['do'] = j
                    elif column == "Temperatura":
                        index['temperatura'] = j
                    elif column == "Luz":
                        index['led1'] = j
                    elif column == "LED2":
                        index['led2'] = j
                    elif column == "LED3":
                        index['led3'] = j
                    elif column == "LED4":
                        index['led4'] = j
                    elif column == "LED5":
                        index['led5'] = j
            else: 
                id_list.append(int(line[index.get("id")]))                
                ph_list.append(float(line[index.get("ph")]))
                od_list.append(float(line[index.get("do")]))
                temp_list.append(float(line[index.get("temperatura")]))
                light_list.append(float(line[index.get("led1")]))  
    
    return id_list, ph_list, od_list, temp_list, light_list

class InstantValuesFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 

        ui_serial.publisher.subscribe(self.process_data_instant_values)
        image_path = os.path.join(os.getcwd(), "images")

        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)
        self.grid_columnconfigure(2, weight=1)        

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

        self.label_control = ctk.CTkLabel(self.left_frame, text="Estado Salidas", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=10, pady=(10, 0), sticky="nsew")

        self.label_co2 = ctk.CTkLabel(self.left_frame, text="CO2", font=ctk.CTkFont(weight="bold"))
        self.label_co2.grid(row=1, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.co2_button = ctk.CTkButton(self.left_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, state="disabled", width=100)
        self.co2_button.grid(row=2, column=0, padx=10, pady=0, sticky="ns")

        self.label_o2 = ctk.CTkLabel(self.left_frame, text="O2", font=ctk.CTkFont(weight="bold"))
        self.label_o2.grid(row=3, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.o2_button = ctk.CTkButton(self.left_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, state="disabled", width=100)
        self.o2_button.grid(row=4, column=0, padx=10, pady=0, sticky="ns")

        self.label_air = ctk.CTkLabel(self.left_frame, text="Aire", font=ctk.CTkFont(weight="bold"))
        self.label_air.grid(row=5, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.air_button = ctk.CTkButton(self.left_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, state="disabled", width=100)
        self.air_button.grid(row=6, column=0, padx=10, pady=0, sticky="ns")

        self.label_pump = ctk.CTkLabel(self.left_frame, text="Bomba", font=ctk.CTkFont(weight="bold"))
        self.label_pump.grid(row=7  , column=0, padx=10, pady=(10,0), sticky="nsew")
        self.pump_button = ctk.CTkButton(self.left_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, state="disabled", width=100)
        self.pump_button.grid(row=8, column=0, padx=10, pady=(0,10), sticky="ns")

        self.center_frame = ctk.CTkFrame(self)
        self.center_frame.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")

        self.center_frame.grid_rowconfigure(0, weight=1)
        self.center_frame.grid_columnconfigure(0, weight=1)

        self.reactor_img = ctk.CTkImage(Image.open(os.path.join(image_path, "reactor_edited.png")), size=(150, 300))
        self.reactor_img_label = ctk.CTkLabel(self.center_frame, text="", image=self.reactor_img)
        self.reactor_img_label.grid(row=0, column=0, padx=0, pady=0, sticky="nsew")

        self.right_frame = ctk.CTkFrame(self)
        self.right_frame.grid(row=0, column=2, padx=10, pady=10, sticky="nsew")

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

        self.label_light = ctk.CTkLabel(self.right_frame, text="Luz", font=ctk.CTkFont(weight="bold"))
        self.label_light.grid(row=1, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.light_button = ctk.CTkButton(self.right_frame, text="--%", fg_color="white", hover=False, state="disabled", text_color_disabled="black", width=100)
        self.light_button.grid(row=2, column=0, padx=10, pady=0, sticky="ns")

        self.label_ph = ctk.CTkLabel(self.right_frame, text="pH", font=ctk.CTkFont(weight="bold"))
        self.label_ph.grid(row=3, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.ph_button = ctk.CTkButton(self.right_frame, text="--", fg_color="white", hover=False, state="disabled", text_color_disabled="black", width=100)
        self.ph_button.grid(row=4, column=0, padx=10, pady=0, sticky="ns")

        self.label_do = ctk.CTkLabel(self.right_frame, text="OD", font=ctk.CTkFont(weight="bold"))
        self.label_do.grid(row=5, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.do_button = ctk.CTkButton(self.right_frame, text="--%", fg_color="white", hover=False, state="disabled", text_color_disabled="black", width=100)
        self.do_button.grid(row=6, column=0, padx=10, pady=0, sticky="ns")

        self.label_temp = ctk.CTkLabel(self.right_frame, text="Temperatura", font=ctk.CTkFont(weight="bold"))
        self.label_temp.grid(row=7  , column=0, padx=10, pady=(10,0), sticky="nsew")
        self.temp_button = ctk.CTkButton(self.right_frame, text="--°C", fg_color="white", hover=False, state="disabled", text_color_disabled="black", width=100)
        self.temp_button.grid(row=8, column=0, padx=10, pady=(0,10), sticky="ns")
    
    def process_data_instant_values(self, data):
        if data == MsgType.ESP_DISCONNECTED:
            self.esp_disconnected()

        if data == MsgType.NEW_MEASUREMENT:  
            self.light_button.configure(text = f"{data_lists['light'][-1]}")
            self.ph_button.configure(text = "{0:.2f}".format(data_lists['ph'][-1]))
            self.do_button.configure(text = "{0:.2f}".format(data_lists['od'][-1]))
            self.temp_button.configure(text = "{0:.2f}".format(data_lists['temperature'][-1]))            
                
            # if match.group(6) == '0':
            #         self.co2_button.configure(text="Apagado")
            #         self.co2_button.configure(fg_color="red")
            # elif match.group(6) == '1':
            #         self.co2_button.configure(text="Encendido")
            #         self.co2_button.configure(fg_color="green")
            # if match.group(7) == '0':
            #     self.o2_button.configure(text="Apagado")
            #     self.o2_button.configure(fg_color="red")
            # elif match.group(7) == '1':
            #     self.o2_button.configure(text="Encendido")
            #     self.o2_button.configure(fg_color="green")
            # if match.group(8) == '0':
            #     self.air_button.configure(text="Apagado")
            #     self.air_button.configure(fg_color="red")
            # elif match.group(8) == '1':
            #     self.air_button.configure(text="Encendido")
            #     self.air_button.configure(fg_color="green")
            # if match.group(9) == '0':
            #     self.pump_button.configure(text="Apagado")
            #     self.pump_button.configure(fg_color="red")
            # elif match.group(9) == '1':
            #     self.pump_button.configure(text="Encendido")
            #     self.pump_button.configure(fg_color="green")

    def esp_disconnected(self):
        self.light_button.configure(text = "--")
        self.ph_button.configure(text = "--")
        self.do_button.configure(text = "--")
        self.temp_button.configure(text = "--")
        self.co2_button.configure(text="Desconectado")
        self.co2_button.configure(fg_color="orange")
        self.o2_button.configure(text="Desconectado")
        self.o2_button.configure(fg_color="orange")
        self.air_button.configure(text="Desconectado")
        self.air_button.configure(fg_color="orange")
        self.pump_button.configure(text="Desconectado")
        self.pump_button.configure(fg_color="orange")

class ActualCycleFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master)         
        ui_serial.publisher.subscribe(self.process_data_actual_cycle)

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(4, weight=1)

        self.label_actual = ctk.CTkLabel(self, text="Ciclo Actual", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_actual.grid(row=0, column=0, padx=20, pady=(10, 0), sticky="w")

        self.label_actual_days = ctk.CTkLabel(self, text="", font=ctk.CTkFont(size=18))
        self.label_actual_days.grid(row=1, column=0, padx=20, pady=(10, 10), sticky="w")
        self.label_actual_days.grid_forget()

        self.progressbar_actual = ctk.CTkProgressBar(self)
        self.progressbar_actual.grid(row=2, column=0, padx=20, pady=(10, 10), sticky="ew")
        self.progressbar_actual.grid_forget()

        self.frame_actual = ctk.CTkFrame(self)
        self.frame_actual.grid(row=3, column=0, padx=20, pady=(10, 10), sticky="ew")
        self.frame_actual.grid_forget()

        self.frame_actual.grid_columnconfigure(0, weight=1)
        self.frame_actual.grid_columnconfigure(1, weight=1)
        self.frame_actual.grid_rowconfigure(0, weight=1)
        self.frame_actual.grid_rowconfigure(1, weight=1)

        self.label_done_colour = ctk.CTkLabel(self.frame_actual, text="Completo")
        self.label_done_colour.grid(row=0, column=0, padx=20, pady=0, sticky="nsew")
        self.label_done_colour.grid_forget()

        self.label_left_colour = ctk.CTkLabel(self.frame_actual, text="Restante")
        self.label_left_colour.grid(row=0, column=1, padx=20, pady=0, sticky="nsew")
        self.label_left_colour.grid_forget()

        self.label_done_text = ctk.CTkLabel(self.frame_actual, text="", font=ctk.CTkFont(size=15, weight="bold"))
        self.label_done_text.grid(row=1, column=0, padx=20, pady=0, sticky="nsew")
        self.label_done_text.grid_forget()

        self.label_left_text = ctk.CTkLabel(self.frame_actual, text="", font=ctk.CTkFont(size=15, weight="bold"))
        self.label_left_text.grid(row=1, column=1, padx=20, pady=0, sticky="nsew")
        self.label_left_text.grid_forget()

    def process_data_actual_cycle(self, data):    
        if data == MsgType.ESP_SYNCRONIZED:
            self.esp_connected()  

        if data == MsgType.NEW_MEASUREMENT:
            self.update_progressbar()
        
        if data == MsgType.ESP_DISCONNECTED:
            self.esp_disconnected() 
    
    def esp_connected(self):
        self.label_actual_days.grid(row=1, column=0, padx=20, pady=(10, 10), sticky="w")
        self.progressbar_actual.grid(row=2, column=0, padx=20, pady=(10, 10), sticky="ew")
        self.frame_actual.grid(row=3, column=0, padx=20, pady=(10, 10), sticky="ew")
        self.label_done_colour.grid(row=0, column=0, padx=20, pady=0, sticky="nsew")
        self.label_left_colour.grid(row=0, column=1, padx=20, pady=0, sticky="nsew")
        self.label_done_text.grid(row=1, column=0, padx=20, pady=0, sticky="nsew")
        self.label_left_text.grid(row=1, column=1, padx=20, pady=0, sticky="nsew")

        total_time = len(data_lists_expected["id"]) * 30 # TODO: hardcodeado a 30 segundos
        elapsed_time = len(data_lists["id"]) * 30
        restant_time = total_time - elapsed_time

        self.progressbar_actual.set(elapsed_time/total_time)
        self.label_actual_days.configure(text=self.format_seconds(total_time))
        self.label_done_text.configure(text=self.format_seconds(elapsed_time))
        self.label_left_text.configure(text=self.format_seconds(restant_time))

    def format_seconds(self, seconds):
        if seconds >= 86400:  
            days = seconds // 86400
            return f"{days} día(s)"
        elif seconds >= 3600:  
            hours = seconds // 3600
            return f"{hours} hora(s)"
        elif seconds >= 60:  
            minutes = seconds // 60
            return f"{minutes} minuto(s)"
        else:
            return f"{seconds} segundo(s)"
    
    def update_progressbar(self): #TODO
        print("TODO: update progress bar")
    
    def esp_disconnected(self):
        self.label_actual_days.grid_forget()
        self.progressbar_actual.grid_forget()
        self.frame_actual.grid_forget()
        self.label_done_colour.grid_forget()
        self.label_left_colour.grid_forget()
        self.label_done_text.grid_forget()
        self.label_left_text.grid_forget()
    
class MyPlot(ctk.CTkFrame):
    def __init__(self, master, var):
        super().__init__(master)
        ui_serial.publisher.subscribe(self.update_plot)
        self.var = var
        id_list_i, ph_list_i, od_list_i, temp_list_i, light_list_i = read_datalog("Log/test_1.csv") # TODO: volar esto a la mierda y poner que se lean los datos de donde corresponde

        self.fig, self.ax = plt.subplots()
        self.line, = self.ax.plot([], [], 'r-')

        self.id_data = []
        self.ph_data = []
        self.od_data = []
        self.temp_data = []
        self.light_data = []
        
        # self.ax.set_xlim(0, 30)
        
        # if var=="ph":
        #     #self.ph_line, = self.ax.plot([], [], 'r-', label="Valores medidos")
        #     self.ax.set_ylim(0, 14)
        #     #self.ax.plot(id_list_i, [x * random.uniform(0.9, 1.1) for x in ph_list_i], label="Valores esperados")
        # elif var=="od":
        #     #self.od_line, = self.ax.plot([], [], 'r-', label="Valores medidos")
        #     #self.ax.set_ylabel("[%]")
        #     self.ax.set_ylim(0, 100)
        #     #self.ax.plot(id_list_i, [x * random.uniform(0.9, 1.1) for x in od_list_i], label="Valores esperados")
        # elif var=="temperature":
        #     #self.temp_line, = self.ax.plot([], [], 'r-', label="Valores medidos")
        #     #self.ax.set_ylabel("[°C]")
        #     self.ax.set_ylim(10, 30)
        #     #self.ax.plot(id_list_i, [x * random.uniform(0.9, 1.1) for x in temp_list_i], label="Valores esperados")
        # elif var=="light":
        #     #self.light_line, = self.ax.plot([], [], 'r-', label="Valores medidos")
        #     #self.ax.set_ylabel("[%]")
        #     self.ax.set_ylim(0, 100)
        #     #self.ax.plot(id_list_i, [x * random.uniform(0.9, 1.1) for x in light_list_i], label="Valores esperados")
        
        # self.ax.legend()  
        # locator = mdates.AutoDateLocator(minticks=7, maxticks=10)
        # formatter = mdates.ConciseDateFormatter(locator)
        # ax.xaxis.set_major_locator(locator)
        # ax.xaxis.set_major_formatter(formatter)
        self.canvas = FigureCanvasTkAgg(self.fig, master=master)  
        self.canvas.draw()

        toolbar = NavigationToolbar2Tk(self.canvas, master, pack_toolbar=False)
        toolbar.update()

        toolbar.pack(side=tkinter.BOTTOM, fill=tkinter.X)
        self.canvas.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=True)

        #self.update_plot(var)

    def update_plot(self, data): # TODO: esto tiene que appendear un dato solo al plot por cada medicion que llega nada mas 
        
        if data == MsgType.ESP_SYNCRONIZED:
            self.ax.plot(data_lists_expected['id'], data_lists_expected[self.var], label="Valores esperados")
            self.ax.plot(data_lists['id'], data_lists[self.var], label="Valores medidos")

        # if data == MsgType.NEW_MEASUREMENT:
        #     if var=="pH":
        #         self.ph_line.set_ydata(self.ph_data)
        #         self.ph_line.set_xdata(self.id_data)
        #     elif var=="OD":
        #         self.od_line.set_ydata(self.od_data)
        #         self.od_line.set_xdata(self.id_data)
        #     elif var=="Temperatura":
        #         self.temp_line.set_ydata(self.temp_data)
        #         self.temp_line.set_xdata(self.id_data)
        #     elif var=="Luz":
        #         self.light_line.set_ydata(self.light_data)  
        #         self.light_line.set_xdata(self.id_data)
            
            self.canvas.draw()
        # self.master.after(30*1000, lambda: self.update_plot(var))


class PlotFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 
        ui_serial.publisher.subscribe(self.process_data_plot_frame)

        datalog_path = os.path.join(os.getcwd(), "test_data")

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)

        self.tabview = ctk.CTkTabview(self)
        self.tabview.grid(row=0, column=0, padx=0, pady=0, sticky="nsew")
        self.tabview.grid_forget()
        self.tabview.add("Luz")
        self.tabview.add("pH")
        self.tabview.add("OD")            
        self.tabview.add("Temperatura")

        self.tabview.tab("Luz").grid_columnconfigure(0, weight=1) 
        self.tabview.tab("pH").grid_columnconfigure(0, weight=1)
        self.tabview.tab("OD").grid_columnconfigure(0, weight=1)
        self.tabview.tab("Temperatura").grid_columnconfigure(0, weight=1)

        self.plot_light = MyPlot(self.tabview.tab("Luz"), "light")#, os.path.join(datalog_path, "datos_generados_logico.csv"), os.path.join(datalog_path, "datos_generados_logico.csv"))
        self.plot_ph = MyPlot(self.tabview.tab("pH"), "ph")#, os.path.join(datalog_path, "datos_generados_logico.csv"), os.path.join(datalog_path, "datos_generados_logico.csv"))
        self.plot_od = MyPlot(self.tabview.tab("OD"), "od")#, os.path.join(datalog_path, "datos_generados_logico.csv"), os.path.join(datalog_path, "datos_generados_logico.csv"))
        self.plot_temp = MyPlot(self.tabview.tab("Temperatura"), "temperature")#, os.path.join(datalog_path, "datos_generados_logico.csv"), os.path.join(datalog_path, "datos_generados_logico.csv"))
        
    def process_data_plot_frame(self, data):
        if data == MsgType.ESP_CONNECTED:
            self.esp_connected()        
        
        if data == MsgType.ESP_DISCONNECTED:
            self.esp_disconnected()  

    def esp_connected(self):
        self.tabview.grid(row=0, column=0, padx=0, pady=0, sticky="nsew")

    def esp_disconnected(self):
        self.tabview.grid_forget()

class HomeFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master, corner_radius=0, fg_color="transparent")    

        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1)

        self.instant_values_frame = InstantValuesFrame(self)
        self.instant_values_frame.grid(row=0, column=0, padx=10, pady=(10, 0), sticky="nsew")

        self.actual_cycle_frame = ActualCycleFrame(self)
        self.actual_cycle_frame.grid(row=0, column=1, padx=10, pady=(10, 0), sticky="nsew")

        self.plot1_frame = PlotFrame(self)
        self.plot1_frame.grid(row=1, column=0, padx=10, pady=(10, 0), sticky="nsew")

        self.plot2_frame = PlotFrame(self)
        self.plot2_frame.grid(row=1, column=1, padx=10, pady=(10, 0), sticky="nsew")