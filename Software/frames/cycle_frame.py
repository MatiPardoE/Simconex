from enum import Enum
import customtkinter as ctk
import tkinter
from PIL import Image
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import os
import csv
from datetime import datetime, timedelta
import frames.serial_handler as ui_serial
import re
from customtkinter import filedialog    
from tkinter import messagebox
import pandas as pd
import time
from frames.serial_handler import MsgType
from frames.serial_handler import CycleStatus 
from frames.serial_handler import data_lists 
from frames.serial_handler import data_lists_expected 

class ActualCycleFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 
        ui_serial.publisher.subscribe(self.process_data_cycle_frame)
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

    def process_data_cycle_frame(self, data):
        if data == MsgType.ESP_CONNECTED:
            self.esp_connected()
            
        if (data == MsgType.ESP_SYNCRONIZED and not ui_serial.cycle_status == CycleStatus.NOT_CYCLE) or data == MsgType.NEW_CYCLE_SENT:
            total_time = len(data_lists_expected["id"]) * ui_serial.cycle_interval
            elapsed_time = len(data_lists["id"]) * ui_serial.cycle_interval
            restant_time = total_time - elapsed_time

            self.esp_connected()  

            if data == MsgType.NEW_CYCLE_SENT:
                self.reset_progressbar(total_time, elapsed_time, restant_time)
                self.label_actual.configure(text="Ciclo Actual: {} (en curso)".format(ui_serial.cycle_alias))
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
        self.label_actual_days.configure(text="Total: " + self.format_seconds(total_time))
        self.label_done_text.configure(text=self.format_seconds(elapsed_time))
        self.label_left_text.configure(text=self.format_seconds(restant_time))

        if(total_time == elapsed_time):
            self.label_actual.configure(text="Ciclo Actual: {} (terminado)".format(ui_serial.cycle_alias))
            self.progressbar_actual.configure(progress_color="green")
            messagebox.showinfo("Ciclo terminado!")

    def reset_progressbar(self, total_time, elapsed_time, restant_time):
        self.progressbar_actual.set(0)
        self.label_actual_days.configure(text="Total: " + self.format_seconds(total_time))
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


class HandshakeStatus(Enum):
    OK = 1
    NOT_YET = 0
    TIMEOUT = -1
    ERROR = -2
    DATA_FAIL = -3
    
    
class ControlCycleFrame(ctk.CTkFrame):
    
    def __init__(self, master):
        super().__init__(master) 
        ui_serial.publisher.subscribe(self.process_data_control_cycle)

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

        self.frame_info.grid_rowconfigure(0, weight=1)
        self.frame_info.grid_columnconfigure(0, weight=1)
        self.frame_info.grid_columnconfigure(1, weight=1)
        self.frame_info.grid_columnconfigure(2, weight=1)

        self.info_label = ctk.CTkLabel(self.frame_info, text="Nombre del ciclo:", justify="right")
        self.info_label.grid(row=0, column=0, padx=(10,0), pady=5, sticky="ew")

        self.entry_label = ctk.CTkEntry(self.frame_info, width=300, state="disabled")
        self.entry_label.grid(row=0, column=1, padx=(0,10), pady=5, sticky="ew")

        self.main_button_interval = ctk.CTkButton(master=self.frame_info, text="Enviar", command=self.send_button_event, width=80, state="disabled")
        self.main_button_interval.grid(row=0, column=2, padx=0, pady=5)
    
    def process_data_control_cycle(self, data):
        if data == MsgType.ESP_CONNECTED:
            self.esp_connected()        
        
        if data == MsgType.ESP_DISCONNECTED:
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
        self.entry_label.configure(state = "normal")
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
        self.fname = filedialog.askopenfilename(title="Selecciona un archivo de ciclo", filetypes=[("Plantilla de ciclo", "*.xlsx")])
        if self.fname == "":
            print("No se eligio ningun archivo")
        else:
            self.timestamp = datetime.now().strftime("%Y%m%d_%H%M")
            ui_serial.cycle_id = self.timestamp
            self.cycle_path = os.path.join(os.getcwd(), "input_csv", self.timestamp)   
            os.makedirs(self.cycle_path, exist_ok=True) 
            self.excel_to_csv(self.fname, os.path.join(self.cycle_path, "data_"+self.timestamp+".csv"))
    
    def excel_to_csv(self, fname_excel, fname_csv):
        df = pd.read_excel(fname_excel, skiprows=1, header=None)
        formatted_lines = []
        self.total_data_lines = 0

        for index, row in df.iterrows():
            field1 = f"{int(row[0]):08d}"
            field2 = f"{float(row[1]):05.2f}"
            field3 = f"{float(row[2]):06.2f}"
            field4 = f"{float(row[3]):05.2f}"
            field5 = f"{int(row[4]):02d}"

            line = f"{field1},{field2},{field3},{field4},{field5}"
            formatted_lines.append(line)

        with open(fname_csv, 'w', newline='') as f:
            for line in formatted_lines:
                f.write(line + '\n')
                self.total_data_lines += 1
            
    
    def delete_cycle_event(self, event):
        self.fname = ""
        self.entry_interval.delete(0, "end")
        self.entry_interval.insert(0,"")
        self.entry_label.delete(0, "end")
        self.entry_label.insert(0,"")
    
    def send_button_event(self):
        try:
            self.interval = int(self.entry_interval.get())
            self.alias_cycle = self.entry_label.get().strip()
            if self.alias_cycle == "":
                raise Exception("No se coloco un alias al ciclo")     
            if self.fname == "":
                raise Exception("No se eligio un archivo para el ciclo")            
        except Exception as e:
            print(e)
            messagebox.showwarning("Advertencia", "Completar todos los campos")
            return      
        
        msg = "¿Esta seguro de que desea comenzar el ciclo " + os.path.basename(self.alias_cycle) + " con intervalos de medicion de " + str(self.interval)
        self.interval_unit = self.radio_var.get()
        if self.interval_unit == 0:
            msg = msg + " segundos?"
        elif self.interval_unit == 1:
            msg = msg + " minutos?"
        
        answer = messagebox.askquestion("Comenzar ciclo", msg)
        if answer == "yes":
            self.cycle_path = os.path.join(os.getcwd(), "input_csv", self.timestamp)   
            os.makedirs(self.cycle_path, exist_ok=True) 
            self.generate_header_csv(os.path.join(self.cycle_path, "header_"+self.timestamp+".csv"))
            self.transfer_cycle(self.timestamp) 
        else:
            print("Descartado")

    def generate_header_csv(self, fname):

        if self.interval_unit == 1:
            self.interval = self.interval*60
        
        ui_serial.cycle_interval = self.interval

        data = [
            ["cycle_name", self.alias_cycle],
            ["cycle_id", self.timestamp],
            ["state", "new_cycle"],
            ["interval_time", self.interval],
            ["interval_total", self.total_data_lines],
            ["interval_current", "0"]
        ]

        with open(fname, mode="w", newline="") as file:
            writer = csv.writer(file)
            writer.writerows(data)
        
    def transfer_cycle(self, id):
        
        try:
            ui_serial.publisher.subscribe(self.wait_for_ok)
            self.send_data_and_wait_hs(b"#TRANSFER0!\n")
            self.send_data_and_wait_hs(b"#HEADER0!\n")
            self.send_file_serial("input_csv/"+id+"/header_"+id+".csv") 
            self.send_data_and_wait_hs(b"#HEADER1!\n")
            self.send_data_and_wait_hs(b"#DATA0!\n")
            self.send_file_serial_hs("input_csv/"+id+"/data_"+id+".csv") 
            self.send_data_and_wait_hs(b"#DATA1!\n")
            self.send_data_and_wait_hs(b"#TRANSFER1!\n")
            ui_serial.publisher.unsubscribe(self.wait_for_ok)
            self.load_expected_lists("input_csv/"+id+"/data_"+id+".csv")
            ui_serial.cycle_status = CycleStatus.CYCLE_RUNNING
            ui_serial.publisher.notify_new_cycle_started()
        except Exception as e:
            print(e)
            messagebox.showerror("Error", "Se produjo un error durante la transferencia del ciclo!")
            return    
    def send_data_and_wait_hs(self, data, timeout = 3):
        self.handshake_status = HandshakeStatus.NOT_YET
        ui_serial.publisher.send_data(data)
        self.wait_handshake(timeout)
    
    def send_file_serial(self, fname):
        with open(fname, mode='r', newline='') as csv_file:
                reader = csv.reader(csv_file)
                for row in reader:
                    row_bytes = [element.encode() for element in row]
                    ui_serial.publisher.send_data(b','.join(row_bytes) + b'\n')
        
    def send_file_serial_hs(self, fname, block_size=160):
        # Calculate the number of blocks
        with open(fname, mode='r', newline='') as csv_file:
            reader = csv.reader(csv_file)
            total_lines = sum(1 for _ in reader)
            total_blocks = (total_lines + block_size - 1) // block_size  # Ceiling division
            print(f"Total blocks to send: {total_blocks}")

        start_time = time.time()  # Start timing the transfer
        with open(fname, mode='r', newline='') as csv_file:
            reader = csv.reader(csv_file)
            limit = 5
            buffer = []
            ii = 0

            for row in reader:
                row_bytes = [element.encode() for element in row]
                buffer.append(b','.join(row_bytes))

                if len(buffer) >= block_size:
                    attemps = 0
                    while attemps < limit:
                        try:
                            block_start_time = time.time()
                            self.send_data_and_wait_hs(b'\n'.join(buffer) + b'\n')
                            block_end_time = time.time()
                            print(f"Sent OK: Block: {(ii/block_size)+1}/{total_blocks-1} in {block_end_time - block_start_time:.2f} seconds")
                            buffer = []
                            break
                        except ValueError as e:
                            attemps += 1
                            print(f"Block starting at line {ii} failed ({attemps})")
                            time.sleep(0.3)
                            if attemps >= limit:
                                print("Too many errors, exiting transfer...")
                                raise ValueError("Comm lost between ESP and UI")
                    ii += block_size

            # Send any remaining lines in the buffer
            if buffer:
                attemps = 0
                while attemps < limit:
                    try:
                        ui_serial.publisher.send_data(b'\n'.join(buffer) + b'\n') # Aca no espero HS porque es el ultimo bloque y el ESP no lo sabe
                        break
                    except ValueError as e:
                        attemps += 1
                        print(f"Final block failed ({attemps})")
                        if attemps >= limit:
                            print("Too many errors, exiting transfer...")
                            raise ValueError("Comm lost between ESP and UI")
        
        end_time = time.time()  # End timing the transfer
        print(f"Transfer completed in {end_time - start_time:.2f} seconds")
        
    def load_expected_lists(self, fname):
        with open(fname, "r") as file:
            for linea in file:
                pattern = r"^(\d{8}),(\d{2}\.\d{2}),(\d{3}\.\d{2}),(\d{2}\.\d{2}),(\d{2})$"
                match = re.match(pattern, linea)

                if match: 
                    data_lists_expected['id'].append(int(match.group(1)))
                    data_lists_expected['light'].append(int(match.group(5)))
                    data_lists_expected['ph'].append(float(match.group(2)))
                    data_lists_expected['od'].append(float(match.group(3)))
                    data_lists_expected['temperature'].append(float(match.group(4)))
    
    def wait_handshake(self,timeout = 5):
        start_time = time.time()
        while True:
            if self.handshake_status == HandshakeStatus.OK:
                print("Handshake OK")
                self.handshake_status = HandshakeStatus.NOT_YET
                break
            if self.handshake_status == HandshakeStatus.DATA_FAIL:
                raise ValueError("Data lost between ESP and UI")
            if time.time() - start_time > timeout:
                self.handshake_status = HandshakeStatus.TIMEOUT
                raise TimeoutError("Timeout waiting for handshake from ESP")
        
    def wait_for_ok(self, data):
        if data.strip() == "#OK!":
            self.handshake_status = HandshakeStatus.OK
        elif data.strip() == "#FAIL!":
            self.handshake_status = HandshakeStatus.DATA_FAIL
        else:
            print("Received unexpected data in HS:", data.strip())
            self.handshake_status = HandshakeStatus.ERROR
    
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
        if data == MsgType.NEW_CYCLE_SENT:
            for widget in self.scrollable_frame.winfo_children():
                widget.destroy()
            return 
           
        # TODO: se tienen que cargar todas las mediciones en el log 
        if data == MsgType.NEW_MEASUREMENT or (data == MsgType.ESP_SYNCRONIZED and not ui_serial.cycle_status == CycleStatus.NOT_CYCLE):
            num_measurements = len(data_lists['id'])
            if num_measurements == 0:
                return

            last_index = num_measurements - 1  

            self.frame_line = ctk.CTkFrame(self.scrollable_frame)
            self.frame_line.pack(fill="x")

            self.in_frame = ctk.CTkFrame(self.frame_line)
            self.in_frame.pack(fill="x")

            date, hour = self.calculate_datetime(last_index)

            self.label_time = ctk.CTkLabel(self.in_frame, text=hour, corner_radius=0, width=150)
            self.label_time.pack(side='left')

            self.label_date = ctk.CTkLabel(self.in_frame, text=date, corner_radius=0, width=200)
            self.label_date.pack(side='left')

            self.label_od = ctk.CTkLabel(self.in_frame, text="{0:.2f}".format(data_lists['od'][last_index]), corner_radius=0, width=150)
            self.label_od.pack(side='left')

            self.label_ph = ctk.CTkLabel(self.in_frame, text="{0:.2f}".format(data_lists['ph'][last_index]), corner_radius=0, width=150)
            self.label_ph.pack(side='left')

            self.label_light = ctk.CTkLabel(self.in_frame, text=f"{data_lists['light'][last_index]}", corner_radius=0, width=150)
            self.label_light.pack(side='left')

            self.label_temp = ctk.CTkLabel(self.in_frame, text="{0:.2f}".format(data_lists['temperature'][last_index]), corner_radius=0, width=200)
            self.label_temp.pack(side='left')

            self.label_cycle = ctk.CTkLabel(self.in_frame, text=ui_serial.cycle_alias, corner_radius=0, width=150)
            self.label_cycle.pack(side='left')

            children = self.scrollable_frame.winfo_children()
            if len(children) > 50:
                children[0].destroy()  

            self.scrollable_frame._parent_canvas.yview_moveto(1.0)
    
        if data == MsgType.ESP_SYNCRONIZED or data == MsgType.ESP_CONNECTED:
            self.label_export.bind("<Enter>", self.on_hover)
            self.label_export.bind("<Leave>", self.off_hover)
            self.label_export.bind("<Button-1>", self.export_event)

        if data == MsgType.ESP_DISCONNECTED:
            self.label_export.unbind("<Enter>")
            self.label_export.unbind("<Leave>")
            self.label_export.unbind("<Button-1>")
    
    def calculate_datetime(self, intervals):
        initial_time = datetime.strptime(ui_serial.cycle_id, "%Y%m%d_%H%M")
        seconds_elapsed = intervals * ui_serial.cycle_interval
        current_time = initial_time + timedelta(seconds=seconds_elapsed)

        current_date = current_time.strftime("%d/%m/%Y") 
        current_hour = current_time.strftime("%H:%M:%S") 

        return current_date, current_hour

    def export_event(self, event):
        file = filedialog.asksaveasfilename(
            defaultextension=".xlsx", 
            filetypes=[("Planilla de Excel", "*.xlsx")], 
            title="Guardar datalog como"
        )
        if file: 
            df = pd.read_csv(os.path.join(os.getcwd(), "Log", ui_serial.cycle_id, "cycle_out_"+ui_serial.cycle_id+".csv"), header=None)
            header = ["ID", "pH", "OD [%]", "Temperatura [°C]", "Luz [%]", "CO2", "O2", "N2", "Aire"]
            df.columns = header
            date_time = df[header[0]].apply(self.calculate_datetime) # Aplica la función calculate_datetime a cada valor de la primera columna
            df['Fecha'], df['Hora'] = zip(*date_time) # Añade las columnas de fecha y hora al DataFrame
            df = df.drop(columns=[header[0]]) # Elimina la columna de intervalo
            df = df[['Hora', 'Fecha'] + header[1:]]  # Reorganiza las columnas
            df.to_excel(file, index=False)

    
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


