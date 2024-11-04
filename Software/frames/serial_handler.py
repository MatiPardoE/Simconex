import serial
import threading
import queue
import time
from enum import Enum
import re

class MsgType(Enum):
    ESP_DISCONNECTED = 0
    ESP_CONNECTED = 1
    NEW_MEASUREMENT = 2

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

    def notify_subscribers(self, data):
        pattern = r"#(STA)([012])\!"
        match = re.match(pattern, data)
        if match:
            for callback in self.subscribers: callback(MsgType.ESP_CONNECTED)
            value = int(match.group(2))
            if value == 0:
                state_fbr["state"] = "running"
            elif value == 1:
                state_fbr["state"] = "syncing"
            elif value == 2:
                state_fbr["state"] = "finished"
            print("** FBR Simconex cycle state: " + state_fbr["state"] + " **")

        if "#Z1!" in data:
            for callback in self.subscribers: callback(MsgType.ESP_DISCONNECTED)
        
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
        
        print("No ESP32 device found")
    
    def start_read_thread(self):
        self.read_thread = threading.Thread(target=self.read_port)
        self.read_thread.daemon = True
        self.read_thread.start()

    def stop_read_thread(self):
        self.ser.close()

    def start_find_thread(self):
        self.find_thread = threading.Thread(target=self.find_esp)
        self.find_thread.daemon = True
        self.find_thread.start()

    def send_data(self, data):
        print("Serial Publisher (send_data)", data)
        self.ser.write(data)

publisher = SerialPublisher()
state_fbr = { "state": "disconnected" }
cycle_id = ""

data_lists = {
    "id": [],
    "ph": [],
    "od": [],
    "temperature": [],
    "light": []
}
