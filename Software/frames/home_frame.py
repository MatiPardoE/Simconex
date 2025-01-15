import customtkinter as ctk
import tkinter
from PIL import Image
import os
import matplotlib.pyplot as plt
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, NavigationToolbar2Tk)
import matplotlib.dates as mdates
from matplotlib.ticker import MaxNLocator
from matplotlib.animation import FuncAnimation
import numpy as np
import csv
from datetime import datetime, timedelta
import random
import frames.serial_handler as ui_serial
from frames.serial_handler import MsgType 
from frames.serial_handler import CycleStatus 
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

        self.label_n2 = ctk.CTkLabel(self.left_frame, text="N2", font=ctk.CTkFont(weight="bold"))
        self.label_n2.grid(row=5  , column=0, padx=10, pady=(10,0), sticky="nsew")
        self.n2_button = ctk.CTkButton(self.left_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, state="disabled", width=100)
        self.n2_button.grid(row=6, column=0, padx=10, pady=(0,10), sticky="ns")

        self.label_air = ctk.CTkLabel(self.left_frame, text="Aire", font=ctk.CTkFont(weight="bold"))
        self.label_air.grid(row=7, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.air_button = ctk.CTkButton(self.left_frame, text="Desconectado", fg_color="orange", text_color_disabled="white", hover=False, state="disabled", width=100)
        self.air_button.grid(row=8, column=0, padx=10, pady=0, sticky="ns")

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

        if data == MsgType.NEW_MEASUREMENT or (data == MsgType.ESP_SYNCRONIZED and not ui_serial.cycle_status == CycleStatus.NOT_CYCLE): 
            self.light_button.configure(text = f"{data_lists['light'][-1]}")
            self.ph_button.configure(text = "{0:.2f}".format(data_lists['ph'][-1]))
            self.do_button.configure(text = "{0:.2f}".format(data_lists['od'][-1]))
            self.temp_button.configure(text = "{0:.2f}".format(data_lists['temperature'][-1]))   

            if data_lists['co2'][-1] == 0:
                self.co2_button.configure(text="Apagado")
                self.co2_button.configure(fg_color="red")
            else:
                self.co2_button.configure(text="Encendido")
                self.co2_button.configure(fg_color="green")
            if data_lists['o2'][-1] == 0:
                self.o2_button.configure(text="Apagado")
                self.o2_button.configure(fg_color="red")
            else:
                self.o2_button.configure(text="Encendido")
                self.o2_button.configure(fg_color="green")
            if data_lists['air'][-1] == 0:
                self.air_button.configure(text="Apagado")
                self.air_button.configure(fg_color="red")
            else:
                self.air_button.configure(text="Encendido")
                self.air_button.configure(fg_color="green")
            if data_lists['n2'][-1] == 0:
                self.n2_button.configure(text="Apagado")
                self.n2_button.configure(fg_color="red")
            else:
                self.n2_button.configure(text="Encendido")
                self.n2_button.configure(fg_color="green")

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
        self.n2_button.configure(text="Desconectado")
        self.n2_button.configure(fg_color="orange")

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
        if data == MsgType.ESP_CONNECTED:
            self.esp_connected()
            
        if (data == MsgType.ESP_SYNCRONIZED and not ui_serial.cycle_status == CycleStatus.NOT_CYCLE) or data == MsgType.NEW_CYCLE_SENT:
            total_time = len(data_lists_expected["id"]) * ui_serial.cycle_interval
            elapsed_time = len(data_lists["id"]) * ui_serial.cycle_interval
            restant_time = total_time - elapsed_time

            self.esp_connected() 

            if data == MsgType.NEW_CYCLE_SENT:
                self.reset_progressbar(total_time, elapsed_time, restant_time)  
            else:
                self.update_progressbar(total_time, elapsed_time, restant_time)  

        if data == MsgType.NEW_MEASUREMENT: 
            total_time = len(data_lists_expected["id"]) * ui_serial.cycle_interval
            elapsed_time = len(data_lists["id"]) * ui_serial.cycle_interval
            restant_time = total_time - elapsed_time

            self.update_progressbar(total_time, elapsed_time, restant_time)
        
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
    
    def update_progressbar(self, total_time, elapsed_time, restant_time):
        self.progressbar_actual.set(elapsed_time/total_time)
        self.label_actual_days.configure(text=self.format_seconds(total_time))
        self.label_done_text.configure(text=self.format_seconds(elapsed_time))
        self.label_left_text.configure(text=self.format_seconds(restant_time))
    
    def reset_progressbar(self, total_time, elapsed_time, restant_time):
        self.progressbar_actual.set(0)
        self.label_actual_days.configure(text=self.format_seconds(total_time))
        self.label_done_text.configure(text=self.format_seconds(elapsed_time))
        self.label_left_text.configure(text=self.format_seconds(restant_time))
    
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

        self.fig, self.ax = plt.subplots()
        self.line, = self.ax.plot([], [], 'r-')

        self.id_data = []
        self.ph_data = []
        self.od_data = []
        self.temp_data = []
        self.light_data = []
        self.datetime_axis = []
        self.datetime_axis_expected = []
        
        if var=="ph":
            self.ax.set_ylim(6, 10)
        elif var=="od":
            self.ax.set_ylabel("[%]")
            self.ax.set_ylim(0, 100)
        elif var=="temperature":
            self.ax.set_ylabel("[°C]")
            self.ax.set_ylim(10, 30)
        elif var=="light":
            self.ax.set_ylabel("[%]")
            self.ax.set_ylim(0, 100)
        
        self.canvas = FigureCanvasTkAgg(self.fig, master=master)  
        self.canvas.draw()

        toolbar = NavigationToolbar2Tk(self.canvas, master, pack_toolbar=False)
        toolbar.update()

        toolbar.pack(side=tkinter.BOTTOM, fill=tkinter.X)
        self.canvas.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=True)

    def reset_data(self):
        self.id_data = []
        self.ph_data = []
        self.od_data = []
        self.temp_data = []
        self.light_data = []
        self.datetime_axis = []
        self.datetime_axis_expected = []

    def update_plot(self, data): # TODO: esto tiene que appendear un dato solo al plot por cada medicion que llega nada mas 

        if data == MsgType.NEW_CYCLE_SENT:
            self.initial_time = datetime.strptime(ui_serial.cycle_id, "%Y%m%d_%H%M")  
            self.reset_data()    
            # TODO: aca tengo que limpiar lo que este posiblemente graficado
        
        if data == MsgType.ESP_SYNCRONIZED and not ui_serial.cycle_status == CycleStatus.NOT_CYCLE: # Aca es que grafico un ciclo que esta empezado y sigue funcionando
            self.initial_time = datetime.strptime(ui_serial.cycle_id, "%Y%m%d_%H%M")
            num_measurements = len(data_lists['id'])
            self.datetime_axis = [self.initial_time + timedelta(seconds=i * ui_serial.cycle_interval) for i in range(num_measurements)]

            self.line_expected, = self.ax.plot(self.datetime_axis, data_lists_expected[self.var][:num_measurements], label="Valores esperados")
            self.line, = self.ax.plot(self.datetime_axis, data_lists[self.var], label="Valores medidos")

            self.ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
            self.ax.xaxis.set_major_locator(MaxNLocator(nbins=10))
            self.fig.autofmt_xdate()

            self.ax.legend()  

        if data == MsgType.NEW_MEASUREMENT:
            num_measurements = len(data_lists['id'])
            if num_measurements == 1:
                new_time = self.initial_time + timedelta(seconds=ui_serial.cycle_interval)
            else:
                new_time = self.datetime_axis[-1] + timedelta(seconds=ui_serial.cycle_interval)

            self.datetime_axis.append(new_time)
            self.line.set_data(self.datetime_axis, data_lists[self.var])
            self.ax.set_xlim(self.datetime_axis[0], self.datetime_axis[-1])
            self.fig.canvas.draw_idle()
            
            #self.datetime_axis = [self.initial_time + timedelta(seconds=(i * ui_serial.cycle_interval)) for i in range(num_measurements)]
            #self.datetime_axis_expected = [self.initial_time + timedelta(seconds=(i * ui_serial.cycle_interval)) for i in range(num_measurements)]

            #if num_measurements == 1:
                

                #self.line_expected, = self.ax.plot(data_lists['id'], data_lists_expected[self.var][:num_measurements], label="Valores esperados")
                #self.line, = self.ax.plot(data_lists['id'], data_lists[self.var], label="Valores medidos")


            # self.line.set_xdata(data_lists['id'])            
            # self.line.set_ydata(data_lists[self.var])
            # #print(self.datetime_axis)

            # self.line_expected.set_xdata(data_lists['id'])            
            # self.line_expected.set_ydata(data_lists_expected[self.var][:num_measurements])
            # #print(self.datetime_axis_expected)

            # self.ax.relim()  
            # self.ax.autoscale_view()  
            
            # self.canvas.draw()

        

class PlotFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 
        ui_serial.publisher.subscribe(self.process_data_plot_frame)

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

        self.plot_light = MyPlot(self.tabview.tab("Luz"), "light")
        self.plot_ph = MyPlot(self.tabview.tab("pH"), "ph")
        self.plot_od = MyPlot(self.tabview.tab("OD"), "od")
        self.plot_temp = MyPlot(self.tabview.tab("Temperatura"), "temperature")
        
    def process_data_plot_frame(self, data):
        if data == MsgType.ESP_CONNECTED or data == MsgType.ESP_SYNCRONIZED or data == MsgType.NEW_CYCLE_SENT:
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