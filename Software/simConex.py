import customtkinter as ctk
import os
import tkinter
from PIL import Image, ImageTk
import serial
import serial.tools.list_ports
import threading
import logging
from frames.home_frame import HomeFrame
from frames.sync_frame import SecondFrame
from frames.manual_frame import ManualFrame
from frames.calibration_frame import CalibrationFrame
from frames.alerts_frame import AlertsFrame
from frames.cycle_frame import CycleFrame
import frames.serial_handler as ui_serial

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

def view_data(data):
    print(f"simConex.py: {data}")

class App(ctk.CTk):
    def __init__(self):
        super().__init__()

        self.title("FBR SIMCONEX")
        #self.geometry("700x450")
        ctk.set_appearance_mode("Light")

        # set grid layout 1x2
        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)

        # load images with light and dark mode image
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

        # create navigation frame
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

        # Connection Menu SideBar
        self.connection_label = ctk.CTkLabel(self.navigation_frame, text="Estado:   ", anchor="w",
                                             font=ctk.CTkFont(size=12, weight="bold"), image=self.unlink_image, compound="right")
        self.connection_label.grid(row=7, column=0, padx=20, pady=(10, 0))
        self.connection_button = ctk.CTkButton(self.navigation_frame, text="Conectar", command=self.connection_button_event)
        self.connection_button.grid(row=8, column=0, padx=20, pady=(10, 0))

        # Scaling Menu SideBar
        self.scaling_label = ctk.CTkLabel(self.navigation_frame, text="Zoom:", anchor="w")
        self.scaling_label.grid(row=9, column=0, padx=20, pady=(10, 0))
        self.scaling_optionemenu = ctk.CTkOptionMenu(self.navigation_frame, values=["80%", "90%", "100%", "110%", "120%"],
                                                     command=self.change_scaling_event)
        self.scaling_optionemenu.set("100%")  # set default value
        self.scaling_optionemenu.grid(row=10, column=0, padx=20, pady=(10, 20), sticky="s")

        # create frames
        self.home_frame = HomeFrame(self)
        self.cycle_frame = CycleFrame(self)
        self.manual_frame = ManualFrame(self)
        self.calibration_frame = CalibrationFrame(self)
        self.alerts_frame = AlertsFrame(self)

        # select default frame
        self.select_frame_by_name("home")

    def select_frame_by_name(self, name):
        # set button color for selected button
        self.home_button.configure(fg_color=("gray75", "gray25") if name == "home" else "transparent")
        self.cycle_button.configure(fg_color=("gray75", "gray25") if name == "cycle" else "transparent")
        self.manual_button.configure(fg_color=("gray75", "gray25") if name == "manual" else "transparent")
        self.calibration_button.configure(fg_color=("gray75", "gray25") if name == "calibration" else "transparent")
        self.alerts_button.configure(fg_color=("gray75", "gray25") if name == "alerts" else "transparent")

        # show selected frame
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

    def connection_button_event(self): 
        if self.connection_button.cget("text") == "Conectar":        
            ui_serial.publisher.start_find_thread()
            ui_serial.publisher.find_thread.join()
            if ui_serial.publisher.ser.is_open:
                self.connection_label.configure(image=self.link_image)  
                self.connection_button.configure(text="Desconectar")
        else: 
            ui_serial.publisher.send_data(b"#Z1!")
            self.connection_label.configure(image=self.unlink_image)  
            self.connection_button.configure(text="Conectar")
            print(f"Desconectado")
            ui_serial.publisher.stop_read_thread()

if __name__ == "__main__":
    ui_serial.publisher.subscribe(view_data)

    app = App()
    app.mainloop()
