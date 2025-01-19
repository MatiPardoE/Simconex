import serial
import threading
import queue
import time
from enum import Enum
import re
import tkinter
from tkinter import messagebox

class MsgType(Enum):
    ESP_DISCONNECTED = 0
    ESP_CONNECTED = 1
    ESP_SYNCRONIZED = 2
    NEW_MEASUREMENT = 3
    NEW_CYCLE_SENT = 4

class CycleStatus(Enum):
    NOT_CYCLE = 0 # No hay un ciclo corriendo
    CYCLE_RUNNING = 1 # Hay un ciclo corriendo
    CYCLE_PAUSED = 2 # Hay un ciclo corriendo
    CYCLE_FINISHED = 3 # Hay un ciclo terminado
    CYCLE_ERROR = 4 # El ciclo tuvo un error

class SerialPublisher:
    def __init__(self):
        self.data_queue = queue.Queue()
        self.subscribers = [] 
        self.ser = serial.Serial()
        self.start_time = 0

        self.read_thread = threading.Thread(target=self.read_port)
        self.read_thread.daemon = True

        self.find_thread = threading.Thread(target=self.find_esp)
        self.find_thread.daemon = True

    def save_to_queue(self, data):
        self.notify_subscribers(data)

    def notify_new_cycle_started(self):
        data_lists['id'] = []
        data_lists['light'] = []
        data_lists['ph'] = []
        data_lists['od'] = []
        data_lists['temperature'] = []
        data_lists['co2'] = []
        data_lists['o2'] = []
        data_lists['n2'] = []
        data_lists['air'] = []

        data_lists_expected['id'] = []
        data_lists_expected['light'] = []
        data_lists_expected['ph'] = []
        data_lists_expected['od'] = []
        data_lists_expected['temperature'] = []

        for callback in self.subscribers:
            callback(MsgType.NEW_CYCLE_SENT)
    
    def notify_sync(self):
        for callback in self.subscribers: 
            callback(MsgType.ESP_SYNCRONIZED)

    def notify_disconnected(self):
        for callback in self.subscribers: 
            callback(MsgType.ESP_DISCONNECTED)
        
    def notify_connected(self):
        for callback in self.subscribers: callback(MsgType.ESP_CONNECTED)

    def notify_subscribers(self, data):
        if "#Z1!" in data:
            for callback in self.subscribers: callback(MsgType.ESP_DISCONNECTED)
        
        pattern = r"^(\d{8}),(\d{2}\.\d{2}),(\d{3}\.\d{2}),(\d{2}\.\d{2}),(\d{2}),(0|1),(0|1),(0|1),(0|1)$"
        match = re.match(pattern, data)

        if match and cycle_status == CycleStatus.CYCLE_RUNNING: 
            print("Valid measurement and cycle running!")
            data_lists['id'].append(int(match.group(1)))
            data_lists['light'].append(int(match.group(5)))
            data_lists['ph'].append(float(match.group(2)))
            data_lists['od'].append(float(match.group(3)))
            data_lists['temperature'].append(float(match.group(4)))
            data_lists['co2'].append(int(match.group(6)))
            data_lists['o2'].append(int(match.group(7)))
            data_lists['n2'].append(int(match.group(8)))
            data_lists['air'].append(int(match.group(9)))
            self.send_data(b"#OK!\n")

            for callback in self.subscribers: callback(MsgType.NEW_MEASUREMENT)
        
        else:
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

publisher = SerialPublisher()
cycle_id = "" 
cycle_alias = "" 
cycle_interval = 0
cycle_status = CycleStatus.NOT_CYCLE

data_lists = {
    "id": [],
    "ph": [],
    "od": [],
    "temperature": [],
    "light": [],
    "co2": [],
    "o2": [],
    "n2": [],
    "air": []
}

data_lists_expected = {
    "id": [],
    "ph": [],
    "od": [],
    "temperature": [],
    "light": []
}
