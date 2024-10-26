import customtkinter as ctk
import tkinter
from PIL import Image
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import os
import csv
from datetime import datetime
import frames.serial_handler as ui_serial
import re
from customtkinter import filedialog    

class ActualCycleFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 
        ui_serial.publisher.subscribe(self.process_data)

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(4, weight=1)

        self.label_actual = ctk.CTkLabel(self, text="Ciclo Actual", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_actual.grid(row=0, column=0, padx=20, pady=(10, 0), sticky="w")

        self.label_actual_days = ctk.CTkLabel(self, text="10 Dias", font=ctk.CTkFont(size=18))
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

        self.label_done_text = ctk.CTkLabel(self.frame_actual, text="5 dias", font=ctk.CTkFont(size=15, weight="bold"))
        self.label_done_text.grid(row=1, column=0, padx=20, pady=0, sticky="nsew")
        self.label_done_text.grid_forget()

        self.label_left_text = ctk.CTkLabel(self.frame_actual, text="5 dias", font=ctk.CTkFont(size=15, weight="bold"))
        self.label_left_text.grid(row=1, column=1, padx=20, pady=0, sticky="nsew")
        self.label_left_text.grid_forget()

    def process_data(self, data):
        pattern = r"#(STA)([012])\!"
        if re.match(pattern, data):
            self.esp_connected()
        
        if "#Z1!" in data:
            self.esp_disconnected()

    def esp_connected(self):
        self.label_actual_days.grid(row=1, column=0, padx=20, pady=(10, 10), sticky="w")
        self.progressbar_actual.grid(row=2, column=0, padx=20, pady=(10, 10), sticky="ew")
        self.frame_actual.grid(row=3, column=0, padx=20, pady=(10, 10), sticky="ew")
        self.label_done_colour.grid(row=0, column=0, padx=20, pady=0, sticky="nsew")
        self.label_left_colour.grid(row=0, column=1, padx=20, pady=0, sticky="nsew")
        self.label_done_text.grid(row=1, column=0, padx=20, pady=0, sticky="nsew")
        self.label_left_text.grid(row=1, column=1, padx=20, pady=0, sticky="nsew")
    
    def esp_disconnected(self):
        self.label_actual_days.grid_forget()
        self.progressbar_actual.grid_forget()
        self.frame_actual.grid_forget()
        self.label_done_colour.grid_forget()
        self.label_left_colour.grid_forget()
        self.label_done_text.grid_forget()
        self.label_left_text.grid_forget()

class ControlCycleFrame(ctk.CTkFrame):
    
    def __init__(self, master):
        super().__init__(master) 
        ui_serial.publisher.subscribe(self.process_data)

        image_path = os.path.join(os.getcwd(), "images")
        self.validate = self.register(self.only_numbers)

        self.grid_columnconfigure(0, weight=1)

        self.label_control = ctk.CTkLabel(self, text="Control del ciclo", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=20, pady=(10, 0), sticky="w")

        self.frame_buttons = ctk.CTkFrame(self, width=150)
        self.frame_buttons.grid(row=1, column=0, padx=20, pady=(10, 0))

        self.frame_buttons.grid_rowconfigure(0, weight=1)
        self.frame_buttons.grid_columnconfigure(0, weight=1)
        self.frame_buttons.grid_columnconfigure(1, weight=1)
        self.frame_buttons.grid_columnconfigure(2, weight=1)
        self.frame_buttons.grid_columnconfigure(3, weight=1)
        
        self.play_image = ctk.CTkImage(Image.open(os.path.join(image_path, "play.png")), size=(40, 40))
        self.pause_image = ctk.CTkImage(Image.open(os.path.join(image_path, "pause.png")), size=(40, 40))

        self.play_pause_image_label = ctk.CTkLabel(self.frame_buttons, text="", image=self.play_image)
        self.play_pause_image_label.grid(row=0, column=0, padx=15, pady=5, sticky="ns")        

        self.stop_image = ctk.CTkImage(Image.open(os.path.join(image_path, "stop.png")), size=(40, 40))
        self.stop_image_label = ctk.CTkLabel(self.frame_buttons, text="", image=self.stop_image)
        self.stop_image_label.grid(row=0, column=1, padx=15, pady=5, sticky="ns")    

        self.bin_image = ctk.CTkImage(Image.open(os.path.join(image_path, "bin.png")), size=(40, 40))
        self.bin_image_label = ctk.CTkLabel(self.frame_buttons, text="", image=self.bin_image)
        self.bin_image_label.grid(row=0, column=2, padx=15, pady=5, sticky="ns")        

        self.add_file_image = ctk.CTkImage(Image.open(os.path.join(image_path, "add-file.png")), size=(40, 40))
        self.add_file_image_label = ctk.CTkLabel(self.frame_buttons, text="", image=self.add_file_image)
        self.add_file_image_label.grid(row=0, column=3, padx=15, pady=5, sticky="ns")      

        self.frame_commands = ctk.CTkFrame(self)
        self.frame_commands.grid(row=2, column=0, padx=20, pady=(10, 0), sticky="ew")

        self.frame_commands.grid_rowconfigure(0, weight=1)
        self.frame_commands.grid_columnconfigure(0, weight=1)
        self.frame_commands.grid_columnconfigure(1, weight=1)
        self.frame_commands.grid_columnconfigure(2, weight=1)
        self.frame_commands.grid_columnconfigure(3, weight=1)
        self.frame_commands.grid_columnconfigure(4, weight=1)

        self.entry_interval = ctk.CTkLabel(self.frame_commands, text="Intervalo:", justify="right")
        self.entry_interval.grid(row=0, column=0, padx=(10,0), pady=5, sticky="ew")

        self.entry_interval = ctk.CTkEntry(self.frame_commands, placeholder_text="15", width=60, state="disabled", validate="key", validatecommand=(self.validate, "%P"))
        self.entry_interval.grid(row=0, column=1, padx=(0,10), pady=5, sticky="ew")

        self.radio_var = tkinter.IntVar(value=0)
        self.radio_button_seg = ctk.CTkRadioButton(master=self.frame_commands, text="seg", variable=self.radio_var, value=0, width=60, state="disabled")
        self.radio_button_seg.grid(row=0, column=2, padx=0, pady=5)
        self.radio_button_min = ctk.CTkRadioButton(master=self.frame_commands, text="min", variable=self.radio_var, value=1, width=60, state="disabled")
        self.radio_button_min.grid(row=0, column=3, padx=0, pady=5)

        self.frame_info = ctk.CTkFrame(self)
        self.frame_info.grid(row=3, column=0, padx=20, pady=(10, 0), sticky="ew")

        self.info_label = ctk.CTkLabel(self.frame_info, text="Nombre del ciclo:", justify="right")
        self.info_label.pack(side="left", padx=(60,0), pady=5)

        self.main_button_interval = ctk.CTkButton(master=self.frame_info, text="Enviar", command=self.send_button_event, width=80, state="disabled")
        self.main_button_interval.pack(side="right", padx=(0,60), pady=5)
    
    def process_data(self, data):
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
        self.bin_image_label.bind("<Button-1>", self.delete_cycle_event)

        self.add_file_image_label.bind("<Enter>", self.on_hover)
        self.add_file_image_label.bind("<Leave>", self.off_hover)
        self.add_file_image_label.bind("<Button-1>", self.load_cycle_event)

        self.main_button_interval.configure(state = "normal")
        self.entry_interval.configure(state = "normal")
        self.radio_button_seg.configure(state = "normal")
        self.radio_button_min.configure(state = "normal")

    def esp_disconnected(self):
        self.play_pause_image_label.unbind("<Enter>")
        self.play_pause_image_label.unbind("<Leave>")
        self.play_pause_image_label.unbind("<Button-1>")
        self.is_playing = False

        self.stop_image_label.unbind("<Enter>")
        self.stop_image_label.unbind("<Leave>")

        self.bin_image_label.unbind("<Enter>")
        self.bin_image_label.unbind("<Leave>")
        self.bin_image_label.unbind("<Button-1>")

        self.add_file_image_label.unbind("<Enter>")
        self.add_file_image_label.unbind("<Leave>")

        self.main_button_interval.configure(state = "disabled")
        self.entry_interval.configure(state = "disabled")
        self.radio_button_seg.configure(state = "disabled")
        self.radio_button_min.configure(state = "disabled")

    def on_hover(self, event):
        self.play_pause_image_label.configure(cursor="hand2") 
        self.stop_image_label.configure(cursor="hand2") 
        self.bin_image_label.configure(cursor="hand2")
        self.add_file_image_label.configure(cursor="hand2") 

    def off_hover(self, event):
        self.play_pause_image_label.configure(cursor="arrow") 
        self.stop_image_label.configure(cursor="arrow") 
        self.bin_image_label.configure(cursor="arrow")
        self.add_file_image_label.configure(cursor="arrow") 
    
    def play_pause_event(self, event):
        if self.is_playing:
            self.play_pause_image_label.configure(image=self.pause_image)
        else:
            self.play_pause_image_label.configure(image=self.play_image)
        self.is_playing = not self.is_playing
    
    def load_cycle_event(self, event):
        self.fname = filedialog.askopenfilename(title="Selecciona un archivo de ciclo", filetypes=[("Archivos CSV", "*.csv")])
        if not self.fname == "":
            self.info_label.configure(text="Nombre del ciclo: " + os.path.basename(self.fname))
            with open(self.fname, mode='r', newline='') as csv_file:
                reader = csv.reader(csv_file)
                for row in reader:
                    print(','.join(row)) 
        else:
            print("No se eligio ningun archivo")
    
    def delete_cycle_event(self, event):
        self.info_label.configure(text="Nombre del ciclo:")
    
    def send_button_event(self):
        interval = int(self.entry_interval.get())
        print(interval)
        interval_unit = self.radio_var.get()
        if interval_unit == 0:
            print("Segundos")
        elif interval_unit == 1:
            print("Minutos")

    def only_numbers(self, text):
        return text.isdigit() or text == ""
   
class LogFrame(ctk.CTkFrame):

    def __init__(self, master):
        super().__init__(master) 

        self.id_list = []
        self.ph_list = []
        self.od_list = []
        self.temp_list = []
        self.light_list = []

        ui_serial.publisher.subscribe(self.update_log)
        image_path = os.path.join(os.getcwd(), "images")

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1)
        self.grid_rowconfigure(2, weight=1)

        self.label_control = ctk.CTkLabel(self, text="Registro de datos", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=(20, 10), pady=(10, 0), sticky="w")
        
        self.export_image = ctk.CTkImage(Image.open(os.path.join(image_path, "download-file.png")), size=(25, 25))
        self.label_export = ctk.CTkLabel(self, text="", image=self.export_image)
        self.label_export.grid(row=0, column=1, padx=(10, 20), pady=(10, 0), sticky="e")

        self.label_export.bind("<Enter>", self.on_hover)
        self.label_export.bind("<Leave>", self.off_hover)

        self.frame_lines = ctk.CTkFrame(self, width=1500)
        self.frame_lines.grid(row=1, column=0, padx=10, pady=10, columnspan=2)
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

    def on_hover(self, event):
        self.label_export.configure(cursor="hand2") 

    def off_hover(self, event):
        self.label_export.configure(cursor="arrow") 
    
    def update_log(self, data):
        pattern = r"#(STA)([012])\!"
        if re.match(pattern, data):
            self.create_log()

        pattern = r"^(\d{10}),(\d{2}\.\d{2}),(\d{3}\.\d{2}),(\d{2}\.\d{2}),(\d{2}),(\d{1}),(\d{1}),(\d{1}),(\d{1})$" # linea de log
        match = re.match(pattern, data)
        if match: 
            self.id_list.insert(0, int(match.group(1)))
            self.light_list.insert(0, int(match.group(5)))
            self.ph_list.insert(0, float(match.group(2)))
            self.od_list.insert(0, float(match.group(3)))
            self.temp_list.insert(0, float(match.group(4)))
            
            # self.append_log([int(match.group(1)),
            #                  float(match.group(2)),
            #                  float(match.group(3)),
            #                  float(match.group(4)),
            #                  int(match.group(5))])

            self.append_log(data)

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
    
    def create_log(self):
        fname = "Log/test.csv"
        header = ['ID', 'pH', 'OD', 'Temperatura', 'Luz']
        with open(fname, mode='w', newline='') as csv_file:
            w_csv = csv.writer(csv_file)
            w_csv.writerow(header) 

    def append_log(self, line):
        fname = "Log/test.csv"
        with open(fname, mode='a', newline='') as csv_file:
            #w_csv = csv.writer(csv_file)
            csv_file.write(line+"\n")  

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

        self.log_frame = LogFrame(self)#, os.path.join(datalog_path, "datos_generados_logico.csv"))
        self.log_frame.grid(row=1, column=0, padx=10, pady=(10, 0), sticky="nsew", columnspan=2)


