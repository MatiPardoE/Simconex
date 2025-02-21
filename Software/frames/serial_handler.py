import csv
import os
import serial
import threading
import queue
import time
from enum import Enum
import re
import tkinter
from tkinter import messagebox

__COUNTER_ALARM_INTERVAL__ = 15

class MsgType(Enum):
    ESP_DISCONNECTED = 0
    ESP_CONNECTED = 1
    ESP_SYNCRONIZED = 2
    NEW_MEASUREMENT = 3
    NEW_CYCLE_SENT = 4
    ESP_PAUSED = 5
    ESP_PLAYED = 6
    CYCLE_DELETED = 7
    TEMP_OUT_OF_RANGE = 8
    OD_OUT_OF_RANGE = 9
    PH_OUT_OF_RANGE = 10
    OD_OUT_OF_CALIB = 11
    PH_OUT_OF_CALIB = 12
    CYCLE_FINISHED = 13
    NEW_MEASURE_CALIB = 14

class CycleStatus(Enum):
    NOT_CYCLE = 0 # No hay un ciclo corriendo
    CYCLE_RUNNING = 1 # Hay un ciclo corriendo
    CYCLE_PAUSED = 2 # Hay un ciclo corriendo
    CYCLE_FINISHED = 3 # Hay un ciclo terminado
    CYCLE_ERROR = 4 # El ciclo tuvo un error
    CYCLE_MANUAL = 5 # El ciclo es manual
    CYCLE_CALIB = 6 # El ciclo es de calibración
    
class ModeStatus(Enum):
    NOT_MODE = 0 # No estoy en modo alguno
    MODE_MANUAL = 1 # El MODO es manual
    MODE_CALIB = 2 # El MODO es de calibración
    MODE_SYNC = 3 # El MODO es de sincronización

class SerialPublisher:
    def __init__(self):
        self.data_queue = queue.Queue()
        self.subscribers = [] 
        self.ser = serial.Serial()
        self.start_time = 0
        self.noti_ph = False
        self.noti_od = False
        self.noti_temp = False

        self.read_thread = threading.Thread(target=self.read_port)
        self.read_thread.daemon = True

        self.find_thread = threading.Thread(target=self.find_esp)
        self.find_thread.daemon = True

    def save_to_queue(self, data):
        self.notify_subscribers(data)

    def notify_new_cycle_started(self):
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

        self.noti_ph = False
        self.noti_od = False
        self.noti_temp = False

        for callback in self.subscribers:
            callback(MsgType.NEW_CYCLE_SENT)
    
    def notify_sync(self):
        for callback in self.subscribers: 
            print(f"Executing callback notify_sync: {callback.__name__}")
            callback(MsgType.ESP_SYNCRONIZED)

    def notify_disconnected(self):
        for callback in self.subscribers: 
            callback(MsgType.ESP_DISCONNECTED)

    def notify_paused(self):
        for callback in self.subscribers: 
            callback(MsgType.ESP_PAUSED)
    
    def notify_played(self):
        for callback in self.subscribers: 
            callback(MsgType.ESP_PLAYED)
        
    def notify_cycle_finished(self):
        for callback in self.subscribers: 
            print(f"Executing callback ciclo finished: {callback.__name__}")
            callback(MsgType.CYCLE_FINISHED)
    
    def notify_deleted(self):
        for callback in self.subscribers: 
            callback(MsgType.CYCLE_DELETED)

    def notify_out_of_calib(self, variable):
        if variable == 'ph':
            for callback in self.subscribers: 
                callback(MsgType.PH_OUT_OF_CALIB)
        elif variable == 'od':
            for callback in self.subscribers: 
                callback(MsgType.OD_OUT_OF_CALIB)
        
    def notify_connected(self):
        for callback in self.subscribers: callback(MsgType.ESP_CONNECTED)

    def notify_out_of_range(self, variable):
        if variable == 'ph':
            for callback in self.subscribers: 
                callback(MsgType.PH_OUT_OF_RANGE)
        elif variable == 'od':
            for callback in self.subscribers: 
                callback(MsgType.OD_OUT_OF_RANGE)
        elif variable == 'temperature':
            for callback in self.subscribers: 
                callback(MsgType.TEMP_OUT_OF_RANGE)

    def notify_subscribers(self, data):
        if "#Z1!" in data:
            for callback in self.subscribers: callback(MsgType.ESP_DISCONNECTED)
        #TODO: AGREGAR LA CONCENTRACION
        pattern = r"^(\d{8}),(\d{2}\.\d{2}),(\d{3}\.\d{2}),(\d{2}\.\d{2}),(\d{3}),(\d{3}),(\d{3}),(\d{3}),(\d{3}),(0|1),(0|1),(0|1),(0|1),(\d{2}\.\d{2})$"
        # pattern = r"^(\d{8}),(?!0{2}\.0{2},0{3}\.0{2},0{2}\.0{2}$)(\d{2}\.\d{2}),(\d{3}\.\d{2}),(\d{2}\.\d{2}),(\d{3}),(\d{3}),(\d{3}),(\d{3}),(\d{3}),(0|1),(0|1),(0|1),(0|1),(\d{2}\.\d{2})$"
        match = re.match(pattern, data)

        if match:
            if cycle_status == CycleStatus.CYCLE_RUNNING: # Caso prioritario si esta corriendo solo guardo en data_list
                # Modo LIVE
                print("Valid measurement and cycle running!")
                data_lists['id'].append(int(match.group(1)))
                data_lists['light_t'].append(int(match.group(5)))
                data_lists['light_mt'].append(int(match.group(6)))
                data_lists['light_mm'].append(int(match.group(7)))
                data_lists['light_ml'].append(int(match.group(8)))
                data_lists['light_l'].append(int(match.group(9)))
                data_lists['ph'].append(float(match.group(2)))
                data_lists['od'].append(float(match.group(3)))
                data_lists['temperature'].append(float(match.group(4)))
                data_lists['co2'].append(int(match.group(10)))
                data_lists['o2'].append(int(match.group(11)))
                data_lists['n2'].append(int(match.group(12)))
                data_lists['air'].append(int(match.group(13)))
                data_lists['conc'].append(float(match.group(14)))
                self.send_data(b"#OK!\n")

                if data_lists_expected['od'][0] != 0:
                    self.in_range(data_lists['od'], data_lists_expected['od'], len(data_lists['od']), 'od')
                if data_lists_expected['ph'][0] != 0:
                    self.in_range(data_lists['ph'], data_lists_expected['ph'], len(data_lists['ph']), 'ph')
                if data_lists_expected['temperature'][0] != 0:
                    self.in_range(data_lists['temperature'], data_lists_expected['temperature'], len(data_lists['temperature']), 'temperature')

                self.cycle_out_path = os.path.join(os.getcwd(), "Log", cycle_id)
                os.makedirs(self.cycle_out_path, exist_ok=True) 
                fname = os.path.join(os.getcwd(), "Log", cycle_id, "cycle_out_"+cycle_id+".csv")
                with open(fname, mode="a", newline="") as csvfile:
                    writer = csv.writer(csvfile)

                    row = [
                        f"{data_lists['id'][-1]:08d}",      
                        f"{data_lists['ph'][-1]:05.2f}",       
                        f"{data_lists['od'][-1]:06.2f}",     
                        f"{data_lists['temperature'][-1]:05.2f}",  
                        f"{data_lists['light_t'][-1]:03d}",
                        f"{data_lists['light_mt'][-1]:03d}",
                        f"{data_lists['light_mm'][-1]:03d}",
                        f"{data_lists['light_ml'][-1]:03d}",
                        f"{data_lists['light_l'][-1]:03d}",
                        data_lists['co2'][-1],
                        data_lists['o2'][-1],
                        data_lists['n2'][-1],
                        data_lists['air'][-1],
                        data_lists['conc'][-1] 
                    ]
                    writer.writerow(row)

                for callback in self.subscribers: callback(MsgType.NEW_MEASUREMENT)
        
            elif mode_status == ModeStatus.MODE_MANUAL: # Si el estado del ciclo no esta corriendo, pero el modo es manual, guardo en data_lists_manual
                # Modo MANUAL
                data_lists_manual['id'].append(int(match.group(1)))
                data_lists_manual['light_t'].append(int(match.group(5)))
                data_lists_manual['light_mt'].append(int(match.group(6)))
                data_lists_manual['light_mm'].append(int(match.group(7)))
                data_lists_manual['light_ml'].append(int(match.group(8)))
                data_lists_manual['light_l'].append(int(match.group(9)))
                data_lists_manual['ph'].append(float(match.group(2)))
                data_lists_manual['od'].append(float(match.group(3)))
                data_lists_manual['temperature'].append(float(match.group(4)))
                data_lists_manual['co2'].append(int(match.group(10)))
                data_lists_manual['o2'].append(int(match.group(11)))
                data_lists_manual['n2'].append(int(match.group(12)))
                data_lists_manual['air'].append(int(match.group(13)))
                data_lists_manual['conc'].append(float(match.group(14)))
                self.send_data(b"#OK!\n")

                for callback in self.subscribers: callback(MsgType.NEW_MEASUREMENT)
            elif mode_status == ModeStatus.MODE_CALIB: # Si el estado del ciclo no esta corriendo, pero el modo es calibración, guardo en data_calib
                # Modo CALIB
                data_calib['ph'] = float(match.group(2))
                data_calib['od'] = float(match.group(3))
                data_calib['temperature'] = float(match.group(4))
                self.send_data(b"#OK!\n")
                
                for callback in self.subscribers: callback(MsgType.NEW_MEASURE_CALIB)

            elif mode_status == ModeStatus.MODE_SYNC: # Si el estado del ciclo no esta corriendo, pero el modo es sincronización, guardo
                
                if (int(match.group(5))==0 and float(match.group(2))==0 and float(match.group(3))==0 and float(match.group(4))==0):
                    data = self.prev_data
                
                for callback in self.subscribers:          
                    callback(data)
                    self.prev_data = data
        else:
            # Resto de comandos que no son intervalos
            for callback in self.subscribers:          
                callback(data)

    def subscribe(self, callback):
        self.subscribers.append(callback)
        print(f"serial_handler.py: Se suscribio alguien! Cantidad de suscriptores: {len(self.subscribers)}")
        
    def unsubscribe(self, callback):
        if callback in self.subscribers:
            self.subscribers.remove(callback)
            print(f"serial_handler.py: Alguien se desuscribió. Cantidad de suscriptores: {len(self.subscribers)}")
    
    def read_port(self):
        buffer = bytearray()
        last_received_time = time.time()
        self.notify_connected()  
        print("Starting read_port")

        while True:
            try:
                if self.ser.in_waiting > 0:
                    new_data = self.ser.read(self.ser.in_waiting)
                    buffer.extend(new_data)
                    last_received_time = time.time()

                    while b'\n' in buffer:
                        line, buffer = buffer.split(b'\n', 1)
                        try:
                            decoded_line = line.decode('utf-8').strip()
                            if decoded_line:  # Only process non-empty lines
                                print(f"----------------- ESP Response: {decoded_line} -----------------")
                                self.save_to_queue(decoded_line)
                        except UnicodeDecodeError as e:
                            print(f"Decode error: {e}")
                            continue
                else:
                    if time.time() - last_received_time > 5:    # Si el fin de la cadena no llega en 5 segundos, se limpia el buffer
                        if buffer:
                            print(f"Timeout reached. Flushing buffer: {buffer.strip()}")
                            buffer.clear()
                        last_received_time = time.time()
            except Exception as e:
                print(f"Exception occurred: {e}")
                if not self.ser.is_open:
                    print("Serial port is closed. Notifying subscribers and breaking loop.")
                    self.notify_subscribers("#Z1!")
                    break        

    def start_read_port(self):       
        self.read_thread.start()
    
    def find_esp(self):
        ports = list(serial.tools.list_ports.comports())
        for port in ports:
            print(f"\nTrying {port.device}...")
            try:
                self.ser.baudrate = 230400
                self.ser.port = port.device
                self.ser.timeout = 2  # Reduced timeout for better responsiveness
                self.ser.open()
                
                # Wait for initial bootup messages
                print(f"Reading initial logs from {port.device}...")
                initial_wait = 1.0  # Wait 1 second for bootup messages
                time.sleep(initial_wait)
                
                # Read and print all available initial messages
                while self.ser.in_waiting > 0:
                    try:
                        buffer = self.ser.read(self.ser.in_waiting).decode('utf-8')
                        if buffer.strip():  # Only print non-empty messages
                            print(f"Initial logs: {buffer.strip()}")
                        time.sleep(0.1)  # Small delay to allow more data to arrive
                    except UnicodeDecodeError:
                        print("Warning: Received non-UTF8 data, skipping...")
                        self.ser.flush()
                        break
                
                # Now send the INIT command and wait for response
                print(f"\n{port.device}: Sending #INIT!")
                self.ser.write("#INIT!\n".encode())
                
                # Wait for response with timeout
                start_time = time.time()
                response = ""
                timeout = 5  # 5 seconds timeout for response
                
                while time.time() - start_time < timeout:
                    if self.ser.in_waiting > 0:
                        response = self.ser.readline().decode('utf-8').strip()
                        print("Response:", response)
                        if response == "#ESP!":
                            print(f"Successfully connected to ESP on {port.device}")
                            self.start_read_thread()
                            return True
                        time.sleep(0.1)
                
                print(f"No valid response from {port.device}")
                self.ser.close()
                
            except (OSError, serial.SerialException) as e:
                print(f"Failed to connect to {port.device}: {e}")
                if self.ser.is_open:
                    self.ser.close()
                continue
        print("No ESP32 device found") # TODO: mostrar un mensaje de error en la interfaz    
    
    def start_read_thread(self):
        self.read_thread = threading.Thread(target=self.read_port)
        self.read_thread.daemon = True
        self.read_thread.start()

    def stop_read_thread(self):
        for callback in self.subscribers: callback(MsgType.ESP_DISCONNECTED)
        self.ser.close()

    def start_find_thread(self):
        self.find_thread = threading.Thread(target=self.find_esp)
        self.find_thread.daemon = True
        self.find_thread.start()

    def send_data(self, data):
        print("Serial Publisher (send_data)", data)
        self.ser.write(data)

    def force_sync(self):
        for callback in self.subscribers: callback(MsgType.ESP_SYNCRONIZED)

    def in_range(self, list_1, list_2, index, variable):
        if index-__COUNTER_ALARM_INTERVAL__ < 0:
            return True
        last_list_1 = list_1[-__COUNTER_ALARM_INTERVAL__:]
        last_list_2 = list_2[index-__COUNTER_ALARM_INTERVAL__:index]

        # Comparar los valores
        for v1, v2 in zip(last_list_1, last_list_2):
            if abs(v2 - v1) < 0.1 * v1:
                if variable == "ph":
                    self.noti_ph = False
                if variable == "od":
                    self.noti_od = False
                if variable == "temperature":
                    self.noti_temp = False
                return True
        
        print("[" + variable + "] Fuera de rango!")

        if variable == "ph" and not self.noti_ph:
            self.noti_ph = True
            self.notify_out_of_range(variable)
        
        if variable == "od" and not self.noti_od:
            self.noti_od = True
            self.notify_out_of_range(variable)

        if variable == "temperature" and not self.noti_temp:
            self.noti_temp = True
            self.notify_out_of_range(variable)

        return False

publisher = SerialPublisher()
cycle_id = "" 
cycle_alias = "" 
cycle_interval = 0
cycle_status = CycleStatus.NOT_CYCLE
mode_status = ModeStatus.NOT_MODE

data_lists = {
    "id": [],
    "ph": [],
    "od": [],
    "temperature": [],
    "light_t": [],
    "light_mt": [],
    "light_mm": [],
    "light_ml": [],
    "light_l": [],
    "co2": [],
    "o2": [],
    "n2": [],
    "air": [],
    "conc": [],
}

data_lists_manual = {
    "id"            : [],
    "ph"            : [],
    "od"            : [],
    "temperature"   : [],
    "light_t": [],
    "light_mt": [],
    "light_mm": [],
    "light_ml": [],
    "light_l"         : [],
    "co2"   : [],
    "o2"    : [],
    "n2"    : [],
    "air"   : [],
    "conc"  : [],
}

data_calib = {
    "ph": 0,
    "od": 0,
    "temperature": 0,
}

data_lists_expected = {
    "id": [],
    "ph": [],
    "od": [],
    "temperature": [],
    "light_t": [],
    "light_mt": [],
    "light_mm": [],
    "light_ml": [],
    "light_l": [],
}
