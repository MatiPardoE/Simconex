import customtkinter as ctk
import os
from PIL import Image
import serial

class SensorCalibrateFrame(ctk.CTkFrame):
    def __init__(self, master, sensor_name, last_calibration):
        super().__init__(master) 

        image_path = os.path.join(os.getcwd(), "images")

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1) 
        self.grid_rowconfigure(2, weight=1)
        self.grid_rowconfigure(3, weight=1)

        self.label_sensor = ctk.CTkLabel(self, text=f"Calibracion Sensor {sensor_name}", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_sensor.grid(row=0, column=0, padx=10, pady=(10,0), sticky="nsew")

        if sensor_name == "pH":
            self.sensor_img = ctk.CTkImage(Image.open(os.path.join(image_path, "ph_probe_edited.png")), size=(150, 300))
        elif sensor_name == "OD/Temperatura":
            self.sensor_img = ctk.CTkImage(Image.open(os.path.join(image_path, "rdo_probe_edited.png")), size=(150, 300))
        self.sensor_img_label = ctk.CTkLabel(self, text="", image=self.sensor_img)
        self.sensor_img_label.grid(row=1, column=0, padx=0, pady=0, sticky="nsew")

        self.label_last_cal = ctk.CTkLabel(self, text=f"Ultima calibracion realizada hace {last_calibration} meses", font=ctk.CTkFont(size=12))
        self.label_last_cal.grid(row=2, column=0, padx=10, pady=0, sticky="nsew")

        self.cal_button = ctk.CTkButton(self, text="Calibrar sensor", command=self.cal_button_event, height=40)
        self.cal_button.grid(row=3, column=0, padx=10, pady=(0,15))
    
    def cal_button_event(self):
        print("Calibrar")

class RecommendationsFrame(ctk.CTkFrame):
    def __init__(self, master, recommendations):
        super().__init__(master) 

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1)

        self.label_control = ctk.CTkLabel(self, text="Recomendaciones", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=(20, 10), pady=(10, 0), sticky="w")

        self.frame_lines = ctk.CTkFrame(self)
        self.frame_lines.grid(row=1, column=0, padx=10, pady=10, sticky="ew")
        self.frame_lines.grid_columnconfigure(0, weight=1)
        self.frame_lines.grid_rowconfigure(0, weight=1)

        for i in (range(len(recommendations))):
            self.frame_line = ctk.CTkFrame(self.frame_lines)
            self.frame_line.grid(row=i, column=0, padx=5, pady=0, sticky="ew")

            self.frame_line.grid_rowconfigure(0, weight=1)
            self.frame_line.grid_columnconfigure(0, weight=1)

            self.line = ctk.CTkLabel(self.frame_line, text=recommendations[i], corner_radius=0)
            self.line.grid(row=0, column=0, padx=5, pady=0, sticky="ns")

class CalibrationFrame(ctk.CTkFrame):
    def __init__(self, master, ser):
        super().__init__(master, corner_radius=0, fg_color="transparent")

        self.serial = ser
        
        self.grid_columnconfigure((0, 1), weight=1)
        self.grid_rowconfigure(0, weight=2)
        self.grid_rowconfigure(1, weight=1)

        self.instant_values_frame = SensorCalibrateFrame(self, "pH", 6)
        self.instant_values_frame.grid(row=0, column=0, padx=10, pady=(10, 0), sticky="nsew")

        self.actual_cycle_frame = SensorCalibrateFrame(self, "OD/Temperatura", 2)
        self.actual_cycle_frame.grid(row=0, column=1, padx=10, pady=(10, 0), sticky="nsew")

        self.plot1_frame = RecommendationsFrame(self, ["Limpiar cabezal del sensor"])
        self.plot1_frame.grid(row=1, column=0, padx=10, pady=(10, 0), sticky="nsew")

        self.plot2_frame = RecommendationsFrame(self, ["Ajustar fijacion del sensor", "Limpiar cabezal del sensor"])
        self.plot2_frame.grid(row=1, column=1, padx=10, pady=(10, 0), sticky="nsew")

    def update_serial_obj(self, ser):
        self.serial = ser
