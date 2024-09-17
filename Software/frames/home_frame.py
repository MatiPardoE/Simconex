import customtkinter as ctk
from PIL import Image
import os
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np

class InstantValuesFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 

        self.grid_columnconfigure((0, 1, 2), weight=0)

        left_frame = ctk.CTkFrame(self, fg_color="blue")
        left_frame.grid(row=0, column=0, padx=0, pady=(0, 20), sticky="w")
        center_frame = ctk.CTkFrame(self, fg_color="green")
        center_frame.grid(row=0, column=1, padx=0, pady=(0, 20), sticky="w")
        right_frame = ctk.CTkFrame(self, fg_color="pink")
        right_frame.grid(row=0, column=2, padx=0, pady=(0, 20), sticky="w")

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

class ActualCycleFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(4, weight=1)

        self.label_actual = ctk.CTkLabel(self, text="Ciclo Actual", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_actual.grid(row=0, column=0, padx=(20, 10), pady=(10, 0), sticky="w")

        self.label_actual_days = ctk.CTkLabel(self, text="10 Dias", font=ctk.CTkFont(size=18))
        self.label_actual_days.grid(row=1, column=0, padx=(20, 10), pady=(10, 10), sticky="w")

        self.progressbar_actual = ctk.CTkProgressBar(self)
        self.progressbar_actual.grid(row=2, column=0, padx=(20, 10), pady=(10, 10), sticky="ew")

        self.frame_actual = ctk.CTkFrame(self)
        self.frame_actual.grid(row=3, column=0, padx=(20, 10), pady=(10, 10), sticky="ew")

        self.frame_actual.grid_columnconfigure((0, 1), weight=1)
        self.frame_actual.grid_rowconfigure((0, 1), weight=1)

        self.label_done_colour = ctk.CTkLabel(self.frame_actual, text="Done")
        self.label_done_colour.grid(row=0, column=0, padx=20, pady=0, sticky="w")

        self.label_left_colour = ctk.CTkLabel(self.frame_actual, text="Left")
        self.label_left_colour.grid(row=0, column=1, padx=20, pady=0, sticky="w")

        self.label_done_text = ctk.CTkLabel(self.frame_actual, text="5 dias", font=ctk.CTkFont(size=15, weight="bold"))
        self.label_done_text.grid(row=1, column=0, padx=20, pady=0, sticky="w")

        self.label_left_text = ctk.CTkLabel(self.frame_actual, text="5 dias", font=ctk.CTkFont(size=15, weight="bold"))
        self.label_left_text.grid(row=1, column=1, padx=20, pady=0, sticky="w")
    
class MyPlot(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master)

        # generate random numbers for the plot
        x,y,s,c = np.random.rand(4,100)
        # generate the figure and plot object which will be linked to the root element
        fig, ax = plt.subplots()
        fig.set_size_inches(8,4)
        ax.scatter(x,y,s*50,c)
        ax.axis("off")
        fig.subplots_adjust(left=0, right=1, bottom=0, top=1, wspace=0, hspace=0)
        canvas = FigureCanvasTkAgg(fig, master=master)
        canvas.draw()
        canvas.get_tk_widget().grid(row=0, column=0, padx=0, pady=0, sticky="nsew")

class PlotFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 

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

        self.plot_light = MyPlot(self.tabview.tab("Luz"))
        self.plot_ph = MyPlot(self.tabview.tab("pH"))
        self.plot_od = MyPlot(self.tabview.tab("OD"))
        self.plot_temp = MyPlot(self.tabview.tab("Temperatura"))
        

class HomeFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master, corner_radius=0, fg_color="transparent")    

        self.grid_columnconfigure((0, 1), weight=1)
        self.grid_rowconfigure((0, 1), weight=1)

        self.instant_values_frame = InstantValuesFrame(self)
        self.instant_values_frame.grid(row=0, column=0, padx=10, pady=(10, 0), sticky="nsew")

        self.actual_cycle_frame = ActualCycleFrame(self)
        self.actual_cycle_frame.grid(row=0, column=1, padx=10, pady=(10, 0), sticky="nsew")

        self.plot1_frame = PlotFrame(self)
        self.plot1_frame.grid(row=1, column=0, padx=10, pady=(10, 0), sticky="nsew")

        self.plot2_frame = PlotFrame(self)
        self.plot2_frame.grid(row=1, column=1, padx=10, pady=(10, 0), sticky="nsew")
        
