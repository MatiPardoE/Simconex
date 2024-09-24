import customtkinter as ctk
import tkinter
from PIL import Image
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import os
import csv
from datetime import datetime

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

class ControlCycleFrame(ctk.CTkFrame):
    
    def __init__(self, master):
        super().__init__(master) 

        def callback(event):
            image_path = os.path.join(os.getcwd(), "images")
        #     play_image = ctk.CTkImage(Image.open(os.path.join(image_path, "play.png")), size=(40, 40))
        #     pause_image = ctk.CTkImage(Image.open(os.path.join(image_path, "pause.png")), size=(40, 40))

        #     tk_image = ctk.Ctkim(pause_image)

        #     #if event.widget.cget("image") == play_image:
        #     #event.widget.config(image=pause_image)
        #     #else:
        #     #    event.widget.config(image=play_image)

        image_path = os.path.join(os.getcwd(), "images")

        self.grid_columnconfigure(0, weight=1)

        self.label_control = ctk.CTkLabel(self, text="Control del ciclo", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=20, pady=(10, 0), sticky="w")

        self.frame_buttons = ctk.CTkFrame(self, width=100)
        self.frame_buttons.grid(row=1, column=0, padx=20, pady=(10, 0))

        self.frame_buttons.grid_rowconfigure(0, weight=1)
        self.frame_buttons.grid_columnconfigure(0, weight=1)
        self.frame_buttons.grid_columnconfigure(1, weight=1)
        self.frame_buttons.grid_columnconfigure(2, weight=1)
        
        self.play_image = ctk.CTkImage(Image.open(os.path.join(image_path, "play.png")), size=(40, 40))
        self.pause_image = ctk.CTkImage(Image.open(os.path.join(image_path, "pause.png")), size=(40, 40))

        self.play_pause_image_label = ctk.CTkLabel(self.frame_buttons, text="", image=self.play_image)
        self.play_pause_image_label.grid(row=0, column=0, padx=15, pady=5, sticky="ns")

        self.play_pause_image_label.bind("<Enter>", self.on_hover)
        self.play_pause_image_label.bind("<Leave>", self.off_hover)
        self.play_pause_image_label.bind("<Button-1>", self.play_pause_event)
        self.is_playing = True

        self.stop_image = ctk.CTkImage(Image.open(os.path.join(image_path, "stop.png")), size=(40, 40))
        self.stop_image_label = ctk.CTkLabel(self.frame_buttons, text="", image=self.stop_image)
        self.stop_image_label.grid(row=0, column=1, padx=15, pady=5, sticky="ns")

        self.stop_image_label.bind("<Enter>", self.on_hover)
        self.stop_image_label.bind("<Leave>", self.off_hover)


        self.bin_image = ctk.CTkImage(Image.open(os.path.join(image_path, "bin.png")), size=(40, 40))
        self.bin_image_label = ctk.CTkLabel(self.frame_buttons, text="", image=self.bin_image)
        self.bin_image_label.grid(row=0, column=2, padx=15, pady=5, sticky="ns")

        self.bin_image_label.bind("<Enter>", self.on_hover)
        self.bin_image_label.bind("<Leave>", self.off_hover)

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

        self.entry_interval = ctk.CTkEntry(self.frame_commands, placeholder_text="15", width=60)
        self.entry_interval.grid(row=0, column=1, padx=5, pady=5, sticky="ew")

        self.radio_var = tkinter.IntVar(value=0)
        self.radio_button_seg = ctk.CTkRadioButton(master=self.frame_commands, text="seg", variable=self.radio_var, value=0, width=60)
        self.radio_button_seg.grid(row=0, column=2, padx=0, pady=5)
        self.radio_button_min = ctk.CTkRadioButton(master=self.frame_commands, text="min", variable=self.radio_var, value=1, width=60)
        self.radio_button_min.grid(row=0, column=3, padx=0, pady=5)

        self.main_button_interval = ctk.CTkButton(master=self.frame_commands, text="Enviar", command=self.send_button_event, width=80)
        self.main_button_interval.grid(row=0, column=4, padx=5, pady=5, sticky="ew")

    def on_hover(self, event):
        self.play_pause_image_label.configure(cursor="hand2") 
        self.stop_image_label.configure(cursor="hand2") 
        self.bin_image_label.configure(cursor="hand2") 

    def off_hover(self, event):
        self.play_pause_image_label.configure(cursor="arrow") 
        self.stop_image_label.configure(cursor="hand2") 
        self.bin_image_label.configure(cursor="hand2") 
    
    def play_pause_event(self, event):
        if self.is_playing:
            self.play_pause_image_label.configure(image=self.pause_image)
        else:
            self.play_pause_image_label.configure(image=self.play_image)
        self.is_playing = not self.is_playing
    
    def send_button_event(self):
        print("Enviar")
   
class LogFrame(ctk.CTkScrollableFrame):

    def __init__(self, master, fname):
        super().__init__(master) 

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1)

        self.label_control = ctk.CTkLabel(self, text="Registro de datos", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=(20, 10), pady=(10, 0), sticky="w")

        datetime_list, ph_list, od_list, temp_list, light_list = read_datalog(fname)

        self.frame_lines = ctk.CTkFrame(self, width=1500)
        self.frame_lines.grid(row=1, column=0, padx=10, pady=10)
        self.frame_lines.grid_columnconfigure(0, weight=1)
        self.frame_lines.grid_rowconfigure(0, weight=1)

        self.frame_line = ctk.CTkFrame(self.frame_lines)
        self.frame_line.grid(row=0, column=0, padx=5, pady=0, sticky="ew")

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

        for i in range(len(datetime_list)):
            self.frame_line = ctk.CTkFrame(self.frame_lines)
            self.frame_line.grid(row=i+1, column=0, padx=5, pady=0, sticky="ew")

            self.frame_line.grid_rowconfigure(0, weight=1)
            self.frame_line.grid_columnconfigure(0, weight=1)
            self.frame_line.grid_columnconfigure(1, weight=1)
            self.frame_line.grid_columnconfigure(2, weight=1)
            self.frame_line.grid_columnconfigure(3, weight=1)
            self.frame_line.grid_columnconfigure(4, weight=1)
            self.frame_line.grid_columnconfigure(5, weight=1)
            self.frame_line.grid_columnconfigure(6, weight=1)

            self.label_time = ctk.CTkLabel(self.frame_line, text=datetime_list[i].strftime("%H:%M"), corner_radius=0, width=150)
            self.label_time.grid(row=0, column=0, padx=5, pady=0, sticky="ns")

            self.label_date = ctk.CTkLabel(self.frame_line, text=datetime_list[i].strftime("%d/%m/%Y"), corner_radius=0, width=200)
            self.label_date.grid(row=0, column=1, padx=5, pady=0, sticky="ns")

            self.label_od = ctk.CTkLabel(self.frame_line, text=od_list[i], corner_radius=0, width=150)
            self.label_od.grid(row=0, column=2, padx=5, pady=0, sticky="ns")

            self.label_ph = ctk.CTkLabel(self.frame_line, text=ph_list[i], corner_radius=0, width=150)
            self.label_ph.grid(row=0, column=3, padx=5, pady=0, sticky="ns")

            self.label_light = ctk.CTkLabel(self.frame_line, text=light_list[i], corner_radius=0, width=150)
            self.label_light.grid(row=0, column=4, padx=5, pady=0, sticky="ns")

            self.label_temp = ctk.CTkLabel(self.frame_line, text=temp_list[i], corner_radius=0, width=200)
            self.label_temp.grid(row=0, column=5, padx=5, pady=0, sticky="ns")

            self.label_cycle = ctk.CTkLabel(self.frame_line, text="Ciclo1", corner_radius=0, width=150)
            self.label_cycle.grid(row=0, column=6, padx=5, pady=0, sticky="ns")

class CycleFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master, corner_radius=0, fg_color="transparent")

        datalog_path = os.path.join(os.getcwd(), "test_data")

        self.grid_columnconfigure(0, weight=2)
        self.grid_columnconfigure(1, weight=2)
        self.grid_columnconfigure(2, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=5)
        
        self.instant_values_frame = ActualCycleFrame(self)
        self.instant_values_frame.grid(row=0, column=0, padx=10, pady=(10, 0), sticky="nsew")

        self.actual_cycle_frame = ControlCycleFrame(self)
        self.actual_cycle_frame.grid(row=0, column=1, padx=10, pady=(10, 0), sticky="nsew")

        self.log_frame = LogFrame(self, os.path.join(datalog_path, "datos_generados_logico.csv"))
        self.log_frame.grid(row=1, column=0, padx=10, pady=(10, 0), sticky="nsew", columnspan=2)

