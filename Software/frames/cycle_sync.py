import customtkinter as ctk
import time
from enum import Enum
import frames.serial_handler as ui_serial
from frames.serial_handler import data_lists
from frames.serial_handler import CycleStatus
from frames.serial_handler import ModeStatus
from PIL import Image, ImageTk
import re
import csv
import os
import threading

class HandshakeStatus(Enum):
    STA = 11
    STA2 = 10
    STA1 = 0
    STA0 = 8
    MSG_VALID = 7
    TIMESTAMP = 6
    DATAOUT1 = 5
    DATAOUT0 = 4
    ID1 = 3
    ID0 = 2
    OK = 1
    NOT_YET = 0
    TIMEOUT = -1
    ERROR = -2
    DATA_FAIL = -3

class CycleSync:
    def __init__(self):
        self.handshake_status = HandshakeStatus.NOT_YET
        self.start_validate_time = 0
        self.esp_status_reported = CycleStatus.NOT_CYCLE

        # Listas temporales acumuladoras de toda la comunicacion
        self.id_list = []
        self.ph_list = []
        self.od_list = []
        self.temp_list = []
        self.light_t_list = []
        self.light_mt_list = []
        self.light_mm_list = []
        self.light_ml_list = []
        self.light_l_list = []    
        self.co2_list = []
        self.o2_list = []
        self.n2_list = []
        self.air_list = []
        self.conc_list = []

        # Listas temporales acumuladoras de cada bloque
        self.id_list_tmp = []
        self.ph_list_tmp = []
        self.od_list_tmp = []
        self.temp_list_tmp = []
        self.light_t_list_tmp = []
        self.light_mt_list_tmp = []
        self.light_mm_list_tmp = []
        self.light_ml_list_tmp = []
        self.light_l_list_tmp = [] 
        self.co2_list_tmp = []
        self.o2_list_tmp = []
        self.n2_list_tmp = []
        self.air_list_tmp = []
        self.conc_list_tmp = []

        self.line_count = 0

    def sync_running_cycle(self, timeout=3):
        print("Syncronization of running cycle started!")
        self.show_loading_window()    
       
    def start_sync_cycle(self, timeout=5):
        try:
            ui_serial.mode_status = ModeStatus.MODE_SYNC       
            ui_serial.publisher.subscribe(self.wait_for_response)
            ui_serial.publisher.send_data(b"#SYNC0!\n")
            self.wait_message(HandshakeStatus.STA, timeout)
            ui_serial.publisher.send_data(b"#OK!\n")

            if not self.esp_status_reported == CycleStatus.NOT_CYCLE:
                self.wait_message(HandshakeStatus.ID0, timeout)
                ui_serial.publisher.send_data(b"#OK!\n")
                self.wait_message(HandshakeStatus.TIMESTAMP, timeout)
                ui_serial.publisher.send_data(b"#OK!\n")
                self.wait_message(HandshakeStatus.ID1, timeout)
                ui_serial.publisher.send_data(b"#OK!\n")
                self.wait_message(HandshakeStatus.DATAOUT0, timeout)
                ui_serial.publisher.send_data(b"#OK!\n")
                ui_serial.publisher.unsubscribe(self.wait_for_response)

                start_time = time.time()  # Start timing the transfer

                ui_serial.publisher.subscribe(self.wait_for_dataout)
                self.receive_data(timeout=5)
                ui_serial.publisher.unsubscribe(self.wait_for_dataout)

                end_time = time.time()  # End timing the transfer
                print(f"Transfer completed in {end_time - start_time:.2f} seconds")

                ui_serial.publisher.subscribe(self.wait_for_response)

            self.send_data_and_wait_hs(b"#SYNC1!\n")
            ui_serial.publisher.unsubscribe(self.wait_for_response) 

            #if not ui_serial.cycle_status == CycleStatus.NOT_CYCLE:
            self.generate_cycleout_file()

            self.success_loading_window()
            ui_serial.cycle_status = self.esp_status_reported
            ui_serial.mode_status = ModeStatus.NOT_MODE # Ya termino la sync asi que salgo de este modo
            ui_serial.publisher.notify_sync()           
        
        except Exception as e:
            ui_serial.mode_status = ModeStatus.NOT_MODE
            print("Syncronization of running cycle failed!")
            print(e)
            self.failed_loading_window()
            ui_serial.publisher.unsubscribe(self.wait_for_response)
            ui_serial.publisher.unsubscribe(self.wait_for_dataout)

    def wait_for_ok(self, data):
        if data.strip() == "#OK!":
            self.handshake_status = HandshakeStatus.OK
        elif data.strip() == "#FAIL!":
            self.handshake_status = HandshakeStatus.DATA_FAIL
        else:
            print("Received unexpected data in HS:", data.strip())
            self.handshake_status = HandshakeStatus.ERROR
    
    def receive_data(self, timeout=5):
        start_time = time.time()
        while True:
            if self.handshake_status == HandshakeStatus.DATAOUT1:
                print("Handshake OK")
                start_time = time.time()
                self.handshake_status = HandshakeStatus.NOT_YET
                break
            if self.handshake_status == HandshakeStatus.MSG_VALID:
                print("Valid line of data received")
                start_time = time.time()
                self.handshake_status = HandshakeStatus.NOT_YET
            if self.handshake_status == HandshakeStatus.DATA_FAIL:
                raise ValueError("Data lost between ESP and UI")
            if time.time() - start_time > timeout:
                self.handshake_status = HandshakeStatus.TIMEOUT
                raise TimeoutError("Timeout waiting for handshake from ESP")

    def wait_for_dataout(self, data):
        pattern = r"^(\d{8}),(\d{2}\.\d{2}),(\d{3}\.\d{2}),(\d{2}\.\d{2}),(\d{3}),(\d{3}),(\d{3}),(\d{3}),(\d{3}),(0|1),(0|1),(0|1),(0|1),(\d{2}\.\d{2})$"
        if data == "#DATAOUT1!":
            self.id_list += self.id_list_tmp
            self.light_t_list += self.light_t_list_tmp
            self.light_mt_list += self.light_mt_list_tmp
            self.light_mm_list += self.light_mm_list_tmp
            self.light_ml_list += self.light_ml_list_tmp
            self.light_l_list += self.light_l_list_tmp
            self.ph_list += self.ph_list_tmp
            self.od_list += self.od_list_tmp
            self.temp_list += self.temp_list_tmp
            self.co2_list += self.co2_list_tmp
            self.o2_list += self.o2_list_tmp
            self.n2_list += self.n2_list_tmp
            self.air_list += self.air_list_tmp
            self.conc_list += self.conc_list_tmp
        
            data_lists['id'] = self.id_list
            data_lists['light_t'] = self.light_t_list
            data_lists['light_mt'] = self.light_mt_list
            data_lists['light_mm'] = self.light_mm_list
            data_lists['light_ml'] = self.light_ml_list
            data_lists['light_l'] = self.light_l_list
            data_lists['ph'] = self.ph_list
            data_lists['od'] = self.od_list
            data_lists['temperature'] = self.temp_list
            data_lists['co2'] = self.co2_list
            data_lists['o2'] = self.o2_list
            data_lists['n2'] = self.n2_list
            data_lists['air'] = self.air_list
            data_lists['conc'] = self.conc_list
            print("Valid block of data received")
            ui_serial.publisher.send_data(b"#OK!\n")
            self.handshake_status = HandshakeStatus.DATAOUT1
            return
        match = re.match(pattern, data)
        if match: 
            self.line_count += 1
            self.id_list_tmp.append(int(match.group(1)))
            self.light_t_list_tmp.append(int(match.group(5)))
            self.light_mt_list_tmp.append(int(match.group(6)))
            self.light_mm_list_tmp.append(int(match.group(7)))
            self.light_ml_list_tmp.append(int(match.group(8)))
            self.light_l_list_tmp.append(int(match.group(9)))
            self.ph_list_tmp.append(float(match.group(2)))
            self.od_list_tmp.append(float(match.group(3)))
            self.temp_list_tmp.append(float(match.group(4)))
            self.co2_list_tmp.append(int(match.group(10)))
            self.o2_list_tmp.append(int(match.group(11)))
            self.n2_list_tmp.append(int(match.group(12)))
            self.air_list_tmp.append(int(match.group(13)))
            self.conc_list_tmp.append(float(match.group(14)))
            self.handshake_status = HandshakeStatus.MSG_VALID

            if self.line_count == 160:
                print("Valid block of data received")
                self.line_count = 0

                self.id_list += self.id_list_tmp
                self.light_t_list += self.light_t_list_tmp
                self.light_mt_list += self.light_mt_list_tmp
                self.light_mm_list += self.light_mm_list_tmp
                self.light_ml_list += self.light_ml_list_tmp
                self.light_l_list += self.light_l_list_tmp
                self.ph_list += self.ph_list_tmp
                self.od_list += self.od_list_tmp
                self.temp_list += self.temp_list_tmp
                self.co2_list += self.co2_list_tmp
                self.o2_list += self.o2_list_tmp
                self.n2_list += self.n2_list_tmp
                self.air_list += self.air_list_tmp
                self.conc_list += self.conc_list_tmp

                self.id_list_tmp = []
                self.ph_list_tmp = []
                self.od_list_tmp = []
                self.temp_list_tmp = []
                self.light_t_list_tmp = []
                self.light_mt_list_tmp = []
                self.light_mm_list_tmp = []
                self.light_ml_list_tmp = []
                self.light_l_list_tmp = []
                self.co2_list_tmp = []
                self.o2_list_tmp = []
                self.n2_list_tmp = []
                self.air_list_tmp = []
                self.conc_list_tmp = []
                ui_serial.publisher.send_data(b"#OK!\n")
        else: 
            self.line_count = 0                   
            print("Mensaje desconocido:", data)
            time.sleep(0.3)   
            self.id_list_tmp = []
            self.ph_list_tmp = []
            self.od_list_tmp = []
            self.temp_list_tmp = []
            self.light_t_list_tmp = []
            self.light_mt_list_tmp = []
            self.light_mm_list_tmp = []
            self.light_ml_list_tmp = []
            self.light_l_list_tmp = []
            self.co2_list_tmp = []
            self.o2_list_tmp = []
            self.n2_list_tmp = []
            self.air_list_tmp = []
            self.conc_list_tmp = []

            ui_serial.publisher.send_data("#FAIL!\n")
    
    def wait_for_response(self, data):
        pattern = r'\b\d{8}_\d{4}\b'        

        if data.strip() == "#OK!":
            self.handshake_status = HandshakeStatus.OK
        elif data.strip() == "#FAIL!":
            self.handshake_status = HandshakeStatus.DATA_FAIL
        elif data.strip() == "#STA0!":
            self.esp_status_reported = CycleStatus.NOT_CYCLE
            self.handshake_status = HandshakeStatus.STA
        elif data.strip() == "#STA1!":
            self.esp_status_reported = CycleStatus.CYCLE_RUNNING
            self.handshake_status = HandshakeStatus.STA
        elif data.strip() == "#STA2!":
            self.esp_status_reported = CycleStatus.CYCLE_FINISHED
            self.handshake_status = HandshakeStatus.STA
        elif data.strip() == "#STA3!":
            self.esp_status_reported = CycleStatus.CYCLE_PAUSED
            self.handshake_status = HandshakeStatus.STA
        elif data.strip() == "#ID0!":
            self.handshake_status = HandshakeStatus.ID0
        elif data.strip() == "#ID1!":
            self.handshake_status = HandshakeStatus.ID1
        elif data.strip() == "#DATAOUT0!":
            self.handshake_status = HandshakeStatus.DATAOUT0
        elif data.strip() == "#DATAOUT1!":
            self.handshake_status = HandshakeStatus.DATAOUT1
        else:
            if re.match(pattern, data):
                ui_serial.cycle_id = data
                self.handshake_status = HandshakeStatus.TIMESTAMP
            else:
                print("Received unexpected data in HS:", data.strip())
                self.handshake_status = HandshakeStatus.ERROR
        
        print("") # NO SACAR ESTE PRINT QUE SE ROMPE LA COMUNICACION

    def send_data_and_wait_hs(self, data, timeout = 3):
        self.handshake_status = HandshakeStatus.NOT_YET
        ui_serial.publisher.send_data(data)
        self.wait_handshake(timeout)
    
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
    
    def wait_message(self, status, timeout = 5): # Implementar esto en todos los handshakes porque es mas generico
        start_time = time.time()
        while True:
            if self.handshake_status == status:
                print("Recibi la respuesta esperada")
                self.handshake_status = HandshakeStatus.NOT_YET
                break
            if self.handshake_status == HandshakeStatus.DATA_FAIL:
                raise ValueError("Data lost between ESP and UI")
            if time.time() - start_time > timeout:
                self.handshake_status = HandshakeStatus.TIMEOUT
                raise TimeoutError("Timeout waiting for handshake from ESP")
    
    def generate_cycleout_file(self):
        self.cycle_out_path = os.path.join(os.getcwd(), "Log", ui_serial.cycle_id)
        os.makedirs(self.cycle_out_path, exist_ok=True) 
        fname = os.path.join(os.getcwd(), "Log", ui_serial.cycle_id, "cycle_out_"+ui_serial.cycle_id+".csv")
        with open(fname, mode="w", newline='') as csvfile:
            writer = csv.writer(csvfile)
            
            for i in range(len(data_lists["id"])):
                row = [
                    f"{data_lists['id'][i]:08d}",      
                    f"{data_lists['ph'][i]:05.2f}",       
                    f"{data_lists['od'][i]:06.2f}",     
                    f"{data_lists['temperature'][i]:05.2f}",  
                    f"{data_lists['light_t'][i]:03d}",
                    f"{data_lists['light_mt'][i]:03d}",
                    f"{data_lists['light_mm'][i]:03d}",
                    f"{data_lists['light_ml'][i]:03d}",
                    f"{data_lists['light_l'][i]:03d}",
                    data_lists['co2'][i],
                    data_lists['o2'][i],
                    data_lists['n2'][i],
                    data_lists['air'][i],
                    data_lists['conc'][i] 
                ]
                writer.writerow(row)
    
    def show_loading_window(self):
        self.loading_window = ctk.CTkToplevel()
        self.loading_window.title("Cargando...")
        self.loading_window.geometry("400x100")

        image_path = os.path.join(os.getcwd(), "images")

        loading_image = ctk.CTkImage(light_image=Image.open(os.path.join(image_path, "loading.png")), size=(30, 30))

        self.label = ctk.CTkLabel(self.loading_window, text="Por favor espere, FBR Simconex se está sincronizando...", image=loading_image, compound="left", padx=10)
        self.label.pack(pady=20)

        self.loading_window.lift()  
        self.loading_window.attributes("-topmost", True) 
        self.loading_window.after(100, lambda: self.loading_window.attributes("-topmost", False))  

        threading.Thread(target=self.start_sync_cycle, daemon=True).start()

        self.loading_window.focus() 
        self.loading_window.mainloop()
    
    def success_loading_window(self):
        image_path = os.path.join(os.getcwd(), "images")
        success_image = ctk.CTkImage(light_image=Image.open(os.path.join(image_path, "complete.png")), size=(30, 30))
        self.loading_window.title("Sincronización exitosa")
        self.label.configure(text="Sincronización finalizada con éxito!")
        self.label.configure(image=success_image)
    
    def failed_loading_window(self):
        image_path = os.path.join(os.getcwd(), "images")
        fail_image = ctk.CTkImage(light_image=Image.open(os.path.join(image_path, "alert.png")), size=(30, 30))
        self.loading_window.title("Error")
        self.label.configure(text="Se produjo un problema y la sincronización falló!")
        self.label.configure(image=fail_image)
        ui_serial.publisher.notify_disconnected()   
