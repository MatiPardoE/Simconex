import customtkinter as ctk
import os
import time
import threading
from PIL import Image
import frames.serial_handler as ui_serial
from frames.serial_handler import MsgType 
from frames.serial_handler import CycleStatus 
from frames.serial_handler import data_lists 
from frames.serial_handler import data_lists_manual
import csv
from datetime import datetime, timedelta
from tkinter import messagebox


class CalibPhWindow(ctk.CTkToplevel):
    def __init__(self, master = None):
        super().__init__(master = master)

        image_path = os.path.join(os.getcwd(), "images")

        self.title('Calibracion sensor de pH')
        self.master = master
        self.geometry("700x450")
        self.resizable(0,0)

        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1) 
        self.grid_rowconfigure(2, weight=1)
        self.grid_rowconfigure(3, weight=1)
        self.grid_rowconfigure(4, weight=1)

        self.label_title = ctk.CTkLabel(self, text="Como es el proceso de calibracion del sensor de pH", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_title.grid(column=0, row=0, columnspan=2)

        self.label_text = ctk.CTkLabel(self, text="Se recomienda calibrar el sensor en 3 puntos: medio (7), bajo (4) y alto (10)", font=ctk.CTkFont(size=15))
        self.label_text.grid(column=0, row=1, columnspan=2)

        self.img_steps = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_steps.png")), size=(565, 282))
        self.img_solutions = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_solutions.png")), size=(476, 282))
        self.img_attention = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_attention.png")), size=(520, 282))
        self.img_start = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_start.png")), size=(160, 282))
        self.img_mid = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_mid.png")), size=(560, 282))
        self.img_low = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_low.png")), size=(560, 282))
        self.img_high = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_high.png")), size=(560, 282))
        self.img_check = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_check.png")), size=(300, 282))
        self.img_label = ctk.CTkLabel(self, text="", image=self.img_steps)
        self.img_label.grid(row=2, column=0, padx=0, pady=0, sticky="nsew", columnspan=2)

        self.ph_button = ctk.CTkButton(self, text="pH: --", fg_color="white", hover=False, state="disabled", text_color_disabled="black", width=100)
        self.ph_button.grid(column=0, row=3, padx=10, pady=0, sticky="ns", columnspan=2)
        self.ph_button.grid_forget()

        self.btn_end = ctk.CTkButton(self, text="Terminar calibracion", command=self.btn_end_press, fg_color="red")
        self.btn_end.grid(column=0, row=4, pady=15)
        self.btn_end.grid_forget()

        self.btn = ctk.CTkButton(self, text="Siguiente", command=self.btn_press)
        self.btn.grid(column=0, row=4, pady=15, columnspan=2)
    
    def update_seconds(self):
        for i in range(5, 0 ,-1):
            self.btn.configure(text="Siguiente ({seconds})".format(seconds=i))
            time.sleep(1)
        self.btn.configure(text="Siguiente".format(seconds=i))
        self.btn.configure(state="normal")
        self.btn.grid(column=1, row=4, padx=15, pady=15, columnspan=1, sticky="w")
        self.btn_end.grid(column=0, row=4, padx=15, pady=15, sticky="e")

    def btn_end_press(self):
        self.label_title.configure(text="Verificacion")
        self.label_text.configure(text="Espere a verificar la correcta finalizacion de la calibracion")
        self.btn_end.grid_forget()
        self.btn.configure(text="Finalizar")
        self.btn.grid(column=0, row=4, pady=15, columnspan=2, sticky="ns")
        self.img_label.configure(image=self.img_check)

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
            self.ph_button.grid(column=0, row=3, padx=10, pady=0, sticky="ns", columnspan=2)
            self.img_label.configure(image=self.img_mid)
            self.btn.configure(state="disabled")
            thread = threading.Thread(target=self.update_seconds)
            thread.start()            
        elif self.label_title.cget("text") == "Punto medio":
            self.label_title.configure(text="Punto bajo")
            self.img_label.configure(image=self.img_low)
            self.btn.configure(state="disabled")
            
            thread = threading.Thread(target=self.update_seconds)
            thread.start() 
        elif self.label_title.cget("text") == "Punto bajo":
            self.label_title.configure(text="Punto alto")
            self.img_label.configure(image=self.img_high)
            self.btn.configure(state="disabled")
            thread = threading.Thread(target=self.update_seconds)
            thread.start() 
        elif self.label_title.cget("text") == "Punto alto":
            self.label_title.configure(text="Verificacion")
            self.label_text.configure(text="Espere a verificar la correcta finalizacion de la calibracion")
            self.btn_end.grid_forget()
            self.btn.configure(text="Finalizar")
            self.btn.grid(column=0, row=4, pady=15, columnspan=2, sticky="ns")
            self.img_label.configure(image=self.img_check)
        elif self.label_title.cget("text") == "Verificacion":
            self.destroy()

class CalibOdWindow(ctk.CTkToplevel):
    def __init__(self, master = None):
        super().__init__(master = master)

        image_path = os.path.join(os.getcwd(), "images")

        self.title('Calibracion sensor de OD/Temperatura')
        self.master = master
        self.geometry("700x450")
        self.resizable(0,0)

        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1) 
        self.grid_rowconfigure(2, weight=1)
        self.grid_rowconfigure(3, weight=1)
        self.grid_rowconfigure(4, weight=1)

        self.label_title = ctk.CTkLabel(self, text="Como es el proceso de calibracion del sensor de OD", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_title.grid(column=0, row=0, columnspan=2)

        self.label_text = ctk.CTkLabel(self, text="", font=ctk.CTkFont(size=15))
        self.label_text.grid(column=0, row=1, columnspan=2)

        self.img_calib_od = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_od.png")), size=(436, 320))
        self.img_calib_od_1 = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_od_1.png")), size=(207, 221))
        self.img_calib_od_2 = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_od_2.png")), size=(450, 306))
        self.img_calib_od_3 = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_od_3.png")), size=(432, 281))
        self.img_calib_od_4 = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_od_4.png")), size=(440, 295))
        self.img_calib_od_5 = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_od_5.png")), size=(560, 282))
        self.img_calib_od_6 = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_od_6.png")), size=(440, 295))
        self.img_calib_od_7 = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_od_7.png")), size=(440, 295))
        self.img_high = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_high.png")), size=(560, 282))
        self.img_check = ctk.CTkImage(Image.open(os.path.join(image_path, "calib_check.png")), size=(300, 282))
        
        self.img_label = ctk.CTkLabel(self, text="", image=self.img_calib_od)
        self.img_label.grid(row=2, column=0, padx=0, pady=0, sticky="nsew", columnspan=2)

        self.od_button = ctk.CTkButton(self, text="od: --", fg_color="white", hover=False, state="disabled", text_color_disabled="black", width=100)
        self.od_button.grid(column=0, row=3, padx=10, pady=0, sticky="ns", columnspan=2)
        self.od_button.grid_forget()
        
        self.temp_button = ctk.CTkButton(self, text="temp: --", fg_color="white", hover=False, state="disabled", text_color_disabled="black", width=100)
        self.temp_button.grid(column=0, row=3, padx=10, pady=0, sticky="ns", columnspan=2)
        self.temp_button.grid_forget()

        self.btn_a = ctk.CTkButton(self, text="Calibracion por saturacion", command=self.btn_a_press)
        self.btn_a.grid(column=0, row=4, padx=15, pady=15, sticky="e")

        self.btn_b = ctk.CTkButton(self, text="Calibracion por concentracion", command=self.btn_b_press)
        self.btn_b.grid(column=1, row=4, padx=15, pady=15, columnspan=1, sticky="w")
    
    def update_seconds(self):
        for i in range(5, 0 ,-1):
            self.btn_b.configure(text="Siguiente ({seconds})".format(seconds=i))
            time.sleep(1)
        self.btn_b.configure(text="Siguiente")
        self.btn_b.configure(state="normal")
        self.btn_b.grid(column=1, row=4, padx=15, pady=15, columnspan=1, sticky="w")
        self.btn_a.configure(text="Terminar calibracion", fg_color="red", command=self.btn_end_press)
        self.btn_a.grid(column=0, row=4, padx=15, pady=15, sticky="e")

    def btn_a_press(self):
        if self.label_title.cget("text") == "Como es el proceso de calibracion del sensor de OD":
            self.label_title.configure(text="Retire la esponja del recipiente de calibración")
            self.label_text.configure(text="Llene el recipiente hasta la línea con 60 mL de solución fresca")
            self.img_label.configure(image=self.img_calib_od_6)

            self.btn_a.grid_forget()
            self.btn_b.configure(text="Siguiente")
            self.btn_b.grid(column=0, row=4, pady=15, columnspan=2, sticky="ns")

    def btn_b_press(self):
        if self.label_title.cget("text") == "Como es el proceso de calibracion del sensor de OD":
            self.label_title.configure(text="Retire la tapa de la parte superior")
            self.label_text.configure(text="Reemplácela con la tapa de calibración.")
            self.btn_a.grid_forget()
            self.btn_b.configure(text="Siguiente")
            self.btn_b.grid(column=0, row=4, pady=15, columnspan=2, sticky="ns")
            self.img_label.configure(image=self.img_calib_od_1)
        elif self.label_title.cget("text") == "Retire la tapa de la parte superior":
            self.label_title.configure(text="Sature la esponja (aprox 10 mL de agua)")
            self.label_text.configure(text="Colóquela en el fondo de la cámara de calibración")
            self.img_label.configure(image=self.img_calib_od_2)
        elif self.label_title.cget("text") == "Sature la esponja (aprox 10 mL de agua)":
            self.label_title.configure(text="Seque la sonda y el elemento sensor")
            self.label_text.configure(text="Hagalo suavemente con una toalla de papel")
            self.img_label.configure(image=self.img_calib_od_3)
        elif self.label_title.cget("text") == "Seque la sonda y el elemento sensor":
            self.label_title.configure(text="Coloque la sonda en la cámara de calibración")
            self.label_text.configure(text="El sensor debe quedar 2.5 cm por encima de la esponja saturada de agua.")
            self.img_label.configure(image=self.img_calib_od_4)
                     
        elif self.label_title.cget("text") == "Coloque la sonda en la cámara de calibración":
            self.label_title.configure(text="Espere a que las mediciones se estabilicen")
            self.label_text.configure(text="El sensor debe estar 2.5 cm por encima de la esponja saturada de agua.")

            self.btn_b.configure(state="disabled")
            thread = threading.Thread(target=self.update_seconds)
            thread.start()   

        elif self.label_title.cget("text") == "Espere a que las mediciones se estabilicen":
            self.label_title.configure(text="Retire la esponja de la cámara de calibración")
            self.label_text.configure(text="Llene la cámara con 60 mL de sulfito de sodio fresco.")
            self.img_label.configure(image=self.img_calib_od_5)

        elif self.label_title.cget("text") == "Retire la esponja de la cámara de calibración":
            self.label_title.configure(text="Espere a que las mediciones se estabilicen.")

            self.btn_b.configure(state="disabled")
            thread = threading.Thread(target=self.update_seconds)
            thread.start() 

        elif self.label_title.cget("text") == "Espere a que las mediciones se estabilicen.":
            self.label_title.configure(text="Verificacion")
            self.label_text.configure(text="Espere a verificar la correcta finalizacion de la calibracion")
            self.btn_a.grid_forget()
            self.btn_b.configure(text="Finalizar")
            self.btn_b.grid(column=0, row=4, pady=15, columnspan=2, sticky="ns")
            self.img_label.configure(image=self.img_check)
        
        elif self.label_title.cget("text") == "Retire la esponja del recipiente de calibración":
            self.label_title.configure(text="Coloque la sonda en la solución")
            self.label_text.configure(text="Deje 13 mm entre la superficie del sensor y el fondo del recipiente de calibración")
            self.img_label.configure(image=self.img_calib_od_7)

        elif self.label_title.cget("text") == "Coloque la sonda en la solución":
            self.label_title.configure(text="Espere a que las mediciones se estabilicen-")
            self.label_text.configure(text="El sensor debe estar 13 mm por encima del fondo del recipiente de calibracion.")
            # TODO : Aca es donde deberiamos enviar el comando para calibrar
            ui_serial.publisher.send_data(b"#STARTCALODSAT!\n")
            ui_serial.publisher.subscribe(self.update_rdo_value)
            self.od_button.grid(column=0, row=3, padx=10, pady=0, sticky="ns", columnspan=2)
            self.temp_button.grid(column=1, row=3, padx=10, pady=0, sticky="ns", columnspan=2)
            self.btn_b.configure(state="disabled")
            thread = threading.Thread(target=self.update_seconds)
            thread.start() 
        
        elif self.label_title.cget("text") == "Espere a que las mediciones se estabilicen-":
            self.label_title.configure(text="Verificacion")
            self.label_text.configure(text="Espere a verificar la correcta finalizacion de la calibracion")
            self.btn_a.grid_forget()
            self.btn_b.configure(text="Finalizar")
            self.btn_b.grid(column=0, row=4, pady=15, columnspan=2, sticky="ns")
            self.img_label.configure(image=self.img_check)

        elif self.label_title.cget("text") == "Verificacion":
            self.destroy()
    
    def btn_end_press(self):
        ui_serial.publisher.send_data(b"#FINISHCALODSAT1P!\n")
        ui_serial.publisher.unsubscribe(self.update_rdo_value)
        self.od_button.grid_forget()
        self.temp_button.grid_forget()
        self.label_title.configure(text="Verificacion")
        self.label_text.configure(text="Espere a verificar la correcta finalizacion de la calibracion")
        self.btn_a.grid_forget()
        self.btn_b.configure(text="Finalizar")
        self.btn_b.grid(column=0, row=4, pady=15, columnspan=2, sticky="ns")
        self.img_label.configure(image=self.img_check)
        
    def update_rdo_value(self, data):
        # TODO: el data no me esta llegando con el tipo de mensaje
        #if data == MsgType.NEW_MEASUREMENT:
        print("New measurement en RDO CALIB")
        print(f"OD: {data_lists_manual['od'][-1]:.2f}")
        print(f"Temperature: {data_lists_manual['temperature'][-1]:.2f}")
        self.od_button.configure(text=f"{data_lists_manual['od'][-1]}")
        self.temp_button.configure(text=f"{data_lists_manual['temperature'][-1]}")

class SensorCalibrateFrame(ctk.CTkFrame):
    def __init__(self, master, sensor_name):
        super().__init__(master) 

        image_path = os.path.join(os.getcwd(), "images")

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1) 
        self.grid_rowconfigure(2, weight=1)
        self.grid_rowconfigure(3, weight=1)

        self.calib_dates = {}

        if sensor_name == "ph":
            sensor_alias = "pH"
        else:
            sensor_alias = "OD/Temperatura"

        self.label_sensor = ctk.CTkLabel(self, text=f"Calibracion Sensor {sensor_alias}", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_sensor.grid(row=0, column=0, padx=10, pady=(10,0), sticky="nsew")

        if sensor_name == "ph":
            self.sensor_img = ctk.CTkImage(Image.open(os.path.join(image_path, "ph_probe_edited.png")), size=(150, 300))
        else:
            self.sensor_img = ctk.CTkImage(Image.open(os.path.join(image_path, "rdo_probe_edited.png")), size=(150, 300))
        self.sensor_img_label = ctk.CTkLabel(self, text="", image=self.sensor_img)
        self.sensor_img_label.grid(row=1, column=0, padx=0, pady=0, sticky="nsew")

        self.label_last_cal = ctk.CTkLabel(self, text=f"Ultima calibracion realizada el " + self.get_calib_file(sensor_name), font=ctk.CTkFont(size=12))
        self.label_last_cal.grid(row=2, column=0, padx=10, pady=0, sticky="nsew")

        self.cal_button = ctk.CTkButton(self, text="Calibrar sensor", height=40)
        if sensor_name == "ph":
            self.cal_button.configure(command=self.cal_ph_button_event)
        else:
            self.cal_button.configure(command=self.cal_od_button_event)
        self.cal_button.grid(row=3, column=0, padx=10, pady=(0,15))
    
    def cal_ph_button_event(self):
        self.calib_window = CalibPhWindow(self)
        self.update_date("ph") # TODO: hacer que esto ocurra al finalizar la calibracion con exito

        self.calib_window.lift()  
        self.calib_window.attributes("-topmost", True) 
        self.calib_window.after(100, lambda: self.loading_window.attributes("-topmost", False)) 

        self.calib_window.focus() 
        self.calib_window.mainloop()        
    
    def cal_od_button_event(self):
        self.calib_window = CalibOdWindow(self)
        self.update_date("od") # TODO: hacer que esto ocurra al finalizar la calibracion con exito

        self.calib_window.lift()  
        self.calib_window.attributes("-topmost", True) 
        self.calib_window.after(100, lambda: self.loading_window.attributes("-topmost", False)) 

        self.calib_window.focus() 
        self.calib_window.mainloop()  

    def get_calib_file(self, sensor):
        with open("calib/calib_data.csv", mode="r", encoding="utf-8") as csvfile:
            reader = csv.reader(csvfile)
            for row in reader:
                if len(row) == 2: 
                    key, date_str = row
                    date_obj = datetime.strptime(date_str, "%d/%m/%Y")
                    if sensor == key:
                        return str(date_obj.strftime('%d/%m/%Y'))
    
    def read_calib_file(self):
        with open("calib/calib_data.csv", mode="r", encoding="utf-8") as csvfile:
            reader = csv.reader(csvfile)
            for row in reader:
                if len(row) == 2: 
                    key, date_str = row
                    date_obj = datetime.strptime(date_str, "%d/%m/%Y")
                    self.check_date(date_obj, key)
                    self.calib_dates[key] = date_obj
    
    def check_date(self, date, key):
        if date:
            three_months_ago = datetime.now() - timedelta(days=90)
            if date < three_months_ago:
                ui_serial.publisher.notify_out_of_calib(key)
                name = "OD"
                if key == "ph":
                    name = "pH"
                messagebox.showwarning("Advertencia", "El sensor de " + name + " requiere calibración")
                print("Advertencia", "El sensor de " + name + " requiere calibración")
    
    def update_date(self, sensor_name):
        data = []
        with open("calib/calib_data.csv", mode="r", encoding="utf-8") as csvfile:
            reader = csv.reader(csvfile)
            data = list(reader)
        
        today_date = datetime.now().strftime("%d/%m/%Y")
        
        for row in data:
            if len(row) == 2 and row[0] == sensor_name:
                row[1] = today_date
        
        with open("calib/calib_data.csv", mode="w", encoding="utf-8", newline="") as csvfile:
            writer = csv.writer(csvfile)
            writer.writerows(data)        
        self.label_last_cal.configure(text=f"Ultima calibracion realizada el " + str(datetime.now().strftime('%d/%m/%Y')))


class CalibrationFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master, corner_radius=0, fg_color="transparent")
        
        self.grid_columnconfigure((0, 1), weight=1)
        self.grid_rowconfigure(0, weight=2)
        self.grid_rowconfigure(1, weight=1)

        self.instant_values_frame = SensorCalibrateFrame(self, "ph")
        self.instant_values_frame.grid(row=0, column=0, padx=10, pady=(10, 0), sticky="nsew")

        self.actual_cycle_frame = SensorCalibrateFrame(self, "od")
        self.actual_cycle_frame.grid(row=0, column=1, padx=10, pady=(10, 0), sticky="nsew")

