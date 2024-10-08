import customtkinter as ctk
import os
from PIL import Image

class CalibWindow(ctk.CTkToplevel):
    def __init__(self, master = None):
        super().__init__(master = master)

        image_path = os.path.join(os.getcwd(), "images")

        self.title('Calibracion sensor de pH')
        self.master = master
        self.geometry("700x450")
        self.resizable(0,0)

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1) 
        self.grid_rowconfigure(2, weight=1)
        self.grid_rowconfigure(3, weight=1)

        self.label_title = ctk.CTkLabel(self, text="Como es el proceso de calibracion del sensor de pH", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_title.grid(column=0, row=0)

        self.label_text = ctk.CTkLabel(self, text="Se recomienda calibrar el sensor en 3 puntos: medio (7), bajo (4) y alto (10)", font=ctk.CTkFont(size=15))
        self.label_text.grid(column=0, row=1)

        self.img_steps = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_steps.png")), size=(565, 282))
        self.img_solutions = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_solutions.png")), size=(476, 282))
        self.img_attention = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_attention.png")), size=(520, 282))
        self.img_start = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_start.png")), size=(160, 282))
        self.img_mid = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_mid.png")), size=(560, 282))
        self.img_low = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_low.png")), size=(560, 282))
        self.img_high = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_high.png")), size=(560, 282))
        self.img_check = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_check.png")), size=(300, 282))
        self.img_label = ctk.CTkLabel(self, text="", image=self.img_steps)
        self.img_label.grid(row=2, column=0, padx=0, pady=0, sticky="nsew")

        self.btn = ctk.CTkButton(self, text="Siguiente", command=self.btn_press)
        self.btn.grid(column=0, row=3, pady=15)

    def btn_press(self):
        if self.label_title.cget("text") == "Como es el proceso de calibracion del sensor de pH":
            self.label_title.configure(text="Que soluciones usar para la calibracion")
            self.label_text.configure(text="Se recomienda usar soluciones que tengan valores sencillos")
            self.img_label.configure(image=self.img_solutions)
        elif self.label_title.cget("text") == "Que soluciones usar para la calibracion":
            self.label_title.configure(text="Buenas practicas durante la calibracion")
            self.label_text.configure(text="Siempre prestar atencion a las mediciones durante el proceso. Esperar a que se estabilicen las lecturas.")
            self.img_label.configure(image=self.img_attention)
        elif self.label_title.cget("text") == "Buenas practicas durante la calibracion":
            self.label_title.configure(text="Comienzo del proceso de calibracion")
            self.label_text.configure(text="Para eliminar la calibracion actual, haga click en Siguiente")
            self.img_label.configure(image=self.img_start)
        elif self.label_title.cget("text") == "Comienzo del proceso de calibracion":
            self.label_title.configure(text="Punto medio")
            self.label_text.configure(text="Coloque el sensor en la solucion. Espere durante al menos un minuto y hasta que las lecturas sean estables")
            self.img_label.configure(image=self.img_mid)
        elif self.label_title.cget("text") == "Punto medio":
            self.label_title.configure(text="Punto bajo")
            self.img_label.configure(image=self.img_low)
        elif self.label_title.cget("text") == "Punto bajo":
            self.label_title.configure(text="Punto alto")
            self.img_label.configure(image=self.img_high)
        elif self.label_title.cget("text") == "Punto alto":
            self.label_title.configure(text="Verificacion")
            self.label_text.configure(text="Espere a verificar la correcta finalizacion de la calibracion")
            self.btn.configure(text="Finalizar")
            self.img_label.configure(image=self.img_check)
        elif self.label_title.cget("text") == "Verificacion":
            self.destroy()

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
        self.calib_window = CalibWindow(self)
        self.calib_window.focus() 


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
    def __init__(self, master):
        super().__init__(master, corner_radius=0, fg_color="transparent")
        
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


