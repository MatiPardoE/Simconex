import customtkinter as ctk
import os
import tkinter
from PIL import Image, ImageTk
import serial
import serial.tools.list_ports
import threading
import logging
from frames.home_frame import HomeFrame
from frames.manual_frame import ManualFrame
from frames.calibration_frame import CalibrationFrame
from frames.alerts_frame import AlertsFrame
from frames.cycle_frame import CycleFrame
import frames.serial_handler as ui_serial
from frames.cycle_sync import CycleSync
from frames.serial_handler import MsgType 
import csv
from frames.serial_handler import data_lists 
from frames.serial_handler import CycleStatus 
from frames.serial_handler import data_lists_expected 

# Crear un logger
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)  # Configura el nivel de registro

log_dir = os.path.join(os.getcwd(), "log")

# Crear la carpeta Log si no existe
if not os.path.exists(log_dir):
    os.makedirs(log_dir)

# Crear un manejador para el archivo en la carpeta Log
file_handler = logging.FileHandler(os.path.join(log_dir, "app.log"))
file_handler.setLevel(logging.DEBUG)

# Crear un manejador para la consola
console_handler = logging.StreamHandler()
console_handler.setLevel(logging.DEBUG)

# Crear un formateador y configurarlo para ambos manejadores
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
file_handler.setFormatter(formatter)
console_handler.setFormatter(formatter)

# Agregar los manejadores al logger
logger.addHandler(file_handler)
logger.addHandler(console_handler)

class App(ctk.CTk):
    def __init__(self):
        super().__init__()

        screen_width = self.winfo_screenwidth()
        screen_height = self.winfo_screenheight()
        self.geometry(f"{screen_width}x{screen_height}+0+0")
        self.alert_counter = 0

        ui_serial.publisher.subscribe(self.update_btn)

        self.title("FBR SIMCONEX")
        ctk.set_appearance_mode("Light")

        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)

        image_path = os.path.join(os.getcwd(), "images")
        self.logo_image = ctk.CTkImage(Image.open(os.path.join(image_path, "logo_edited.png")), size=(40, 40))
        self.large_test_image = ctk.CTkImage(Image.open(os.path.join(image_path, "large_test_image.png")), size=(500, 150))
        self.image_icon_image = ctk.CTkImage(Image.open(os.path.join(image_path, "image_icon_light.png")), size=(20, 20))
        self.home_image = ctk.CTkImage(light_image=Image.open(os.path.join(image_path, "home_dark.png")),
                                       dark_image=Image.open(os.path.join(image_path, "home_light.png")), size=(20, 20))
        self.cycle_image = ctk.CTkImage(light_image=Image.open(os.path.join(image_path, "cycle.png")), size=(20, 20))
        self.manual_image = ctk.CTkImage(light_image=Image.open(os.path.join(image_path, "manual.png")), size=(20, 20))
        self.calibration_image = ctk.CTkImage(light_image=Image.open(os.path.join(image_path, "calibration.png")), size=(20, 20))
        self.alerts_image = ctk.CTkImage(light_image=Image.open(os.path.join(image_path, "alert.png")), size=(20, 20))
        self.unlink_image = ctk.CTkImage(Image.open(os.path.join(image_path, "unlink.png")), size=(24, 24))
        self.link_image = ctk.CTkImage(Image.open(os.path.join(image_path, "link.png")), size=(24, 24))
        self.unsync_image = ctk.CTkImage(Image.open(os.path.join(image_path, "unsync.png")), size=(24, 24))

        self.navigation_frame = ctk.CTkFrame(self, corner_radius=0)
        self.navigation_frame.grid(row=0, column=0, sticky="nsew")
        self.navigation_frame.grid_rowconfigure(6, weight=1)

        self.navigation_frame_label = ctk.CTkLabel(self.navigation_frame, text=" FBR SIMCONEX", image=self.logo_image,
                                                   compound="left", anchor="w", font=ctk.CTkFont(size=12, weight="bold"))
        self.navigation_frame_label.grid(row=0, column=0, padx=5, pady=5, sticky="ew")

        self.home_button = ctk.CTkButton(self.navigation_frame, corner_radius=0, height=40, border_spacing=10, text="Home",
                                         fg_color="transparent", text_color=("gray10", "gray90"), hover_color=("gray70", "gray30"),
                                         image=self.home_image, anchor="w", command=self.home_button_event)
        self.home_button.grid(row=1, column=0, sticky="ew")

        self.cycle_button = ctk.CTkButton(self.navigation_frame, corner_radius=0, height=40, border_spacing=10, text="Ciclos",
                                            fg_color="transparent", text_color=("gray10", "gray90"), hover_color=("gray70", "gray30"),
                                            image=self.cycle_image, anchor="w", command=self.cycle_button_event)
        self.cycle_button.grid(row=2, column=0, sticky="ew")

        self.manual_button = ctk.CTkButton(self.navigation_frame, corner_radius=0, height=40, border_spacing=10, text="Manual",
                                            fg_color="transparent", text_color=("gray10", "gray90"), hover_color=("gray70", "gray30"),
                                            image=self.manual_image, anchor="w", command=self.manual_button_event)
        self.manual_button.grid(row=3, column=0, sticky="ew")

        self.calibration_button = ctk.CTkButton(self.navigation_frame, corner_radius=0, height=40, border_spacing=10, text="Calibracion",
                                            fg_color="transparent", text_color=("gray10", "gray90"), hover_color=("gray70", "gray30"),
                                            image=self.calibration_image, anchor="w", command=self.calibration_button_event)
        self.calibration_button.grid(row=4, column=0, sticky="ew")

        self.alerts_button = ctk.CTkButton(self.navigation_frame, corner_radius=0, height=40, border_spacing=10, text="Alertas",
                                            fg_color="transparent", text_color=("gray10", "gray90"), hover_color=("gray70", "gray30"),
                                            image=self.alerts_image, anchor="w", command=self.alerts_button_event)
        self.alerts_button.grid(row=5, column=0, sticky="ew")

        self.connection_label = ctk.CTkLabel(self.navigation_frame, text="Estado:   ", anchor="w",
                                             font=ctk.CTkFont(size=12, weight="bold"), image=self.unlink_image, compound="right")
        self.connection_label.grid(row=7, column=0, padx=20, pady=(10, 0))
        self.connection_button = ctk.CTkButton(self.navigation_frame, text="Conectar", command=self.connection_button_event)
        self.connection_button.grid(row=8, column=0, padx=20, pady=(10, 0))

        self.sync_button = ctk.CTkButton(self.navigation_frame, text="Sincronizar", command=self.sync_button_event, state="disabled")
        self.sync_button.grid(row=9, column=0, padx=20, pady=(10, 0))

        self.scaling_label = ctk.CTkLabel(self.navigation_frame, text="Zoom:", anchor="w")
        self.scaling_label.grid(row=10, column=0, padx=20, pady=(10, 0))
        self.scaling_optionemenu = ctk.CTkOptionMenu(self.navigation_frame, values=["80%", "90%", "100%", "110%", "120%"],
                                                     command=self.change_scaling_event)
        self.scaling_optionemenu.set("120%")
        ctk.set_widget_scaling(0.9)
        self.scaling_optionemenu.grid(row=11, column=0, padx=20, pady=(10, 20), sticky="s")

        self.home_frame = HomeFrame(self)
        self.cycle_frame = CycleFrame(self)
        self.manual_frame = ManualFrame(self)
        self.calibration_frame = CalibrationFrame(self)
        self.alerts_frame = AlertsFrame(self)

        self.select_frame_by_name("home")

    def select_frame_by_name(self, name):
        self.home_button.configure(fg_color=("gray75", "gray25") if name == "home" else "transparent")
        self.cycle_button.configure(fg_color=("gray75", "gray25") if name == "cycle" else "transparent")
        self.manual_button.configure(fg_color=("gray75", "gray25") if name == "manual" else "transparent")
        self.calibration_button.configure(fg_color=("gray75", "gray25") if name == "calibration" else "transparent")
        self.alerts_button.configure(fg_color=("gray75", "gray25") if name == "alerts" else "transparent")

        if name == "home":
            self.home_frame.grid(row=0, column=1, sticky="nsew")
        else:
            self.home_frame.grid_forget()
        if name == "cycle":
            self.cycle_frame.grid(row=0, column=1, sticky="nsew")
        else:
            self.cycle_frame.grid_forget()
        if name == "manual":
            self.manual_frame.grid(row=0, column=1, sticky="nsew")
        else:
            self.manual_frame.grid_forget()
        if name == "calibration":
            self.calibration_frame.grid(row=0, column=1, sticky="nsew")
        else:
            self.calibration_frame.grid_forget()
        if name == "alerts":
            self.alerts_frame.grid(row=0, column=1, sticky="nsew")
        else:
            self.alerts_frame.grid_forget()

    def home_button_event(self):
        self.select_frame_by_name("home")

    def cycle_button_event(self):
        self.select_frame_by_name("cycle")

    def manual_button_event(self):
        self.select_frame_by_name("manual")

    def calibration_button_event(self):
        self.select_frame_by_name("calibration")

    def alerts_button_event(self):
        self.select_frame_by_name("alerts")

    def change_appearance_mode_event(self, new_appearance_mode):
        ctk.set_appearance_mode(new_appearance_mode)

    def change_scaling_event(self, new_scaling: str):
        new_scaling_float = int(new_scaling.replace("%", "")) / 100
        ctk.set_widget_scaling(new_scaling_float)

    def sync_button_event(self): 
        cycle_sync = CycleSync()
        cycle_sync.sync_running_cycle()

    def connection_button_event(self): 
        if self.connection_button.cget("text") == "Conectar":        
            ui_serial.publisher.start_find_thread()
            ui_serial.publisher.find_thread.join()
            if ui_serial.publisher.ser.is_open:
                self.connection_label.configure(image=self.unsync_image)  
                self.connection_button.configure(text="Desconectar")     
                self.sync_button.configure(state="normal")         
        else: 
            self.connection_label.configure(image=self.unlink_image)  
            self.connection_button.configure(text="Conectar")
            self.sync_button.configure(state="disabled")     
            ui_serial.publisher.send_data(b"#Z1!")
            ui_serial.publisher.stop_read_thread() 

    def update_btn(self, data):        
        if data == MsgType.ESP_SYNCRONIZED or data == MsgType.NEW_CYCLE_SENT:
            self.connection_label.configure(image=self.link_image)  
        if data == MsgType.ESP_DISCONNECTED:
            self.connection_label.configure(image=self.unsync_image)  

    def check_calib(self):
        self.calibration_frame.instant_values_frame.read_calib_file()

def backend(data):
    if data == MsgType.ESP_SYNCRONIZED or data == MsgType.NEW_CYCLE_SENT:
        app.sync_button.configure(state="disabled")
        app.sync_button.configure(text="Sincronizado")

        app.alerts_button.configure(text="Alertas")
        app.alert_counter = 0
        
        fname = os.path.join(os.getcwd(), "input_csv", ui_serial.cycle_id, "data_"+ui_serial.cycle_id+".csv")
        with open(fname, "r") as csvfile:
            reader = csv.reader(csvfile)
            for row in reader:
                if len(row) == 9:
                    data_lists_expected['id'].append(int(row[0]))
                    data_lists_expected['ph'].append(float(row[1]))
                    data_lists_expected['od'].append(float(row[2]))
                    data_lists_expected['temperature'].append(float(row[3]))
                    data_lists_expected['light_t'].append(int(row[4]))
                    data_lists_expected['light_mt'].append(int(row[5]))
                    data_lists_expected['light_mm'].append(int(row[6]))
                    data_lists_expected['light_ml'].append(int(row[7]))
                    data_lists_expected['light_l'].append(int(row[8]))
                    
        
        fname = os.path.join(os.getcwd(), "input_csv", ui_serial.cycle_id, "header_"+ui_serial.cycle_id+".csv")
        with open(fname, newline='') as csvfile:
            reader = csv.reader(csvfile)
            for row in reader:
                if row[0] == 'cycle_name':
                    ui_serial.cycle_alias = row[1]
                elif row[0] == 'interval_time':
                    ui_serial.cycle_interval = int(row[1])  
                elif row[0] == 'cycle_id':
                    ui_serial.cycle_id = row[1]
    
    if data == MsgType.ESP_DISCONNECTED:
        app.sync_button.configure(state="disabled")
        app.sync_button.configure(text="Sincronizar")

        app.alerts_button.configure(text="Alertas")
        app.alert_counter = 0

        data_lists['id'] = []
        data_lists['light_t'] = []
        data_lists['light_mt'] = []
        data_lists['light_mm'] = []
        data_lists['light_ml'] = []
        data_lists['light_l'] = []
        data_lists['ph'] = []
        data_lists['od'] = []
        data_lists['temperature'] = []
        data_lists['co2'] = []
        data_lists['o2'] = []
        data_lists['n2'] = []
        data_lists['air'] = []
        data_lists['conc'] = []

        data_lists_expected['id'] = []
        data_lists_expected['light_t'] = []
        data_lists_expected['light_mt'] = []
        data_lists_expected['light_mm'] = []
        data_lists_expected['light_ml'] = []
        data_lists_expected['light_l'] = []
        data_lists_expected['ph'] = []
        data_lists_expected['od'] = []
        data_lists_expected['temperature'] = []

        ui_serial.cycle_status = CycleStatus.NOT_CYCLE
    
    if data == MsgType.PH_OUT_OF_RANGE:
        app.alert_counter += 1
        app.alerts_button.configure(text="Alertas [" + str(app.alert_counter) +"]")
    if data == MsgType.OD_OUT_OF_RANGE:
        app.alert_counter += 1
        app.alerts_button.configure(text="Alertas [" + str(app.alert_counter) +"]")
    if data == MsgType.TEMP_OUT_OF_RANGE:
        app.alert_counter += 1
        app.alerts_button.configure(text="Alertas [" + str(app.alert_counter) +"]")
    
    if data == MsgType.PH_OUT_OF_CALIB:
        app.alert_counter += 1 # TODO: tengo que corregir esto porque app todavia no existe
        app.alerts_button.configure(text="Alertas [" + str(app.alert_counter) +"]")
    if data == MsgType.OD_OUT_OF_CALIB:
        app.alert_counter += 1
        app.alerts_button.configure(text="Alertas [" + str(app.alert_counter) +"]")

if __name__ == "__main__":
    ui_serial.publisher.subscribe(backend)

    app = App()
    app.check_calib()
    app.mainloop()
