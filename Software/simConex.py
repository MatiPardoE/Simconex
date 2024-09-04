import customtkinter as ctk
import os
from PIL import Image
import serial
import serial.tools.list_ports
import threading
import logging
from frames.home_frame import HomeFrame
from frames.sync_frame import SecondFrame
from frames.manual_frame import ThirdFrame

# Crear un logger
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)  # Configura el nivel de registro

# Obtener la ruta del directorio actual donde está el archivo Python
current_dir = os.path.dirname(os.path.abspath(__file__))

# Definir la ruta de la carpeta Log relativa al directorio del script
log_dir = os.path.join(current_dir, "log")

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

        self.title("simConex.py")
        self.geometry("700x450")
        ctk.set_appearance_mode("Light")

        # set grid layout 1x2
        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)

        # load images with light and dark mode image
        image_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "images")
        self.logo_image = ctk.CTkImage(Image.open(os.path.join(image_path, "simconex_Logo.png")), size=(40, 40))
        self.large_test_image = ctk.CTkImage(Image.open(os.path.join(image_path, "large_test_image.png")), size=(500, 150))
        self.image_icon_image = ctk.CTkImage(Image.open(os.path.join(image_path, "image_icon_light.png")), size=(20, 20))
        self.home_image = ctk.CTkImage(light_image=Image.open(os.path.join(image_path, "home_dark.png")),
                                       dark_image=Image.open(os.path.join(image_path, "home_light.png")), size=(20, 20))
        self.chat_image = ctk.CTkImage(light_image=Image.open(os.path.join(image_path, "chat_dark.png")),
                                       dark_image=Image.open(os.path.join(image_path, "chat_light.png")), size=(20, 20))
        self.add_user_image = ctk.CTkImage(light_image=Image.open(os.path.join(image_path, "add_user_dark.png")),
                                           dark_image=Image.open(os.path.join(image_path, "add_user_light.png")), size=(20, 20))
        self.unlink_image = ctk.CTkImage(Image.open(os.path.join(image_path, "unlink.png")), size=(24, 24))
        self.link_image = ctk.CTkImage(Image.open(os.path.join(image_path, "link.png")), size=(24, 24))
        self.connection_image = self.unlink_image  # Inicialmente no conectado

        # create navigation frame
        self.navigation_frame = ctk.CTkFrame(self, corner_radius=0)
        self.navigation_frame.grid(row=0, column=0, sticky="nsew")
        self.navigation_frame.grid_rowconfigure(4, weight=1)

        self.navigation_frame_label = ctk.CTkLabel(self.navigation_frame, text=" SimConEx", image=self.logo_image,
                                                   compound="left", anchor="w", font=ctk.CTkFont(size=12, weight="bold"))
        self.navigation_frame_label.grid(row=0, column=0, padx=5, pady=5, sticky="ew")

        self.home_button = ctk.CTkButton(self.navigation_frame, corner_radius=0, height=40, border_spacing=10, text="Home",
                                         fg_color="transparent", text_color=("gray10", "gray90"), hover_color=("gray70", "gray30"),
                                         image=self.home_image, anchor="w", command=self.home_button_event)
        self.home_button.grid(row=1, column=0, sticky="ew")

        self.frame_2_button = ctk.CTkButton(self.navigation_frame, corner_radius=0, height=40, border_spacing=10, text="Cycle",
                                            fg_color="transparent", text_color=("gray10", "gray90"), hover_color=("gray70", "gray30"),
                                            image=self.chat_image, anchor="w", command=self.frame_2_button_event)
        self.frame_2_button.grid(row=2, column=0, sticky="ew")

        self.frame_3_button = ctk.CTkButton(self.navigation_frame, corner_radius=0, height=40, border_spacing=10, text="Manual",
                                            fg_color="transparent", text_color=("gray10", "gray90"), hover_color=("gray70", "gray30"),
                                            image=self.add_user_image, anchor="w", command=self.frame_3_button_event)
        self.frame_3_button.grid(row=3, column=0, sticky="ew")

        # Connection Menu SideBar
        self.connection_label = ctk.CTkLabel(self.navigation_frame, text="Status:   ", anchor="w",
                                             font=ctk.CTkFont(size=12, weight="bold"), image=self.connection_image, compound="right")
        self.connection_label.grid(row=5, column=0, padx=20, pady=(10, 0))
        self.connection_button = ctk.CTkButton(self.navigation_frame, text="Connect", command=self.connection_button_event)
        self.connection_button.grid(row=6, column=0, padx=20, pady=(10, 0))

        # Scaling Menu SideBar
        self.scaling_label = ctk.CTkLabel(self.navigation_frame, text="UI Scaling:", anchor="w")
        self.scaling_label.grid(row=7, column=0, padx=20, pady=(10, 0))
        self.scaling_optionemenu = ctk.CTkOptionMenu(self.navigation_frame, values=["80%", "90%", "100%", "110%", "120%"],
                                                     command=self.change_scaling_event)
        self.scaling_optionemenu.set("100%")  # set default value
        self.scaling_optionemenu.grid(row=8, column=0, padx=20, pady=(10, 20), sticky="s")

        # create frames
        self.home_frame = HomeFrame(self, self.large_test_image, self.image_icon_image)
        self.second_frame = SecondFrame(self)
        self.third_frame = ThirdFrame(self)

        # select default frame
        self.select_frame_by_name("home")

    def select_frame_by_name(self, name):
        # set button color for selected button
        self.home_button.configure(fg_color=("gray75", "gray25") if name == "home" else "transparent")
        self.frame_2_button.configure(fg_color=("gray75", "gray25") if name == "frame_2" else "transparent")
        self.frame_3_button.configure(fg_color=("gray75", "gray25") if name == "frame_3" else "transparent")

        # show selected frame
        if name == "home":
            self.home_frame.grid(row=0, column=1, sticky="nsew")
        else:
            self.home_frame.grid_forget()
        if name == "frame_2":
            self.second_frame.grid(row=0, column=1, sticky="nsew")
        else:
            self.second_frame.grid_forget()
        if name == "frame_3":
            self.third_frame.grid(row=0, column=1, sticky="nsew")
        else:
            self.third_frame.grid_forget()

    def home_button_event(self):
        self.select_frame_by_name("home")

    def frame_2_button_event(self):
        self.select_frame_by_name("frame_2")

    def frame_3_button_event(self):
        self.select_frame_by_name("frame_3")

    def change_appearance_mode_event(self, new_appearance_mode):
        ctk.set_appearance_mode(new_appearance_mode)

    def change_scaling_event(self, new_scaling: str):
        new_scaling_float = int(new_scaling.replace("%", "")) / 100
        ctk.set_widget_scaling(new_scaling_float)

    def connection_button_event(self):
        def find_esp():
            ports = list(serial.tools.list_ports.comports())
            for port in ports:
                logger.info(f"Trying {port.device}...")
                try:
                    ser = serial.Serial(port.device, 115200, timeout=1)
                    ser.write(b"INIT")
                    response = ser.readline().decode('utf-8').strip()
                    if response == "ESP":
                        logger.info(f"Connected to ESP on {port.device}")
                        self.connection_label.configure(image=self.link_image)  # Cambiar la imagen
                        ser.close()
                        break
                    ser.close()
                except (OSError, serial.SerialException) as e:
                    logger.error(f"Failed to connect to {port.device}: {e}")

        # Ejecutar la búsqueda en un hilo separado
        threading.Thread(target=find_esp).start()

if __name__ == "__main__":
    app = App()
    app.mainloop()
