import customtkinter as ctk
import tkinter
from PIL import Image
import os
import matplotlib.pyplot as plt
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, NavigationToolbar2Tk)
import matplotlib.dates as mdates
import numpy as np
import csv
from datetime import datetime
import random

def read_datalog(fname):
    datetime_list = []
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
                    #print(line)
                    if column == "Fecha":
                        index['fecha'] = j
                    elif column == "Hora":
                        index['hora'] = j
                    elif column == "pH":
                        index['ph'] = j
                    elif column == "DO":
                        index['do'] = j
                    elif column == "Temperatura":
                        index['temperatura'] = j
                    elif column == "LED1":
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
                datetime_str = line[index.get("fecha")] + ' ' + line[index.get("hora")]
                datetime_single = datetime.strptime(datetime_str, '%d/%m/%Y %H:%M:%S')
                datetime_list.append(datetime_single)
                
                # Extraer los valores
                ph_list.append(float(line[index.get("ph")]))
                od_list.append(float(line[index.get("do")]))
                temp_list.append(float(line[index.get("temperatura")]))
                light_list.append(float(line[index.get("led1")]))  
    
    return datetime_list, ph_list, od_list, temp_list, light_list

class InstantValuesFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 

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
        self.co2_button = ctk.CTkButton(self.left_frame, text="Encendido", fg_color="green", text_color_disabled="white", hover=False, state="disabled", width=100)
        self.co2_button.grid(row=2, column=0, padx=10, pady=0, sticky="ns")

        self.label_o2 = ctk.CTkLabel(self.left_frame, text="O2", font=ctk.CTkFont(weight="bold"))
        self.label_o2.grid(row=3, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.o2_button = ctk.CTkButton(self.left_frame, text="Encendido", fg_color="green", text_color_disabled="white", hover=False, state="disabled", width=100)
        self.o2_button.grid(row=4, column=0, padx=10, pady=0, sticky="ns")

        self.label_air = ctk.CTkLabel(self.left_frame, text="Aire", font=ctk.CTkFont(weight="bold"))
        self.label_air.grid(row=5, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.air_button = ctk.CTkButton(self.left_frame, text="Encendido", fg_color="green", text_color_disabled="white", hover=False, state="disabled", width=100)
        self.air_button.grid(row=6, column=0, padx=10, pady=0, sticky="ns")

        self.label_pump = ctk.CTkLabel(self.left_frame, text="Bomba", font=ctk.CTkFont(weight="bold"))
        self.label_pump.grid(row=7  , column=0, padx=10, pady=(10,0), sticky="nsew")
        self.pump_button = ctk.CTkButton(self.left_frame, text="Encendido", fg_color="green", text_color_disabled="white", hover=False, state="disabled", width=100)
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
        self.light_button = ctk.CTkButton(self.right_frame, text="42%", fg_color="white", hover=False, state="disabled", text_color_disabled="black", width=100)
        self.light_button.grid(row=2, column=0, padx=10, pady=0, sticky="ns")

        self.label_ph = ctk.CTkLabel(self.right_frame, text="pH", font=ctk.CTkFont(weight="bold"))
        self.label_ph.grid(row=3, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.ph_button = ctk.CTkButton(self.right_frame, text="6.536", fg_color="white", hover=False, state="disabled", text_color_disabled="black", width=100)
        self.ph_button.grid(row=4, column=0, padx=10, pady=0, sticky="ns")

        self.label_do = ctk.CTkLabel(self.right_frame, text="OD", font=ctk.CTkFont(weight="bold"))
        self.label_do.grid(row=5, column=0, padx=10, pady=(10,0), sticky="nsew")
        self.do_button = ctk.CTkButton(self.right_frame, text="342.4", fg_color="white", hover=False, state="disabled", text_color_disabled="black", width=100)
        self.do_button.grid(row=6, column=0, padx=10, pady=0, sticky="ns")

        self.label_temp = ctk.CTkLabel(self.right_frame, text="Temperatura", font=ctk.CTkFont(weight="bold"))
        self.label_temp.grid(row=7  , column=0, padx=10, pady=(10,0), sticky="nsew")
        self.temp_button = ctk.CTkButton(self.right_frame, text="24.3°C", fg_color="white", hover=False, state="disabled", text_color_disabled="black", width=100)
        self.temp_button.grid(row=8, column=0, padx=10, pady=(0,10), sticky="ns")

class ActualCycleFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(4, weight=1)

        self.label_actual = ctk.CTkLabel(self, text="Ciclo Actual", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_actual.grid(row=0, column=0, padx=20, pady=(10, 0), sticky="w")

        self.label_actual_days = ctk.CTkLabel(self, text="10 Dias", font=ctk.CTkFont(size=18))
        self.label_actual_days.grid(row=1, column=0, padx=20, pady=(10, 10), sticky="w")

        self.progressbar_actual = ctk.CTkProgressBar(self)
        self.progressbar_actual.grid(row=2, column=0, padx=20, pady=(10, 10), sticky="ew")

        self.frame_actual = ctk.CTkFrame(self)
        self.frame_actual.grid(row=3, column=0, padx=20, pady=(10, 10), sticky="ew")

        self.frame_actual.grid_columnconfigure(0, weight=1)
        self.frame_actual.grid_columnconfigure(1, weight=1)
        self.frame_actual.grid_rowconfigure(0, weight=1)
        self.frame_actual.grid_rowconfigure(1, weight=1)

        self.label_done_colour = ctk.CTkLabel(self.frame_actual, text="Completo")
        self.label_done_colour.grid(row=0, column=0, padx=20, pady=0, sticky="nsew")

        self.label_left_colour = ctk.CTkLabel(self.frame_actual, text="Restante")
        self.label_left_colour.grid(row=0, column=1, padx=20, pady=0, sticky="nsew")

        self.label_done_text = ctk.CTkLabel(self.frame_actual, text="5 dias", font=ctk.CTkFont(size=15, weight="bold"))
        self.label_done_text.grid(row=1, column=0, padx=20, pady=0, sticky="nsew")

        self.label_left_text = ctk.CTkLabel(self.frame_actual, text="5 dias", font=ctk.CTkFont(size=15, weight="bold"))
        self.label_left_text.grid(row=1, column=1, padx=20, pady=0, sticky="nsew")
    
class MyPlot(ctk.CTkFrame):
    def __init__(self, master, var, datalog_meas, datalog_ideal):
        super().__init__(master)

        datetime_list_m, ph_list_m, od_list_m, temp_list_m, light_list_m = read_datalog(datalog_meas)
        datetime_list_i, ph_list_i, od_list_i, temp_list_i, light_list_i = read_datalog(datalog_ideal)

        fig = Figure(figsize=(5, 4), dpi=100)
        ax = fig.add_subplot()

        if var=="pH":
            ax.plot(datetime_list_i, ph_list_i, label="Valores esperados")
            ax.plot(datetime_list_m, [x * random.uniform(0.9, 1.1) for x in ph_list_i], label="Valores medidos")
        elif var=="OD":
            ax.plot(datetime_list_i, od_list_i, label="Valores esperados")
            ax.plot(datetime_list_m, [x * random.uniform(0.9, 1.1) for x in od_list_i], label="Valores medidos")
            ax.set_ylabel("[%]")
        elif var=="Temperatura":
            ax.plot(datetime_list_i, temp_list_i, label="Valores esperados")
            ax.plot(datetime_list_m, [x * random.uniform(0.9, 1.1) for x in temp_list_i], label="Valores medidos")
            ax.set_ylabel("[°C]")
        elif var=="Luz":
            ax.plot(datetime_list_i, light_list_i, label="Valores esperados")
            ax.plot(datetime_list_m, [x * random.uniform(0.9, 1.1) for x in light_list_i], label="Valores medidos")
            ax.set_ylabel("[%]")
        
        ax.legend()  
        locator = mdates.AutoDateLocator(minticks=7, maxticks=10)
        formatter = mdates.ConciseDateFormatter(locator)
        ax.xaxis.set_major_locator(locator)
        ax.xaxis.set_major_formatter(formatter)
        canvas = FigureCanvasTkAgg(fig, master=master)  
        canvas.draw()

        toolbar = NavigationToolbar2Tk(canvas, master, pack_toolbar=False)
        toolbar.update()

        toolbar.pack(side=tkinter.BOTTOM, fill=tkinter.X)
        canvas.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=True)

class PlotFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 

        datalog_path = os.path.join(os.getcwd(), "test_data")

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)

        self.tabview = ctk.CTkTabview(self)
        self.tabview.grid(row=0, column=0, padx=0, pady=0, sticky="nsew")
        self.tabview.add("Luz")
        self.tabview.add("pH")
        self.tabview.add("OD")            
        self.tabview.add("Temperatura")

        self.tabview.tab("Luz").grid_columnconfigure(0, weight=1) 
        self.tabview.tab("pH").grid_columnconfigure(0, weight=1)
        self.tabview.tab("OD").grid_columnconfigure(0, weight=1)
        self.tabview.tab("Temperatura").grid_columnconfigure(0, weight=1)

        x = np.arange(0, 3, .01)
        y_light = 2 * np.sin(2 * np.pi * x)
        y_ph = 2 * np.sin(2 * np.pi * 2 * x)
        y_od = 2 * np.sin(2 * np.pi * 3 * x)
        y_temp = 2 * np.sin(2 * np.pi * x)

        self.plot_light = MyPlot(self.tabview.tab("Luz"), "Luz", os.path.join(datalog_path, "datos_generados_logico.csv"), os.path.join(datalog_path, "datos_generados_logico.csv"))
        self.plot_ph = MyPlot(self.tabview.tab("pH"), "pH", os.path.join(datalog_path, "datos_generados_logico.csv"), os.path.join(datalog_path, "datos_generados_logico.csv"))
        self.plot_od = MyPlot(self.tabview.tab("OD"), "OD", os.path.join(datalog_path, "datos_generados_logico.csv"), os.path.join(datalog_path, "datos_generados_logico.csv"))
        self.plot_temp = MyPlot(self.tabview.tab("Temperatura"), "Temperatura", os.path.join(datalog_path, "datos_generados_logico.csv"), os.path.join(datalog_path, "datos_generados_logico.csv"))
        

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
        
